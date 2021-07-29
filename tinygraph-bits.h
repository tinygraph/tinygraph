#ifndef TINYGRAPH_BITS_H
#define TINYGRAPH_BITS_H

#include <stdint.h>

#include "tinygraph-utils.h"

/*
 * Efficient bits and broadword manipulation.
 *
 * We need this for our succinct rank/select
 * implementations, mostly.
 */


TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_count(uint64_t v);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_rank(uint64_t v, uint32_t n);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_select(uint64_t v, uint32_t n);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_leading0(uint64_t v);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_trailing0(uint64_t v);


#endif
