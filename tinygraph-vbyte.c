#include "tinygraph-vbyte.h"


static uint32_t tinygraph_vbyte_encode_one(uint8_t *out, uint32_t value) {
  TINYGRAPH_ASSERT(out);

  const uint8_t * const first = out;

  for (; value >= 128; ++out) {
    *out = (value & 127U) | 128U;
    value = value >> 7;
  }

  *out = (uint8_t)value;
  out += 1;

  return out - first;
}


uint32_t tinygraph_vbyte_encode(const uint32_t *data, uint8_t *out, uint32_t n) {
  TINYGRAPH_ASSERT(data);
  TINYGRAPH_ASSERT(out);

  const uint8_t * const first = out;

  for (uint32_t i = 0; i < n; ++i) {
    out += tinygraph_vbyte_encode_one(out, data[i]);
  }

  return out - first;
}


uint32_t tinygraph_vbyte_decode(const uint8_t *data, uint32_t *out, uint32_t n) {
  TINYGRAPH_ASSERT(data);
  TINYGRAPH_ASSERT(out);

  (void)data;
  (void)out;
  (void)n;

  return -1;
}
