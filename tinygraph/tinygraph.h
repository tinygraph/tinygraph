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
 * The tiny graph: stores nodes and directed edges in a
 * compressed and efficient format making the graph tiny.
 */
typedef struct tinygraph* tinygraph_s;
typedef const struct tinygraph* tinygraph_const_s;

/**
 * Creates a tiny graph from `n` source nodes in
 * `sources` and `n` target nodes in `targets`.
 *
 * Edges are required to be sorted by `sources`
 * first, and then by `targets`, with consecutive
 * node ids starting at zero.
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
 * Creates a tiny graph from `n` source nodes in
 * `sources` and `n` target nodes in `targets`.
 *
 * Edges are not required to be sorted, but
 * node ids are required to start at zero and
 * are consecutive.
 *
 * The caller is responsible to destruct the
 * returned object with `tinygraph_destruct`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
tinygraph_s tinygraph_construct_from_unsorted_edges(
    const uint32_t* sources,
    const uint32_t* targets,
    uint32_t n);

/**
 * Copies `graph` and returns a new graph with
 * the same nodes and edges as `graph`.
 *
 * The caller is responsible to destruct the
 * returned object with `tinygraph_destruct`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
tinygraph_s tinygraph_copy(tinygraph_const_s graph);

/**
 * Copies `graph` and returns a new graph with
 * the same nodes as `graph` but the edges
 * are reversed.
 *
 * The caller is responsible to destruct the
 * returned object with `tinygraph_destruct`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
tinygraph_s tinygraph_copy_reversed(tinygraph_const_s graph);

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
bool tinygraph_is_empty(tinygraph_const_s graph);

/**
 * Returns the number of nodes in `graph`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_get_num_nodes(tinygraph_const_s graph);

/**
 * Returns the number of edges in `graph`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_get_num_edges(tinygraph_const_s graph);

/**
 * Writes the edge range [first, last) for all out
 * edges of node `source` into `first` and `last`.
 */
TINYGRAPH_API
void tinygraph_get_out_edges(
    tinygraph_const_s graph,
    uint32_t source,
    uint32_t* first,
    uint32_t* last);

/**
 * Returns the edge `e`'s target node.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_get_edge_target(tinygraph_const_s graph, uint32_t e);

/**
 * Returns the number of outgoing edges at node `v`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_get_out_degree(tinygraph_const_s graph, uint32_t v);

/**
 * Writes the node `v`'s neighbors delimited by
 * [first, last) into `first` and `last`.
 *
 * This function is a shortcut for the common use case of
 * - retrieving a node's out edges `e` with `tinygraph_get_out_edges()`,
 * - iterating over all targets for `e` with `tinygraph_get_edge_target()`
 */
TINYGRAPH_API
void tinygraph_get_neighbors(
    tinygraph_const_s graph,
    const uint32_t **first,
    const uint32_t **last,
    uint32_t v);

/**
 * Returns true if `graph` contains node `v`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_has_node(tinygraph_const_s graph, uint32_t v);

/**
 * Returns true if `graph` contains edge `e`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_has_edge(tinygraph_const_s graph, uint32_t e);

/**
 * Returns true if `graph` contains an edge from `s` to `t`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_has_edge_from_to(tinygraph_const_s graph, uint32_t s, uint32_t t);

/**
 * Writes the all-pair shortest paths distances into `result`.
 *
 * The caller is responsible for
 * - providing the n^2 sized `weights` matrix, and
 * - providing the n^2 sized `results` matrix
 *
 * The `results` matrix will contain saturated distances for
 * pairs (i,j) at `results[i * num_nodes + j]`.
 */
TINYGRAPH_API
void tinygraph_apsp(tinygraph_const_s graph, const uint8_t* weights, uint8_t* results);

/**
 * Prints a human readable version of `graph` to stderr.
 */
TINYGRAPH_API
void tinygraph_print(tinygraph_const_s graph);

/**
 * Returns the total size in bytes `graph` uses.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_size_in_bytes(tinygraph_const_s graph);

/**
 * Reorders graph nodes based on their spatial embedding
 * in the `n` sized `lngs` and `lats` parallel arrays:
 *
 * - `nodes[i]` the node i's id (can be the identity)
 * - `lngs[i]` the longitude fixed-point for node i
 * - `lats[i]` the latitude fixed-point for node i
 *
 * The caller is responsible for providing the n sized
 * `nodes` array which will get spatially sorted inplace.
 *
 * It is recommended to reorder graph nodes spatially
 * for better memory locality and reduced cache misses.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_reorder(
    uint32_t* nodes,
    const uint16_t* lngs,
    const uint16_t* lats,
    uint32_t n);

/**
 * Iterates over all nodes in `graph`.
 */
