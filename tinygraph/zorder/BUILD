cc_library(
  name = "zorder",

  srcs = ["zorder.c"],
  hdrs = ["zorder.h"],

  deps = ["//tinygraph/utils:utils"],

  visibility = ["//visibility:public"],
)

cc_test(
  name = "zorder-test",

  srcs = ["zorder-test.c"],

  deps = [":zorder"],
)
