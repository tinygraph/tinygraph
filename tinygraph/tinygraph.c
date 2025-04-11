#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinygraph.h"
#include "tinygraph-utils.h"
#include "tinygraph-zorder.h"
#include "tinygraph-sort.h"
#include "tinygraph-impl.h"
#include "tinygraph-array.h"
#include "tinygraph-bitset.h"
#include "tinygraph-heap.h"



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

  TINYGRAPH_ASSERT(tinygraph_min_u32(min_sources_node, min_targets_node) == 0);

  const uint32_t max_node = tinygraph_max_u32(max_sources_node, max_targets_node);
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


typedef struct tinygraph_reorder_item {
  uint32_t node;
  uint32_t zval;
} tinygraph_reorder_item;

TINYGRAPH_WARN_UNUSED
static inline int tinygraph_reorder_item_cmp(const void* lhs, const void *rhs) {
  const tinygraph_reorder_item ilhs = *(const tinygraph_reorder_item *)lhs;
  const tinygraph_reorder_item irhs = *(const tinygraph_reorder_item *)rhs;

  if (ilhs.zval < irhs.zval) {
    return -1;
  } else if (ilhs.zval > irhs.zval) {
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
  if (n == 0) {
    return true;
  }

  TINYGRAPH_ASSERT(nodes);
  TINYGRAPH_ASSERT(lngs);
  TINYGRAPH_ASSERT(lats);

  // Reorder graph nodes spatially based on a z-order
  // space filling curve to improve memory locality

  // Note that we don't really need a precise sorting
  // down to every pair of numbers. We could do e.g.
  // with an ordering that stops at e.g. the 4k page
  // size and keeps them unsorted. We can use this
  // as a trade-off between run-time and quality.
  // The second idea is to use radix sort on the
  // z-order keyed nodes.

  tinygraph_reorder_item* items = malloc(n * sizeof(tinygraph_reorder_item));

  if (!items) {
    return false;
  }

  for (uint32_t i = 0; i < n; ++i) {
    items[i] = (tinygraph_reorder_item) {
      .node = nodes[i],
      .zval = tinygraph_zorder_encode32(lngs[nodes[i]], lats[nodes[i]]),
    };
  }

  qsort(items, n, sizeof(tinygraph_reorder_item), tinygraph_reorder_item_cmp);

  for (uint32_t i = 0; i < n; ++i) {
    nodes[i] = items[i].node;
  }

  free(items);

  return true;
}


// The purpose of the dijkstra context is to cache state
// like the distance and parents array, so that we don't
// have to allocate memory for every new s-t search.
typedef struct tinygraph_dijkstra {
  uint32_t s;
  uint32_t t;

  uint32_t* dist;
  uint32_t* parent;
  tinygraph_array_s path;

  const uint16_t* weight;

  tinygraph_const_s graph;
  tinygraph_bitset_s seen;
  tinygraph_heap_s heap;
} tinygraph_dijkstra;


static inline void tinygraph_dijkstra_clear(tinygraph_dijkstra_s ctx) {
  ctx->s = UINT32_MAX;
  ctx->t = UINT32_MAX;

  const uint32_t n = tinygraph_get_num_nodes(ctx->graph);

  for (uint32_t i = 0; i < n; ++i) {
    ctx->dist[i] = UINT32_MAX;
  }

  for (uint32_t i = 0; i < n; ++i) {
    ctx->parent[i] = i;
  }

  tinygraph_bitset_clear(ctx->seen);
  tinygraph_heap_clear(ctx->heap);
}


tinygraph_dijkstra_s tinygraph_dijkstra_construct(tinygraph_const_s graph, const uint16_t* weights) {
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(!tinygraph_is_empty(graph));

  tinygraph_dijkstra *out = malloc(sizeof(tinygraph_dijkstra));

  if (!out) {
    return NULL;
  }

  const uint32_t n = tinygraph_get_num_nodes(graph);

  tinygraph_bitset_s seen = tinygraph_bitset_construct(n);

  if (!seen) {
    free(out);

    return NULL;
  }

  tinygraph_heap_s heap = tinygraph_heap_construct();

  if (!heap) {
    tinygraph_bitset_destruct(seen);
    free(out);

    return NULL;
  }

  uint32_t* dist = malloc(n * sizeof(uint32_t));

  if (!dist) {
    tinygraph_heap_destruct(heap);
    tinygraph_bitset_destruct(seen);
    free(out);

    return NULL;
  }

  uint32_t* parent = malloc(n * sizeof(uint32_t));

  if (!parent) {
    free(dist);
    tinygraph_heap_destruct(heap);
    tinygraph_bitset_destruct(seen);
    free(out);

    return NULL;
  }

  tinygraph_array_s path = tinygraph_array_construct(0);

  if (!path) {
    free(parent);
    free(dist);
    tinygraph_heap_destruct(heap);
    tinygraph_bitset_destruct(seen);
    free(out);

    return NULL;
  }

  *out = (tinygraph_dijkstra){
    .s = UINT32_MAX,
    .t = UINT32_MAX,
    .dist = dist,
    .parent = parent,
    .path = path,
    .weight = weights,
    .graph = graph,
    .seen = seen,
    .heap = heap,
  };

  // Resets the internal state e.g. sets
  // self-loops for the parents array
  tinygraph_dijkstra_clear(out);

  return out;
}


void tinygraph_dijkstra_destruct(tinygraph_dijkstra * const ctx) {
  if (!ctx) {
    return;
  }

  ctx->s = UINT32_MAX;
  ctx->t = UINT32_MAX;

  free(ctx->dist);
  free(ctx->parent);

  tinygraph_array_destruct(ctx->path);
  tinygraph_heap_destruct(ctx->heap);
  tinygraph_bitset_destruct(ctx->seen);

  free(ctx);
}


static inline uint32_t tinygraph_saturated_add_u32(uint32_t a, uint32_t b) {
  const uint32_t sum = a + b;

  if (sum < a) {
    return UINT32_MAX;
  }

  if (sum < b) {
    return UINT32_MAX;
  }

  return sum;
}


bool tinygraph_dijkstra_shortest_path(
    tinygraph_dijkstra_s ctx,
    uint32_t s,
    uint32_t t
) {
  TINYGRAPH_ASSERT(ctx);
  TINYGRAPH_ASSERT(tinygraph_has_node(ctx->graph, s));
  TINYGRAPH_ASSERT(tinygraph_has_node(ctx->graph, t));
  TINYGRAPH_ASSERT(ctx->heap);
  TINYGRAPH_ASSERT(ctx->seen);
  TINYGRAPH_ASSERT(ctx->dist);
  TINYGRAPH_ASSERT(ctx->parent);
  TINYGRAPH_ASSERT(ctx->weight);

  if (s == t) {
    ctx->s = s;
    ctx->t = t;

    return true;
  }

  // Invalidate the retrieved path cache
  // when source or target nodes change
  if (s != ctx->s || t != ctx->t) {
    tinygraph_array_clear(ctx->path);
  }

  // If the source node stays the same across searches
  // and only the target node changes, we can re-use
  // the internal state. Otherwise clear it and start
  // the search from scratch with the new source node.
  if (s != ctx->s) {
    tinygraph_dijkstra_clear(ctx);

    ctx->s = s;
    ctx->t = t;
    ctx->dist[s] = 0;

    if (!tinygraph_heap_push(ctx->heap, s, 0)) {
      tinygraph_dijkstra_clear(ctx);
      tinygraph_array_clear(ctx->path);
      return false;
    }
  } else {
    // The source node is the same and we have explored t already
    // in a previous search, this means we're done here
    ctx->t = t;
    ctx->dist[s] = 0;

    if (tinygraph_bitset_get_at(ctx->seen, t)) {
      return true;
    }
  }

  while (!tinygraph_heap_is_empty(ctx->heap)) {
    const uint32_t u = tinygraph_heap_pop(ctx->heap);

    if (tinygraph_bitset_get_at(ctx->seen, u)) {
      continue;
    } else {
      tinygraph_bitset_set_at(ctx->seen, u);
    }

    const uint32_t distu = ctx->dist[u];

    // The following is a bit different to the classical Dijkstra
    // implementation: even if u is the target node, we still want
    // to process its neighbors. This makes sure two cached sub-
    // sequent queries such as (0, 2), (0, 3) with path 0->2->3
    // will find the node 3. The down-side is that if users don't
    // run repeated s-t queries with the same s, we will do extra
    // work for the very last node t.

    uint32_t it, last;

    tinygraph_get_out_edges(ctx->graph, u, &it, &last);

    for (; it != last; ++it) {
      const uint32_t v = tinygraph_get_edge_target(ctx->graph, it);

      const uint32_t alt = tinygraph_saturated_add_u32(distu, ctx->weight[it]);

      if (alt < ctx->dist[v]) {
        ctx->dist[v] = alt;
        ctx->parent[v] = u;

        if (!tinygraph_heap_push(ctx->heap, v, alt)) {
          tinygraph_dijkstra_clear(ctx);
          tinygraph_array_clear(ctx->path);
          return false;
        }
      }
    }

    // Now that we have worked through u's neighbors
    // if it turns out u==t then u was our target node
    // and we're done here. In subsequent s-t calls with
    // the same fixed s we will re-use the search state
    if (u == t) {
      return true;
    }

    // If the smallest distance on the heap is at max and we
    // haven't reached our target node above, it means we are
    // on a path that has saturated the uint32_t data type.
    // In this case we do a saturated addition but it also
    // means we can not longer distinguish shortest paths
    // from each other and therefore there is no path found.
    if (distu == UINT32_MAX) {
      tinygraph_dijkstra_clear(ctx);
      tinygraph_array_clear(ctx->path);
      return false;
    }
  }

  return false;
}


uint32_t tinygraph_dijkstra_get_distance(tinygraph_dijkstra_s ctx) {
  TINYGRAPH_ASSERT(ctx);
  TINYGRAPH_ASSERT(ctx->dist);

  if (ctx->s == ctx->t) {
    return 0;
  }

  return ctx->dist[ctx->t];
}


bool tinygraph_dijkstra_get_path(
    tinygraph_dijkstra_s ctx,
    const uint32_t **first,
    const uint32_t **last
) {
  TINYGRAPH_ASSERT(ctx);
  TINYGRAPH_ASSERT(first);
  TINYGRAPH_ASSERT(last);
  TINYGRAPH_ASSERT(ctx->parent);
  TINYGRAPH_ASSERT(ctx->path);

  if (ctx->s == ctx->t) {
    *first = NULL;
    *last = NULL;

    return true;
  }

  if (!tinygraph_array_is_empty(ctx->path)) {
    *first = tinygraph_array_get_data(ctx->path);
    *last = *first + tinygraph_array_get_size(ctx->path);

    return true;
  }

  // Below in case we fail e.g. pushing a node onto
  // the array for path retrieval, we need to recover
  // and clear the state. Resizing down never fails
  // meaning we clear the path so that the user can
  // continue without unrecoverable ctx corruption.

  uint32_t p = ctx->t;

  while (p != ctx->parent[p]) {
    if (!tinygraph_array_push(ctx->path, p)) {
      tinygraph_array_clear(ctx->path);
      return false;
    }

    p = ctx->parent[p];
  }

  if (!tinygraph_array_push(ctx->path, ctx->s)) {
    tinygraph_array_clear(ctx->path);
    return false;
  }

  tinygraph_array_reverse(ctx->path);

  *first = tinygraph_array_get_data(ctx->path);
  *last = *first + tinygraph_array_get_size(ctx->path);

  return true;
}
