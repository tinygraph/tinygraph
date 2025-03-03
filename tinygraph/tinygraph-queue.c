#include <stdio.h>
#include <stdlib.h>

#include "tinygraph-utils.h"
#include "tinygraph-stack.h"
#include "tinygraph-queue.h"


typedef struct tinygraph_queue {
  tinygraph_stack_s lhs;
  tinygraph_stack_s rhs;
} tinygraph_queue;


TINYGRAPH_WARN_UNUSED
static inline bool tinygraph_queue_refill(tinygraph_queue * const queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  if (!tinygraph_stack_reserve(queue->rhs,
        tinygraph_stack_get_size(queue->lhs))) {

    return false;
  }


  while (!tinygraph_stack_is_empty(queue->lhs)) {
    const uint32_t tos = tinygraph_stack_pop(queue->lhs);

    if (!tinygraph_stack_push(queue->rhs, tos)) {
      return false;
    }
  }

  return true;
}


tinygraph_queue* tinygraph_queue_construct(void) {
  tinygraph_queue *out = malloc(sizeof(tinygraph_queue));

  if (!out) {
    return NULL;
  }

  tinygraph_stack_s lhs = tinygraph_stack_construct();

  if (!lhs) {
    free(out);

    return NULL;
  }

  tinygraph_stack_s rhs = tinygraph_stack_construct();

  if (!rhs) {
    tinygraph_stack_destruct(lhs);

    free(out);

    return NULL;
  }

  *out = (tinygraph_queue) {
    .lhs = lhs,
    .rhs = rhs,
  };

  return out;
}


tinygraph_queue* tinygraph_queue_copy(const tinygraph_queue * const queue) {
  if (!queue) {
    return NULL;
  }

  tinygraph_queue *copy = tinygraph_queue_construct();

  if (!copy) {
    return NULL;
  }

  tinygraph_stack_s lcopy = tinygraph_stack_copy(queue->lhs);

  if (!lcopy) {
    tinygraph_queue_destruct(copy);

    return NULL;
  }

  tinygraph_stack_s rcopy = tinygraph_stack_copy(queue->rhs);

  if (!rcopy) {
    tinygraph_stack_destruct(lcopy);

    tinygraph_queue_destruct(copy);

    return NULL;
  }

  copy->lhs = lcopy;
  copy->rhs = rcopy;

  return copy;
}


void tinygraph_queue_destruct(tinygraph_queue * const queue) {
  if (!queue) {
    return;
  }

  tinygraph_stack_destruct(queue->lhs);
  tinygraph_stack_destruct(queue->rhs);

  queue->lhs = NULL;
  queue->rhs = NULL;

  free(queue);
}


bool tinygraph_queue_reserve(tinygraph_queue * const queue, uint32_t capacity) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  if (!tinygraph_stack_reserve(queue->lhs, capacity)) {
    return false;
  }

  if (!tinygraph_stack_reserve(queue->rhs, capacity)) {
    return false;
  }

  return true;
}


uint32_t tinygraph_queue_get_front(const tinygraph_queue * const queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);
  TINYGRAPH_ASSERT(!(tinygraph_stack_is_empty(queue->lhs)
        && tinygraph_stack_is_empty(queue->rhs)));

  if (!tinygraph_stack_is_empty(queue->rhs)) {
    return tinygraph_stack_get_top(queue->rhs);
  }

  return tinygraph_stack_get_bottom(queue->lhs);
}


uint32_t tinygraph_queue_get_back(const tinygraph_queue * const queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);
  TINYGRAPH_ASSERT(!(tinygraph_stack_is_empty(queue->lhs)
        && tinygraph_stack_is_empty(queue->rhs)));

  if (!tinygraph_stack_is_empty(queue->lhs)) {
    return tinygraph_stack_get_top(queue->lhs);
  }

  return tinygraph_stack_get_bottom(queue->rhs);
}


uint32_t tinygraph_queue_get_size(const tinygraph_queue * const queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  const uint32_t lsize = tinygraph_stack_get_size(queue->lhs);
  const uint32_t rsize = tinygraph_stack_get_size(queue->rhs);

  return lsize + rsize;
}


uint32_t tinygraph_queue_get_capacity(const tinygraph_queue * const queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  const uint32_t lcapacity = tinygraph_stack_get_capacity(queue->lhs);
  const uint32_t rcapacity = tinygraph_stack_get_capacity(queue->lhs);

  return lcapacity < rcapacity ? lcapacity : rcapacity;
}


bool tinygraph_queue_is_empty(const tinygraph_queue * const queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  if (!tinygraph_stack_is_empty(queue->lhs)) {
    return false;
  }

  if (!tinygraph_stack_is_empty(queue->rhs)) {
    return false;
  }

  return true;
}


void tinygraph_queue_clear(tinygraph_queue * const queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  tinygraph_stack_clear(queue->lhs);
  tinygraph_stack_clear(queue->rhs);
}


bool tinygraph_queue_push(tinygraph_queue * const queue, uint32_t value) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  return tinygraph_stack_push(queue->lhs, value);
}


uint32_t tinygraph_queue_pop(tinygraph_queue * const queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);
  TINYGRAPH_ASSERT(!(tinygraph_stack_is_empty(queue->lhs)
        && tinygraph_stack_is_empty(queue->rhs)));

  if (tinygraph_stack_is_empty(queue->rhs)) {
    const bool ok = tinygraph_queue_refill(queue);
    TINYGRAPH_ASSERT(ok);  // stack is corrupted
  }

  TINYGRAPH_ASSERT(tinygraph_stack_is_empty(queue->lhs));
  TINYGRAPH_ASSERT(!tinygraph_stack_is_empty(queue->rhs));

  return tinygraph_stack_pop(queue->rhs);
}


void tinygraph_queue_print_internal(const tinygraph_queue * const queue) {
  TINYGRAPH_ASSERT(queue);
  TINYGRAPH_ASSERT(queue->lhs);
  TINYGRAPH_ASSERT(queue->rhs);

  fprintf(stderr, "queue internals\n");

  tinygraph_stack_print_internal(queue->lhs);
  tinygraph_stack_print_internal(queue->rhs);
}
