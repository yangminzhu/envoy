#!/bin/bash


# sudo cpupower frequency-set --governor performance

bazel build -c opt --dynamic_mode=off --copt=-gmlt //test/cel:benchmark_test && ./bazel-bin/test/cel/benchmark_test
# -c opt and --dynamic_mode=off cause the code to be built in optimized mode with static linking, which is representative of production work.
# --copt=-gmlt includes some line number information which can be very helpful when profiling.

# sudo cpupower frequency-set --governor powersave
