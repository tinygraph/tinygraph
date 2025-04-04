#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tinygraph.h"
#include "tinygraph-impl.h"
#include "tinygraph-utils.h"
#include "tinygraph-array.h"
#include "tinygraph-stack.h"
#include "tinygraph-queue.h"
#include "tinygraph-bitset.h"
#include "tinygraph-vbyte.h"
#include "tinygraph-delta.h"
#include "tinygraph-zigzag.h"
#include "tinygraph-zorder.h"
#include "tinygraph-bits.h"
#include "tinygraph-eliasfano.h"
#include "tinygraph-align.h"
#include "tinygraph-heap.h"
#include "tinygraph-hash.h"
#include "tinygraph-rng.h"
#include "tinygraph-sort.h"


void test1(void) {
  const uint32_t *sources = NULL;
  const uint32_t *targets = NULL;

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 0);

  assert(graph);
  assert(tinygraph_is_empty(graph));
  assert(tinygraph_get_num_nodes(graph) == 0);
  assert(tinygraph_get_num_edges(graph) == 0);

  tinygraph_destruct(graph);
}


void test2(void) {
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

  tinygraph_destruct(graph);
}


void test3(void) {
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


void test4(void) {
  const uint32_t sources[5] = {0, 1, 2, 3, 4};
  const uint32_t targets[5] = {5, 6, 7, 8, 9};

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 5);

  assert(graph);
  assert(tinygraph_get_num_edges(graph) == 5);
  assert(tinygraph_get_num_nodes(graph) == 10);

  tinygraph_destruct(graph);
}


