#include <stdio.h>
#include <stdlib.h>

#include "tinygraph-utils.h"
#include "tinygraph-array.h"
#include "tinygraph-stack.h"


typedef struct tinygraph_stack {
  tinygraph_array_s array;
} tinygraph_stack;


tinygraph_stack* tinygraph_stack_construct(void) {
  tinygraph_stack *out = malloc(sizeof(tinygraph_stack));

  if (!out) {
    return NULL;
  }

  tinygraph_array_s array = tinygraph_array_construct(0);

  if (!array) {
    free(out);

    return NULL;
  }

  *out = (tinygraph_stack) {
    .array = array,
  };

  return out;
}


tinygraph_stack* tinygraph_stack_copy(const tinygraph_stack * const stack) {
  if (!stack) {
    return NULL;
  }

  tinygraph_stack *copy = tinygraph_stack_construct();

  if (!copy) {
    return NULL;
  }

  tinygraph_array_s acopy = tinygraph_array_copy(stack->array);

  if (!acopy) {
    tinygraph_stack_destruct(copy);

    return NULL;
  }

  copy->array = acopy;

  return copy;
}


void tinygraph_stack_destruct(tinygraph_stack * const stack) {
  if (!stack) {
    return;
  }

  tinygraph_array_destruct(stack->array);

  stack->array = NULL;

  free(stack);
}


bool tinygraph_stack_reserve(tinygraph_stack * const stack, uint32_t capacity) {
  TINYGRAPH_ASSERT(stack);
  TINYGRAPH_ASSERT(stack->array);

  return tinygraph_array_reserve(stack->array, capacity);
}


uint32_t tinygraph_stack_get_top(const tinygraph_stack * const stack) {
  TINYGRAPH_ASSERT(stack);
  TINYGRAPH_ASSERT(stack->array);
  TINYGRAPH_ASSERT(!tinygraph_array_is_empty(stack->array));

  const uint32_t i = tinygraph_array_get_size(stack->array) - 1;
  const uint32_t value = tinygraph_array_get_at(stack->array, i);

  return value;
}


uint32_t tinygraph_stack_get_bottom(const tinygraph_stack * const stack) {
  TINYGRAPH_ASSERT(stack);
  TINYGRAPH_ASSERT(stack->array);
  TINYGRAPH_ASSERT(!tinygraph_array_is_empty(stack->array));

  return tinygraph_array_get_at(stack->array, 0);
}


uint32_t tinygraph_stack_get_size(const tinygraph_stack * const stack) {
  TINYGRAPH_ASSERT(stack);
  TINYGRAPH_ASSERT(stack->array);

  return tinygraph_array_get_size(stack->array);
}


uint32_t tinygraph_stack_get_capacity(const tinygraph_stack * const stack) {
  TINYGRAPH_ASSERT(stack);
  TINYGRAPH_ASSERT(stack->array);

  return tinygraph_array_get_capacity(stack->array);
}


bool tinygraph_stack_is_empty(const tinygraph_stack * const stack) {
  TINYGRAPH_ASSERT(stack);
  TINYGRAPH_ASSERT(stack->array);

  return tinygraph_array_is_empty(stack->array);
}


void tinygraph_stack_clear(tinygraph_stack * const stack) {
  TINYGRAPH_ASSERT(stack);
  TINYGRAPH_ASSERT(stack->array);

  tinygraph_array_clear(stack->array);
}


bool tinygraph_stack_push(tinygraph_stack * const stack, uint32_t value) {
  TINYGRAPH_ASSERT(stack);
  TINYGRAPH_ASSERT(stack->array);

  return tinygraph_array_push(stack->array, value);
}


uint32_t tinygraph_stack_pop(tinygraph_stack * const stack) {
  TINYGRAPH_ASSERT(stack);
  TINYGRAPH_ASSERT(stack->array);
  TINYGRAPH_ASSERT(!tinygraph_array_is_empty(stack->array));

  return tinygraph_array_pop(stack->array);
}


void tinygraph_stack_print_internal(const tinygraph_stack * const stack) {
  TINYGRAPH_ASSERT(stack);
  TINYGRAPH_ASSERT(stack->array);

  fprintf(stderr, "stack internals\n");

  tinygraph_array_print_internal(stack->array);
}
