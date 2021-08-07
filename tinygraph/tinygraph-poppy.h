#ifndef TINYGRAPH_POPPY_H
#define TINYGRAPH_POPPY_H

#include "tinygraph-bitset.h"
#include "tinygraph-utils.h"

/*
 * The Poppy Rank/Select data structures from the paper
 *
 *  "Space-Efficient, High-Performance Rank & Select
 *   Structures on Uncompressed Bit Sequences"
 *
 * with space-overhead 3.2% (rank) and 0.39% (select)
 *
 * See
 * - https://www.cs.cmu.edu/~dga/papers/zhou-sea2013.pdf
 */


typedef struct tinygraph_poppy_rank* tinygraph_poppy_rank_s;


TINYGRAPH_WARN_UNUSED
tinygraph_poppy_rank_s tinygraph_poppy_rank_construct(tinygraph_bitset_s bitset);

void tinygraph_poppy_rank_destruct(tinygraph_poppy_rank_s rank);

TINYGRAPH_WARN_UNUSED
uint64_t tinygraph_poppy_rank_get(tinygraph_poppy_rank_s rank, uint64_t n);

void tinygraph_poppy_rank_print_internal(tinygraph_poppy_rank_s rank);



#endif
