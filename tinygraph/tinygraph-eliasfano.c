#include <math.h>

#include "tinygraph-bits.h"
#include "tinygraph-eliasfano.h"


/*
 * Gamma coding
 *
 * Write floor(log2(x)) zeros, followed by bin(x), e.g.
 * gamma(13) = 000 1101. Decoding works by counting n
 * leading zeros, if n==0 return 1. Otherwise we read
 * n + 1 bits which is the decoded binary value.
 *
 * Delta coding
 *
 * Write floor(log2(x)) + 1 as gamma code, followed by
 * by bin(x), but skip the first 1 in bin(x) because it
 * is always 1 (by design), e.g. delta(13) = 00100 101.
 * Decoding works by decoding the gamma coded length,
 * then reading length bits, pre-pending a one.
 *
 * See
 * - https://www.lx.it.pt/~mtf/Elias.pdf
 */


TINYGRAPH_WARN_UNUSED
static uint32_t tinygraph_eliasfano_floorlog2_u32(uint32_t value) {
  return 31 - tinygraph_bits_leading0_u32(value);
}

TINYGRAPH_WARN_UNUSED
static uint32_t tinygraph_eliasfano_gamma_num_bits(uint32_t value) {
  return 2 * tinygraph_eliasfano_floorlog2_u32(value) + 1;
}

TINYGRAPH_WARN_UNUSED
static uint32_t tinygraph_eliasfano_delta_num_bits(uint32_t value) {
  const uint32_t n = tinygraph_eliasfano_floorlog2_u32(value);
  return tinygraph_eliasfano_gamma_num_bits(n + 1) + (n + 1) - 1;
}


tinygraph_bitset_s tinygraph_eliasfano_encode(const uint32_t * restrict data, uint32_t n) {
  uint64_t nbits = 0;

  for (uint32_t i = 0; i < n; ++i) {
    nbits += tinygraph_eliasfano_delta_num_bits(data[i]);
  }

  tinygraph_bitset_s out = tinygraph_bitset_construct(nbits);

  uint64_t it = 0;

  for (uint32_t i = 0; i < n; ++i) {
    const uint32_t value = data[i];

    TINYGRAPH_ASSERT(value > 0);

    const uint32_t len = tinygraph_eliasfano_floorlog2_u32(value) + 1;

    // Write number of digits in the binary value (len) as gamma code

    const uint32_t lenlen = tinygraph_eliasfano_floorlog2_u32(len);

    it += lenlen;

    for (uint32_t j = 0; j < (lenlen + 1); ++j) {
      if ((len & (UINT32_C(1) << (lenlen + 1 - j - 1))) != 0) {
        tinygraph_bitset_set_at(out, it);
      }

      it += 1;
    }

    // Write binary value itself, but discard first bit (always 1)

    for (uint32_t k = 0; k < (len - 1); ++k) {
      if ((value & (UINT32_C(1) << (len - 1 - k - 1))) != 0) {
        tinygraph_bitset_set_at(out, it);
      }

      it += 1;
    }
  }

  TINYGRAPH_ASSERT(it == nbits);

  return out;
}
