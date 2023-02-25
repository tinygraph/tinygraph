#include <assert.h>
#include <stdint.h>

#include "tinygraph/vbyte/vbyte.h"


void test1() {
  const uint32_t original[5] = {0, 256+1, 65536+1, 16777216+1, 4294967295};

  const uint8_t expected[5 * 5] = {
    0,
    129, 2,
    129, 128, 4,
    129, 128, 128, 8,
    255, 255, 255, 255, 15,
  };

  uint8_t encoded[5 * 5];

  const uint32_t n = tinygraph_vbyte_encode(original, 5, encoded);
  assert(n == 15);

  for (uint32_t i = 0; i < n; ++i) {
    assert(encoded[i] == expected[i]);
  }

  uint32_t decoded[5];

  const uint32_t m = tinygraph_vbyte_decode(expected, 5, decoded);
  assert(m == n);

  for (uint32_t i = 0; i < 5; ++i) {
    assert(decoded[i] == original[i]);
  }
}



int main() {
  test1();
}
