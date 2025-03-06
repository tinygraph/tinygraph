#ifndef TINYGRAPH_RNG_H
#define TINYGRAPH_RNG_H

#include <stdint.h>

#include "tinygraph-utils.h"

/*
 * Simple pseudo-random number generator.
 *
 * Note: Must not be used for cryptographic
 * use cases. It's invertible: from its output
 * you can re-construct its state and sequence.
 */


typedef struct tinygraph_rng* tinygraph_rng_s;
typedef const struct tinygraph_rng* tinygraph_rng_const_s;


TINYGRAPH_WARN_UNUSED
tinygraph_rng_s tinygraph_rng_construct(void);

TINYGRAPH_WARN_UNUSED
tinygraph_rng_s tinygraph_rng_construct_from_seed(uint32_t seed);

TINYGRAPH_WARN_UNUSED
tinygraph_rng_s tinygraph_rng_copy(tinygraph_rng_const_s rng);

void tinygraph_rng_destruct(tinygraph_rng_s rng);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_rng_random(tinygraph_rng_s rng);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_rng_bounded(tinygraph_rng_s rng, uint32_t bound);

void tinygraph_rng_print_internal(tinygraph_rng_const_s rng);

#endif
