#include <assert.h>

#include "tinygraph/bits/bits.h"


void test1() {
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
}


int main() {
  test1();
}
