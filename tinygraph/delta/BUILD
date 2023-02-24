cc_library(
  name = "delta",

  srcs = ["delta.c"],
  hdrs = ["delta.h"],

  deps = ["//tinygraph/utils:utils"],
)

cc_test(
  name = "delta-test",

  srcs = ["delta-test.c"],

  deps = [":delta"],
)
