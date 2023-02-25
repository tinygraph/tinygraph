#ifndef TINYGRAPH_BITSET_H
#define TINYGRAPH_BITSET_H

#include <stdint.h>
#include <stdbool.h>

#include "tinygraph/utils/utils.h"

/*
 * Simple bitset to mask items with true/false.
 *
 * Todo: the bitset is static and requires us
 * knowing about the size upfront; good enough
 * for now?
 */

typedef struct tinygraph_bitset* tinygraph_bitset_s;


TINYGRAPH_WARN_UNUSED
tinygraph_bitset_s tinygraph_bitset_construct(uint64_t size);

TINYGRAPH_WARN_UNUSED
tinygraph_bitset_s tinygraph_bitset_copy(tinygraph_bitset_s bitset);

void tinygraph_bitset_destruct(tinygraph_bitset_s bitset);

void tinygraph_bitset_set_at(tinygraph_bitset_s bitset, uint64_t i);

TINYGRAPH_WARN_UNUSED
bool tinygraph_bitset_get_at(tinygraph_bitset_s bitset, uint64_t i);

TINYGRAPH_WARN_UNUSED
uint64_t tinygraph_bitset_get_size(tinygraph_bitset_s bitset);

void tinygraph_bitset_clear(tinygraph_bitset_s bitset);

void tinygraph_bitset_not(tinygraph_bitset_s bitset);
void tinygraph_bitset_and(tinygraph_bitset_s bitset, tinygraph_bitset_s other);
void tinygraph_bitset_or(tinygraph_bitset_s bitset, tinygraph_bitset_s other);
void tinygraph_bitset_xor(tinygraph_bitset_s bitset, tinygraph_bitset_s other);

void tinygraph_bitset_print_internal(tinygraph_bitset_s bitset);


#endif
