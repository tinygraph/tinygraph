#include <assert.h>
#include <stdint.h>

#include "tinygraph/tinygraph/impl.h"


void test1() {
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


int main() {
  test1();
}
