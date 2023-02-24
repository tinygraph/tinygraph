#ifndef TINYGRAPH_STACK_H
#define TINYGRAPH_STACK_H

#include <stdint.h>
#include <stdbool.h>

#include "tinygraph/utils/utils.h"

/*
 * Simple stack to push to the top and pop
 * from the top efficiently. If you know
 * about its size, make sure to reserve().
 */

typedef struct tinygraph_stack* tinygraph_stack_s;


TINYGRAPH_WARN_UNUSED
tinygraph_stack_s tinygraph_stack_construct();

TINYGRAPH_WARN_UNUSED
tinygraph_stack_s tinygraph_stack_copy(tinygraph_stack_s stack);

void tinygraph_stack_destruct(tinygraph_stack_s stack);

TINYGRAPH_WARN_UNUSED
bool tinygraph_stack_reserve(tinygraph_stack_s stack, uint32_t capacity);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_stack_get_top(tinygraph_stack_s stack);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_stack_get_bottom(tinygraph_stack_s stack);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_stack_get_size(tinygraph_stack_s stack);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_stack_get_capacity(tinygraph_stack_s stack);

TINYGRAPH_WARN_UNUSED
bool tinygraph_stack_is_empty(tinygraph_stack_s stack);

void tinygraph_stack_clear(tinygraph_stack_s stack);

TINYGRAPH_WARN_UNUSED
bool tinygraph_stack_push(tinygraph_stack_s stack, uint32_t value);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_stack_pop(tinygraph_stack_s stack);

void tinygraph_stack_print_internal(tinygraph_stack_s stack);


#endif
