#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinygraph-utils.h"
#include "tinygraph-zorder.h"
#include "tinygraph-index.h"
#include "tinygraph-bits.h"

/*
 * Spatial index for nearest neighbor queries implemented
 * on top of a z-order space filling curve. This simple
 * technique allows us to reduce the problem of spatial
 * nearest neighbor queries to binary searches on (sub)
 * ranges of the z-order space filling curve.
 *
 * See
 *
 * - https://en.wikipedia.org/wiki/Z-order_curve#Use_with_one-dimensional_data_structures_for_range_searching
 *
 * - Multidimensional Range Search in Dynamically Balanced Trees, H. Tropf, H. Herzog
 *   https://www.vision-tools.com/h-tropf/multidimensionalrangequery.pdf
 *   https://hermanntropf.de/media/multidimensionalrangequery.pdf
 *
 * - Z-Order Indexing for Multifaceted Queries in Amazon DynamoDB
 *   https://aws.amazon.com/blogs/database/z-order-indexing-for-multifaceted-queries-in-amazon-dynamodb-part-1/
 *   https://aws.amazon.com/blogs/database/z-order-indexing-for-multifaceted-queries-in-amazon-dynamodb-part-2/
 */


// The spatial index is a collection of parallel arrays
// sorted by the z-order space filling curve (zvals).
typedef struct tinygraph_index {
  uint32_t size;
  uint32_t* nodes;
  uint64_t* zvals;
  uint32_t* lngs;
  uint32_t* lats;
} tinygraph_index;


// Structure of arrays to array of structs: for sorting
// by z-order space filling curve during construction.
typedef struct tinygraph_index_item {
  uint32_t node;
  uint64_t zval;
  uint32_t lng;
  uint32_t lat;
} tinygraph_index_item;

// Sort items by z-order space filling curve
TINYGRAPH_WARN_UNUSED
static inline int tinygraph_index_item_cmp(const void* lhs, const void *rhs) {
  const tinygraph_index_item ilhs = *(const tinygraph_index_item *)lhs;
  const tinygraph_index_item irhs = *(const tinygraph_index_item *)rhs;

  if (ilhs.zval < irhs.zval) {
    return -1;
  } else if (ilhs.zval > irhs.zval) {
    return +1;
  } else {
    return 0;
  }
}

tinygraph_index* tinygraph_index_construct(
    const uint32_t* nodes,
    const uint32_t* lngs,
    const uint32_t* lats,
    uint32_t n
) {
  TINYGRAPH_ASSERT(nodes);
  TINYGRAPH_ASSERT(lngs);
  TINYGRAPH_ASSERT(lats);
  TINYGRAPH_ASSERT(n > 0);

  tinygraph_index *out = malloc(sizeof(tinygraph_index));

  if (!out) {
    return NULL;
  }

  // To construct the index we sort by the z-order space filling curve
  // so that we can do range queries efficiently later on. This also
  // means below we create new arrays and take ownership over them.
  // It would be possible simply storing the original arrays and a
  // permutation but it would have downsides in terms of spacial
  // locality in memory. In addition at the moment we are storing
  // both the z-order values as well as the longitudes and latitudes:
  // We could compute the z-order values from the longitudes and the
  // latitude (or the opposite direction) on the fly but that is not
  // the cheapest, as we're seeing in the space filling curve reorder
  // feature we have implemented, too. We might want to re-consider
  // this here and for reordering (tinygraph_reorder) in the future.

  tinygraph_index_item * items = malloc(n * sizeof(tinygraph_index_item));

  if (!items) {
    free(out);

    return NULL;
  }

  for (uint32_t i = 0; i < n; ++i) {
    tinygraph_index_item item = (tinygraph_index_item){
      .node = nodes[i],
      .zval = tinygraph_zorder_encode64(lngs[i], lats[i]),
      .lng = lngs[i],
      .lat = lats[i],
    };

    items[i] = item;
  }

  qsort(items, n, sizeof(tinygraph_index_item), tinygraph_index_item_cmp);

  uint32_t* nodes_ = malloc(n * sizeof(uint32_t));

  if (!nodes) {
    free(items);
    free(out);

    return NULL;
  }

  uint64_t* zvals_ = malloc(n * sizeof(uint64_t));

  if (!zvals_) {
    free(nodes_);
    free(items);
    free(out);

    return NULL;
  }

  uint32_t* lngs_ = malloc(n * sizeof(uint32_t));

  if (!lngs_) {
    free(zvals_);
    free(nodes_);
    free(items);
    free(out);

    return NULL;
  }

  uint32_t* lats_ = malloc(n * sizeof(uint32_t));

  if (!lats_) {
    free(lngs_);
    free(zvals_);
    free(nodes_);
    free(items);
    free(out);

    return NULL;
  }

  for (uint32_t i = 0; i < n; ++i) {
    nodes_[i] = items[i].node;
    zvals_[i] = items[i].zval;
    lngs_[i] = items[i].lng;
    lats_[i] = items[i].lat;
  }

  free(items);

  *out = (tinygraph_index) {
    .size = n,
    .nodes = nodes_,
    .zvals = zvals_,
    .lngs = lngs_,
    .lats = lats_,
  };

  return out;
}


