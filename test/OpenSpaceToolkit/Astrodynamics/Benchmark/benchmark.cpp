#include "./scenarios/scenario001.hpp"
#include "./scenarios/scenario002.hpp"
#include "benchmark/benchmark.h"

static void scenario001(benchmark::State &state) {
  for (auto _ : state) {
    // Suppress optimization otherwise this line is removed by DCE
    int i = 10;
    benchmark::DoNotOptimize(i);
    benchmark::DoNotOptimize(scenario001(i));
  }
}

static void scenario002(benchmark::State &state) {
  for (auto _ : state) {
    int i = 20;
    benchmark::DoNotOptimize(i);
    benchmark::DoNotOptimize(scenario002(i));
  }
}


// Register the function as a benchmark
BENCHMARK(scenario001);
BENCHMARK(scenario002);

// Run the benchmark
BENCHMARK_MAIN();
