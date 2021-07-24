#include "tinygraph-vbyte.h"


static uint32_t tinygraph_vbyte_encode_one(uint8_t *out, uint32_t value) {
  TINYGRAPH_ASSERT(out);

  const uint8_t * const first = out;

  for (; value >= 128; ++out) {
    *out = (value & 127) | 128;
    value = value >> 7;
  }

  *out = (uint8_t)value;
  out += 1;

  return out - first;
}

static uint32_t tinygraph_vbyte_decode_one(const uint8_t *data, uint32_t *out) {
  TINYGRAPH_ASSERT(data);
  TINYGRAPH_ASSERT(out);

  uint32_t value = 0;

  const uint8_t b0 = *(data + 0);
  value |= (uint32_t)(b0 & 127) << 0;

  if (!(b0 & 128)) {
    *out = value;
    return 1;
  }

  const uint8_t b1 = *(data + 1);
  value |= (uint32_t)(b1 & 127) << 7;

  if (!(b1 & 128)) {
    *out = value;
    return 2;
  }

  const uint8_t b2 = *(data + 2);
  value |= (uint32_t)(b2 & 127) << 14;

  if (!(b2 & 128)) {
    *out = value;
    return 3;
  }

  const uint8_t b3 = *(data + 3);
  value |= (uint32_t)(b3 & 127) << 21;

  if (!(b3 & 128)) {
    *out = value;
    return 4;
  }

  const uint8_t b4 = *(data + 4);
  value |= (uint32_t)(b4 & 127) << 28;

  if (!(b4 & 128)) {
    *out = value;
    return 5;
  }

  return 5;
}


uint32_t tinygraph_vbyte_encode(const uint32_t *data, uint32_t n, uint8_t *out) {
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


uint32_t tinygraph_vbyte_decode(const uint8_t *data, uint32_t n, uint32_t *out) {
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
