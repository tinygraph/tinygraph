#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "tinygraph.h"
#include "tinygraph-impl.h"
#include "tinygraph-array.h"
#include "tinygraph-bitset.h"


void test1() {
  const uint32_t *sources = NULL;
  const uint32_t *targets = NULL;

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 0);

  assert(tinygraph_is_empty(graph));
  assert(tinygraph_get_num_vertices(graph) == 0);
  assert(tinygraph_get_num_edges(graph) == 0);
}


void test2() {
  const uint32_t sources[2] = {0, 1};
  const uint32_t targets[2] = {0, 2};

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 2);

  assert(!tinygraph_is_empty(graph));
  assert(tinygraph_get_num_vertices(graph) == 3);
  assert(tinygraph_get_num_edges(graph) == 2);
}


void test3() {
  uint32_t sources[5];
  uint32_t targets[5];

  tinygraph_iota_u32(sources, 5, 0);
  tinygraph_iota_u32(targets, 5, 1);

  assert(sources[0] == 0);
  assert(sources[4] == 4);

  assert(targets[0] == 1);
  assert(targets[4] == 5);

  assert(tinygraph_find_if_u32(sources, sources + 5, 0) == sources + 0);
  assert(tinygraph_find_if_not_u32(sources, sources + 5, 0) == sources + 1);

  assert(tinygraph_binary_search_u32(sources, sources + 5, 0) == sources + 0);
  assert(tinygraph_exponential_search_u32(sources, sources + 5, 0) == sources + 0);

  assert(tinygraph_binary_search_u32(sources, sources + 5, -1) == sources + 5);
  assert(tinygraph_exponential_search_u32(sources, sources + 5, -1) == sources + 5);
}


void test4() {
  uint32_t sources[5] = {0, 1, 2, 3, 4};
  uint32_t targets[5] = {5, 6, 7, 8, 9};

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 5);

  assert(tinygraph_get_num_edges(graph) == 5);
  assert(tinygraph_get_num_vertices(graph) == 10);
}


void test5() {
  tinygraph_bitset_s bitset1 = tinygraph_bitset_construct(0);
  tinygraph_bitset_destruct(bitset1);

  tinygraph_bitset_s bitset2 = tinygraph_bitset_construct(1);
  tinygraph_bitset_set_at(bitset2, 0);
  assert(tinygraph_bitset_get_at(bitset2, 0) == true);
  tinygraph_bitset_destruct(bitset2);

  tinygraph_bitset_s bitset3 = tinygraph_bitset_construct(9);
  tinygraph_bitset_set_at(bitset3, 0);
  tinygraph_bitset_set_at(bitset3, 8);
  assert(tinygraph_bitset_get_at(bitset3, 0) == true);
  assert(tinygraph_bitset_get_at(bitset3, 1) == false);
  assert(tinygraph_bitset_get_at(bitset3, 2) == false);
  assert(tinygraph_bitset_get_at(bitset3, 3) == false);
  assert(tinygraph_bitset_get_at(bitset3, 4) == false);
  assert(tinygraph_bitset_get_at(bitset3, 5) == false);
  assert(tinygraph_bitset_get_at(bitset3, 6) == false);
  assert(tinygraph_bitset_get_at(bitset3, 7) == false);
  assert(tinygraph_bitset_get_at(bitset3, 8) == true);
  tinygraph_bitset_destruct(bitset3);
}


void test6() {
  tinygraph_array_s array1 = tinygraph_array_construct(0);
  assert(tinygraph_array_is_empty(array1) == true);
  assert(tinygraph_array_get_size(array1) == 0);

  tinygraph_array_s array2 = tinygraph_array_copy(array1);
  tinygraph_array_destruct(array1); // no longer needed
  assert(tinygraph_array_push(array2, 1));
  assert(tinygraph_array_push(array2, 2));
  assert(tinygraph_array_get_at(array2, 0) == 1);
  assert(tinygraph_array_get_at(array2, 1) == 2);
  assert(tinygraph_array_is_empty(array2) == false);
  assert(tinygraph_array_get_size(array2) == 2);
  assert(tinygraph_array_pop(array2) == 2);
  assert(tinygraph_array_pop(array2) == 1);
  assert(tinygraph_array_is_empty(array2) == true);
  assert(tinygraph_array_get_size(array2) == 0);
  tinygraph_array_destruct(array2);

  tinygraph_array_s array3 = tinygraph_array_construct(32);
  assert(tinygraph_array_is_empty(array3) == false);
  assert(tinygraph_array_get_size(array3) == 32);
  tinygraph_array_set_at(array3, 0, 3);
  tinygraph_array_set_at(array3, 1, 4);
  assert(tinygraph_array_pop(array3) == 0);
  assert(tinygraph_array_pop(array3) == 0);
  assert(tinygraph_array_get_at(array3, 0) == 3);
  assert(tinygraph_array_get_at(array3, 1) == 4);
  tinygraph_array_clear(array3);
  assert(tinygraph_array_is_empty(array3) == true);
  assert(tinygraph_array_get_size(array3) == 0);
  tinygraph_array_destruct(array3);
}


