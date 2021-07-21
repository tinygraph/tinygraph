#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinygraph.h"
#include "tinygraph-utils.h"
#include "tinygraph-impl.h"


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

  uint32_t min_sources_vert;
  uint32_t max_sources_vert;

  tinygraph_minmax_u32(sources, n, &min_sources_vert, &max_sources_vert);

  uint32_t min_targets_vert;
  uint32_t max_targets_vert;

  tinygraph_minmax_u32(targets, n, &min_targets_vert, &max_targets_vert);

  const uint32_t min_vert = tinygraph_min_u32(min_sources_vert, min_targets_vert);
  const uint32_t max_vert = tinygraph_max_u32(max_sources_vert, max_targets_vert);

  TINYGRAPH_ASSERT(min_vert == 0);
  TINYGRAPH_ASSERT(max_vert >= min_vert);
  TINYGRAPH_ASSERT(max_vert != UINT32_MAX);

  const uint32_t num_verts = max_vert + 1;
  const uint32_t num_edges = n;

  bool ok = tinygraph_reserve(graph, num_verts, num_edges);

  if (!ok) {
    return NULL;
  }

  TINYGRAPH_ASSERT(num_edges > 0);

  memcpy(graph->targets, targets, num_edges * sizeof(uint32_t));

  const uint32_t *it = sources;
  const uint32_t * const last = sources + n;

  graph->offsets[0] = 0;

  for (uint32_t v = 0; v < max_sources_vert + 1; ++v) {
    it = tinygraph_find_if_not_u32(it, last, v);

    const uint32_t offset = it - sources;

    graph->offsets[v + 1] = offset;
  }

  for (uint32_t v = max_sources_vert + 1; v < num_verts; ++v) {
    const uint32_t offset = graph->targets_len;

    graph->offsets[v + 1] = offset;
  }

  return graph;
}


tinygraph_s tinygraph_copy(tinygraph *graph) {
  if (!graph) {
    return graph;
  }

  tinygraph *copy = tinygraph_construct_empty();

  if (!copy) {
    return NULL;
  }

  if (tinygraph_is_empty(graph)) {
    return copy;
  }

  bool ok = tinygraph_reserve(copy,
      tinygraph_get_num_vertices(graph),
      tinygraph_get_num_edges(graph));

  if (!ok) {
    return NULL;
  }

  TINYGRAPH_ASSERT(graph->offsets_len > 0);
  TINYGRAPH_ASSERT(graph->targets_len > 0);

  memcpy(copy->offsets, graph->offsets, graph->offsets_len * sizeof(uint32_t));
  memcpy(copy->targets, graph->targets, graph->targets_len * sizeof(uint32_t));

  copy->offsets_len = graph->offsets_len;
  copy->targets_len = graph->targets_len;

  return copy;
}


void tinygraph_destruct(tinygraph *graph) {
  if (!graph) {
    return;
  }

  TINYGRAPH_ASSERT(graph->offsets);
  TINYGRAPH_ASSERT(graph->targets);

  free(graph->offsets);
  free(graph->targets);

  graph->offsets = NULL;
  graph->targets = NULL;

  graph->offsets_len = 0;
  graph->targets_len = 0;

  free(graph);
}


bool tinygraph_is_empty(tinygraph_s graph) {
  TINYGRAPH_ASSERT(graph);

  return tinygraph_get_num_vertices(graph) == 0;
}


uint32_t tinygraph_get_num_vertices(tinygraph *graph) {
  TINYGRAPH_ASSERT(graph);

  return graph->offsets_len > 1 ? graph->offsets_len - 1 : 0;  // tombstone
}


uint32_t tinygraph_get_num_edges(tinygraph *graph) {
  TINYGRAPH_ASSERT(graph);

  return graph->targets_len;
}


void tinygraph_get_out_edges(
    tinygraph *graph,
    uint32_t source,
    uint32_t* first,
    uint32_t* last)
{
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(tinygraph_has_vertex(graph, source));
  TINYGRAPH_ASSERT(first);
  TINYGRAPH_ASSERT(last);

  *first = graph->offsets[source];
  *last = graph->offsets[source + 1];

  TINYGRAPH_ASSERT(*first <= *last);
}


uint32_t tinygraph_get_edge_target(tinygraph *graph, uint32_t e) {
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(tinygraph_has_edge(graph, e));

  return graph->targets[e];
}