void test5(void) {
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


void test6(void) {
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


  tinygraph_array_s array4 = tinygraph_array_construct(0);
  assert(array4);
  assert(tinygraph_array_push(array4, 3));
  assert(tinygraph_array_push(array4, 5));
  assert(tinygraph_array_push(array4, 7));
  assert(tinygraph_array_push(array4, 1));
  assert(tinygraph_array_push(array4, 9));
  assert(tinygraph_array_get_at(array4, 0) == 3);
  assert(tinygraph_array_get_at(array4, 4) == 9);
  tinygraph_array_reverse(array4);
  assert(tinygraph_array_get_at(array4, 0) == 9);
  assert(tinygraph_array_get_at(array4, 4) == 3);
  assert(tinygraph_array_pop(array4) == 3);
  assert(tinygraph_array_pop(array4) == 5);
  assert(tinygraph_array_pop(array4) == 7);
  assert(tinygraph_array_pop(array4) == 1);
  assert(tinygraph_array_pop(array4) == 9);
  tinygraph_array_destruct(array4);
}


void test7(void) {
  tinygraph_array_s array1 = tinygraph_array_construct(0);
  assert(array1);
  assert(tinygraph_array_is_empty(array1) == true);
  assert(tinygraph_array_get_size(array1) == 0);

  tinygraph_array_s array2 = tinygraph_array_copy(array1);
  assert(array2);
  tinygraph_array_destruct(array1); // no longer needed
  assert(tinygraph_array_reserve(array2, 8));
  assert(tinygraph_array_reserve(array2, 8));
  assert(tinygraph_array_reserve(array2, 0));
  assert(tinygraph_array_reserve(array2, 0));
  assert(tinygraph_array_reserve(array2, 8));
  assert(tinygraph_array_reserve(array2, 8));
  assert(tinygraph_array_reserve(array2, 9));
  assert(tinygraph_array_get_size(array2) == 0);
  assert(tinygraph_array_get_capacity(array2) >= 9);
  assert(tinygraph_array_resize(array2, 8));
  assert(tinygraph_array_get_size(array2) == 8);
  assert(tinygraph_array_get_capacity(array2) >= 8);
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

  tinygraph_array_s array3 = tinygraph_array_construct(8);
  assert(array3);
  assert(tinygraph_array_reserve(array3, 0));
  assert(tinygraph_array_reserve(array3, 16));
  assert(tinygraph_array_get_capacity(array3) >= 16);
  assert(tinygraph_array_reserve(array3, 17));
  assert(tinygraph_array_get_capacity(array3) >= 17);
  assert(tinygraph_array_reserve(array3, 0));
  assert(tinygraph_array_resize(array3, 0));
  assert(tinygraph_array_get_size(array3) == 0);
  assert(tinygraph_array_resize(array3, 16));
  assert(tinygraph_array_get_size(array3) == 16);
  assert(tinygraph_array_get_capacity(array3) >= 16);
  assert(tinygraph_array_resize(array3, 17));
  assert(tinygraph_array_get_size(array3) == 17);
  assert(tinygraph_array_get_capacity(array3) >= 17);
  tinygraph_array_destruct(array3);
}


void test8(void) {
  tinygraph_array_s array = tinygraph_array_construct(32);
  assert(array);

  for (uint32_t i = 0; i < tinygraph_array_get_size(array); ++i) {
    assert(tinygraph_array_get_at(array, i) == 0);
  }

  tinygraph_array_destruct(array);
}


void test9(void) {
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


void test10(void) {
  const uint32_t original[8] = {1, 2, UINT32_MAX, 0, UINT32_MAX, 0, 1, 2};

  uint32_t deltas[8];

  tinygraph_delta_encode(original, deltas, 8, 0);

  uint32_t roundtrip[8];

  tinygraph_delta_decode(deltas, roundtrip, 8, 0);

  for (uint32_t i = 0; i < 8; ++i) {
    assert(original[i] == roundtrip[i]);
  }
}


void test11(void) {
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


void test12(void) {
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
  assert(tinygraph_queue_get_size(queue2) == 2);
  tinygraph_queue_destruct(queue2);

  tinygraph_queue_s queue3 = tinygraph_queue_construct();
  assert(queue3);
  assert(tinygraph_queue_push(queue3, 2));
  assert(tinygraph_queue_push(queue3, 3));
  assert(tinygraph_queue_pop(queue3) == 2);
  assert(tinygraph_queue_get_size(queue3) == 1);
  tinygraph_queue_destruct(queue3);
}


TINYGRAPH_WARN_UNUSED
static inline bool bfs(tinygraph_const_s graph, uint32_t *out, uint32_t init) {
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


TINYGRAPH_WARN_UNUSED
static inline bool dfs(tinygraph_const_s graph, uint32_t *out, uint32_t init) {
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

void test13(void) {
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

  tinygraph_destruct(graph);
}


void test14(void) {
  const int32_t values[] = {0, -1, 1, -2, 2147483647, -2147483648};

  for (uint32_t i = 0; i < 6; ++i) {
    const int32_t value = values[i];

    const uint32_t encoded = tinygraph_zigzag_encode(value);
    const int32_t decoded = tinygraph_zigzag_decode(encoded);

    assert(decoded == value);
  }
}


void test15(void) {
  const uint32_t original[5] = {0, 256+1, 65536+1, 16777216+1, 4294967295};

  const uint8_t expected[5 * 5] = {
    0,
    129, 2,
    129, 128, 4,
    129, 128, 128, 8,
    255, 255, 255, 255, 15,
  };

  uint8_t encoded[5 * 5];

  const uint32_t n = tinygraph_vbyte_encode(original, encoded, 5);
  assert(n == 15);

  for (uint32_t i = 0; i < n; ++i) {
    assert(encoded[i] == expected[i]);
  }

  uint32_t decoded[5];

  const uint32_t m = tinygraph_vbyte_decode(expected, decoded, 5);
  assert(m == n);

  for (uint32_t i = 0; i < 5; ++i) {
    assert(decoded[i] == original[i]);
  }
}


void test16(void) {
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


void test17(void) {
  assert(tinygraph_bits_count(UINT64_C(0)) == 0);
  assert(tinygraph_bits_count(UINT64_C(1)) == 1);
  assert(tinygraph_bits_count(UINT64_C(-1)) == 64);

  assert(tinygraph_bits_select(UINT64_C(0xd84c8a0), 0) == 5);
  assert(tinygraph_bits_select(UINT64_C(0xd84c8a0), 1) == 7);
  assert(tinygraph_bits_select(UINT64_C(0xd84c8a0), 3) == 14);
  assert(tinygraph_bits_select(UINT64_C(0xd84c8a0), 9) == 27);

  assert(tinygraph_bits_leading0_u64(UINT64_C(0xd84c8a0)) == 36);
  assert(tinygraph_bits_trailing0_u64(UINT64_C(0xd84c8a0)) == 5);

  assert(tinygraph_bits_rank(UINT64_C(0xd84c8a0), 0) == 0);
  assert(tinygraph_bits_rank(UINT64_C(0xd84c8a0), 64)
      == tinygraph_bits_count(UINT64_C(0xd84c8a0)));
  assert(tinygraph_bits_rank(UINT64_C(0xd84c8a0), 6) == 1);
  assert(tinygraph_bits_rank(UINT64_C(0xd84c8a0), 8) == 2);

  assert(tinygraph_bits_rank(UINT64_C(-1), 0) == 0);

  for (uint32_t i = 0; i <= 64; ++i) {
    assert(tinygraph_bits_rank(UINT64_C(-1), i) == i);
  }

  // 0x11 = 0b00010001
  // pos  =   76543210
  assert(tinygraph_bits_select(UINT64_C(0x11), 0) == 0);
  assert(tinygraph_bits_select(UINT64_C(0x11), 1) == 4);

  // 0x11 = 0b00010001
  // pos  =   76543210
  assert(tinygraph_bits_rank(UINT64_C(0x11), 0) == 0);
  assert(tinygraph_bits_rank(UINT64_C(0x11), 1) == 1);
  assert(tinygraph_bits_rank(UINT64_C(0x11), 2) == 1);
  assert(tinygraph_bits_rank(UINT64_C(0x11), 3) == 1);
  assert(tinygraph_bits_rank(UINT64_C(0x11), 4) == 1);
  assert(tinygraph_bits_rank(UINT64_C(0x11), 5) == 2);
  assert(tinygraph_bits_rank(UINT64_C(0x11), 6) == 2);
  assert(tinygraph_bits_rank(UINT64_C(0x11), 7) == 2);
  assert(tinygraph_bits_rank(UINT64_C(0x11), 8) == 2);
}


void test18(void) {
  uint32_t data[] = {1, 2, 10, 19, 147};

  tinygraph_bitset_s bits = tinygraph_eliasfano_encode(data, 5);

  tinygraph_bitset_print_internal(bits);

  tinygraph_bitset_destruct(bits);
}


void test19(void) {
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

  tinygraph_destruct(graph);
}


void test20(void) {
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

  tinygraph_destruct(graph);
}


void test21(void) {
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

  tinygraph_destruct(graph);
  tinygraph_destruct(rgraph);
}


void test22(void) {
  void* p = tinygraph_align_malloc(64, 4096);

  assert(p);
  assert(((uintptr_t)p % 64) == 0);

  tinygraph_align_free(p);

  void* q = tinygraph_align_malloc(4096, 4096);

  assert(q);
  assert(((uintptr_t)q % 4096) == 0);

  tinygraph_align_free(q);
}


void test23(void) {
  uint64_t cacheline0[8] = {
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
  };

  assert(tinygraph_bits_count_512(cacheline0) == UINT32_C(0));

  uint64_t cacheline2[8] = {
    UINT64_C(1),
    UINT64_C(1),
    UINT64_C(1),
    UINT64_C(1),
    UINT64_C(1),
    UINT64_C(1),
    UINT64_C(1),
    UINT64_C(1),
  };

  assert(tinygraph_bits_count_512(cacheline2) == UINT32_C(8));

  uint64_t cacheline3[8] = {
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
  };

  assert(tinygraph_bits_count_512(cacheline3) == UINT32_C(512));

  uint64_t cacheline4[8] = {
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
  };

  assert(tinygraph_bits_count_512(cacheline4) == UINT32_C(256));
}


void test24(void) {
  uint64_t cacheline0[9] = {
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(0),
    UINT64_C(-1),
  };

  for (uint32_t i = 0; i <= 512; ++i) {
    assert(tinygraph_bits_rank_512(cacheline0, i) == UINT32_C(0));
  }

  assert(tinygraph_bits_count_512(cacheline0)
      == tinygraph_bits_rank_512(cacheline0, 512));

  uint64_t cacheline1[8] = {
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
    UINT64_C(-1),
  };

  for (uint32_t i = 0; i <= 512; ++i) {
    assert(tinygraph_bits_rank_512(cacheline1, i) == i);
  }

  assert(tinygraph_bits_count_512(cacheline1)
      == tinygraph_bits_rank_512(cacheline1, 512));

  for (uint32_t i = 0; i < 512; ++i) {
    assert(tinygraph_bits_select_512(cacheline1, i) == i);
  }

  // 0b..0101
  // 0b..1010
  uint64_t cacheline2[8] = {
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
  };

  assert(tinygraph_bits_rank(cacheline2[0], 0)
      == tinygraph_bits_rank_512(cacheline2, 0));

  assert(tinygraph_bits_rank(cacheline2[0], 64)
      == tinygraph_bits_rank_512(cacheline2, 64));

  assert(tinygraph_bits_rank(cacheline2[0], 32)
      == tinygraph_bits_rank_512(cacheline2, 32));

  for (uint32_t i = 0; i <= 64; ++i) {
    assert(tinygraph_bits_rank(cacheline2[4], i)
        == tinygraph_bits_rank_512(cacheline2, 4 * 64 + i)
            - tinygraph_bits_rank_512(cacheline2, 4 * 64 + 0));
  }

  for (uint32_t i = 0; i < 32; ++i) {
    assert(tinygraph_bits_select_512(cacheline2, i) == i * 2);
  }

  for (uint32_t i = 32; i < 64; ++i) {
    assert(tinygraph_bits_select_512(cacheline2, i) == i * 2 + 1);
  }

  for (uint32_t i = 64; i < 96; ++i) {
    assert(tinygraph_bits_select_512(cacheline2, i) == i * 2);
  }

  for (uint32_t i = 96; i < 128; ++i) {
    assert(tinygraph_bits_select_512(cacheline2, i) == i * 2 + 1);
  }

  for (uint32_t i = 128; i < 160; ++i) {
    assert(tinygraph_bits_select_512(cacheline2, i) == i * 2);
  }

  for (uint32_t i = 160; i < 192; ++i) {
    assert(tinygraph_bits_select_512(cacheline2, i) == i * 2 + 1);
  }

  for (uint32_t i = 192; i < 224; ++i) {
    assert(tinygraph_bits_select_512(cacheline2, i) == i * 2);
  }

  for (uint32_t i = 224; i < 256; ++i) {
    assert(tinygraph_bits_select_512(cacheline2, i) == i * 2 + 1);
  }
}


void test25(void) {
  // 0b..0101
  // 0b..1010
  uint64_t cacheline0[8] = {
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
    UINT64_C(0x5555555555555555),
    UINT64_C(0xaaaaaaaaaaaaaaaa),
  };

  for (uint32_t i = 0; i < 256; ++i) {
    assert(tinygraph_bits_rank_512(cacheline0,
            tinygraph_bits_select_512(cacheline0, i)) == i);
  }
}


void test26(void) {
  tinygraph_heap_s heap1 = tinygraph_heap_construct();
  assert(heap1);
  assert(tinygraph_heap_is_empty(heap1) == true);
  assert(tinygraph_heap_get_size(heap1) == 0);

  tinygraph_heap_s heap2 = tinygraph_heap_copy(heap1);
  assert(heap2);
  assert(tinygraph_heap_is_empty(heap2) == true);
  assert(tinygraph_heap_get_size(heap2) == 0);

  tinygraph_heap_destruct(heap1); // no longer needed

  // push {value, priority} tuple, min-heap on priority
  assert(tinygraph_heap_push(heap2, 100, 3));
  assert(tinygraph_heap_push(heap2, 200, 1));
  assert(tinygraph_heap_push(heap2, 300, 2));

  assert(tinygraph_heap_is_empty(heap2) == false);
  assert(tinygraph_heap_get_size(heap2) == 3);
  assert(tinygraph_heap_get_capacity(heap2) >= 3);

  assert(tinygraph_heap_pop(heap2) == 200);
  assert(tinygraph_heap_pop(heap2) == 300);
  assert(tinygraph_heap_pop(heap2) == 100);

  tinygraph_heap_destruct(heap2);
}


void test27(void) {
  tinygraph_heap_s heap = tinygraph_heap_construct();
  assert(heap);

  for (uint32_t i = 2000; i < 3000; ++i) {
    assert(tinygraph_heap_push(heap, i, i));
  }

  for (uint32_t i = 0; i < 2000; ++i) {
    assert(tinygraph_heap_push(heap, i, i));
  }

  for (uint32_t i = 3000; i < 4000; ++i) {
    assert(tinygraph_heap_push(heap, i, i));
  }

  for (uint32_t i = 0; i < 4000; ++i) {
    assert(tinygraph_heap_pop(heap) == i);
  }

  assert(tinygraph_heap_is_empty(heap));

  tinygraph_heap_destruct(heap);
}


void test28(void) {
  uint32_t h = 0;

  h = tinygraph_hash_combine_u32(h, 0);

  assert(h != 0);
}


void test29(void) {
  tinygraph_rng_s rng = tinygraph_rng_construct();

  uint32_t v1 = tinygraph_rng_random(rng);
  uint32_t v2 = tinygraph_rng_random(rng);

  assert(v1 != 0);
  assert(v2 != 0);
  assert(v1 != v2);

  for (uint32_t i = 0; i < 100; ++i) {
    assert(tinygraph_rng_bounded(rng, 10) < 10);
  }

  tinygraph_rng_destruct(rng);
}


void test30(void) {
  tinygraph_rng_s rng1 = tinygraph_rng_construct();
  tinygraph_rng_s rng2 = tinygraph_rng_construct_from_seed(1);
  tinygraph_rng_s rng3 = tinygraph_rng_construct_from_seed(2);

  const uint32_t v1 = tinygraph_rng_random(rng1);
  const uint32_t v2 = tinygraph_rng_random(rng2);
  const uint32_t v3 = tinygraph_rng_random(rng3);

  assert(v1 != v2);
  assert(v2 != v3);

  tinygraph_rng_destruct(rng1);
  tinygraph_rng_destruct(rng2);
  tinygraph_rng_destruct(rng3);
}


TINYGRAPH_WARN_UNUSED
int32_t less_u32(const uint32_t* l, const uint32_t* r, void* arg) {
  (void)arg;

  const uint32_t lhs = *l;
  const uint32_t rhs = *r;

  if (lhs < rhs) {
    return -1;
  } else if (lhs > rhs) {
    return +1;
  } else {
    return 0;
  }
}


void test31(void) {
  uint32_t a0[1];
  uint32_t a1[32];
  uint32_t a2[64];

  tinygraph_rng_s rng = tinygraph_rng_construct();

  for (uint32_t i = 0; i < sizeof(a0) / sizeof(a0[0]); ++i) {
    a0[i] = tinygraph_rng_random(rng);
  }

  for (uint32_t i = 0; i < sizeof(a1) / sizeof(a1[0]); ++i) {
    a1[i] = tinygraph_rng_random(rng);
  }

  for (uint32_t i = 0; i < sizeof(a2) / sizeof(a2[0]); ++i) {
    a2[i] = tinygraph_rng_random(rng);
  }

  tinygraph_rng_destruct(rng);

  tinygraph_sort_u32(a0, sizeof(a0) / sizeof(a0[0]), less_u32, NULL);
  tinygraph_sort_u32(a1, sizeof(a1) / sizeof(a1[0]), less_u32, NULL);
  tinygraph_sort_u32(a2, sizeof(a2) / sizeof(a2[0]), less_u32, NULL);

  assert(tinygraph_is_sorted_u32(a0, sizeof(a0) / sizeof(a0[0])));
  assert(tinygraph_is_sorted_u32(a1, sizeof(a1) / sizeof(a1[0])));
  assert(tinygraph_is_sorted_u32(a2, sizeof(a2) / sizeof(a2[0])));
}


void test32(void) {
  const uint16_t lngs[] = {0, 12, 13, 11, 1, 2};
  const uint16_t lats[] = {0, 49, 47, 49, 1, 3};

  uint32_t nodes[] = {2,  1,  0,  3, 4, 5};

  assert(sizeof(lngs) / sizeof(lngs[0]) == sizeof(lats) / sizeof(lats[0]));
  assert(sizeof(lats) / sizeof(lats[0]) == sizeof(nodes) / sizeof(nodes[0]));
  assert(sizeof(nodes) / sizeof(nodes[0]) == 6);

  const bool ok = tinygraph_reorder(nodes, lngs, lats, 6);
  assert(ok);

  for (uint32_t i = 1; i < 6; ++i) {
    const uint32_t l = tinygraph_zorder_encode32(lngs[nodes[i - 1]], lats[nodes[i - 1]]);
    const uint32_t r = tinygraph_zorder_encode32(lngs[nodes[i]], lats[nodes[i]]);

    assert(l <= r);
  }
}

TINYGRAPH_WARN_UNUSED
uint32_t deref_u32(const uint32_t* item, void* arg) {
  (void)arg;

  return *item;
}

void test33(void) {
  uint32_t a0[1];
  uint32_t a1[32];
  uint32_t a2[64];

  tinygraph_rng_s rng = tinygraph_rng_construct();

  for (uint32_t i = 0; i < sizeof(a0) / sizeof(a0[0]); ++i) {
    a0[i] = tinygraph_rng_random(rng);
  }

  for (uint32_t i = 0; i < sizeof(a1) / sizeof(a1[0]); ++i) {
    a1[i] = tinygraph_rng_random(rng);
  }

  for (uint32_t i = 0; i < sizeof(a2) / sizeof(a2[0]); ++i) {
    a2[i] = tinygraph_rng_random(rng);
  }

  tinygraph_rng_destruct(rng);

  assert(tinygraph_radix_sort_u32(a0, sizeof(a0) / sizeof(a0[0]), deref_u32, NULL));
  assert(tinygraph_radix_sort_u32(a1, sizeof(a1) / sizeof(a1[0]), deref_u32, NULL));
  assert(tinygraph_radix_sort_u32(a2, sizeof(a2) / sizeof(a2[0]), deref_u32, NULL));

  assert(tinygraph_is_sorted_u32(a0, sizeof(a0) / sizeof(a0[0])));
  assert(tinygraph_is_sorted_u32(a1, sizeof(a1) / sizeof(a1[0])));
  assert(tinygraph_is_sorted_u32(a2, sizeof(a2) / sizeof(a2[0])));
}

void test34(void) {
  tinygraph_rng_s rng = tinygraph_rng_construct();

  const uint32_t n = 1000;

  uint32_t * const nodes = malloc(n * sizeof(uint32_t));
  uint16_t * const lngs = malloc(n * sizeof(uint16_t));
  uint16_t * const lats = malloc(n * sizeof(uint16_t));

  assert(nodes);
  assert(lngs);
  assert(lats);

  tinygraph_iota_u32(nodes, n, 0);

  for (uint32_t i = 0; i < n; ++i) {
    lngs[i] = tinygraph_rng_random(rng);
    lats[i] = tinygraph_rng_random(rng);
  }

  const bool ok = tinygraph_reorder(nodes, lngs, lats, n);
  assert(ok);

  free(nodes);
  free(lngs);
  free(lats);

  tinygraph_rng_destruct(rng);
}


TINYGRAPH_WARN_UNUSED
tinygraph_s construct_random_graph(tinygraph_rng_s rng, uint32_t nodes, uint32_t degree) {
  assert(rng);
  assert(nodes > 0);
  assert(degree > 1);

  const uint32_t edges = nodes * degree;

  uint32_t * const sources = malloc(edges * sizeof(uint32_t));

  if (!sources) {
    return NULL;
  }

  uint32_t * const targets = malloc(edges * sizeof(uint32_t));

  if (!targets) {
    free(sources);
    return NULL;
  }

  for (uint32_t i = 0; i < nodes; ++i) {
    for (uint32_t j = 0; j < degree; ++j) {
      const uint32_t source = i;

      const uint32_t target = tinygraph_rng_bounded(rng, nodes);

      sources[i * degree + j] = source;
      targets[i * degree + j] = target;
    }
  }

  tinygraph_s graph = tinygraph_construct_from_unsorted_edges(
      sources, targets, edges);

  free(sources);
  free(targets);

  return graph;
}

TINYGRAPH_WARN_UNUSED
tinygraph_s construct_embedded_graph(tinygraph_rng_s rng, uint32_t nodes, uint32_t degree) {
  assert(rng);
  assert(nodes > 0);
  assert(degree > 1);

  const uint32_t edges = nodes * degree;

  uint32_t * const sources = malloc(edges * sizeof(uint32_t));

  if (!sources) {
    return NULL;
  }

  uint32_t * const targets = malloc(edges * sizeof(uint32_t));

  if (!targets) {
    free(sources);
    return NULL;
  }

  // Simulate what we will get with a reordering based on a
  // spatial embedding: edges (s, t) where: |s-t| is min.

  for (uint32_t i = 0; i < nodes; ++i) {
    for (uint32_t j = 0; j < degree; ++j) {
      const uint32_t source = i;

      uint32_t f = tinygraph_rng_bounded(rng, degree * 2);
      uint32_t b = tinygraph_rng_bounded(rng, degree * 2);

      uint32_t target = i - f + b;
      target = target >= nodes ? i : target;

      sources[i * degree + j] = source;
      targets[i * degree + j] = target;
    }
  }

  tinygraph_s graph = tinygraph_construct_from_unsorted_edges(
      sources, targets, edges);

  free(sources);
  free(targets);

  return graph;
}

TINYGRAPH_WARN_UNUSED
tinygraph_s construct_path_graph(uint32_t nodes) {
  assert(nodes > 1);

  const uint32_t edges = nodes - 1;

  uint32_t * const sources = malloc(edges * sizeof(uint32_t));

  if (!sources) {
    return NULL;
  }

  uint32_t * const targets = malloc(edges * sizeof(uint32_t));

  if (!targets) {
    free(sources);
    return NULL;
  }

  for (uint32_t i = 0; i < edges; ++i) {
    sources[i] = i;
    targets[i] = i + 1;
  }

  tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, edges);

  free(sources);
  free(targets);

  return graph;
}

TINYGRAPH_WARN_UNUSED
tinygraph_s construct_dense_graph(uint32_t nodes) {
  assert(nodes > 1);
  assert(nodes < (1u << 16u));

  const uint32_t edges = nodes * nodes;

  uint32_t * const sources = malloc(edges * sizeof(uint32_t));

  if (!sources) {
    return NULL;
  }

  uint32_t * const targets = malloc(edges * sizeof(uint32_t));

  if (!targets) {
    free(sources);
    return NULL;
  }

  for (uint32_t i = 0; i < nodes; ++i) {
    for (uint32_t j = 0; j < nodes; ++j) {
      sources[i * nodes + j] = i;
      targets[i * nodes + j] = j;
    }
  }

  tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, edges);

  free(sources);
  free(targets);

  return graph;
}


// Note: with sanitizers enabled (e.g.
// `make sanitize`) traversing large
// graphs becomes noticable slower.
//
// This is an address sanitizer bug
// I haven't been able to look into.
void test35(void) {
  tinygraph_rng_s rng = tinygraph_rng_construct();

  // Note: the synthetic graphs can contain
  // self-loops and multi-edges and might be
  // disconnected into separate (strongly)
  // connected components.
  const uint32_t n = 10;

  tinygraph_s graph = construct_random_graph(rng, n, 3);
  //tinygraph_s graph = construct_embedded_graph(rng, n, 3);
  //tinygraph_s graph = construct_dense_graph(n);
  //tinygraph_s graph = construct_path_graph(n);

  //tinygraph_print(graph);

  // note that number of edges depends on the graph
  // type above e.g. quadratic for dense, etc.
  assert(tinygraph_get_num_nodes(graph) == n);
  assert(tinygraph_get_num_edges(graph) == n * 3);

  // don't care about a path, just check reachability
  uint32_t * path = calloc(n, sizeof(uint32_t));

  // note that this might fail for the random and
  // embedded graphs above since they use rngs for
  // edges and there is a chance the graph is not
  // fully connected. For path graph and dense we
  // can guarantee a path by their definition.
  assert(dfs(graph, path, 0) == 1);

  free(path);

  tinygraph_destruct(graph);
  tinygraph_rng_destruct(rng);
}


// Note: with sanitizers enabled (e.g.
// `make sanitize`) the test below
// becomes noticable slower.
//
// This is an address sanitizer bug
// I haven't been able to look into.
void test36(void) {
  tinygraph_queue_s queue = tinygraph_queue_construct();
  assert(queue);

  uint64_t sum = 0;

  for (uint64_t i = 0; i < 10; ++i) {
    for (uint32_t j = 0; j < 2; ++j) {
      assert(tinygraph_queue_push(queue, j));
    }

    const uint32_t item = tinygraph_queue_pop(queue);
    assert(item == (i % 2 == 0) ? 0 : 1);

    sum += item;
  }

  assert(sum == 5);

  tinygraph_queue_destruct(queue);
}


void test37(void) {
  const uint32_t sources[5] = {0, 0, 1, 2, 3};
  const uint32_t targets[5] = {1, 2, 0, 3, 2};
  const uint16_t weights[5] = {4, 1, 1, 9, 1};

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 5);
  assert(graph);

  tinygraph_dijkstra_s ctx = tinygraph_dijkstra_construct(graph, weights);
  assert(ctx);

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, 1));
  assert(tinygraph_dijkstra_get_distance(ctx) == 4);

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, 3));
  assert(tinygraph_dijkstra_get_distance(ctx) == 10);

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, 2));
  assert(tinygraph_dijkstra_get_distance(ctx) == 1);

  assert(tinygraph_dijkstra_shortest_path(ctx, 3, 3));
  assert(tinygraph_dijkstra_get_distance(ctx) == 0);

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, 2));
  assert(tinygraph_dijkstra_get_distance(ctx) == 1);

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, 0));
  assert(tinygraph_dijkstra_shortest_path(ctx, 1, 1));
  assert(tinygraph_dijkstra_shortest_path(ctx, 2, 2));
  assert(tinygraph_dijkstra_shortest_path(ctx, 3, 3));

  const uint32_t *it, *last;

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, 3));
  assert(tinygraph_dijkstra_get_distance(ctx) == 10);
  assert(tinygraph_dijkstra_get_path(ctx, &it, &last));

  assert(*it++ == 0);
  assert(*it++ == 2);
  assert(*it++ == 3);
  assert(it == last);

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, 2));
  assert(tinygraph_dijkstra_get_distance(ctx) == 1);
  assert(tinygraph_dijkstra_get_path(ctx, &it, &last));

  assert(*it++ == 0);
  assert(*it++ == 2);
  assert(it == last);

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, 2));
  assert(tinygraph_dijkstra_get_distance(ctx) == 1);
  assert(tinygraph_dijkstra_get_path(ctx, &it, &last));

  assert(*it++ == 0);
  assert(*it++ == 2);
  assert(it == last);

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, 3));
  assert(tinygraph_dijkstra_get_distance(ctx) == 10);
  assert(tinygraph_dijkstra_get_path(ctx, &it, &last));

  assert(*it++ == 0);
  assert(*it++ == 2);
  assert(*it++ == 3);
  assert(it == last);

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, 3));
  assert(tinygraph_dijkstra_get_distance(ctx) == 10);
  assert(tinygraph_dijkstra_get_path(ctx, &it, &last));

  assert(*it++ == 0);
  assert(*it++ == 2);
  assert(*it++ == 3);
  assert(it == last);

  tinygraph_dijkstra_destruct(ctx);

  tinygraph_destruct(graph);
}


