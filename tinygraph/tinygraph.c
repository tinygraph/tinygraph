#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinygraph.h"
#include "tinygraph-utils.h"
#include "tinygraph-zorder.h"
#include "tinygraph-sort.h"
#include "tinygraph-impl.h"
#include "tinygraph-bitset.h"
#include "tinygraph-stack.h"
#include "tinygraph-queue.h"



tinygraph_s tinygraph_construct_from_sorted_edges(
    const uint32_t* sources,
    const uint32_t* targets,
    uint32_t n)
{
  TINYGRAPH_ASSERT(n != UINT32_MAX);
  TINYGRAPH_ASSERT(tinygraph_is_sorted_sources_targets(sources, targets, n));

  tinygraph *graph = tinygraph_construct_empty();

  if (!graph) {
    return NULL;
  }

  if (n == 0) {
    return graph;
  }

  TINYGRAPH_ASSERT(sources);
  TINYGRAPH_ASSERT(targets);

  uint32_t min_sources_node;
  uint32_t max_sources_node;

  tinygraph_minmax_u32(sources, n, &min_sources_node, &max_sources_node);

  uint32_t min_targets_node;
  uint32_t max_targets_node;

  tinygraph_minmax_u32(targets, n, &min_targets_node, &max_targets_node);

  const uint32_t min_node = tinygraph_min_u32(min_sources_node, min_targets_node);
  const uint32_t max_node = tinygraph_max_u32(max_sources_node, max_targets_node);

  TINYGRAPH_ASSERT(min_node == 0);
  TINYGRAPH_ASSERT(max_node >= min_node);
  TINYGRAPH_ASSERT(max_node != UINT32_MAX);

  const uint32_t num_nodes = max_node + 1;
  const uint32_t num_edges = n;

  bool ok = tinygraph_reserve(graph, num_nodes, num_edges);

  if (!ok) {
    tinygraph_destruct(graph);

    return NULL;
  }

  TINYGRAPH_ASSERT(num_edges > 0);

  TINYGRAPH_ASSERT(graph->targets);
  TINYGRAPH_ASSERT(targets);

  memcpy(graph->targets, targets, num_edges * sizeof(uint32_t));

  const uint32_t *it = sources;
  const uint32_t * const last = sources + n;

  graph->offsets[0] = 0;

  for (uint32_t v = 0; v < max_sources_node + 1; ++v) {
    it = tinygraph_find_if_not_u32(it, last, v);

    const uint32_t offset = it - sources;

    graph->offsets[v + 1] = offset;
  }

  const uint32_t tombstone = graph->targets_len;

  for (uint32_t v = max_sources_node + 1; v < num_nodes; ++v) {
    graph->offsets[v + 1] = tombstone;
  }

  return graph;
}


tinygraph_s tinygraph_copy_reversed(const tinygraph * const graph) {
  TINYGRAPH_ASSERT(graph);

  if (tinygraph_is_empty(graph)) {
    return tinygraph_construct_empty();
  }

  const uint32_t n = tinygraph_get_num_edges(graph);

  uint32_t* sources = calloc(n, sizeof(uint32_t));
  uint32_t* targets = calloc(n, sizeof(uint32_t));

  if (!sources || !targets) {
    free(sources);
    free(targets);

    return NULL;
  }

  uint32_t i = 0;

  TINYGRAPH_FOR_EACH_NODE(s, graph) {
    const uint32_t *it, *last;

    tinygraph_get_neighbors(graph, &it, &last, s);

    for (; it != last; ++it) {
      const uint32_t t = *it;

      sources[i] = t;
      targets[i] = s;

      i += 1;
    }
  }

  TINYGRAPH_ASSERT(n == i);

  tinygraph *copy = tinygraph_construct_from_unsorted_edges(sources, targets, n);

  free(sources);
  free(targets);

  return copy;
}


