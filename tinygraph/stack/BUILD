cc_library(
  name = "stack",

  srcs = ["stack.c"],
  hdrs = ["stack.h"],

  deps = ["//tinygraph/utils:utils", "//tinygraph/array:array"],

  visibility = ["//visibility:public"],
)

cc_test(
  name = "stack-test",

  srcs = ["stack-test.c"],

  deps = [":stack"],
)