void test38(void) {
  const uint32_t sources[5] = {0, 1, 2, 3, 4};
  const uint32_t targets[5] = {0, 1, 2, 3, 4};
  const uint16_t weights[5] = {1, 1, 1, 1, 1};

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 5);
  assert(graph);

  tinygraph_dijkstra_s ctx = tinygraph_dijkstra_construct(graph, weights);
  assert(ctx);

  for (uint32_t i = 0; i < 5; ++i) {
    assert(tinygraph_dijkstra_shortest_path(ctx, i, i));
    assert(tinygraph_dijkstra_get_distance(ctx) == 0);

    const uint32_t *it, *last;
    assert(tinygraph_dijkstra_get_path(ctx, &it, &last));
    assert(it == last);
  }

  for (uint32_t i = 0; i < 5; ++i) {
    for (uint32_t j = 0; j < 5; ++j) {
      if (i == j) {
        continue;
      }

      assert(!tinygraph_dijkstra_shortest_path(ctx, i, j));
      assert(!tinygraph_dijkstra_shortest_path(ctx, j, i));
    }
  }

  tinygraph_dijkstra_destruct(ctx);
  tinygraph_destruct(graph);
}


void test39(void) {
  tinygraph_rng_s rng = tinygraph_rng_construct();
  assert(rng);

  const uint32_t n = 100;

  tinygraph_s graph = construct_random_graph(rng, n, 3);
  assert(graph);

  uint16_t* weights = malloc(n * 3 * sizeof(uint16_t));
  assert(weights);

  for (uint32_t i = 0; i < (n * 3); ++i) {
    weights[i] = tinygraph_rng_bounded(rng, 1u << 16u);
  }

  tinygraph_dijkstra_s ctx = tinygraph_dijkstra_construct(graph, weights);
  assert(ctx);

  uint32_t c = 0;

  for (uint32_t i = 0; i < 10; ++i) {
    const uint32_t s = tinygraph_rng_bounded(rng, n);
    const uint32_t t = tinygraph_rng_bounded(rng, n);

    const bool ok = tinygraph_dijkstra_shortest_path(ctx, s, t);

    c += (int)ok;
  }

  assert(n > 0 && c <= n);

  tinygraph_dijkstra_destruct(ctx);
  free(weights);
  tinygraph_destruct(graph);
  tinygraph_rng_destruct(rng);
}


