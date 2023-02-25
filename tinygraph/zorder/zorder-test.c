#include <assert.h>
#include <stdint.h>

#include "tinygraph/zorder/zorder.h"


void test1() {
  const uint16_t xs[5] = {0, 2, 800, 7, 123};
  const uint16_t ys[5] = {0, 2, 600, 6, 456};
  const uint32_t zs[5] = {0, 12, 861824, 61, 177605};

  for (uint32_t i = 0; i < 5; ++i) {
    assert(tinygraph_zorder_encode32(xs[i], ys[i]) == zs[i]);
    assert(tinygraph_zorder_encode64(xs[i], ys[i]) == zs[i]);

    uint16_t x, y;
    tinygraph_zorder_decode32(zs[i], &x, &y);
    assert(x == xs[i]);
    assert(y == ys[i]);

    uint32_t xx, yy;
    tinygraph_zorder_decode64(zs[i], &xx, &yy);
    assert(xx == xs[i]);
    assert(yy == ys[i]);
  }
}


int main() {
  test1();
}
