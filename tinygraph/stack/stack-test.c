#include <assert.h>
#include <stdbool.h>

#include "tinygraph/stack/stack.h"


void test1() {
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


int main() {
  test1();
}
