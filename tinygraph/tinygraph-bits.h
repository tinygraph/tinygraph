#ifndef TINYGRAPH_BITS_H
#define TINYGRAPH_BITS_H

#include <stdint.h>

#include "tinygraph-utils.h"

/*
 * Efficient bits and broadword manipulation.
 *
 * We need this for our succinct rank/select
 * implementations, mostly.
 *
 * TODO:
 *  - rank1 vs rank0: rank0(i) = i - rank1(i)
 *  - rank1(i, j) = rank1(j) - rank1(i)
 *  - rank0(i, j) = rank0(j) - rank0(i)
 */


TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_count(uint64_t v);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_rank(uint64_t v, uint32_t n);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_select(uint64_t v, uint32_t n);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_leading0_u32(uint32_t v);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_leading0_u64(uint64_t v);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_trailing0_u32(uint32_t v);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_trailing0_u64(uint64_t v);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_count_512(const uint64_t *p);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_rank_512(const uint64_t *p, uint32_t n);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_bits_select_512(const uint64_t *p, uint32_t n);


#endif
