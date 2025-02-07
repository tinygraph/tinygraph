#ifndef TINYGRAPH_HASH_H
#define TINYGRAPH_HASH_H

#include <stdint.h>

#include "tinygraph-utils.h"

/*
 * Simple non-cryptographic hash functions
 * for usage in hash-based data structures
 * such as hash sets and hash tables.
 *
 * Note: combining hashes is not trivial,
 * use the hash_combine function if you
 * want to create a single hash from
 * multiple hashes incrementally.
 */


TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_hash_u32(uint32_t v);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_hash_combine_u32(uint32_t seed, uint32_t h);

#endif