#define TINYGRAPH_FOR_EACH_NODE(item, graph) \
  for (uint32_t item = 0; item < tinygraph_get_num_nodes(graph); ++item)

/**
 * Iterates over all edges in `graph`.
 */
#define TINYGRAPH_FOR_EACH_EDGE(item, graph) \
  for (uint32_t item = 0; item < tinygraph_get_num_edges(graph); ++item)


/**
 * Depth-first search context, caching internal
 * state between multiple runs for efficiency.
 */
typedef struct tinygraph_dfs* tinygraph_dfs_s;
typedef const struct tinygraph_dfs* tinygraph_dfs_const_s;

/**
 * Creates a depth-first search context for `graph`.
 *
 * The use case is to create a context for a graph
 * and then run searches caching internal state.
 *
 * The context needs to be reset between multiple
 * searches with `tinygraph_dfs_clear` and a start
 * node set with `tinygraph_dfs_set_start`.
 *
 * Note: during the lifetime of the context, the
 * graph it was bound to must not change.
 *
 * The caller is responsible to destruct the
 * returned object with `tinygraph_dfs_destruct`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
tinygraph_dfs_s tinygraph_dfs_construct(tinygraph_const_s graph);

/**
 * Destructs `ctx` releasing resources.
 */
TINYGRAPH_API
void tinygraph_dfs_destruct(tinygraph_dfs_s ctx);

/**
 * Sets the source node to `v` from which the
 * depth-first search will start at.
 *
 * Returns true if the start node could be set.
 *
 * Setting a start node must happen exactly once
 * either after context cunstruction, or after
 * clearing the context for repeated searches
 * with `tinygraph_dfs_clear`.
 */
TINYGRAPH_API
bool tinygraph_dfs_set_start(tinygraph_dfs_s ctx, uint32_t v);

/**
 * Returns true if the depth-first search
 * on the graph is exhausted.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_dfs_is_done(tinygraph_dfs_const_s ctx);

/**
 * Writes the next node in the depth-first
 * search context bound to a graph into `v`.
 *
 * Returns true if the next node could be set.
 *
 * Note: the search must not be exhausted; use
 * in combination with `tinygraph_dfs_is_done`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_dfs_step(tinygraph_dfs_s ctx, uint32_t* v);

/**
 * Clears the depth-first search context so that
 * a new graph search can be performed re-using
 * the internal state cached by the context.
 */
TINYGRAPH_API
void tinygraph_dfs_clear(tinygraph_dfs_s ctx);


/**
 * Breadth-first search context, caching internal
 * state between multiple runs for efficiency.
 */
typedef struct tinygraph_bfs* tinygraph_bfs_s;
typedef const struct tinygraph_bfs* tinygraph_bfs_const_s;

/**
 * Creates a breadth-first search context for `graph`.
 *
 * The use case is to create a context for a graph
 * and then run searches caching internal state.
 *
 * The context needs to be reset between multiple
 * searches with `tinygraph_bfs_clear` and a start
 * node set with `tinygraph_bfs_set_start`.
 *
 * Note: during the lifetime of the context, the
 * graph it was bound to must not change.
 *
 * The caller is responsible to destruct the
 * returned object with `tinygraph_bfs_destruct`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
tinygraph_bfs_s tinygraph_bfs_construct(tinygraph_const_s graph);

/**
 * Destructs `ctx` releasing resources.
 */
TINYGRAPH_API
void tinygraph_bfs_destruct(tinygraph_bfs_s ctx);

/**
 * Sets the source node to `v` from which the
 * breadth-first search will start at.
 *
 * Returns true if the start node could be set.
 *
 * Setting a start node must happen exactly once
 * either after context cunstruction, or after
 * clearing the context for repeated searches
 * with `tinygraph_bfs_clear`.
 */
TINYGRAPH_API
bool tinygraph_bfs_set_start(tinygraph_bfs_s ctx, uint32_t v);

/**
 * Returns true if the breadth-first search
 * on the graph is exhausted.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_bfs_is_done(tinygraph_bfs_const_s ctx);

/**
 * Writes the next node in the breadth-first
 * search context bound to a graph into `v`.
 *
 * Returns true if the next node could be set.
 *
 * Note: the search must not be exhausted; use
 * in combination with `tinygraph_bfs_is_done`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_bfs_step(tinygraph_bfs_s ctx, uint32_t* v);

/**
 * Clears the breadth-first search context so that
 * a new graph search can be performed re-using
 * the internal state cached by the context.
 */
TINYGRAPH_API
void tinygraph_bfs_clear(tinygraph_bfs_s ctx);


#ifdef __cplusplus
}
#endif

#endif
