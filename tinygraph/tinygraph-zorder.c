#include "tinygraph-zorder.h"

#ifdef __BMI2__
#include <x86intrin.h>
#endif

/*
 * The z-order space filling curve for n dimensions
 * works by bit-interleaving the n different values.
 *
 * We implement bit-interleaving recursively for all
 * data types by means of combining the even and odd
 * bit-blasted patterns. We support 2d only for now.
 *
 * Bit-blasting 4 bits into 8 bits: 1010 -> 01000100
 *
 * For the 4-to-8 bit-blasting we define a small
 * look up table; bigger data types then build up
 * from the four bit look up table bottom to top.
 */

#ifdef __BMI2__

uint32_t tinygraph_zorder_encode32(uint16_t x, uint16_t y) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint32_t) == sizeof(unsigned int));

  return _pdep_u32(y, UINT32_C(0xaaaaaaaa)) | _pdep_u32(x, UINT32_C(0x55555555));
}

uint64_t tinygraph_zorder_encode64(uint32_t x, uint32_t y) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));

  return _pdep_u64(y, UINT64_C(0xaaaaaaaaaaaaaaaa)) | _pdep_u64(x, UINT64_C(0x5555555555555555));
}

void tinygraph_zorder_decode32(uint32_t z, uint16_t * restrict x, uint16_t * restrict y) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint32_t) == sizeof(unsigned int));

  *y = _pext_u32(z, UINT32_C(0xaaaaaaaa));
  *x = _pext_u32(z, UINT32_C(0x55555555));
}

void tinygraph_zorder_decode64(uint64_t z, uint32_t * restrict x, uint32_t * restrict y) {
  TINYGRAPH_STATIC_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));

  *y = _pext_u64(z, UINT64_C(0xaaaaaaaaaaaaaaaa));
  *x = _pext_u64(z, UINT64_C(0x5555555555555555));
}

#else // __BMI2__

static const uint8_t tinygraph_bitblast4[16] = {
   0x0,  0x1,  0x4,  0x5, 0x10, 0x11, 0x14, 0x15,
  0x40, 0x41, 0x44, 0x45, 0x50, 0x51, 0x54, 0x55
};

TINYGRAPH_WARN_UNUSED
static inline uint16_t tinygraph_bitblast8(uint8_t x) {
  uint16_t v = 0;

  v |= (uint16_t)tinygraph_bitblast4[(x >> 0) & 0xf] << 0;
  v |= (uint16_t)tinygraph_bitblast4[(x >> 4) & 0xf] << 8;

  return v;
}

TINYGRAPH_WARN_UNUSED
static inline uint32_t tinygraph_bitblast16(uint16_t x) {
  uint32_t v = 0;

  v |= (uint32_t)tinygraph_bitblast8((x >> 0) & 0xff) << 0;
  v |= (uint32_t)tinygraph_bitblast8((x >> 8) & 0xff) << 16;

  return v;
}

TINYGRAPH_WARN_UNUSED
static inline uint64_t tinygraph_bitblast32(uint32_t x) {
  uint64_t v = 0;

  v |= (uint64_t)tinygraph_bitblast16((x >> 0) & 0xffff) << 0;
  v |= (uint64_t)tinygraph_bitblast16((x >> 16) & 0xffff) << 32;

  return v;
}


TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_zorder_encode32(uint16_t x, uint16_t y) {
  return (tinygraph_bitblast16(y) << 1) | tinygraph_bitblast16(x);
}


TINYGRAPH_WARN_UNUSED
uint64_t tinygraph_zorder_encode64(uint32_t x, uint32_t y) {
  return (tinygraph_bitblast32(y) << 1) | tinygraph_bitblast32(x);
}


static const uint8_t tinygraph_bitpack8[] = {
   [0x0] = 0,   [0x1] = 1,   [0x4] = 2,   [0x5] = 3,
  [0x10] = 4,  [0x11] = 5,  [0x14] = 6,  [0x15] = 7,
  [0x40] = 8,  [0x41] = 9,  [0x44] = 10, [0x45] = 11,
  [0x50] = 12, [0x51] = 13, [0x54] = 14, [0x55] = 15
};

TINYGRAPH_WARN_UNUSED
static inline uint8_t tinygraph_bitpack16(uint16_t x) {
  uint8_t v = 0;

  v |= tinygraph_bitpack8[(x & 0x00ff) >> 0] << 0;
  v |= tinygraph_bitpack8[(x & 0xff00) >> 8] << 4;

  return v;
}

TINYGRAPH_WARN_UNUSED
static inline uint16_t tinygraph_bitpack32(uint32_t x) {
  uint16_t v = 0;

  v |= tinygraph_bitpack16((x & UINT32_C(0x0000ffff)) >> 0) << 0;
  v |= tinygraph_bitpack16((x & UINT32_C(0xffff0000)) >> 16) << 8;

  return v;
}

TINYGRAPH_WARN_UNUSED
static inline uint32_t tinygraph_bitpack64(uint64_t x) {
  uint32_t v = 0;

  v |= tinygraph_bitpack32((x & UINT64_C(0x00000000ffffffff)) >> 0) << 0;
  v |= tinygraph_bitpack32((x & UINT64_C(0xffffffff00000000)) >> 32) << 16;

  return v;
}


void tinygraph_zorder_decode32(uint32_t z, uint16_t * restrict x, uint16_t * restrict y) {
  TINYGRAPH_ASSERT(y);
  TINYGRAPH_ASSERT(x);

  *x = tinygraph_bitpack32(z & UINT32_C(0x55555555));
  *y = tinygraph_bitpack32((z >> 1) & UINT32_C(0x55555555));
}


void tinygraph_zorder_decode64(uint64_t z, uint32_t * restrict x, uint32_t * restrict y) {
  TINYGRAPH_ASSERT(y);
  TINYGRAPH_ASSERT(x);

  *x = tinygraph_bitpack64(z & UINT64_C(0x5555555555555555));
  *y = tinygraph_bitpack64((z >> 1) & UINT64_C(0x5555555555555555));
}

#endif // __BMI2__
