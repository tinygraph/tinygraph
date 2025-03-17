#include <stdbool.h>

#include "tinygraph-vbyte.h"


TINYGRAPH_WARN_UNUSED
static inline uint32_t tinygraph_vbyte_encode_one(uint8_t * restrict out, uint32_t value) {
  TINYGRAPH_ASSERT(out);

  uint32_t i = 0;

  for (; i < 5; ++i) {
    if (value < 128) {
      break;
    }

    out[i] = (value & 127) | 128;

    value = value >> 7;
  }

  out[i] = value;

  return i + 1;
}

TINYGRAPH_WARN_UNUSED
static inline uint32_t tinygraph_vbyte_decode_one(const uint8_t * restrict data, uint32_t * restrict out) {
  TINYGRAPH_ASSERT(data);
  TINYGRAPH_ASSERT(out);

  uint32_t i = 0;
  uint32_t value = 0;

  for (; i < 5; ++i) {
    const uint32_t b = data[i];

    value |= (uint32_t)(b & 127) << (i * 7);

    if (!(b & 128)) {
      break;
    }
  }

  *out = value;

  return i + 1;
}


uint32_t tinygraph_vbyte_encode(const uint32_t * restrict data, uint8_t * restrict out, uint32_t n) {
  if (n == 0) {
    return 0;
  }

  TINYGRAPH_ASSERT(data);
  TINYGRAPH_ASSERT(out);

  const uint8_t * const first = out;

  for (uint32_t i = 0; i < n; ++i) {
    out += tinygraph_vbyte_encode_one(out, data[i]);
  }

  const uint32_t len = out - first;

  TINYGRAPH_ASSERT(len <= n * sizeof(uint32_t) * 5);

  return len;
}


uint32_t tinygraph_vbyte_decode(const uint8_t * restrict data, uint32_t * restrict out, uint32_t n) {
  if (n == 0) {
    return 0;
  }

  TINYGRAPH_ASSERT(data);
  TINYGRAPH_ASSERT(out);

  uint32_t len = 0;

  for (uint32_t i = 0; i < n; ++i) {
    len += tinygraph_vbyte_decode_one(data + len, out);
    out += 1;
  }

  TINYGRAPH_ASSERT(len <= n * sizeof(uint32_t) * 5);

  return len;
}
