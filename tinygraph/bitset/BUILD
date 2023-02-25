cc_library(
  name = "bitset",

  srcs = ["bitset.c"],
  hdrs = ["bitset.h"],

  deps = ["//tinygraph/utils:utils"],

  visibility = ["//visibility:public"],
)

cc_test(
  name = "bitset-test",

  srcs = ["bitset-test.c"],

  deps = [":bitset"],
)
