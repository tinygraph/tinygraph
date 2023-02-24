cc_library(
  name = "queue",

  srcs = ["queue.c"],
  hdrs = ["queue.h"],

  deps = ["//tinygraph/utils:utils", "//tinygraph/stack:stack"],

  visibility = ["//visibility:public"],
)

cc_test(
  name = "queue-test",

  srcs = ["queue-test.c"],

  deps = [":queue"],
)
