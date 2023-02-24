cc_library(
  name = "vbyte",

  srcs = ["vbyte.c"],
  hdrs = ["vbyte.h"],

  deps = ["//tinygraph/utils:utils"],

  visibility = ["//visibility:public"],
)

cc_test(
  name = "vbyte-test",

  srcs = ["vbyte-test.c"],

  deps = [":vbyte"],
)
