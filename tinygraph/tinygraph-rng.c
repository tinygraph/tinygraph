#include <stdio.h>
#include <stdlib.h>

#ifdef __AVX2__
#include <x86intrin.h>
#include <string.h>
#include "tinygraph-align.h"
#endif

#include "tinygraph-rng.h"

/*
 * Non-cryptographic pseudo-random number generator
 * based on the PCG design guidelines: a linear
 * congruential generator with output permutation.
 *
 * See
 *
 * - https://www.pcg-random.org
 *
 * - https://www.pcg-random.org/pdf/hmc-cs-2014-0905.pdf
 *   Section 6.3.4 "PCG-RXS-M-XS"
 */

#ifdef __AVX2__


typedef struct tinygraph_rng {
  TINYGRAPH_ALIGN(32) __m256i state;
  TINYGRAPH_ALIGN(32) uint32_t cache[8];
  uint32_t cache_idx;
} tinygraph_rng;


tinygraph_rng* tinygraph_rng_construct(void) {
  tinygraph_rng *out = tinygraph_align_malloc(32, sizeof(tinygraph_rng));

  if (!out) {
    return NULL;
  }

  // Default state initializer, will always return
  // the same sequence of pseudo-random numbers.

  // We compute and store 8x 32bit random numbers
  // in one go, the idx then keeps track of which
  // to return to the user or when to re-compute

  *out = (tinygraph_rng) {
    .state = _mm256_set1_epi32(UINT32_C(0x46b56677)),
    .cache = {0, 0, 0, 0, 0, 0, 0, 0},
    .cache_idx = -1,
  };

  // step once so that the custom increment
  // can have an effect on the state already
  const uint32_t v = tinygraph_rng_random(out);
  (void)v; // ignore, just step state

  // make sure we re-compute next time
  out->cache_idx = -1; // force re-computation

  return out;
}


tinygraph_rng* tinygraph_rng_construct_from_seed(uint32_t seed) {
  tinygraph_rng *out = tinygraph_rng_construct();

  if (!out) {
    return NULL;
  }

  // Mixing in the seed follows the upstream pcg
  // technique to set state to zero, step, add the
  // seed to the state, step, and go from there.

  out->state = _mm256_setzero_si256();

  out->cache_idx = -1; // force re-computation
  const uint32_t v1 = tinygraph_rng_random(out);
  (void)v1; // ignore, just step state

  out->state = _mm256_add_epi32(out->state, _mm256_set1_epi32(seed));

  out->cache_idx = -1; // force re-computation
  const uint32_t v2 = tinygraph_rng_random(out);
  (void)v2; // ignore, just step state

  // make sure we re-compute next time
  out->cache_idx = -1; // force re-computation

  return out;
}


tinygraph_rng* tinygraph_rng_copy(const tinygraph_rng * const rng) {
  TINYGRAPH_ASSERT(rng);

  tinygraph_rng *copy = tinygraph_rng_construct();

  if (!copy) {
    return NULL;
  }

  copy->state = rng->state;

  memcpy(copy->cache, rng->cache, 8 * sizeof(uint32_t));

  copy->cache_idx = rng->cache_idx;

  return copy;
}


void tinygraph_rng_destruct(tinygraph_rng * const rng) {
  tinygraph_align_free(rng);
}


uint32_t tinygraph_rng_random(tinygraph_rng * const rng) {
  TINYGRAPH_ASSERT(rng);

  // We generate 8 random values in one go and cache them,
  // if we still have values in the cache return one of them,
  // otherwise we need to re-compute another 8 random values.

  if (rng->cache_idx < 8) {
    const uint32_t rv = rng->cache[rng->cache_idx];
    rng->cache_idx += 1;
    return rv;
  }

  // we permute the previous state and step the rng state
  const __m256i state = rng->state;

  // The multiplier is fixed for all 8 rngs, but
  // the increment is different (and must be odd)
  // to select different streams of each rng. This
  // allows us to e.g. seed each rng with the same
  // user seed and still get distinct sequences.
  // Increments randomly drawn, odd numbers with:
  // [randint(0, 2**32-1)|1 for _ in range(8)]

  const __m256i mul = _mm256_set1_epi32(UINT32_C(747796405));
  const __m256i inc = _mm256_set_epi32(
      UINT32_C(3278650715), UINT32_C(882272065), UINT32_C(2351453077), UINT32_C(995526969),
      UINT32_C(609188517), UINT32_C(2725112281), UINT32_C(3210862681), UINT32_C(67173735));

  // step all 8 states at the same time
  rng->state = _mm256_add_epi32(_mm256_mullo_epi32(rng->state, mul), inc);

  // permute the previous state, and not the updated rng->state
  const __m256i shift_amount = _mm256_add_epi32(
      _mm256_srli_epi32(state, 28), _mm256_set1_epi32(4));
  const __m256i shifted_state = _mm256_srlv_epi32(state, shift_amount);
  const __m256i word = _mm256_mullo_epi32(
      _mm256_xor_si256(shifted_state, state),
      _mm256_set1_epi32(UINT32_C(277803737)));
  const __m256i final_shift = _mm256_srli_epi32(word, 22);
  const __m256i result = _mm256_xor_si256(final_shift, word);

  _mm256_store_si256((__m256i *)rng->cache, result);

  rng->cache_idx = 1;  // below we return the 0th, next is at 1

  return rng->cache[0];
}


