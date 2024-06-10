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
 *
 * Todo
 * - faster popcnt e.g. https://arxiv.org/abs/1611.07612
 */

uint32_t tinygraph_bits_count(uint64_t v) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));
  return __builtin_popcountll(v);
}

uint32_t tinygraph_bits_rank(uint64_t v, uint32_t n) {
  TINYGRAPH_ASSERT(n <= UINT32_C(64));

  if (n > 64) {
    TINYGRAPH_UNREACHABLE();
  }

  if (n == 0) {
    return 0;
  }

  return tinygraph_bits_count(v << (UINT32_C(64) - n));
}

#ifdef TINYGRAPH_HAS_BMI2

uint32_t tinygraph_bits_select(uint64_t v, uint32_t n) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));
  TINYGRAPH_ASSERT(n < tinygraph_bits_count(v));

  if (n >= 64) {
    TINYGRAPH_UNREACHABLE();
  }

  // "A Fast x86 Implementation of Select", Pandey, Bender, Johnson
  // https://arxiv.org/abs/1706.00990
  return tinygraph_bits_trailing0_u64(_pdep_u64(UINT64_C(1) << n, v));
}

#else // TINYGRAPH_HAS_BMI2

// TODO: this non-BMI2 fallback is pretty slow; can we do better?
// use select impl. idea from https://vigna.di.unimi.it/ftp/papers/Broadword.pdf
uint32_t tinygraph_bits_select(uint64_t v, uint32_t n) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));
  TINYGRAPH_ASSERT(n < tinygraph_bits_count(v));

  if (n >= 64) {
    TINYGRAPH_UNREACHABLE();
  }

  uint32_t set = 0;

  for (uint32_t i = 0; i < 64; ++i) {
    if ((v & (UINT64_C(1) << i)) != 0) {
      if (set == n) {
        return i;
      }

      set += 1;
    }
  }

  return 64;
}

#endif // TINYGRAPH_HAS_BMI2


uint32_t tinygraph_bits_leading0_u32(uint32_t v) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint32_t) == sizeof(unsigned int));

  if (v == 0) {
    return 32;
  }

  return __builtin_clz(v);
}

uint32_t tinygraph_bits_leading0_u64(uint64_t v) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));

  if (v == 0) {
    return 64;
  }

  return __builtin_clzll(v);
}

uint32_t tinygraph_bits_trailing0_u32(uint32_t v) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint32_t) == sizeof(unsigned int));

  if (v == 0) {
    return 32;
  }

  return __builtin_ctz(v);
}

uint32_t tinygraph_bits_trailing0_u64(uint64_t v) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));

  if (v == 0) {
    return 64;
  }

  return __builtin_ctzll(v);
}

uint32_t tinygraph_bits_count_512(const uint64_t *p) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));
  TINYGRAPH_ASSERT(p);

  //TINYGRAPH_PREFETCH(p);

  // Manually unroll loop and accumulate into separare registers,
  // otherwise we're running into a false dependency on popcount's
  // destination register; for an explanation see e.g.
  // http://danluu.com/assembly-intrinsics/

  uint32_t count[4] = {0, 0, 0, 0};

  count[0] += tinygraph_bits_count(p[0]);
  count[1] += tinygraph_bits_count(p[1]);
  count[2] += tinygraph_bits_count(p[2]);
  count[3] += tinygraph_bits_count(p[3]);
  count[0] += tinygraph_bits_count(p[4]);
  count[1] += tinygraph_bits_count(p[5]);
  count[2] += tinygraph_bits_count(p[6]);
  count[3] += tinygraph_bits_count(p[7]);

  return count[0] + count[1] + count[2] + count[3];
}

uint32_t tinygraph_bits_rank_512(const uint64_t *p, uint32_t n) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));
  TINYGRAPH_ASSERT(n <= UINT32_C(512));
  TINYGRAPH_ASSERT(p);

  if (n > 512) {
    TINYGRAPH_UNREACHABLE();
  }

  if (n == 0) {
    return 0;
  }

  //TINYGRAPH_PREFETCH(p);

  if (n == 512) {
    return tinygraph_bits_count_512(p);
  }

  uint32_t count = 0;

  for (uint32_t i = 0; i < (n / 64); ++i) {
    count += tinygraph_bits_count(*p++);
  }

  count += tinygraph_bits_rank(*p, n % 64);

  return count;
}

uint32_t tinygraph_bits_select_512(const uint64_t *p, uint32_t n) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));
  TINYGRAPH_ASSERT(n < tinygraph_bits_count_512(p));
  TINYGRAPH_ASSERT(p);

  if (n >= 512) {
    TINYGRAPH_UNREACHABLE();
  }

  //TINYGRAPH_PREFETCH(p);

  // "A Fast x86 Implementation of Select", Pandey, Bender, Johnson
  // https://arxiv.org/abs/1706.00990

  uint32_t count = 0;

  for (uint32_t i = 0; i < 7; ++i) {
    uint32_t block = tinygraph_bits_count(p[i]);

    if ((count + block) > n) {
      return i * 64 + tinygraph_bits_select(p[i], n - count);
    }

    count += block;
  }

  // no popcount needed for last block, has to be in there
  return 7 * 64 + tinygraph_bits_select(p[7], n - count);
}
