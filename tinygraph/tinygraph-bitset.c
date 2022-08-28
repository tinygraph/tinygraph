#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tinygraph-utils.h"
#include "tinygraph-bitset.h"


typedef struct tinygraph_bitset {
  uint64_t *blocks;
  uint64_t blocks_len;
  uint64_t size;
} tinygraph_bitset;


tinygraph_bitset* tinygraph_bitset_construct(uint64_t size) {
  tinygraph_bitset *out = malloc(sizeof(tinygraph_bitset));

  if (!out) {
    return NULL;
  }

  *out = (tinygraph_bitset){
    .blocks = NULL,
    .blocks_len = 0,
    .size = size,
  };

  if (size == 0) {
    return out;
  }

  uint64_t num_blocks = ceil(size / 64.f);

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

    memcpy(copy->blocks, bitset->blocks, bitset->blocks_len * sizeof(uint64_t));
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


void tinygraph_bitset_set_at(tinygraph_bitset *bitset, uint64_t i) {
  TINYGRAPH_ASSERT(bitset);
  TINYGRAPH_ASSERT(bitset->blocks_len > 0);
  TINYGRAPH_ASSERT((i >> 6) < bitset->blocks_len);

  bitset->blocks[i >> 6] |= (UINT64_C(1) << (i & UINT64_C(63)));
}


bool tinygraph_bitset_get_at(tinygraph_bitset *bitset, uint64_t i) {
  TINYGRAPH_ASSERT(bitset);
  TINYGRAPH_ASSERT(bitset->blocks_len > 0);
  TINYGRAPH_ASSERT((i >> 6) < bitset->blocks_len);

  return (bitset->blocks[i >> 6] & (UINT64_C(1) << (i & UINT64_C(63)))) != 0;
}


uint64_t tinygraph_bitset_get_size(tinygraph_bitset_s bitset) {
  TINYGRAPH_ASSERT(bitset);

  return bitset->size;
}


uint64_t* tinygraph_bitset_get_data(tinygraph_bitset_s bitset) {
  TINYGRAPH_ASSERT(bitset);

  return bitset->blocks;
}


void tinygraph_bitset_clear(tinygraph_bitset_s bitset) {
  TINYGRAPH_ASSERT(bitset);

  if (bitset->blocks_len < 1) {
    return;
  }

  memset(bitset->blocks, 0, bitset->blocks_len * sizeof(uint64_t));
}


void tinygraph_bitset_not(tinygraph_bitset_s bitset) {
  TINYGRAPH_ASSERT(bitset);

  for (uint64_t i = 0; i < bitset->blocks_len; ++i) {
    bitset->blocks[i] = ~bitset->blocks[i];
  }
}


void tinygraph_bitset_and(tinygraph_bitset_s bitset, const tinygraph_bitset_s other) {
  TINYGRAPH_ASSERT(bitset);
  TINYGRAPH_ASSERT(bitset->blocks_len == other->blocks_len);

  for (uint64_t i = 0; i < bitset->blocks_len; ++i) {
    bitset->blocks[i] = bitset->blocks[i] & other->blocks[i];
  }
}


void tinygraph_bitset_or(tinygraph_bitset_s bitset, const tinygraph_bitset_s other) {
  TINYGRAPH_ASSERT(bitset);
  TINYGRAPH_ASSERT(bitset->blocks_len == other->blocks_len);

  for (uint64_t i = 0; i < bitset->blocks_len; ++i) {
    bitset->blocks[i] = bitset->blocks[i] | other->blocks[i];
  }
}


void tinygraph_bitset_xor(tinygraph_bitset_s bitset, const tinygraph_bitset_s other) {
  TINYGRAPH_ASSERT(bitset);
  TINYGRAPH_ASSERT(bitset->blocks_len == other->blocks_len);

  for (uint64_t i = 0; i < bitset->blocks_len; ++i) {
    bitset->blocks[i] = bitset->blocks[i] ^ other->blocks[i];
  }
}


void tinygraph_bitset_print_internal(tinygraph_bitset *bitset) {
  TINYGRAPH_ASSERT(bitset);

  fprintf(stderr, "bitset internals\n");

  for (uint64_t i = 0; i < tinygraph_bitset_get_size(bitset); ++i) {
    fprintf(stderr, "%ju ", (uintmax_t)tinygraph_bitset_get_at(bitset, i));
  }

  fprintf(stderr, "\n");
}
