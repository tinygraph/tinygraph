#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

#include "tinygraph-array.h"
#include "tinygraph-utils.h"
#include "tinygraph-bits.h"
#include "tinygraph-rs.h"

// A succinct rank-select datastructure over a bitset.
//
//   rank1(bits, n): sum of 1s in bits before position n
//
//   select1(bits, n): position of nth 1 bit set in bits
//
// We implement a very simple rank-select datastructure
// with room for improvement in terms of space and runtime
// but the benefit is a relatively simple implementation.
//
// For rank: We make use of recent learnings from [1] and
// [3]: We use basic blocks of 512 bits (cache line size)
// and within these basic blocks make use of repeated
// popcnt instructions. This is similar to rank9 from [4]
// but without the 9-bit second level index and with the
// popcnt hardware instruction instead of broadword tricks.
// With a cache line aligned bitvector we should see a max
// of two cache misses: one for the basic block and then one
// for the remainder in the bitvector itself. Potential
// for improvements are listed in [1] and [2] e.g. the
// use of multi level rank inventories.
//
// For select: We make use of recent learnings from [1] and
// [3] where we sample every e.g. 8192'th bit set and from
// there use a scan over the rank inventory to speed up
// the search. The final cache line select works using
// the recent pdep hardware instruction. Note that worst
// case runtime is in O(n) for adversarily bitvector cases
// such as: a very large bit vector but only one bit set at
// the very start and one bit set at the very end.
// Potential for improvements are listed in [1] and [2].
// In addition one idea could be tuning that 8192 constant
// e.g. based on the bitvector's load factor (how many bits
// are set) to make sure scanning ranks stays reasonable.
// The simple-select datastructure in [3] is especially
// tuned for select (does not rank) and seems to be strong
// in runtime but also needs more space.
//
//
// [1] SPIDER: Improved Succinct Rank and Select Performance
// https://arxiv.org/abs/2405.05214
//
// [2] Engineering Compact Data Structures for Rank and Select Queries on Bit Vectors
// https://arxiv.org/abs/2206.01149
//
// [3] A Fast x86 Implementation of Select
// https://arxiv.org/abs/1706.00990
//
// [4] Broadword Implementation of Rank/Select Queries
// https://vigna.di.unimi.it/ftp/papers/Broadword.pdf


typedef struct tinygraph_rs {

  // a view onto the read-only bitset, max 2^32 bits supported
  tinygraph_bitset_const_s bitset;

  // the pre-computed ranks for each cache line of size 512 bits
  tinygraph_array_s ranks;

  // position of the 8192'th set bit, to speed up select search
  tinygraph_array_s samples;

  // total bits set in bitset, mostly used for checks
  uint32_t popcount;

} tinygraph_rs;


tinygraph_rs* tinygraph_rs_construct(tinygraph_bitset_const_s bitset) {
  TINYGRAPH_ASSERT(bitset);
  TINYGRAPH_ASSERT(tinygraph_bitset_get_size(bitset) <= UINT32_MAX);

  // TODO: For now this only works with cache line aligned memory and
  // a bitset the size of a cache line multiple; for internal use this
  // is fine and makes our code simpler and more efficient. In case you
  // want to re-use this code you'll need to handle the residuals on
  // your own, especially all rank/select_512 functions need changing.
  TINYGRAPH_ASSERT(tinygraph_bitset_get_size(bitset) % 512 == 0);
  TINYGRAPH_ASSERT(((uintptr_t)tinygraph_bitset_get_data(bitset) % 64) == 0);

  tinygraph_rs *out = malloc(sizeof(tinygraph_rs));

  if (!out) {
    return NULL;
  }

  // rank1

  const uint64_t * const data = tinygraph_bitset_get_data(bitset);
  const uint32_t n = tinygraph_bitset_get_size(bitset) / 512;

  tinygraph_array_s ranks = tinygraph_array_construct(n);

  if (!ranks) {
    free(out);

    return NULL;
  }

  tinygraph_array_s samples = tinygraph_array_construct(0);

  if (!samples) {
    tinygraph_array_destruct(ranks);
    free(out);

    return NULL;
  }

  // Our data structure can be constructed in a single linear scan
  // over the bitvector: for every block of 512 bits we store the
  // computed rank up to this position in the ranks array. And for
  // every 8192th bit set we store its position in the samples array.
  //
  // By iterating one cache line at a time we can make use of our
  // highly tuned rank512 and select512 implementations.

  uint32_t rank = 0;
  uint32_t sample = 0;

  for (uint32_t i = 0; i < n; ++i) {
    const uint32_t count = tinygraph_bits_count_512(data + (i * 8));

    rank += count;
    tinygraph_array_set_at(ranks, i, rank);

    sample += count;

    if (sample >= 8192) {
      const uint32_t p = tinygraph_bits_select_512(data + (i * 8), 8192 - (sample - count) - 1);

      const bool ok = tinygraph_array_push(samples, i * 512 + p);

      if (!ok) {
        tinygraph_array_destruct(ranks);
        tinygraph_array_destruct(samples);
        free(out);
        return NULL;
      }

      sample = sample - 8192;
    }
  }

  // There might be a residual left smaller than a cache line
  // of 512 bits. We take care of this case explicitly here
  // for the select samples.

  const uint32_t r = tinygraph_bitset_get_size(bitset) % 512;

  if (r != 0) {
    const uint32_t count = tinygraph_bits_rank_512(data + (n * 8), r);

    rank += count;
    sample += count;

    if (sample >= 8192) {
      const uint32_t p = tinygraph_bits_select_512(data + (n * 8), 8192 - (sample - count) - 1);

      const bool ok = tinygraph_array_push(samples, n * 512 + p);

      if (!ok) {
        tinygraph_array_destruct(ranks);
        tinygraph_array_destruct(samples);
        free(out);
        return NULL;
      }
    }
  }

  *out = (tinygraph_rs) {
    .bitset = bitset,
    .ranks = ranks,
    .samples = samples,
    .popcount = rank,
  };

  return out;
}


