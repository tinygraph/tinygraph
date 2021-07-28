#include "tinygraph-utils.h"
#include "tinygraph-bits.h"

#ifdef TINYGRAPH_HAS_BMI2
#include <x86intrin.h>
#endif

/*
 * If we can not use the builtin functions, we should
 * look into the broadword ideas from the paper below.
 *
 * See
 * - https://vigna.di.unimi.it/ftp/papers/Broadword.pdf
 */

uint32_t tinygraph_bits_count(uint64_t v) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));
  return __builtin_popcountll(v);
}

#ifdef TINYGRAPH_HAS_BMI2

uint32_t tinygraph_bits_find(uint64_t v, uint32_t n) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));
  TINYGRAPH_ASSERT(n < tinygraph_bits_count(v));

  return tinygraph_bits_trailing0(_pdep_u64(UINT64_C(1) << n, v));
}

#else // TINYGRAPH_HAS_BMI2

uint32_t tinygraph_bits_find(uint64_t v, uint32_t n) {
  TINYGRAPH_STATIC_ASSERT(false); // TODO: implement non BMI2 version
  return 64;
}

#endif // TINYGRAPH_HAS_BMI2


uint32_t tinygraph_bits_leading0(uint64_t v) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));

  if (v == 0) {
    return 64;
  }

  return __builtin_clzll(v);
}

uint32_t tinygraph_bits_trailing0(uint64_t v) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));

  if (v == 0) {
    return 64;
  }

  return __builtin_ctzll(v);
}
