#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#include "tinygraph/array/array.h"


void test1() {
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


void test2() {
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


void test3() {
  tinygraph_array_s array = tinygraph_array_construct(32);
  assert(array);

  for (uint32_t i = 0; i < tinygraph_array_get_size(array); ++i) {
    assert(tinygraph_array_get_at(array, i) == 0);
  }

  tinygraph_array_destruct(array);
}


int main() {
  test1();
  test2();
  test3();
}
