#ifndef TINYGRAPH_QUADKEY_H
#define TINYGRAPH_QUADKEY_H

#include <stdint.h>
#include <stdbool.h>

#include "tinygraph-utils.h"

/*
 * Quadkey tiles for space-partitioning the globe.
 *
 * See
 * - https://docs.microsoft.com/en-us/bingmaps/articles/bing-maps-tile-system
 * - https://www.maptiler.com/google-maps-coordinates-tile-bounds-projection/
 */


TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_quadkey_get_level(uint64_t quadkey);

TINYGRAPH_WARN_UNUSED
bool tinygraph_quadkey_has_parent(uint64_t quadkey);

TINYGRAPH_WARN_UNUSED
uint64_t tinygraph_quadkey_get_parent(uint64_t quadkey);

TINYGRAPH_WARN_UNUSED
bool tinygraph_quadkey_is_valid(uint64_t quadkey);


#endif
