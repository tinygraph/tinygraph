#include "tinygraph/bits/bits.h"
#include "tinygraph/quadkey/quadkey.h"

#define TINYGRAPH_R_EARTH 6378137
#define TINYGRAPH_MIN_LAT -85.05112878
#define TINYGRAPH_MAX_LAT 85.05112878
#define TINYGRAPH_MIN_LNG -180
#define TINYGRAPH_MAX_LNG 180


uint32_t tinygraph_quadkey_get_level(uint64_t quadkey) {
  return -1;
}


bool tinygraph_quadkey_has_parent(uint64_t quadkey) {
  return false;
}


uint64_t tinygraph_quadkey_get_parent(uint64_t quadkey) {
  return -1;
}


bool tinygraph_quadkey_is_valid(uint64_t quadkey) {
  return false;
}
