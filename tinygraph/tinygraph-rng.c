#include <stdio.h>
#include <stdlib.h>

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
  if (!rng) {
    return;
  }

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