void test40(void) {
  const uint32_t nodes = (1u << 16u) + 2;
  const uint32_t edges = nodes - 1;

  tinygraph_s graph = construct_path_graph(nodes);
  assert(graph);

  uint16_t* weights = malloc(edges * sizeof(uint16_t));
  assert(weights);

  for (uint32_t i = 0; i < edges; ++i) {
    weights[i] = UINT16_MAX;
  }

  tinygraph_dijkstra_s ctx = tinygraph_dijkstra_construct(graph, weights);
  assert(ctx);

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, nodes - 1));

  assert(tinygraph_dijkstra_get_distance(ctx) == UINT32_MAX);

  tinygraph_dijkstra_destruct(ctx);
  free(weights);
  tinygraph_destruct(graph);
}


void test41(void) {
  const uint32_t sources[5] = {0, 1, 2, 3, 4};
  const uint32_t targets[5] = {1, 0, 3, 2, 4};
  const uint16_t weights[5] = {1, 1, 1, 1, 1};

  const tinygraph_s graph = tinygraph_construct_from_sorted_edges(
      sources, targets, 5);
  assert(graph);

  tinygraph_dijkstra_s ctx = tinygraph_dijkstra_construct(graph, weights);
  assert(ctx);

  assert(tinygraph_dijkstra_shortest_path(ctx, 0, 1));
  assert(tinygraph_dijkstra_shortest_path(ctx, 1, 0));

  assert(tinygraph_dijkstra_shortest_path(ctx, 2, 3));
  assert(tinygraph_dijkstra_shortest_path(ctx, 3, 2));

  assert(tinygraph_dijkstra_shortest_path(ctx, 4, 4));

  assert(!tinygraph_dijkstra_shortest_path(ctx, 0, 2));
  assert(!tinygraph_dijkstra_shortest_path(ctx, 0, 3));
  assert(!tinygraph_dijkstra_shortest_path(ctx, 0, 4));

  assert(!tinygraph_dijkstra_shortest_path(ctx, 1, 2));
  assert(!tinygraph_dijkstra_shortest_path(ctx, 1, 3));
  assert(!tinygraph_dijkstra_shortest_path(ctx, 1, 4));

  assert(!tinygraph_dijkstra_shortest_path(ctx, 2, 0));
  assert(!tinygraph_dijkstra_shortest_path(ctx, 2, 1));
  assert(!tinygraph_dijkstra_shortest_path(ctx, 2, 4));

  assert(!tinygraph_dijkstra_shortest_path(ctx, 3, 0));
  assert(!tinygraph_dijkstra_shortest_path(ctx, 3, 1));
  assert(!tinygraph_dijkstra_shortest_path(ctx, 3, 4));

  assert(!tinygraph_dijkstra_shortest_path(ctx, 4, 0));
  assert(!tinygraph_dijkstra_shortest_path(ctx, 4, 1));
  assert(!tinygraph_dijkstra_shortest_path(ctx, 4, 2));
  assert(!tinygraph_dijkstra_shortest_path(ctx, 4, 3));

  tinygraph_dijkstra_destruct(ctx);
  tinygraph_destruct(graph);
}


int main(void) {
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
  test18();
  test19();
  test20();
  test21();
  test22();
  test23();
  test24();
  test25();
  test26();
  test27();
  test28();
  test29();
  test30();
  test31();
  test32();
  test33();
  test34();
  test35();
  test36();
  test37();
  test38();
  test39();
  test40();
  test41();
}
