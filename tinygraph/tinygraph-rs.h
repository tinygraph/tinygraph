#ifndef TINYGRAPH_RS_H
#define TINYGRAPH_RS_H

#include <stdint.h>

#include "tinygraph-bitset.h"
#include "tinygraph-utils.h"

/*
 * Succinct rank-select data structure
 * answering rank1 and select1 queries
 * over a bitset.
 */

typedef struct tinygraph_rs* tinygraph_rs_s;
typedef const struct tinygraph_rs* tinygraph_rs_const_s;


TINYGRAPH_WARN_UNUSED
tinygraph_rs_s tinygraph_rs_construct(tinygraph_bitset_const_s bitset);

void tinygraph_rs_destruct(tinygraph_rs_s rs);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_rs_rank(tinygraph_rs_const_s rs, uint32_t n);

TINYGRAPH_WARN_UNUSED
uint32_t tinygraph_rs_select(tinygraph_rs_const_s rs, uint32_t n);

void tinygraph_rs_print_internal(tinygraph_rs_const_s rs);


#endif
