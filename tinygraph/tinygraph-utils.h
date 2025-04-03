#ifndef TINYGRAPH_UTILS_H
#define TINYGRAPH_UTILS_H

#include <assert.h>

#ifndef TINYGRAPH_ASSERT
#ifndef NDEBUG
#define TINYGRAPH_ASSERT(x) (assert(x))
#else
#define TINYGRAPH_ASSERT(x)
#endif
#else
#endif

#ifndef TINYGRAPH_STATIC_ASSERT
#define TINYGRAPH_STATIC_ASSERT(x) ((void)sizeof(char[1 - 2 * !(x)]))
#endif

#ifndef TINYGRAPH_WARN_UNUSED
#define TINYGRAPH_WARN_UNUSED __attribute__((warn_unused_result))
#endif

#ifndef TINYGRAPH_UNREACHABLE
#define TINYGRAPH_UNREACHABLE __builtin_unreachable
#endif

#ifndef TINYGRAPH_LIKELY
#define TINYGRAPH_LIKELY(expr) __builtin_expect(!!(expr), 1)
#endif

#ifndef TINYGRAPH_UNLIKELY
#define TINYGRAPH_UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#endif

#ifndef TINYGRAPH_PREFETCH
#define TINYGRAPH_PREFETCH(p, rw, hint) __builtin_prefetch(p, rw, hint)
#endif

#endif