tinygraph_s tinygraph_copy(const tinygraph * const graph) {
  TINYGRAPH_ASSERT(graph);

  tinygraph *copy = tinygraph_construct_empty();

  if (!copy) {
    return NULL;
  }

  if (tinygraph_is_empty(graph)) {
    return copy;
  }

  bool ok = tinygraph_reserve(copy,
      tinygraph_get_num_nodes(graph),
      tinygraph_get_num_edges(graph));

  if (!ok) {
    tinygraph_destruct(copy);

    return NULL;
  }

  TINYGRAPH_ASSERT(graph->offsets_len > 0);
  TINYGRAPH_ASSERT(graph->targets_len > 0);

  TINYGRAPH_ASSERT(copy->offsets);
  TINYGRAPH_ASSERT(graph->offsets);

  TINYGRAPH_ASSERT(copy->targets);
  TINYGRAPH_ASSERT(graph->targets);

  memcpy(copy->offsets, graph->offsets, graph->offsets_len * sizeof(uint32_t));
  memcpy(copy->targets, graph->targets, graph->targets_len * sizeof(uint32_t));

  copy->offsets_len = graph->offsets_len;
  copy->targets_len = graph->targets_len;

  return copy;
}


tinygraph_s tinygraph_construct_from_unsorted_edges(
    const uint32_t* sources,
    const uint32_t* targets,
    uint32_t n)
{
  TINYGRAPH_ASSERT(n != UINT32_MAX);

  TINYGRAPH_ASSERT(sources);
  TINYGRAPH_ASSERT(targets);

  uint32_t* sorted_sources = calloc(n, sizeof(uint32_t));
  uint32_t* sorted_targets = calloc(n, sizeof(uint32_t));

  if (!sorted_sources || !sorted_targets) {
    free(sorted_sources);
    free(sorted_targets);

    return NULL;
  }

  memcpy(sorted_sources, sources, n * sizeof(uint32_t));
  memcpy(sorted_targets, targets, n * sizeof(uint32_t));

  if (!tinygraph_sort_sources_targets(sorted_sources, sorted_targets, n)) {
    free(sorted_sources);
    free(sorted_targets);

    return NULL;
  }

  tinygraph *graph = tinygraph_construct_from_sorted_edges(
      sorted_sources, sorted_targets, n);

  free(sorted_sources);
  free(sorted_targets);

  return graph;
}


void tinygraph_destruct(tinygraph * const graph) {
  if (!graph) {
    return;
  }

  TINYGRAPH_ASSERT(graph->offsets || graph->offsets_len == 0);
  TINYGRAPH_ASSERT(graph->targets || graph->targets_len == 0);

  free(graph->offsets);
  free(graph->targets);

  graph->offsets = NULL;
  graph->targets = NULL;

  graph->offsets_len = 0;
  graph->targets_len = 0;

  free(graph);
}


bool tinygraph_is_empty(const tinygraph * const graph) {
  TINYGRAPH_ASSERT(graph);

  return tinygraph_get_num_nodes(graph) == 0;
}


uint32_t tinygraph_get_num_nodes(const tinygraph * const graph) {
  TINYGRAPH_ASSERT(graph);

  return graph->offsets_len > 1 ? graph->offsets_len - 1 : 0;  // tombstone
}


uint32_t tinygraph_get_num_edges(const tinygraph * const graph) {
  TINYGRAPH_ASSERT(graph);

  return graph->targets_len;
}


void tinygraph_get_out_edges(
    const tinygraph * const graph,
    uint32_t source,
    uint32_t* first,
    uint32_t* last)
{
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(tinygraph_has_node(graph, source));
  TINYGRAPH_ASSERT(first);
  TINYGRAPH_ASSERT(last);

  *first = graph->offsets[source];
  *last = graph->offsets[source + 1];

  // TODO: benchmark if prefetching edge range has an impact
  // TINYGRAPH_PREFETCH(graph->targets[graph->offsets[source]]);

  TINYGRAPH_ASSERT(*first <= *last);
}


uint32_t tinygraph_get_edge_target(const tinygraph * const graph, uint32_t e) {
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(tinygraph_has_edge(graph, e));

  return graph->targets[e];
}


uint32_t tinygraph_get_out_degree(const tinygraph * const graph, uint32_t v) {
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(tinygraph_has_node(graph, v));

  uint32_t efirst;
  uint32_t elast;

  tinygraph_get_out_edges(graph, v, &efirst, &elast);

  TINYGRAPH_ASSERT(efirst <= elast);

  return elast - efirst;
}


