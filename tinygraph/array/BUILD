cc_library(
  name = "array",

  srcs = ["array.c"],
  hdrs = ["array.h"],

  deps = ["//tinygraph/utils:utils"],

  visibility = ["//visibility:public"],
)

cc_test(
  name = "array-test",

  srcs = ["array-test.c"],

  deps = [":array"],
)