tinygraph_index* tinygraph_index_copy(const tinygraph_index * const index) {
  TINYGRAPH_ASSERT(index);
  TINYGRAPH_ASSERT(index->nodes);
  TINYGRAPH_ASSERT(index->zvals);
  TINYGRAPH_ASSERT(index->lngs);
  TINYGRAPH_ASSERT(index->lats);

  tinygraph_index *copy = malloc(sizeof(tinygraph_index));

  if (!copy) {
    return NULL;
  }

  uint32_t* nodes = malloc(index->size * sizeof(uint32_t));

  if (!nodes) {
    free(copy);

    return NULL;
  }

  uint64_t* zvals = malloc(index->size * sizeof(uint64_t));

  if (!zvals) {
    free(nodes);
    free(copy);

    return NULL;
  }

  uint32_t* lngs = malloc(index->size * sizeof(uint32_t));

  if (!lngs) {
    free(zvals);
    free(nodes);
    free(copy);

    return NULL;
  }

  uint32_t* lats = malloc(index->size * sizeof(uint32_t));

  if (!lats) {
    free(lngs);
    free(zvals);
    free(nodes);
    free(copy);

    return NULL;
  }

  memcpy(nodes, index->nodes, index->size * sizeof(uint32_t));
  memcpy(zvals, index->zvals, index->size * sizeof(uint64_t));
  memcpy(lngs, index->lngs, index->size * sizeof(uint32_t));
  memcpy(lats, index->lats, index->size * sizeof(uint32_t));

  *copy = (tinygraph_index) {
    .size = index->size,
    .nodes = nodes,
    .zvals = zvals,
    .lngs = lngs,
    .lats = lats,
  };

  return copy;
}


void tinygraph_index_destruct(tinygraph_index * const index) {
  if (!index) {
    return;
  }

  free(index->lats);
  free(index->lngs);
  free(index->zvals);
  free(index->nodes);

  free(index);
}


static inline const uint64_t* tinygraph_index_bsearch_lt(
    const uint64_t* first,
    const uint64_t* last,
    uint64_t value
) {
  const uint64_t *it = first;

  uint64_t step = 0, n = last - first;

  while (n > 0) {
    step = n / 2;
    it = first + step;

    if (*it < value) { // <- lt means less than here
      first = it + 1;
      n -= step + 1;
    } else {
      n = step;
    }
  }

  return first;
}

static inline const uint64_t* tinygraph_index_bsearch_lte(
    const uint64_t* first,
    const uint64_t* last,
    uint64_t value
) {
  const uint64_t *it = first;

  uint64_t step = 0, n = last - first;

  while (n > 0) {
    step = n / 2;
    it = first + step;

    if (*it <= value) { // <- lte means less than or equal here
      first = it + 1;
      n -= step + 1;
    } else {
      n = step;
    }
  }

  return first;
}

bool tinygraph_index_search(
    tinygraph_index_const_s index,
    uint32_t lngmin, uint32_t latmin,
    uint32_t lngmax, uint32_t latmax,
    uint32_t n,
    uint32_t* out,
    uint32_t* len
) {
  if (n == 0) {
    return true;
  }

  TINYGRAPH_ASSERT(index);
  TINYGRAPH_ASSERT(lngmin <= lngmax);
  TINYGRAPH_ASSERT(latmin <= latmax);

  TINYGRAPH_ASSERT(out);
  TINYGRAPH_ASSERT(len);

  *len = 0;

  // The search for nearest neighbors along the z-order curve
  // works as follows: We compute the z-order value for the
  // bounding box top-left and bottom-right extremes. This
  // gives us a range of z values our nearest neighbors are
  // within. We can then prune this range further by skipping
  // sub-ranges which are outside of the bounding box.

  // The bounding box' z values: We know all nearest neighbors
  // are nodes n in index->nodes[n] with index->zvals[n] >= zmin
  // and index->zvals[n] <= zmax.
  const uint64_t zmin = tinygraph_zorder_encode64(lngmin, latmin);
  const uint64_t zmax = tinygraph_zorder_encode64(lngmax, latmax);

  TINYGRAPH_ASSERT(zmin <= zmax);

  const uint64_t* zfirst = index->zvals;
  const uint64_t* zlast = zfirst + index->size;

  // We know the zvals are sorted (by design) and we can
  // run two consecutive binary searches to find the range
  // the nearest neighbors must be within.
  const uint64_t* first = tinygraph_index_bsearch_lt(zfirst, zlast, zmin);
  const uint64_t* last = tinygraph_index_bsearch_lte(first, zlast, zmax);

  TINYGRAPH_ASSERT(first <= last);

  if (first == last) {
    return false;
  }

  // TODO: Below is the native implementation where we scan the range
  // of z values found through two consecutive binary search above.
  // There is an improved version where we prune this search space
  // further by making use of so called bigmin/litmax computation
  // so that we don't scan sub-ranges outside of the bounding box.

  // TODO: We have already layed out our data structure such that
  // we can work with a struct-of-arrays in a SIMD version where
  // we use e.g. AVX2 to scan eight items at a time.

  for (const uint64_t* it = first; it != last; ++it) {
    if (*len >= n) {
      break;
    }

    const uint64_t i = it - zfirst;
    TINYGRAPH_ASSERT(i < index->size);

    //const uint64_t zval = index->zvals[i];
    const uint32_t lng = index->lngs[i];
    const uint32_t lat = index->lats[i];

    if (lng >= lngmin && lng <= lngmax && lat >= latmin && lat <= latmax) {
      const uint32_t node = index->nodes[i];

      *out = node;
      *len += 1;

      out += 1;
    } else {
      // TODO: jump back in with nextJumpIn / BIGMIN optimization
    }
  }

  return *len > 0;
}


void tinygraph_index_print_internal(const tinygraph_index * const index) {
  TINYGRAPH_ASSERT(index);

  fprintf(stderr, "index internals\n");

  fprintf(stderr, "size: %ju\n", (uintmax_t)index->size);
}