void tinygraph_get_neighbors(
    const tinygraph * const graph,
    const uint32_t **first,
    const uint32_t **last,
    uint32_t v)
{
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(first);
  TINYGRAPH_ASSERT(last);
  TINYGRAPH_ASSERT(tinygraph_has_node(graph, v));

  uint32_t efirst;
  uint32_t elast;

  tinygraph_get_out_edges(graph, v, &efirst, &elast);

  *first = &graph->targets[efirst];
  *last = &graph->targets[elast];

  // TODO: benchmark if prefetching edge range has an impact
  // TINYGRAPH_PREFETCH(graph->targets[graph->offsets[source]]);
}


bool tinygraph_has_node(const tinygraph * const graph, uint32_t v) {
  TINYGRAPH_ASSERT(graph);

  return v < tinygraph_get_num_nodes(graph);
}


bool tinygraph_has_edge(const tinygraph * const graph, uint32_t e) {
  TINYGRAPH_ASSERT(graph);

  return e < tinygraph_get_num_edges(graph);
}


bool tinygraph_has_edge_from_to(
    const tinygraph * const graph,
    uint32_t source,
    uint32_t target)
{
  TINYGRAPH_ASSERT(tinygraph_has_node(graph, source));
  TINYGRAPH_ASSERT(tinygraph_has_node(graph, target));

  const uint32_t *it, *last;

  tinygraph_get_neighbors(graph, &it, &last, source);

  return tinygraph_find_if_u32(it, last, target) != last;

  // TODO: benchmark if prefetching edge range has an impact
  // TINYGRAPH_PREFETCH(graph->targets[graph->offsets[source]]);
}


void tinygraph_apsp(const tinygraph * const graph, const uint8_t* weights, uint8_t* results) {
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(weights);
  TINYGRAPH_ASSERT(results);

  const uint64_t num_nodes = tinygraph_get_num_nodes(graph);

  for (uint32_t i = 0; i < num_nodes; ++i) {
    for (uint32_t j = 0; j < num_nodes; ++j) {
      results[i * num_nodes + j] = -1;
    }
  }

  TINYGRAPH_FOR_EACH_NODE(source, graph) {
    uint32_t efirst, elast;

    tinygraph_get_out_edges(graph, source, &efirst, &elast);

    for (; efirst != elast; ++efirst) {
      const uint32_t edge = efirst;

      const uint32_t target = tinygraph_get_edge_target(graph, edge);

      uint8_t *out = &results[source * num_nodes + target];

      *out = tinygraph_min_u32(weights[edge], *out);
    }
  }

  TINYGRAPH_FOR_EACH_NODE(v, graph) {
    const uint32_t source = v;
    const uint32_t target = v;

    results[source * num_nodes + target] = 0;
  }

  TINYGRAPH_FOR_EACH_NODE(k, graph) {
    TINYGRAPH_FOR_EACH_NODE(i, graph) {
      TINYGRAPH_FOR_EACH_NODE(j, graph) {
        const uint8_t ij = results[i * num_nodes + j];
        const uint8_t ik = results[i * num_nodes + k];
        const uint8_t kj = results[k * num_nodes + j];

        const uint8_t sum = tinygraph_saturated_add_u8(ik, kj);

        if (ij > sum) {
          results[i * num_nodes + j] = sum;
        }
      }
    }
  }
}


void tinygraph_print(const tinygraph * const graph) {
  TINYGRAPH_ASSERT(graph);

  fprintf(stderr, "graph %p with nodes=%ju, edges=%ju of %ju bytes total\n",
      (const void *)graph,
      (uintmax_t)tinygraph_get_num_nodes(graph),
      (uintmax_t)tinygraph_get_num_edges(graph),
      (uintmax_t)tinygraph_size_in_bytes(graph));

  TINYGRAPH_FOR_EACH_NODE(s, graph) {
    const uint32_t *it, *last;

    tinygraph_get_neighbors(graph, &it, &last, s);

    fprintf(stderr, "%ju:", (uintmax_t)s);

    for (; it != last; ++it) {
      const uint32_t t = *it;

      fprintf(stderr, " (%ju -> %ju)", (uintmax_t)s, (uintmax_t)t);
    }

    fprintf(stderr, "\n");
  }

  //tinygraph_print_internal(graph);
}


uint32_t tinygraph_size_in_bytes(const tinygraph * const graph) {
  TINYGRAPH_ASSERT(graph);

  return sizeof(tinygraph)
    + sizeof(uint32_t) * graph->offsets_len
    + sizeof(uint32_t) * graph->targets_len;
}


