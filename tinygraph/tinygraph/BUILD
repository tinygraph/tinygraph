cc_library(
  name = "tinygraph",

  srcs = ["tinygraph.c"],
  hdrs = ["tinygraph.h"],

  implementation_deps = ["//tinygraph/utils:utils", ":tinygraph-impl"],
  copts = ["-fvisibility=hidden"],
  linkopts = ["-fvisibility=hidden"],

  visibility = ["//visibility:public"],
)

cc_test(
  name = "tinygraph-test",

  srcs = ["tinygraph-test.c"],

  deps = [":tinygraph"],
)

cc_library(
  name = "tinygraph-impl",

  srcs = ["impl.c"],
  hdrs = ["impl.h"],

  deps = ["//tinygraph/utils:utils"],
)

cc_test(
  name = "tinygraph-impl-test",

  srcs = ["impl-test.c"],

  deps = [":tinygraph-impl"],
)

cc_test(
  name = "tinygraph-traversal-test",

  srcs = ["traversal-test.c"],

  deps = [
    ":tinygraph",
    "//tinygraph/bitset:bitset",
    "//tinygraph/stack:stack",
    "//tinygraph/queue:queue",
  ],
)