uint32_t tinygraph_rng_bounded(tinygraph_rng * const rng, uint32_t bound) {
  TINYGRAPH_ASSERT(rng);
  TINYGRAPH_ASSERT(bound > 0);

  uint32_t t = -bound % bound;

  for (;;) {
    uint32_t r = tinygraph_rng_random(rng);

    if (r >= t)
      return r % bound;
  }
}


void tinygraph_rng_print_internal(const tinygraph_rng * const rng) {
  TINYGRAPH_ASSERT(rng);

  fprintf(stderr, "rng internals\n");

  fprintf(stderr, "state:\n");

  TINYGRAPH_ALIGN(32) uint32_t tmp[8];

  _mm256_store_si256((__m256i *)tmp, rng->state);

  for (uint32_t i = 0; i < 8; ++i) {
    fprintf(stderr, "%ju\n", (uintmax_t)tmp[i]);
  }
}


#else // non-avx2 fallback


typedef struct tinygraph_rng {
  uint32_t state;
} tinygraph_rng;


tinygraph_rng* tinygraph_rng_construct(void) {
  tinygraph_rng *out = malloc(sizeof(tinygraph_rng));

  if (!out) {
    return NULL;
  }

  // Default state initializer, will always return
  // the same sequence of pseudo-random numbers.

  *out = (tinygraph_rng) {
    .state = UINT32_C(0x46b56677),
  };

  // step once so that the custom increment
  // can have an effect on the state already
  const uint32_t v = tinygraph_rng_random(out);
  (void)v; // ignore, just step state

  return out;
}


tinygraph_rng* tinygraph_rng_construct_from_seed(uint32_t seed) {
  tinygraph_rng *out = malloc(sizeof(tinygraph_rng));

  if (!out) {
    return NULL;
  }

  // Mixing in the seed follows the upstream pcg
  // technique to set state to zero, step, add the
  // seed to the state, step, and go from there.

  *out = (tinygraph_rng) {
    .state = 0,
  };

  const uint32_t v1 = tinygraph_rng_random(out);
  (void)v1; // ignore, just step state

  out->state += seed;

  const uint32_t v2 = tinygraph_rng_random(out);
  (void)v2; // ignore, just step state

  return out;
}


tinygraph_rng* tinygraph_rng_copy(const tinygraph_rng * const rng) {
  TINYGRAPH_ASSERT(rng);

  tinygraph_rng *copy = tinygraph_rng_construct();

  if (!copy) {
    return NULL;
  }

  copy->state = rng->state;

  return copy;
}


void tinygraph_rng_destruct(tinygraph_rng * const rng) {
  free(rng);
}


uint32_t tinygraph_rng_random(tinygraph_rng * const rng) {
  TINYGRAPH_ASSERT(rng);

  // we permute the previous state and step the rng state
  const uint32_t state = rng->state;

  // use the pcg default multiplier and increment for 32 bit
  rng->state = rng->state * UINT32_C(747796405) + UINT32_C(2891336453);

  // permute the previous state, and not the updated rng->state
  uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * UINT32_C(277803737);

  return (word >> 22u) ^ word;
}


uint32_t tinygraph_rng_bounded(tinygraph_rng * const rng, uint32_t bound) {
  TINYGRAPH_ASSERT(rng);
  TINYGRAPH_ASSERT(bound > 0);

  uint32_t t = -bound % bound;

  for (;;) {
    uint32_t r = tinygraph_rng_random(rng);

    if (r >= t)
      return r % bound;
  }
}


void tinygraph_rng_print_internal(const tinygraph_rng * const rng) {
  TINYGRAPH_ASSERT(rng);

  fprintf(stderr, "rng internals\n");

  fprintf(stderr, "state: %ju\n", (uintmax_t)rng->state);
}

#endif  // non-avx2 fallback