void test7() {
  tinygraph_array_s array1 = tinygraph_array_construct(0);
  assert(tinygraph_array_is_empty(array1) == true);
  assert(tinygraph_array_get_size(array1) == 0);
  assert(tinygraph_array_get_capacity(array1) == 0);

  tinygraph_array_s array2 = tinygraph_array_copy(array1);
  tinygraph_array_destruct(array1); // no longer needed
  assert(tinygraph_array_reserve(array2, 8));
  assert(tinygraph_array_get_size(array2) == 0);
  assert(tinygraph_array_get_capacity(array2) == 8);
  assert(tinygraph_array_resize(array2, 8));
  assert(tinygraph_array_get_size(array2) == 8);
  assert(tinygraph_array_get_capacity(array2) == 8);
  assert(tinygraph_array_get_at(array2, 0) == 0);
  assert(tinygraph_array_get_at(array2, 7) == 0);
  tinygraph_array_set_at(array2, 7, 1);
  tinygraph_array_set_at(array2, 0, 1);
  assert(tinygraph_array_get_at(array2, 0) == 1);
  assert(tinygraph_array_get_at(array2, 7) == 1);
  assert(tinygraph_array_resize(array2, 1));
  assert(tinygraph_array_get_at(array2, 0) == 1);
  assert(tinygraph_array_resize(array2, 8));
  assert(tinygraph_array_get_at(array2, 0) == 1);
  assert(tinygraph_array_get_at(array2, 7) == 0);
  tinygraph_array_destruct(array2);
}


void test8() {
  tinygraph_array_s array = tinygraph_array_construct(32);

  for (uint32_t i = 0; i < tinygraph_array_get_size(array); ++i) {
    assert(tinygraph_array_get_at(array, i) == 0);
  }

  tinygraph_array_destruct(array);
}


void test9() {
  tinygraph_bitset_s bitset1 = tinygraph_bitset_construct(9);
  tinygraph_bitset_s bitset2 = tinygraph_bitset_copy(bitset1);

  // 1 0 0 0 0 0 0 0 1
  tinygraph_bitset_set_at(bitset1, 0);
  tinygraph_bitset_set_at(bitset1, 8);

  // 1 1 0 0 0 0 0 0 0
  tinygraph_bitset_set_at(bitset2, 0);
  tinygraph_bitset_set_at(bitset2, 1);

  // 0 1 1 1 1 1 1 1 0
  tinygraph_bitset_not(bitset1);

  assert(tinygraph_bitset_get_at(bitset1, 0) == false);
  assert(tinygraph_bitset_get_at(bitset1, 1) == true);
  assert(tinygraph_bitset_get_at(bitset1, 2) == true);
  assert(tinygraph_bitset_get_at(bitset1, 3) == true);
  assert(tinygraph_bitset_get_at(bitset1, 4) == true);
  assert(tinygraph_bitset_get_at(bitset1, 5) == true);
  assert(tinygraph_bitset_get_at(bitset1, 6) == true);
  assert(tinygraph_bitset_get_at(bitset1, 7) == true);
  assert(tinygraph_bitset_get_at(bitset1, 8) == false);

  tinygraph_bitset_or(bitset1, bitset2);

  assert(tinygraph_bitset_get_at(bitset1, 0) == true);
  assert(tinygraph_bitset_get_at(bitset1, 1) == true);
  assert(tinygraph_bitset_get_at(bitset1, 2) == true);
  assert(tinygraph_bitset_get_at(bitset1, 3) == true);
  assert(tinygraph_bitset_get_at(bitset1, 4) == true);
  assert(tinygraph_bitset_get_at(bitset1, 5) == true);
  assert(tinygraph_bitset_get_at(bitset1, 6) == true);
  assert(tinygraph_bitset_get_at(bitset1, 7) == true);
  assert(tinygraph_bitset_get_at(bitset1, 8) == false);

  tinygraph_bitset_destruct(bitset1);
  tinygraph_bitset_destruct(bitset2);
}


int main() {
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
}