typedef struct tinygraph_reorder_ctx {
  const uint16_t* lngs;
  const uint16_t* lats;
} tinygraph_reorder_ctx;

TINYGRAPH_WARN_UNUSED
static inline int32_t tinygraph_reorder_cmp(
    const uint32_t* lhs,
    const uint32_t* rhs,
    void* arg)
{
  const tinygraph_reorder_ctx* ctx = (const tinygraph_reorder_ctx*)arg;

  const uint32_t lhs_idx = *lhs;
  const uint32_t rhs_idx = *rhs;

  const uint32_t lhs_z = tinygraph_zorder_encode32(ctx->lngs[lhs_idx], ctx->lats[lhs_idx]);
  const uint32_t rhs_z = tinygraph_zorder_encode32(ctx->lngs[rhs_idx], ctx->lats[rhs_idx]);

  if (lhs_z < rhs_z) {
    return -1;
  } else if (lhs_z > rhs_z) {
    return +1;
  } else {
    return 0;
  }
}

bool tinygraph_reorder(
    uint32_t* nodes,
    const uint16_t* lngs,
    const uint16_t* lats,
    uint32_t n)
{
  // Reorder graph nodes spatially based on a z-order
  // space filling curve to improve memory locality

  // Note that we don't really need a precise sorting
  // down to every pair of numbers. We could do e.g.
  // with an ordering that stops at e.g. the 4k page
  // size and keeps them unsorted. We can use this
  // as a trade-off between run-time and quality.
  // The second idea is to use radix sort on the
  // z-order keyed nodes.

  tinygraph_sort_u32(nodes, n, tinygraph_reorder_cmp,
      &(tinygraph_reorder_ctx){ .lngs = lngs, .lats = lats });

  // In case we want to change the sorting impl. and need
  // to be able to communicate to users failure e.g. in
  // allocation. For now this is always returning true.
  return true;
}

typedef struct tinygraph_dfs {
  tinygraph_const_s graph;
  tinygraph_bitset_s seen;
  tinygraph_stack_s stack;
} tinygraph_dfs;

tinygraph_dfs_s tinygraph_dfs_construct(tinygraph_const_s graph) {
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(!tinygraph_is_empty(graph));

  tinygraph_dfs *out = malloc(sizeof(tinygraph_dfs));

  if (!out) {
    return NULL;
  }

  tinygraph_bitset_s seen = tinygraph_bitset_construct(
      tinygraph_get_num_nodes(graph));

  if (!seen) {
    free(out);

    return NULL;
  }

  tinygraph_stack_s stack = tinygraph_stack_construct();

  if (!stack) {
    free(out);
    tinygraph_bitset_destruct(seen);

    return NULL;
  }

  *out = (tinygraph_dfs){
    .graph = graph,
    .seen = seen,
    .stack = stack,
  };

  return out;
}

void tinygraph_dfs_destruct(tinygraph_dfs * const ctx) {
  if (!ctx) {
    return;
  }

  TINYGRAPH_ASSERT(ctx->seen);
  TINYGRAPH_ASSERT(ctx->stack);

  tinygraph_bitset_destruct(ctx->seen);
  tinygraph_stack_destruct(ctx->stack);

  free(ctx);
}

void tinygraph_dfs_clear(tinygraph_dfs_s ctx) {
  TINYGRAPH_ASSERT(ctx->seen);
  TINYGRAPH_ASSERT(ctx->stack);

  tinygraph_bitset_clear(ctx->seen);
  tinygraph_stack_clear(ctx->stack);
}

bool tinygraph_dfs_set_start(tinygraph_dfs_s ctx, uint32_t v) {
  TINYGRAPH_ASSERT(ctx->seen);
  TINYGRAPH_ASSERT(ctx->stack);
  TINYGRAPH_ASSERT(tinygraph_stack_is_empty(ctx->stack));
  TINYGRAPH_ASSERT(tinygraph_has_node(ctx->graph, v));

  tinygraph_bitset_set_at(ctx->seen, v);

  return tinygraph_stack_push(ctx->stack, v);
}

bool tinygraph_dfs_is_done(tinygraph_dfs_const_s ctx) {
  TINYGRAPH_ASSERT(ctx->seen);
  TINYGRAPH_ASSERT(ctx->stack);

  return tinygraph_stack_is_empty(ctx->stack);
}

