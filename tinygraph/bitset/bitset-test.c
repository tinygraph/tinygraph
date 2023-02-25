#include <assert.h>

#include "tinygraph/bitset/bitset.h"


void test1() {
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


void test2() {
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


int main() {
  test1();
  test2();
}
