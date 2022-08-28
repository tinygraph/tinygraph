#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "tinygraph-utils.h"
#include "tinygraph-impl.h"


typedef struct tinygraph_edge {
  uint32_t source;
  uint32_t target;
} tinygraph_edge;

static inline int tinygraph_edge_comp(const void* lhs, const void *rhs) {
  const tinygraph_edge elhs = *(const tinygraph_edge *)lhs;
  const tinygraph_edge erhs = *(const tinygraph_edge *)rhs;

  if (elhs.source < erhs.source) {
    return -1;
  } else if (elhs.source > erhs.source) {
    return 1;
  } else {
    if (elhs.target < erhs.target) {
      return -1;
    } else if (elhs.target > erhs.target) {
      return 1;
    } else {
      return 0;
    }
  }
}


uint8_t tinygraph_saturated_add_u8(uint8_t a, uint8_t b) {
  return (uint8_t)(a + b) < a ? -1 : (a + b);
}


uint32_t tinygraph_max_u32(uint32_t x, uint32_t y) {
  return x > y ? x : y;
}


uint32_t tinygraph_min_u32(uint32_t x, uint32_t y) {
  return x < y ? x : y;
}


void tinygraph_minmax_u32(
    const uint32_t *data,
    uint32_t n,
    uint32_t *min,
    uint32_t *max)
{
  TINYGRAPH_ASSERT(data);
  TINYGRAPH_ASSERT(n > 0);
  TINYGRAPH_ASSERT(min);
  TINYGRAPH_ASSERT(max);

  uint32_t min_ = UINT32_MAX;
  uint32_t max_ = 0;

  for (uint32_t i = 0; i < n; ++i) {
    const uint32_t val = data[i];

    if (val > max_) {
      max_ = val;
    }

    if (val < min_) {
      min_ = val;
    }
  }

  *min = min_;
  *max = max_;
}


const uint32_t* tinygraph_find_if_u32(
    const uint32_t* first,
    const uint32_t* last,
    uint32_t value)
{
  TINYGRAPH_ASSERT(first);
  TINYGRAPH_ASSERT(last);

  for (; first != last; ++first) {
    if (*first == value) {
      break;
    }
  }

  return first;
}


const uint32_t* tinygraph_find_if_not_u32(
    const uint32_t* first,
    const uint32_t* last,
    uint32_t value)
{
  TINYGRAPH_ASSERT(first);
  TINYGRAPH_ASSERT(last);

  for (; first != last; ++first) {
    if (*first != value) {
      break;
    }
  }

  return first;
}


const uint32_t* tinygraph_binary_search_u32(
    const uint32_t* first,
    const uint32_t* last,
    uint32_t value)
{
  TINYGRAPH_ASSERT(first);
  TINYGRAPH_ASSERT(last);

  const uint32_t *it = first;

  uint32_t step = 0;
  uint32_t n = last - first;

  while (n > 0) {
    it = first;
    step = n / 2;

    it = it + step;

    if (*it < value) {
      first = ++it;
      n -= step + 1;
    } else {
      n = step;
    }
  }

  return first;
}


const uint32_t* tinygraph_exponential_search_u32(
    const uint32_t* first,
    const uint32_t* last,
    uint32_t value)
{
  TINYGRAPH_ASSERT(first);
  TINYGRAPH_ASSERT(last);

  uint32_t n = last - first;

  if (n < 1) {
    return last;
  }

  uint32_t upper = 1;

  while (upper < n && *(first + upper) < value) {
    upper *= 2;
  }

  const uint32_t *bfirst = first + (upper / 2);
  const uint32_t *blast = first + (upper + 1);

  blast = blast > last ? last : blast;

  return tinygraph_binary_search_u32(bfirst, blast, value);
}


// TODO: tinygraph_interpolating_search for our sorted dense offset vectors


void tinygraph_iota_u32(uint32_t* data, uint32_t n, uint32_t value) {
  for (const uint32_t * const last = data + n; data != last; ++data) {
    *data = value;
    value += 1;
  }
}


bool tinygraph_is_sorted_u32(const uint32_t *data, uint32_t n) {
  TINYGRAPH_ASSERT(data);

  if (n < 2) {
    return true;
  }

  for (uint32_t i = 1; i < n; ++i) {
    if (data[i] < data[i - 1]) {
      return false;
    }
  }

  return true;
}


