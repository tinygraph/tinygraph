#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "tinygraph.h"
#include "tinygraph-impl.h"
#include "tinygraph-array.h"
#include "tinygraph-stack.h"
#include "tinygraph-queue.h"
#include "tinygraph-bitset.h"
#include "tinygraph-vbyte.h"
#include "tinygraph-delta.h"
#include "tinygraph-zigzag.h"
#include "tinygraph-zorder.h"
#include "tinygraph-bits.h"


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
  const uint32_t sources[5] = {0, 1, 2, 3, 4};
  const uint32_t targets[5] = {5, 6, 7, 8, 9};

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 5);

  assert(graph);
  assert(tinygraph_get_num_edges(graph) == 5);
  assert(tinygraph_get_num_nodes(graph) == 10);
}


void test5() {
  tinygraph_bitset_s bitset1 = tinygraph_bitset_construct(0);
  assert(bitset1);
  tinygraph_bitset_destruct(bitset1);

  tinygraph_bitset_s bitset2 = tinygraph_bitset_construct(1);
  assert(bitset2);
  tinygraph_bitset_set_at(bitset2, 0);
  assert(tinygraph_bitset_get_at(bitset2, 0) == true);
  tinygraph_bitset_destruct(bitset2);

  tinygraph_bitset_s bitset3 = tinygraph_bitset_construct(9);
  assert(bitset3);
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
  assert(array1);
  assert(tinygraph_array_is_empty(array1) == true);
  assert(tinygraph_array_get_size(array1) == 0);

  tinygraph_array_s array2 = tinygraph_array_copy(array1);
  assert(array2);
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
  assert(array3);
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
  assert(array1);
  assert(tinygraph_array_is_empty(array1) == true);
  assert(tinygraph_array_get_size(array1) == 0);
  assert(tinygraph_array_get_capacity(array1) == 0);

  tinygraph_array_s array2 = tinygraph_array_copy(array1);
  assert(array2);
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
  assert(array);

  for (uint32_t i = 0; i < tinygraph_array_get_size(array); ++i) {
    assert(tinygraph_array_get_at(array, i) == 0);
  }

  tinygraph_array_destruct(array);
}


void test9() {
  tinygraph_bitset_s bitset1 = tinygraph_bitset_construct(9);
  assert(bitset1);
  tinygraph_bitset_s bitset2 = tinygraph_bitset_copy(bitset1);
  assert(bitset2);

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

  tinygraph_bitset_clear(bitset1);

  assert(tinygraph_bitset_get_at(bitset1, 0) == false);
  assert(tinygraph_bitset_get_at(bitset1, 1) == false);
  assert(tinygraph_bitset_get_at(bitset1, 2) == false);
  assert(tinygraph_bitset_get_at(bitset1, 3) == false);
  assert(tinygraph_bitset_get_at(bitset1, 4) == false);
  assert(tinygraph_bitset_get_at(bitset1, 5) == false);
  assert(tinygraph_bitset_get_at(bitset1, 6) == false);
  assert(tinygraph_bitset_get_at(bitset1, 7) == false);
  assert(tinygraph_bitset_get_at(bitset1, 8) == false);

  tinygraph_bitset_destruct(bitset1);
  tinygraph_bitset_destruct(bitset2);
}


void test10() {
  const uint32_t original[8] = {1, 2, UINT32_MAX, 0, UINT32_MAX, 0, 1, 2};

  uint32_t deltas[8];

  tinygraph_delta_encode(original, deltas, 8, 0);

  uint32_t roundtrip[8];

  tinygraph_delta_decode(deltas, roundtrip, 8, 0);

  for (uint32_t i = 0; i < 8; ++i) {
    assert(original[i] == roundtrip[i]);
  }
}


void test11() {
  tinygraph_stack_s stack1 = tinygraph_stack_construct();
  assert(stack1);
  assert(tinygraph_stack_is_empty(stack1) == true);
  assert(tinygraph_stack_get_size(stack1) == 0);
  assert(tinygraph_stack_push(stack1, 2));
  assert(tinygraph_stack_push(stack1, 3));
  assert(tinygraph_stack_is_empty(stack1) == false);
  assert(tinygraph_stack_get_size(stack1) == 2);
  assert(tinygraph_stack_pop(stack1) == 3);
  assert(tinygraph_stack_pop(stack1) == 2);
  assert(tinygraph_stack_is_empty(stack1) == true);
  assert(tinygraph_stack_get_size(stack1) == 0);
  tinygraph_stack_destruct(stack1);

  tinygraph_stack_s stack2 = tinygraph_stack_construct();
  assert(stack2);
  assert(tinygraph_stack_reserve(stack2, 8));
  assert(tinygraph_stack_get_capacity(stack2) >= 8);
  assert(tinygraph_stack_is_empty(stack2) == true);
  assert(tinygraph_stack_get_size(stack2) == 0);
  tinygraph_stack_destruct(stack2);
}


