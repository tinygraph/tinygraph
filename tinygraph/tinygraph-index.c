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
 *
 * - Using a Space Filling Curve for the Management of Dynamic Point Cloud Data in a Relational DBMS
 *   https://www.gdmc.nl/projects/open-source/documents/DynamicPCDMS.pdf
 *   https://pdfs.semanticscholar.org/287b/2fdad23e6110d4a524efcd87c70f8696a15a.pdf
 *   https://isprs-annals.copernicus.org/articles/IV-2-W1/107/2016/isprs-annals-IV-2-W1-107-2016.pdf
 *
 * - Z-ordered Range Refinement for Multi-dimensional Range Queries
 *   https://arxiv.org/abs/2305.12732
 *
 * - Towards a general-purpose, multidimensional index: Integration, Optimization, and Enhancement of UB-Trees
 *   https://mediatum.ub.tum.de/doc/601718/document.pdf
 *
 *
 * Ideas for improvements:
 *
 *   1. The binary searches below `tinygraph_index_bsearch_lt`
 *      and `tinygraph_index_bsearch_lte` are very basic. We
 *      could look into optimizing them, especially the lt
 *      version since we use it primarily to prune the search.
 *
 *   2. Decide when to compute the next z-value when we left
 *      the query bounding box `tinygraph_index_bigmin`. At
 *      the lowest levels we often leave the bounding box
 *      only for one or two nodes before we jump back in.
 *      It's probably not worth computing bigmin and then
 *      running a full binary search just to end up a few
 *      places ahead of us.
 *
 *   3. SIMD-ify the linear scan. For example use AVX2 to
 *      load up 8x u32 lng,lat to run the bounding box
 *      check for the next 8 z-values at the same time.
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

  // Now that we have allocated the parallel arrays and the
  // temporary items array-of-struct to sort, do the actual
  // sorting by the z-order values and then turn the single
  // array-of-struct into a struct-of-array again to query.

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


