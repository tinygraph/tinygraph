#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinygraph-utils.h"
#include "tinygraph-bitset.h"


typedef struct tinygraph_bitset {
  uint64_t *blocks;
  uint32_t blocks_len;
} tinygraph_bitset;


tinygraph_bitset* tinygraph_bitset_construct(uint32_t size) {
  tinygraph_bitset *out = malloc(sizeof(tinygraph_bitset));

  if (!out) {
    return NULL;
  }

  *out = (tinygraph_bitset){
    .blocks = NULL,
    .blocks_len = 0,
  };

  if (size == 0) {
    return out;
  }

  uint32_t num_blocks = ceil(size / 64.f);

  uint64_t *blocks = calloc(num_blocks, sizeof(uint64_t));

  if (!blocks) {
    free(out);

    return NULL;
  }

  out->blocks = blocks;
  out->blocks_len = num_blocks;

  return out;
}


tinygraph_bitset* tinygraph_bitset_copy(tinygraph_bitset_s bitset) {
  if (!bitset) {
    return bitset;
  }

  tinygraph_bitset *copy = tinygraph_bitset_construct(bitset->blocks_len);

  if (!copy) {
    return NULL;
  }

  TINYGRAPH_ASSERT(copy->blocks_len == bitset->blocks_len);

  if (bitset->blocks_len > 0) {
    TINYGRAPH_ASSERT(copy->blocks);
    TINYGRAPH_ASSERT(bitset->blocks);

    memcpy(copy->blocks, bitset->blocks, bitset->blocks_len * sizeof(uint32_t));
  }

  return copy;
}


void tinygraph_bitset_destruct(tinygraph_bitset *bitset) {
  if (!bitset) {
    return;
  }

  TINYGRAPH_ASSERT(bitset->blocks || bitset->blocks_len == 0);

  free(bitset->blocks);

  bitset->blocks = NULL;
  bitset->blocks_len = 0;

  free(bitset);
}


void tinygraph_bitset_set_at(tinygraph_bitset *bitset, uint32_t i) {
  TINYGRAPH_ASSERT(bitset);
  TINYGRAPH_ASSERT(bitset->blocks_len > 0);
  TINYGRAPH_ASSERT((i >> 6UL) < bitset->blocks_len);

  bitset->blocks[i >> 6UL] |= ((uint64_t)1 << (i & 63UL));
}


bool tinygraph_bitset_get_at(tinygraph_bitset *bitset, uint32_t i) {
  TINYGRAPH_ASSERT(bitset);
  TINYGRAPH_ASSERT(bitset->blocks_len > 0);
  TINYGRAPH_ASSERT((i >> 6UL) < bitset->blocks_len);

  return (bitset->blocks[i >> 6UL] & ((uint64_t)1 << (i & 63UL))) != 0;
}


void tinygraph_bitset_not(tinygraph_bitset_s bitset) {
  TINYGRAPH_ASSERT(bitset);

  for (uint32_t i = 0; i < bitset->blocks_len; ++i) {
    bitset->blocks[i] = ~bitset->blocks[i];
  }
}


void tinygraph_bitset_and(tinygraph_bitset_s bitset, const tinygraph_bitset_s other) {
  TINYGRAPH_ASSERT(bitset);
  TINYGRAPH_ASSERT(bitset->blocks_len == other->blocks_len);

  for (uint32_t i = 0; i < bitset->blocks_len; ++i) {
    bitset->blocks[i] = bitset->blocks[i] & other->blocks[i];
  }
}


void tinygraph_bitset_or(tinygraph_bitset_s bitset, const tinygraph_bitset_s other) {
  TINYGRAPH_ASSERT(bitset);
  TINYGRAPH_ASSERT(bitset->blocks_len == other->blocks_len);

  for (uint32_t i = 0; i < bitset->blocks_len; ++i) {
    bitset->blocks[i] = bitset->blocks[i] | other->blocks[i];
  }
}


void tinygraph_bitset_xor(tinygraph_bitset_s bitset, const tinygraph_bitset_s other) {
  TINYGRAPH_ASSERT(bitset);
  TINYGRAPH_ASSERT(bitset->blocks_len == other->blocks_len);

  for (uint32_t i = 0; i < bitset->blocks_len; ++i) {
    bitset->blocks[i] = bitset->blocks[i] ^ other->blocks[i];
  }
}


void tinygraph_bitset_print_internal(tinygraph_bitset *bitset) {
  TINYGRAPH_ASSERT(bitset);

  fprintf(stderr, "bitset internals\n");

  for (uint32_t i = 0; i < bitset->blocks_len * 64; ++i) {
    fprintf(stderr, "%ju ", (uintmax_t)tinygraph_bitset_get_at(bitset, i));
  }

  fprintf(stderr, "\n");
}