void test12() {
  tinygraph_queue_s queue1 = tinygraph_queue_construct();
  assert(queue1);

  assert(tinygraph_queue_is_empty(queue1) == true);
  assert(tinygraph_queue_get_size(queue1) == 0);
  assert(tinygraph_queue_push(queue1, 2));
  assert(tinygraph_queue_push(queue1, 3));
  assert(tinygraph_queue_is_empty(queue1) == false);
  assert(tinygraph_queue_get_size(queue1) == 2);
  assert(tinygraph_queue_pop(queue1) == 2);
  assert(tinygraph_queue_pop(queue1) == 3);
  assert(tinygraph_queue_is_empty(queue1) == true);
  assert(tinygraph_queue_get_size(queue1) == 0);
  tinygraph_queue_destruct(queue1);

  tinygraph_queue_s queue2 = tinygraph_queue_construct();
  assert(queue2);
  assert(tinygraph_queue_push(queue2, 2));
  assert(tinygraph_queue_push(queue2, 3));
  assert(tinygraph_queue_get_front(queue2) == 2);
  assert(tinygraph_queue_get_back(queue2) == 3);
  assert(tinygraph_queue_get_size(queue2) == 2);
  tinygraph_queue_destruct(queue2);

  tinygraph_queue_s queue3 = tinygraph_queue_construct();
  assert(queue3);
  assert(tinygraph_queue_push(queue3, 2));
  assert(tinygraph_queue_push(queue3, 3));
  assert(tinygraph_queue_get_front(queue3) == 2);
  assert(tinygraph_queue_get_back(queue3) == 3);
  assert(tinygraph_queue_pop(queue3) == 2);
  assert(tinygraph_queue_get_front(queue3) == 3);
  assert(tinygraph_queue_get_back(queue3) == 3);
  assert(tinygraph_queue_get_size(queue3) == 1);
  tinygraph_queue_destruct(queue3);
}


bool bfs(tinygraph_s graph, uint32_t *out, uint32_t init) {
  const uint32_t n = tinygraph_get_num_nodes(graph);

  tinygraph_bitset_s seen = tinygraph_bitset_construct(n);

  if (!seen) {
    return false;
  }

  tinygraph_bitset_set_at(seen, init);

  tinygraph_queue_s queue = tinygraph_queue_construct();

  if (!queue) {
    tinygraph_bitset_destruct(seen);
    return false;
  }

  const bool ok = tinygraph_queue_push(queue, init);

  if (!ok) {
    tinygraph_queue_destruct(queue);
    tinygraph_bitset_destruct(seen);
    return false;
  }

  while (!tinygraph_queue_is_empty(queue)) {
    const uint32_t s = tinygraph_queue_pop(queue);

    *out = s;
    out = out + 1;

    const uint32_t *it, *last;

    tinygraph_get_neighbors(graph, &it, &last, s);

    for (; it != last; ++it) {
      const uint32_t t = *it;

      if (!tinygraph_bitset_get_at(seen, t)) {
        const bool ok = tinygraph_queue_push(queue, t);

        if (!ok) {
          tinygraph_queue_destruct(queue);
          tinygraph_bitset_destruct(seen);
          return false;
        }

        tinygraph_bitset_set_at(seen, t);
      }
    }
  }

  tinygraph_queue_destruct(queue);
  tinygraph_bitset_destruct(seen);

  return true;
}


bool dfs(tinygraph_s graph, uint32_t *out, uint32_t init) {
  const uint32_t n = tinygraph_get_num_nodes(graph);

  tinygraph_bitset_s seen = tinygraph_bitset_construct(n);

  if (!seen) {
    return false;
  }

  tinygraph_bitset_set_at(seen, init);

  tinygraph_stack_s stack = tinygraph_stack_construct();

  if (!stack) {
    tinygraph_bitset_destruct(seen);
    return false;
  }

  const bool ok = tinygraph_stack_push(stack, init);

  if (!ok) {
    tinygraph_stack_destruct(stack);
    tinygraph_bitset_destruct(seen);
    return false;
  }

  while (!tinygraph_stack_is_empty(stack)) {
    const uint32_t s = tinygraph_stack_pop(stack);

    *out = s;
    out = out + 1;

    const uint32_t *it, *last;

    tinygraph_get_neighbors(graph, &it, &last, s);

    for (; it != last; ++it) {
      const uint32_t t = *it;

      if (!tinygraph_bitset_get_at(seen, t)) {
        const bool ok = tinygraph_stack_push(stack, t);

        if (!ok) {
          tinygraph_stack_destruct(stack);
          tinygraph_bitset_destruct(seen);
          return false;
        }

        tinygraph_bitset_set_at(seen, t);
      }
    }
  }

  tinygraph_stack_destruct(stack);
  tinygraph_bitset_destruct(seen);

  return true;
}

