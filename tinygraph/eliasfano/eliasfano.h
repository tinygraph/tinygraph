#ifndef TINYGRAPH_ELIASFANO_H
#define TINYGRAPH_ELIASFANO_H

#include <stdint.h>

#include "tinygraph/bitset/bitset.h"
#include "tinygraph/utils/utils.h"

/*
 * Quasi succinct Elias-Fano coding.
 */


TINYGRAPH_WARN_UNUSED
tinygraph_bitset_s tinygraph_eliasfano_encode(const uint32_t *data, uint32_t n);


#endif
