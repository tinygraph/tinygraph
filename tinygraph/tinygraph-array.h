#ifndef TINYGRAPH_ARRAY_H
#define TINYGRAPH_ARRAY_H

#include <stdint.h>
#include <stdbool.h>

#include "tinygraph-utils.h"

/*
 * Simple dynamically growing array; if you know its
 * required size in advance, make sure to reserve().
 *
 * The array grows on demand with a factor of 1.5 to
 * allow for memory reuse after four reallocations.
 *
 * See
 * - https://github.com/facebook/folly/blob/ff841baa23/folly/docs/FBVector.md
 * - https://github.com/facebook/folly/blob/ff841baa23/folly/docs/small_vector.md
 *
 * Todo: benchmark and experiment a hybrid design
 * where the first n items are stored on the stack.
 */

typedef struct tinygraph_array* tinygraph_array_s;
typedef const struct tinygraph_array* tinygraph_array_const_s;


TINYGRAPH_WARN_UNUSED
tinygraph_array_s tinygraph_array_construct(uint32_t size);

TINYGRAPH_WARN_UNUSED
tinygraph_array_s tinygraph_array_copy(tinygraph_array_const_s array);

void tinygraph_array_destruct(tinygraph_array_s array);

TINYGRAPH_WARN_UNUSED
bool tinygraph_array_reserve(tinygraph_array_s array, uint32_t capacity);

TINYGRAPH_WARN_UNUSED
bool tinygraph_array_resize(tinygraph_array_s array, uint32_t size);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_array_get_at(tinygraph_array_const_s array, uint32_t i);

void tinygraph_array_set_at(tinygraph_array_s array, uint32_t i, uint32_t value);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_array_get_size(tinygraph_array_const_s array);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_array_get_capacity(tinygraph_array_const_s array);

TINYGRAPH_WARN_UNUSED
bool tinygraph_array_is_empty(tinygraph_array_const_s array);

void tinygraph_array_clear(tinygraph_array_s array);

TINYGRAPH_WARN_UNUSED
bool tinygraph_array_push(tinygraph_array_s array, uint32_t value);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_array_pop(tinygraph_array_s array);

void tinygraph_array_print_internal(tinygraph_array_const_s array);


#endif
