#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "tinygraph/tinygraph/tinygraph.h"
#include "tinygraph/bitset/bitset.h"
#include "tinygraph/queue/queue.h"
#include "tinygraph/stack/stack.h"


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


void test1() {
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


int main() {
  test1();
}