void test13() {
  const uint32_t sources[5] = {0, 0, 1, 2, 3};
  const uint32_t targets[5] = {1, 2, 0, 3, 2};

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 5);

  assert(graph);

  uint32_t bfsout[4]; // four nodes in total
  uint32_t dfsout[4]; // four nodes in total

  assert(bfs(graph, bfsout, 0));
  assert(dfs(graph, dfsout, 0));

  assert(bfsout[0] == 0);
  assert(bfsout[1] == 1);
  assert(bfsout[2] == 2);
  assert(bfsout[3] == 3);

  assert(dfsout[0] == 0);
  assert(dfsout[1] == 2);
  assert(dfsout[2] == 3);
  assert(dfsout[3] == 1);
}


void test14() {
  const int32_t values[] = {0, -1, 1, -2, 2147483647, -2147483648};

  for (uint32_t i = 0; i < 6; ++i) {
    const int32_t value = values[i];

    const uint32_t encoded = tinygraph_zigzag_encode(value);
    const int32_t decoded = tinygraph_zigzag_decode(encoded);

    assert(decoded == value);
  }
}


void test15() {
  const uint32_t original[5] = {0, 256+1, 65536+1, 16777216+1, 4294967295};

  const uint8_t expected[5 * 5] = {
    0,
    129, 2,
    129, 128, 4,
    129, 128, 128, 8,
    255, 255, 255, 255, 15,
  };

  uint8_t encoded[5 * 5];

  const uint32_t n = tinygraph_vbyte_encode(original, 5, encoded);
  assert(n == 15);

  for (uint32_t i = 0; i < n; ++i) {
    assert(encoded[i] == expected[i]);
  }

  uint32_t decoded[5];

  const uint32_t m = tinygraph_vbyte_decode(expected, 5, decoded);
  assert(m == n);

  for (uint32_t i = 0; i < 5; ++i) {
    assert(decoded[i] == original[i]);
  }
}


void test16() {
  const uint16_t xs[5] = {0, 2, 800, 7, 123};
  const uint16_t ys[5] = {0, 2, 600, 6, 456};
  const uint32_t zs[5] = {0, 12, 861824, 61, 177605};

  for (uint32_t i = 0; i < 5; ++i) {
    assert(tinygraph_zorder_encode32(xs[i], ys[i]) == zs[i]);
    assert(tinygraph_zorder_encode64(xs[i], ys[i]) == zs[i]);

    uint16_t x, y;
    tinygraph_zorder_decode32(zs[i], &x, &y);
    assert(x == xs[i]);
    assert(y == ys[i]);

    uint32_t xx, yy;
    tinygraph_zorder_decode64(zs[i], &xx, &yy);
    assert(xx == xs[i]);
    assert(yy == ys[i]);
  }
}


void test17() {
  assert(tinygraph_bits_count(UINT64_C(0)) == 0);
  assert(tinygraph_bits_count(UINT64_C(1)) == 1);
  assert(tinygraph_bits_count(UINT64_C(-1)) == 64);

  assert(tinygraph_bits_select(UINT64_C(0xd84c8a0), 0) == 5);
  assert(tinygraph_bits_select(UINT64_C(0xd84c8a0), 1) == 7);
  assert(tinygraph_bits_select(UINT64_C(0xd84c8a0), 3) == 14);
  assert(tinygraph_bits_select(UINT64_C(0xd84c8a0), 9) == 27);

  assert(tinygraph_bits_leading0(UINT64_C(0xd84c8a0)) == 36);
  assert(tinygraph_bits_trailing0(UINT64_C(0xd84c8a0)) == 5);

  assert(tinygraph_bits_rank(UINT64_C(0xd84c8a0), 0) == 0);
  assert(tinygraph_bits_rank(UINT64_C(0xd84c8a0), 64)
      == tinygraph_bits_count(UINT64_C(0xd84c8a0)));
  assert(tinygraph_bits_rank(UINT64_C(0xd84c8a0), 6) == 1);
  assert(tinygraph_bits_rank(UINT64_C(0xd84c8a0), 8) == 2);
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
  test10();
  test11();
  test12();
  test13();
  test14();
  test15();
  test16();
  test17();
}
