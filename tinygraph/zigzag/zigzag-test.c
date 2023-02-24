#include <assert.h>
#include <stdint.h>

#include "tinygraph/zigzag/zigzag.h"


void test1() {
  const int32_t values[] = {0, -1, 1, -2, 2147483647, -2147483648};

  for (uint32_t i = 0; i < 6; ++i) {
    const int32_t value = values[i];

    const uint32_t encoded = tinygraph_zigzag_encode(value);
    const int32_t decoded = tinygraph_zigzag_decode(encoded);

    assert(decoded == value);
  }
}


int main() {
  test1();
}
