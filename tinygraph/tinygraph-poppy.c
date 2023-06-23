#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "tinygraph-bits.h"
#include "tinygraph-align.h"
#include "tinygraph-poppy.h"


typedef struct tinygraph_poppy_rank {
  uint64_t *level0;
  uint64_t level0_len;

  tinygraph_bitset_s bitset_view;
} tinygraph_poppy_rank;


tinygraph_poppy_rank* tinygraph_poppy_rank_construct(tinygraph_bitset_s bitset) {
  TINYGRAPH_ASSERT(bitset);

  tinygraph_poppy_rank *out = malloc(sizeof(tinygraph_poppy_rank));

  if (!out) {
    return NULL;
  }

  // It's important that we align the index structures at
  // the cache line boundary, which is 64 bytes nowadays
  const uint32_t cacheline = 64;

  const uint64_t *bits = tinygraph_bitset_get_data(bitset);
  const uint64_t nbits = tinygraph_bitset_get_size(bitset);

  // 512 bits basic blocks for now
  const uint64_t level0_len = nbits / 512;

  if (level0_len == 0) {
    *out = (tinygraph_poppy_rank){
      .level0 = NULL,
      .level0_len = 0,
      .bitset_view = bitset,
    };

    return out;
  }

  // TODO: implement optimizations to get 12.5% down to 3.2%
  // At the moment we only implement Fig. 3 from the paper

  uint64_t *level0 = tinygraph_align_malloc(cacheline, level0_len);

  if (!level0) {
    free(out);

    return NULL;
  }

  memset(level0, 0, level0_len);

  uint64_t csum = 0;

  for (uint64_t i = 0; i < level0_len; ++i) {
    csum += tinygraph_bits_count(bits[i * 8 + 0]);
    csum += tinygraph_bits_count(bits[i * 8 + 1]);
    csum += tinygraph_bits_count(bits[i * 8 + 2]);
    csum += tinygraph_bits_count(bits[i * 8 + 3]);
    csum += tinygraph_bits_count(bits[i * 8 + 4]);
    csum += tinygraph_bits_count(bits[i * 8 + 5]);
    csum += tinygraph_bits_count(bits[i * 8 + 6]);
    csum += tinygraph_bits_count(bits[i * 8 + 7]);

    level0[i] = csum;
  }

  *out = (tinygraph_poppy_rank){
    .level0 = level0,
    .level0_len = level0_len,
    .bitset_view = bitset,
  };

  return out;
}


void tinygraph_poppy_rank_destruct(tinygraph_poppy_rank *rank) {
  if (!rank) {
    return;
  }

  TINYGRAPH_ASSERT(rank->level0);

  tinygraph_align_free(rank->level0);

  rank->level0 = NULL;

  free(rank);
}


uint64_t tinygraph_poppy_rank_get(tinygraph_poppy_rank_s rank, uint64_t n) {
  TINYGRAPH_ASSERT(rank);
  TINYGRAPH_ASSERT(n < tinygraph_bitset_get_size(rank->bitset_view));

  const uint64_t block = n / 512;

  TINYGRAPH_ASSERT(block <= rank->level0_len);

  // pre-computed cumulative sums from index
  const uint64_t leftof = block > 0 ? rank->level0[block - 1] : 0;

  // bits within last block we need to count
  const uint64_t *bits = tinygraph_bitset_get_data(rank->bitset_view);

  uint64_t count = 0;

  for (uint64_t i = block * 8; i < block * 8 + ((n % 512) / 64); ++i) {
    count += tinygraph_bits_count(bits[i]);
  }

  count += tinygraph_bits_rank(bits[block * 8 + ((n % 512) / 64)], (n % 512) % 64);

  return leftof + count;
}


void tinygraph_poppy_rank_print_internal(tinygraph_poppy_rank_s rank) {
  TINYGRAPH_ASSERT(rank);

  fprintf(stderr, "poppy rank internals\n");

  for (uint64_t i = 0; i < rank->level0_len; ++i) {
    fprintf(stderr, " %ju", (uintmax_t)rank->level0[i]);
  }

  fprintf(stderr, "\n");
}
