cc_library(
  name = "bits",

  srcs = ["bits.c"],
  hdrs = ["bits.h"],

  deps = ["//tinygraph/utils:utils"],

  visibility = ["//visibility:public"],
)

cc_test(
  name = "bits-test",

  srcs = ["bits-test.c"],

  deps = [":bits"],
)
