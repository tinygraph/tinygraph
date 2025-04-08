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
 * Single-source shortest-path search context, caching
 * internal state between multiple runs for efficiency.
 */
typedef struct tinygraph_dijkstra* tinygraph_dijkstra_s;
typedef const struct tinygraph_dijkstra* tinygraph_dijkstra_const_s;

/**
 * Creates a single-source shortest-path context for
 * `graph` with edge weights `weights`.
 *
 * The use case is to create a context for a graph
 * and then run searches caching internal search
 * state and graph-dependant allocations.
 *
 * Note: during the lifetime of the context, the
 * graph and weights it was bound to must not change.
 *
 * Note: the weights must be non-negative and
 * the graph must not contain zero-weighted loops,
 * `weights` must contain `num_edges` weights.
 *
 * Note: weights are allowed to be UINT16_MAX. The
 * distance on the shortest path will get aggregated
 * as uint32_t. In case of a total distance that is
 * larger than UINT32_MAX a saturating summation is
 * performed. This can happen e.g. if you have
 * UINT16_MAX weights for all edges and enough of
 * them on the shortest path to saturate the type
 * uint32_t sum. In the case of saturation the search
 * does not continue since shortest paths can no
 * longer be distinguished from any other path.
 *
 * The caller is responsible to destruct the
 * returned object with `tinygraph_dijkstra_destruct`.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
tinygraph_dijkstra_s tinygraph_dijkstra_construct(
    tinygraph_const_s graph,
    const uint16_t* weights);

/**
 * Destructs `ctx` releasing resources.
 */
TINYGRAPH_API
void tinygraph_dijkstra_destruct(tinygraph_dijkstra_s ctx);

/**
 * Runs a single-source shortest path search from
 * the source node `s` to the target node `t`.
 *
 * Returns true if a path could be found and the
 * search context is ready for distance and path
 * retrieval with `tinygraph_dijkstra_get_distance`
 * and `tinygraph_dijkstra_get_path`, respectively.
 *
 * Note: The single-source shortest path problem
 * can be extended into a multi-source shortest
 * path problem by adding a virtual node to start
 * the search from and connecting it to multiple
 * source nodes with zero weight.
 *
 * Note: When the source node `s` stays the same
 * between function calls, the internal search
 * space will get cached, leading to faster
 * queries compared to re-starting the search
 * from scratch.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_dijkstra_shortest_path(
    tinygraph_dijkstra_s ctx,
    uint32_t s,
    uint32_t t);

/**
 * Returns the shortest path's distance.
 *
 * Note: before calling this function,
 * `tinygraph_dijkstra_shortest_path`
 * must have been successfull.
 *
 * Note: the distance of a search query
 * where source and target nodes are the
 * same is zero, no matter if there is a
 * weighted self-loop.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_dijkstra_get_distance(tinygraph_dijkstra_s ctx);

/**
 * Retrievs a shortest path's sequence of nodes.
 *
 * In case of multiple shortest paths, this function
 * retrievs an arbitrary shortest path that was found.
 *
 * Returns true if a path could be retrieved.
 *
 * Writes the sequence of nodes delimited by
 * [first, last) into `first` and `last`.
 *
 * Note: before calling this function,
 * `tinygraph_dijkstra_shortest_path`
 * must have been successfull.
 *
 * Note: the shortest path of a search query
 * where the source and target nodes are the
 * same is the empty path, no matter if there
 * is a weighted self-loop.
 *
 * Note: `first` and `last` stay valid until
 * `tinygraph_dijkstra_shortest_path`
 * gets called again.
 */
TINYGRAPH_API
TINYGRAPH_WARN_UNUSED
bool tinygraph_dijkstra_get_path(
    tinygraph_dijkstra_s ctx,
    const uint32_t **first,
    const uint32_t **last);


#ifdef __cplusplus
}
#endif

#endif