bool tinygraph_is_sorted_sources_targets(
    const uint32_t *sources,
    const uint32_t *targets,
    uint32_t n)
{
  if (n < 2) {
    return true;
  }

  TINYGRAPH_ASSERT(sources);
  TINYGRAPH_ASSERT(targets);

  if (!tinygraph_is_sorted_u32(sources, n)) {
    return false;
  }

  const uint32_t *it = sources;
  const uint32_t * const last = sources + n;

  while (it != last) {
    const uint32_t *skip = tinygraph_find_if_not_u32(it, last, *it);

    const uint32_t *tfirst = targets + (it - sources);
    const uint32_t *tlast = tfirst + (skip - it);

    TINYGRAPH_ASSERT(tfirst <= targets + n);
    TINYGRAPH_ASSERT(tlast <= targets + n);

    if (!tinygraph_is_sorted_u32(tfirst, tlast - tfirst)) {
      return false;
    }

    it = skip;
  }

  return true;
}


bool tinygraph_sort_sources_targets(
    uint32_t *sources,
    uint32_t *targets,
    uint32_t n) {

  if (n < 2) {
    return true;
  }

  TINYGRAPH_ASSERT(sources);
  TINYGRAPH_ASSERT(targets);

  tinygraph_edge *edges = calloc(n, sizeof(tinygraph_edge));

  if (!edges) {
    return false;
  }

  for (uint32_t i = 0; i < n; ++i) {
    edges[i] = (tinygraph_edge) {
      .source = sources[i],
      .target = targets[i],
    };
  }

  qsort(edges, n, sizeof(tinygraph_edge), tinygraph_edge_comp);

  for (uint32_t i = 0; i < n; ++i) {
    sources[i] = edges[i].source;
    targets[i] = edges[i].target;
  }

  free(edges);

  return true;
}


tinygraph* tinygraph_construct_empty() {
  tinygraph *out = malloc(sizeof(tinygraph));

  if (!out) {
    return NULL;
  }

  *out = (tinygraph){
    .offsets = NULL,
    .targets = NULL,
    .offsets_len = 0,
    .targets_len = 0,
  };

  return out;
}


bool tinygraph_reserve(
    tinygraph *graph,
    uint32_t num_nodes,
    uint32_t num_edges)
{
  TINYGRAPH_ASSERT(graph);

  TINYGRAPH_ASSERT(num_nodes != UINT32_MAX);

  TINYGRAPH_ASSERT(!graph->offsets);
  TINYGRAPH_ASSERT(!graph->targets);

  TINYGRAPH_ASSERT(graph->offsets_len == 0);
  TINYGRAPH_ASSERT(graph->targets_len == 0);

  uint32_t *offsets = calloc(num_nodes + 1, sizeof(uint32_t));
  uint32_t *targets = calloc(num_edges, sizeof(uint32_t));

  if (!offsets || !targets) {
    free(offsets);
    free(targets);

    return false;
  }

  *graph = (tinygraph){
    .offsets = offsets,
    .targets = targets,
    .offsets_len = num_nodes + 1,  // tombstone for: E[V[v]], E[V[v+1]]
    .targets_len = num_edges,
  };

  return true;
}


void tinygraph_print_internal(tinygraph *graph) {
  TINYGRAPH_ASSERT(graph);

  fprintf(stderr, "graph internals\n");
  fprintf(stderr, "graph->offsets:");

  for (uint32_t i = 0; i < graph->offsets_len; ++i) {
    fprintf(stderr, " %ju", (uintmax_t)graph->offsets[i]);
  }

  fprintf(stderr, "\n");
  fprintf(stderr, "graph->targets:");

  for (uint32_t i = 0; i < graph->targets_len; ++i) {
    fprintf(stderr, " %ju", (uintmax_t)graph->targets[i]);
  }

  fprintf(stderr, "\n");
}


uint8_t tinygraph_requires_num_bytes_u32(uint32_t value) {

  if (value < (1 << 7)) return 1;
  else if (value < (1 << 14)) return 2;
  else if (value < (1 << 21)) return 3;
  else if (value < (1 << 28)) return 4;
  else return 5;
}


uint32_t tinygraph_bytes_wasted(tinygraph *graph) {
  TINYGRAPH_ASSERT(graph);

  uint32_t actual = 0;
  uint32_t needed = 0;

  actual += sizeof(uint32_t) * graph->offsets_len;
  actual += sizeof(uint32_t) * graph->targets_len;

  for (uint32_t i = 0; i < graph->offsets_len; ++i) {
    needed += tinygraph_requires_num_bytes_u32(graph->offsets[i]);
  }

  for (uint32_t i = 0; i < graph->targets_len; ++i) {
    needed += tinygraph_requires_num_bytes_u32(graph->targets[i]);
  }

  TINYGRAPH_ASSERT(actual >= needed);

  return actual - needed;
}
