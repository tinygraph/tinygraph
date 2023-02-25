#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "tinygraph/tinygraph/tinygraph.h"


void test1() {
  const uint32_t *sources = NULL;
  const uint32_t *targets = NULL;

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 0);

  assert(graph);
  assert(tinygraph_is_empty(graph));
  assert(tinygraph_get_num_nodes(graph) == 0);
  assert(tinygraph_get_num_edges(graph) == 0);
}


void test2() {
  const uint32_t sources[2] = {0, 1};
  const uint32_t targets[2] = {0, 2};

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 2);

  assert(graph);
  assert(!tinygraph_is_empty(graph));
  assert(tinygraph_get_num_nodes(graph) == 3);
  assert(tinygraph_get_num_edges(graph) == 2);

  assert(tinygraph_get_edge_target(graph, 0) == 0);
  assert(tinygraph_get_edge_target(graph, 1) == 2);

  assert(tinygraph_get_out_degree(graph, 0) == 1);
  assert(tinygraph_get_out_degree(graph, 1) == 1);
}


void test3() {
  const uint32_t sources[5] = {0, 1, 2, 3, 4};
  const uint32_t targets[5] = {5, 6, 7, 8, 9};

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 5);

  assert(graph);
  assert(tinygraph_get_num_edges(graph) == 5);
  assert(tinygraph_get_num_nodes(graph) == 10);
}


void test4() {
  const uint32_t sources[4] = {0, 0, 1, 1};
  const uint32_t targets[4] = {0, 2, 0, 2};

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 4);

  assert(graph);
  assert(!tinygraph_is_empty(graph));
  assert(tinygraph_get_num_nodes(graph) == 3);
  assert(tinygraph_get_num_edges(graph) == 4);

  assert(tinygraph_has_edge_from_to(graph, 0, 0));
  assert(tinygraph_has_edge_from_to(graph, 0, 2));
  assert(tinygraph_has_edge_from_to(graph, 1, 0));
  assert(tinygraph_has_edge_from_to(graph, 1, 2));
}


void test5() {
  const uint32_t sources[4] = {1, 0, 0, 1};
  const uint32_t targets[4] = {2, 0, 2, 0};

  const tinygraph_s graph = tinygraph_construct_from_unsorted_edges(
      sources, targets, 4);

  assert(graph);
  assert(!tinygraph_is_empty(graph));
  assert(tinygraph_get_num_nodes(graph) == 3);
  assert(tinygraph_get_num_edges(graph) == 4);

  assert(tinygraph_has_edge_from_to(graph, 1, 2));
  assert(tinygraph_has_edge_from_to(graph, 0, 0));
  assert(tinygraph_has_edge_from_to(graph, 0, 2));
  assert(tinygraph_has_edge_from_to(graph, 1, 0));
}


void test6() {
  const uint32_t sources[4] = {1, 0, 0, 1};
  const uint32_t targets[4] = {2, 0, 2, 0};

  const tinygraph_s graph = tinygraph_construct_from_unsorted_edges(
      sources, targets, 4);

  assert(graph);
  assert(!tinygraph_is_empty(graph));
  assert(tinygraph_get_num_nodes(graph) == 3);
  assert(tinygraph_get_num_edges(graph) == 4);

  assert(tinygraph_has_edge_from_to(graph, 1, 2));
  assert(tinygraph_has_edge_from_to(graph, 0, 0));
  assert(tinygraph_has_edge_from_to(graph, 0, 2));
  assert(tinygraph_has_edge_from_to(graph, 1, 0));

  const tinygraph_s rgraph = tinygraph_copy_reversed(graph);

  assert(rgraph);
  assert(!tinygraph_is_empty(rgraph));
  assert(tinygraph_get_num_nodes(rgraph) == 3);
  assert(tinygraph_get_num_edges(rgraph) == 4);

  assert(tinygraph_has_edge_from_to(rgraph, 2, 1));
  assert(tinygraph_has_edge_from_to(rgraph, 0, 0));
  assert(tinygraph_has_edge_from_to(rgraph, 2, 0));
  assert(tinygraph_has_edge_from_to(rgraph, 0, 1));

  tinygraph_print(graph);
  tinygraph_print(rgraph);
}


int main() {
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
}
