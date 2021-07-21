#ifndef TINYGRAPH_UTILS_H
#define TINYGRAPH_UTILS_H

#include <assert.h>

#ifndef TINYGRAPH_ASSERT
#define TINYGRAPH_ASSERT(x) (assert(x))
#else
#endif

#ifndef TINYGRAPH_STATIC_ASSERT
#define TINYGRAPH_STATIC_ASSERT(x) ((void)sizeof(char[1 - 2 * !(x)]))
#endif

#ifndef TINYGRAPH_WARN_UNUSED
#define TINYGRAPH_WARN_UNUSED __attribute__((warn_unused_result))
#endif

#endif