bool tinygraph_dfs_step(tinygraph_dfs_s ctx, uint32_t* v) {
  TINYGRAPH_ASSERT(ctx->seen);
  TINYGRAPH_ASSERT(ctx->stack);
  TINYGRAPH_ASSERT(ctx->graph);

  TINYGRAPH_ASSERT(!tinygraph_stack_is_empty(ctx->stack));

  const uint32_t s = tinygraph_stack_pop(ctx->stack);

  *v = s;

  const uint32_t *it, *last;

  tinygraph_get_neighbors(ctx->graph, &it, &last, s);

  for (; it != last; ++it) {
    const uint32_t t = *it;

    if (!tinygraph_bitset_get_at(ctx->seen, t)) {
      const bool ok = tinygraph_stack_push(ctx->stack, t);

      if (!ok) {
        return false;
      }

      tinygraph_bitset_set_at(ctx->seen, t);
    }
  }

  return true;
}


typedef struct tinygraph_bfs {
  tinygraph_const_s graph;
  tinygraph_bitset_s seen;
  tinygraph_queue_s queue;
} tinygraph_bfs;

tinygraph_bfs_s tinygraph_bfs_construct(tinygraph_const_s graph) {
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(!tinygraph_is_empty(graph));

  tinygraph_bfs *out = malloc(sizeof(tinygraph_bfs));

  if (!out) {
    return NULL;
  }

  tinygraph_bitset_s seen = tinygraph_bitset_construct(
      tinygraph_get_num_nodes(graph));

  if (!seen) {
    free(out);

    return NULL;
  }

  tinygraph_queue_s queue = tinygraph_queue_construct();

  if (!queue) {
    free(out);
    tinygraph_bitset_destruct(seen);

    return NULL;
  }

  *out = (tinygraph_bfs){
    .graph = graph,
    .seen = seen,
    .queue = queue,
  };

  return out;
}

void tinygraph_bfs_destruct(tinygraph_bfs * const ctx) {
  if (!ctx) {
    return;
  }

  TINYGRAPH_ASSERT(ctx->seen);
  TINYGRAPH_ASSERT(ctx->queue);

  tinygraph_bitset_destruct(ctx->seen);
  tinygraph_queue_destruct(ctx->queue);

  free(ctx);
}

void tinygraph_bfs_clear(tinygraph_bfs_s ctx) {
  TINYGRAPH_ASSERT(ctx->seen);
  TINYGRAPH_ASSERT(ctx->queue);

  tinygraph_bitset_clear(ctx->seen);
  tinygraph_queue_clear(ctx->queue);
}

bool tinygraph_bfs_set_start(tinygraph_bfs_s ctx, uint32_t v) {
  TINYGRAPH_ASSERT(ctx->seen);
  TINYGRAPH_ASSERT(ctx->queue);
  TINYGRAPH_ASSERT(tinygraph_queue_is_empty(ctx->queue));
  TINYGRAPH_ASSERT(tinygraph_has_node(ctx->graph, v));

  tinygraph_bitset_set_at(ctx->seen, v);

  return tinygraph_queue_push(ctx->queue, v);
}

bool tinygraph_bfs_is_done(tinygraph_bfs_const_s ctx) {
  TINYGRAPH_ASSERT(ctx->seen);
  TINYGRAPH_ASSERT(ctx->queue);

  return tinygraph_queue_is_empty(ctx->queue);
}

bool tinygraph_bfs_step(tinygraph_bfs_s ctx, uint32_t* v) {
  TINYGRAPH_ASSERT(ctx->seen);
  TINYGRAPH_ASSERT(ctx->queue);
  TINYGRAPH_ASSERT(ctx->graph);

  TINYGRAPH_ASSERT(!tinygraph_queue_is_empty(ctx->queue));

  const uint32_t s = tinygraph_queue_pop(ctx->queue);

  *v = s;

  const uint32_t *it, *last;

  tinygraph_get_neighbors(ctx->graph, &it, &last, s);

  for (; it != last; ++it) {
    const uint32_t t = *it;

    if (!tinygraph_bitset_get_at(ctx->seen, t)) {
      const bool ok = tinygraph_queue_push(ctx->queue, t);

      if (!ok) {
        return false;
      }

      tinygraph_bitset_set_at(ctx->seen, t);
    }
  }

  return true;
}
