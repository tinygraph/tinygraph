#ifndef TINYGRAPH_ARRAY_H
#define TINYGRAPH_ARRAY_H

#include <stdint.h>
#include <stdbool.h>

#include "tinygraph-utils.h"

typedef struct tinygraph_array* tinygraph_array_s;


TINYGRAPH_WARN_UNUSED
tinygraph_array_s tinygraph_array_construct(uint32_t size);

TINYGRAPH_WARN_UNUSED
tinygraph_array_s tinygraph_array_copy(tinygraph_array_s array);

void tinygraph_array_destruct(tinygraph_array_s array);

TINYGRAPH_WARN_UNUSED
bool tinygraph_array_reserve(tinygraph_array_s array, uint32_t capacity);

TINYGRAPH_WARN_UNUSED
bool tinygraph_array_resize(tinygraph_array_s array, uint32_t size);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_array_get_at(tinygraph_array_s array, uint32_t i);

void tinygraph_array_set_at(tinygraph_array_s array, uint32_t i, uint32_t value);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_array_get_size(tinygraph_array_s array);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_array_get_capacity(tinygraph_array_s array);

TINYGRAPH_WARN_UNUSED
bool tinygraph_array_is_empty(tinygraph_array_s array);

void tinygraph_array_clear(tinygraph_array_s array);

TINYGRAPH_WARN_UNUSED
bool tinygraph_array_push(tinygraph_array_s array, uint32_t value);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_array_pop(tinygraph_array_s array);

void tinygraph_array_print_internal(tinygraph_array_s array);


#endif