TINYGRAPH_WARN_UNUSED
static inline const uint64_t* tinygraph_index_bsearch_lt(
    const uint64_t* first,
    const uint64_t* last,
    uint64_t value
) {
  TINYGRAPH_ASSERT(first <= last);

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

TINYGRAPH_WARN_UNUSED
static inline const uint64_t* tinygraph_index_bsearch_lte(
    const uint64_t* first,
    const uint64_t* last,
    uint64_t value
) {
  TINYGRAPH_ASSERT(first <= last);

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

// With a z-value that it outside a bounding box, find the
// smallest next z-value that is again within a bounding box.
// This function goes back to the 80s and has various names:
// BIGMIN/LITMAX, nextJumpIn, GetNextZ-Address
//
// The following implementation is based on the original
// Tropf and Herzog paper linked above and by taking
// inspiration from https://github.com/smatsumt/pyzorder
// with MIT License, Copyright (c) 2019 Shinji Matsumoto.
TINYGRAPH_WARN_UNUSED
static inline uint64_t tinygraph_index_bigmin(uint64_t zval, uint64_t zmin, uint64_t zmax) {
  TINYGRAPH_ASSERT(zval >= zmin);
  TINYGRAPH_ASSERT(zval < zmax);
  TINYGRAPH_ASSERT(zmin <= zmax);

  uint64_t bigmin = zmin;

  uint64_t load_mask = UINT64_C(0x5555555555555555);
  uint64_t load_ones = UINT64_C(0x2aaaaaaaaaaaaaaa);

  uint64_t mask = UINT64_C(0x8000000000000000);

  while (mask) {
    const uint64_t bzval = zval & mask;
    const uint64_t bzmin = zmin & mask;
    const uint64_t bzmax = zmax & mask;

    if (!bzval && !bzmin && !bzmax) {
      // pass
    } else if (!bzval && !bzmin && bzmax) {
      bigmin = (zmin & load_mask) | mask;
      zmax = (zmax & load_mask) | load_ones;
    } else if (!bzval && bzmin && bzmax) {
      TINYGRAPH_ASSERT(zmin > zval);
      return zmin;
    } else if (bzval && !bzmin && !bzmax) {
      TINYGRAPH_ASSERT(bigmin > zval);
      return bigmin;
    } else if (bzval && !bzmin && bzmax) {
      zmin = (zmin & load_mask) | mask;
    } else if (bzval && bzmin && bzmax) {
      // pass
    } else {
      TINYGRAPH_ASSERT(false);
      TINYGRAPH_UNREACHABLE();
    }

    mask >>= 1;
    load_ones >>= 1;
    load_mask >>= 1;
    load_mask |= UINT64_C(0x8000000000000000);
  }

  TINYGRAPH_ASSERT(bigmin > zval);
  return bigmin;
}

bool tinygraph_index_search(
    tinygraph_index_const_s index,
    tinygraph_index_search_opts opts,
    uint32_t* out,
    uint32_t* len
) {
  TINYGRAPH_ASSERT(len);
  *len = 0;

  if (opts.n == 0) {
    return true;
  }

  TINYGRAPH_ASSERT(index);
  TINYGRAPH_ASSERT(opts.lngmin <= opts.lngmax);
  TINYGRAPH_ASSERT(opts.latmin <= opts.latmax);

  TINYGRAPH_ASSERT(out);

  // The search for nearest neighbors along the z-order curve
  // works as follows: We compute the z-order value for the
  // bounding box top-left and bottom-right extremes. This
  // gives us a range of z values our nearest neighbors are
  // within. We can then prune this range further by skipping
  // sub-ranges which are outside of the bounding box.

  // The bounding box' z values: We know all nearest neighbors
  // are nodes n in index->nodes[n] with index->zvals[n] >= zmin
  // and index->zvals[n] <= zmax.
  uint64_t zmin = tinygraph_zorder_encode64(opts.lngmin, opts.latmin);
  uint64_t zmax = tinygraph_zorder_encode64(opts.lngmax, opts.latmax);

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

  // Here we scan along the z-order curve for candidates. It is
  // possible that the z-order curve goes outside our bounding
  // box, that's why we need to filter down candidates further.

  uint32_t outside = 0;
  const uint64_t* it = first;

  while (it != last) {
    if (*len >= opts.n) {
      break;
    }

    const uint64_t i = it - zfirst;
    TINYGRAPH_ASSERT(i < index->size);

    const uint32_t lng = index->lngs[i];
    const uint32_t lat = index->lats[i];

    if (lng >= opts.lngmin && lng <= opts.lngmax && lat >= opts.latmin && lat <= opts.latmax) {
      const uint32_t node = index->nodes[i];

      *out = node;
      out += 1;

      *len += 1;

      // In the case that we are within the bounding box,
      // continue the linear scan through z values.
      it += 1;

      outside = 0;

    } else {
      // The z-order curve has left the bounding box. Here we can prune
      // the search space by finding the next z-order value that again
      // is within the bounding box and jump back in immediately pruning
      // the search space instead of scanning outside the bounding box.
      //
      // We make the following observations
      //
      // 1. The z-order curve has discontinuities causing the search
      //    space to be potentially bigger than the bounding box.
      //
      // 2. The discontinuities happen at the half-point of the
      //    implicit quad-tree represented by the z-order value.
      //
      // 3. The discontinuities are worse the higher the quad-tree
      //    level of the half-point we are crossing over.
      //
      // Example
      //
      //   0000  0001  |  0100   0101
      //               |
      //   0010  0011  |  0110   0111
      //   --------------------------
      //   1000  1001  |  1100   1101
      //               |
      //   1010  1011  |  1110   1111
      //
      // Note that we can compute the discontinuities from two
      // consecutive z-values based on how many bits are changing
      // and at which bit position the first bit difference occurs
      //
      // __builtin_clzll(z1 ^ z2) < t
      //
      // This can help us in the future to decide when to trade-off
      // computation (calculating bigmin and jumping back in) vs.
      // continuing the linear memory scan.
      //
      // Here are two examples for the lowest level of the z-values
      //
      // 1) The bounding box at its right side divides a
      //    lowest z level by half. In this case the z
      //    values in the example below alternate between
      //    1 (in), 2 (out), 3 (in), 4 (out).
      //
      //   ----
      //    1 | 2
      //    3 | 4
      //   ----
      //
      // 2) The bounding box at its bottom side divides
      //    lowest z levels by half. In this case the z
      //    values in the example below alternate between
      //    1 (in), 2 (in), 3 (out), 4 (out),
      //    5 (in), 6 (in), 7 (out), 8 (out).
      //
      //  | 1  2  5  6 |
      //  |------------|
      //    3  4  7  8
      //
      // Note that one idea could be extending the initial
      // z-order range [zmin, zmax] above to make it bigger
      // and snap it to a grid on specific levels based on
      // observations here. It's a trae-off between
      // computation to jump back in and filtering the
      // candidates by their lng, lat in the search above.
      //
      // In the future we could try and special these and
      // similar cases; for now we always run binary search.

      outside += 1;

      const uint64_t zval = *it;

      if (outside > 64 && zval < zmax) {
        const uint64_t bigmin = tinygraph_index_bigmin(zval, zmin, zmax);
        TINYGRAPH_ASSERT(bigmin > zval);
        TINYGRAPH_ASSERT(bigmin > zmin);
        TINYGRAPH_ASSERT(bigmin <= zmax);

        const uint64_t* skip = tinygraph_index_bsearch_lt(it, last, bigmin);
        TINYGRAPH_ASSERT(skip >= it);
        TINYGRAPH_ASSERT(skip <= last);

        it = skip;
      } else {
        it += 1;
      }
    }
  }

  return *len > 0;
}


void tinygraph_index_print_internal(const tinygraph_index * const index) {
  TINYGRAPH_ASSERT(index);

  fprintf(stderr, "index internals\n");

  fprintf(stderr, "size: %ju\n", (uintmax_t)index->size);
}
