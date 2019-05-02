#!/bin/bash


# sudo cpupower frequency-set --governor performance
~/bin/bazel build -c opt --dynamic_mode=off --copt=-gmlt //test/cel:benchmark_test && ./bazel-bin/test/cel/benchmark_test
# sudo cpupower frequency-set --governor powersave

