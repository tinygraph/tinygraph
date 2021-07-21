#ifndef TINYGRAPH_H
#define TINYGRAPH_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __GNUC__ >= 4
  #define TINYGRAPH_API __attribute__((visibility("default")))
  #define TINYGRAPH_WARN_UNUSED __attribute__((warn_unused_result))
#else
  #define TINYGRAPH_API
  #define TINYGRAPH_WARN_UNUSED
#endif

/**
 * The tiny graph: stores vertices and directed edges in a
 * compressed and efficient format making the graph tiny.
 */
typedef struct tinygraph* tinygraph_s;

/**
 * Creates a tiny graph from `n` source vertices in
 * `sources` and `n` target vertices in `edges`.
 *
 * Edges are required to be sorted by `sources`
 * first, and then by `targets`, with consecutive
 * vertex ids starting at zero.
 *
 * The caller is responsible to destruct the
 * returned object with `tinygraph_destruct`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
tinygraph_s tinygraph_construct_from_sorted_edges(
    const uint32_t* sources,
    const uint32_t* targets,
    uint32_t n);

/**
 * Copies `graph` into a new one.
 *
 * The caller is responsible to destruct the
 * returned object with `tinygraph_destruct`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
tinygraph_s tinygraph_copy(tinygraph_s graph);

/**
 * Destructs `graph` releasing resources.
 */
TINYGRAPH_API
void tinygraph_destruct(tinygraph_s graph);

/**
 * Returns true if `graph` is empty.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_is_empty(tinygraph_s graph);

/**
 * Returns the number of vertices in `graph`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_get_num_vertices(tinygraph_s graph);

/**
 * Returns the number of edges in `graph`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_get_num_edges(tinygraph_s graph);

/**
 * Writes the edge range [first, last) for all out
 * edges of vertex `source` into `first` and `last`.
 */
TINYGRAPH_API
void tinygraph_get_edges(
    tinygraph_s graph,
    uint32_t source,
    uint32_t* first,
    uint32_t* last);

/**
 * Returns the edge `e`'s target vertex.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_get_edge_target(tinygraph_s graph, uint32_t e);

/**
 * Returns the number of outgoing edges at vertex `v`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_get_out_degree(tinygraph_s graph, uint32_t v);

/**
 * Writes the vertex `v`'s out edge targets delimited by
 * [first, last) into `first` and `last`.
 *
 * This function is a shortcut for the common use case of
 * - retrieving a vertex' edge `e` with `tinygraph_get_edges()`,
 * - iterating over all targets for `e` with `tinygraph_get_edge_target()`
 */
TINYGRAPH_API
void tinygraph_get_targets(
    tinygraph_s graph,
    const uint32_t **first,
    const uint32_t **last,
    uint32_t v);

/**
 * Returns true if `graph` contains vertex `v`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_has_vertex(tinygraph_s graph, uint32_t v);

/**
 * Returns true if `graph` contains edge `e`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_has_edge(tinygraph_s graph, uint32_t e);

/**
 * Returns true if `graph` contains an edge from `s` to `t`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_has_edge_from_to(tinygraph_s graph, uint32_t s, uint32_t t);

/**
 * Writes the all-pair shortest paths distances into `result`.
 *
 * The caller is responsible for
 * - providing the n^2 sized `weights` matrix, and
 * - providing the n^2 sized `results` matrix
 *
 * The `results` matrix will contain saturated distances for
 * pairs (i,j) at `results[i * tinygraph_num_vertices + j]`.
 *
 */
TINYGRAPH_API
void tinygraph_apsp(tinygraph_s graph, const uint8_t* weights, uint8_t* results);

/**
 * Prints a human readable version of `graph` to stderr.
 */
TINYGRAPH_API
void tinygraph_print(tinygraph_s graph);

/**
 * Returns the total size in bytes `graph` uses.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_size_in_bytes(tinygraph_s graph);


/**
 * Iterates over all vertices in `graph`.
 */
#define TINYGRAPH_FOR_EACH_VERTEX(item, graph) \
  for (uint32_t item = 0; item < tinygraph_get_num_vertices(graph); ++item)

/**
 * Iterates over all edges in `graph`.
 */
#define TINYGRAPH_FOR_EACH_EDGE(item, graph) \
  for (uint32_t item = 0; item < tinygraph_get_num_edges(graph); ++item)


#ifdef __cplusplus
}
#endif

#endif
