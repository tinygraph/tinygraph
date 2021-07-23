#include "tinygraph-delta.h"


void tinygraph_delta_encode(const uint32_t *data, uint32_t *out, uint32_t n, uint32_t prev) {
  for (uint32_t i = 0; i < n; ++i) {
    const uint32_t val = data[i];

    out[i] = val - prev;
    prev = val;
  }
}


void tinygraph_delta_decode(const uint32_t *data, uint32_t *out, uint32_t n, uint32_t prev) {
  for (uint32_t i = 0; i < n; ++i) {
    const uint32_t val = data[i];

    out[i] = val + prev;
    prev += val;
  }
}