uint32_t tinygraph_get_out_degree(tinygraph *graph, uint32_t v) {
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(tinygraph_has_vertex(graph, v));

  const uint32_t *first = NULL;
  const uint32_t *last = NULL;

  tinygraph_get_targets(graph, &first, &last, v);

  TINYGRAPH_ASSERT(first);
  TINYGRAPH_ASSERT(last);

  TINYGRAPH_ASSERT(first <= last);

  return last - first;
}


void tinygraph_get_targets(
    tinygraph *graph,
    const uint32_t **first,
    const uint32_t **last,
    uint32_t v)
{
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(first);
  TINYGRAPH_ASSERT(last);
  TINYGRAPH_ASSERT(tinygraph_has_vertex(graph, v));

  uint32_t efirst;
  uint32_t elast;

  tinygraph_get_out_edges(graph, v, &efirst, &elast);

  *first = &graph->targets[efirst];
  *last = &graph->targets[elast];
}


bool tinygraph_has_vertex(tinygraph *graph, uint32_t v) {
  TINYGRAPH_ASSERT(graph);

  return v < tinygraph_get_num_vertices(graph);
}


bool tinygraph_has_edge(tinygraph *graph, uint32_t e) {
  TINYGRAPH_ASSERT(graph);

  return e < tinygraph_get_num_edges(graph);
}


bool tinygraph_has_edge_from_to(
    tinygraph *graph,
    uint32_t source,
    uint32_t target)
{
  TINYGRAPH_ASSERT(tinygraph_has_vertex(graph, source));
  TINYGRAPH_ASSERT(tinygraph_has_vertex(graph, target));

  const uint32_t *it, *last;

  tinygraph_get_targets(graph, &it, &last, source);

  return tinygraph_find_if_u32(it, last, target) != last;
}


void tinygraph_apsp(tinygraph *graph, const uint8_t* weights, uint8_t* results) {
  TINYGRAPH_ASSERT(graph);
  TINYGRAPH_ASSERT(weights);
  TINYGRAPH_ASSERT(results);

  const uint64_t num_verts = tinygraph_get_num_vertices(graph);

  for (uint32_t i = 0; i < num_verts; ++i) {
    for (uint32_t j = 0; j < num_verts; ++j) {
      results[i * num_verts + j] = -1;
    }
  }

  TINYGRAPH_FOR_EACH_VERTEX(source, graph) {
    uint32_t efirst, elast;

    tinygraph_get_out_edges(graph, source, &efirst, &elast);

    for (; efirst != elast; ++efirst) {
      const uint32_t edge = efirst;

      const uint32_t target = tinygraph_get_edge_target(graph, edge);

      uint8_t *out = &results[source * num_verts + target];

      *out = tinygraph_min_u32(weights[edge], *out);
    }
  }

  TINYGRAPH_FOR_EACH_VERTEX(v, graph) {
    const uint32_t source = v;
    const uint32_t target = v;

    results[source * num_verts + target] = 0;
  }

  TINYGRAPH_FOR_EACH_VERTEX(k, graph) {
    TINYGRAPH_FOR_EACH_VERTEX(i, graph) {
      TINYGRAPH_FOR_EACH_VERTEX(j, graph) {
        const uint8_t ij = results[i * num_verts + j];
        const uint8_t ik = results[i * num_verts + k];
        const uint8_t kj = results[k * num_verts + j];

        const uint8_t sum = tinygraph_saturated_add_u8(ik, kj);

        if (ij > sum) {
          results[i * num_verts + j] = sum;
        }
      }
    }
  }
}


void tinygraph_print(tinygraph *graph) {
  TINYGRAPH_ASSERT(graph);

  fprintf(stderr, "graph %p with vertices=%ju, edges=%ju of %ju bytes total\n",
      (const void *)graph,
      (uintmax_t)tinygraph_get_num_vertices(graph),
      (uintmax_t)tinygraph_get_num_edges(graph),
      (uintmax_t)tinygraph_size_in_bytes(graph));

  TINYGRAPH_FOR_EACH_VERTEX(s, graph) {
    const uint32_t *it, *last;

    tinygraph_get_targets(graph, &it, &last, s);

    fprintf(stderr, "%ju:", (uintmax_t)s);

    for (; it != last; ++it) {
      const uint32_t t = *it;

      fprintf(stderr, " (%ju -> %ju)", (uintmax_t)s, (uintmax_t)t);
    }

    fprintf(stderr, "\n");
  }

  //tinygraph_print_internal(graph);
}


uint32_t tinygraph_size_in_bytes(tinygraph *graph) {
  if (!graph) {
    return 0;
  }

  return sizeof(tinygraph)
    + sizeof(uint32_t) * graph->offsets_len
    + sizeof(uint32_t) * graph->targets_len;
}