void tinygraph_rs_destruct(tinygraph_rs * const rs) {
  if (!rs) {
    return;
  }

  tinygraph_array_destruct(rs->ranks);
  tinygraph_array_destruct(rs->samples);

  rs->ranks = NULL;
  rs->samples = NULL;

  free(rs);
}


uint32_t tinygraph_rs_rank(const tinygraph_rs * const rs, uint32_t n) {
  TINYGRAPH_ASSERT(rs);
  TINYGRAPH_ASSERT(rs->bitset);
  TINYGRAPH_ASSERT(rs->ranks);
  TINYGRAPH_ASSERT(n <= tinygraph_bitset_get_size(rs->bitset));

  if (n > tinygraph_bitset_get_size(rs->bitset)) {
    TINYGRAPH_UNREACHABLE();
  }

  // We can compute rank from two parts: the pre-computed
  // ranks for each cache line of size 512, and in case
  // we rank inside a cache line adding the remaining
  // residual from ranking the bits inside the cache line.

  const uint64_t * const data = tinygraph_bitset_get_data(rs->bitset);

  const uint32_t p = n / 512;
  const uint32_t r = n % 512;

  if (p == 0) {
    return tinygraph_bits_rank_512(data, r);
  } else {
    return tinygraph_array_get_at(rs->ranks, p - 1)
      + tinygraph_bits_rank_512(data + (p * 8), r);
  }
}


uint32_t tinygraph_rs_select(const tinygraph_rs * const rs, uint32_t n) {
  TINYGRAPH_ASSERT(rs);
  TINYGRAPH_ASSERT(rs->bitset);
  TINYGRAPH_ASSERT(rs->ranks);
  TINYGRAPH_ASSERT(rs->samples);
  TINYGRAPH_ASSERT(n < rs->popcount);

  if (n >= rs->popcount) {
    TINYGRAPH_UNREACHABLE();
  }

  // To select the nth set bit on the bitset
  //
  //  1. Use the samples array to find two positions
  //     where the nth bit has to be within. This range
  //     in general is unrestricted and in adversarial
  //     cases the range might span the whole bitset.
  //     That is why select is in O(n) but fast in
  //     practice and works fine for our use cases.
  //
  //  2. Use the ranks array to search for a block in
  //     the range where the nth bit must occur.
  //
  //  3. Within the 512 bit block where the nth bit
  //     must occur, rely on our highly tuned
  //     select512 implementations.

  const uint64_t * const data = tinygraph_bitset_get_data(rs->bitset);

  if (tinygraph_array_get_size(rs->ranks) == 0) {
    return tinygraph_bits_select_512(data, n);
  }

  // TODO: There is room for improvement when scanning
  // over the ranks array e.g. using a different search.
  //
  // - We are running a linear scan over the ranks array
  //   but might benefit from a binary search or a more
  //   clever heuristic based on the distance
  //
  // - Even in our linear scan we could benefit from
  //   skipping multiple steps based on the knowledge
  //   that in a single block there can only ever be
  //   512 bits set at best; if we search for the nth
  //   bit with n > 512 we can skip one block (repeat)

  const uint32_t p = n / 8192;
  const uint32_t first = (p == 0) ? 0 : tinygraph_array_get_at(rs->samples, p - 1);

  uint32_t count = 0;

  for (uint32_t it = first / 512, last = tinygraph_array_get_size(rs->ranks); it < last; ++it) {
    const uint32_t block = tinygraph_array_get_at(rs->ranks, it);

    if (block > n) {
      return (it * 512) + tinygraph_bits_select_512(data + (it * 8), n - count);
    }

    count = block;
  }

  TINYGRAPH_ASSERT(false);
  TINYGRAPH_UNREACHABLE();

  return tinygraph_bitset_get_size(rs->bitset);
}


void tinygraph_rs_print_internal(const tinygraph_rs * const rs) {
  TINYGRAPH_ASSERT(rs);
  TINYGRAPH_ASSERT(rs->ranks);
  TINYGRAPH_ASSERT(rs->samples);

  fprintf(stderr, "rs internals\n");

  tinygraph_array_print_internal(rs->ranks);
  tinygraph_array_print_internal(rs->samples);
}
