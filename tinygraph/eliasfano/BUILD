cc_library(
  name = "eliasfano",

  srcs = ["eliasfano.c"],
  hdrs = ["eliasfano.h"],

  deps = ["//tinygraph/utils:utils", "//tinygraph/bits:bits", "//tinygraph/bitset:bitset"],

  visibility = ["//visibility:public"],
)

cc_test(
  name = "eliasfano-test",

  srcs = ["eliasfano-test.c"],

  deps = [":eliasfano"],
)
