#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "bitty_benchmark.h"

static const uint64_t _factorial_table[] = {
  1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800,
  479001600, 6227020800, 87178291200, 1307674368000, 20922789888000,
  355687428096000, 6402373705728000, 121645100408832000, 2432902008176640000
};

#define ITERATIONS (1 << 24)
#define NUMBERS (sizeof(_factorial_table)/sizeof(uint64_t))

/* Factorial algorithm with recursion */
static uint64_t
factorial(int i)
{
  return _factorial_table[i];
}

static uint64_t
factorialr(int i)
{
  if (i == 0)
    return 1;
  else
    return i * factorialr(i - 1);
}

/* Factorial algorithm with memoization */
static uint64_t _memo[NUMBERS] = { 0 };

static uint64_t
factorial_memo(int i)
{
  if (i == 0) {
    return 1;
  }
  else if (0 != _memo[i]) {
    return _memo[i];
  }
  else {
    _memo[i] = i * factorial_memo(i - 1);
    return _memo[i];
  }
}

int main(int argc, char const *argv[])
{
  NEW_BENCH(one, "Factorial with recursion");
  START_BENCH(one);
  // or
  // START_NEW_BENCH(one, "Factorial with recursion");

  for (int i = 0; i < ITERATIONS; i++) {
    int t = i % NUMBERS;
    uint64_t f = factorialr(t);
    assert(_factorial_table[t] == f);

    // STOP_BENCH(one);
    // fprintf(stderr, "%d: factorial(%d) = %ld\n", i, t, f);
    // START_BENCH(one);
  }

  STOP_BENCH_AND_PRINT_RESULTS(one);
  // or
  // STOP_BENCH(one);
  // PRINT_BENCH_RESULTS(one);

  RESTART_BENCH_WITH_DESC(one, "Test IO speed");
  // or
  // SET_BENCH_DESC(one, "test IO speed");
  // RESTART_BENCH(one);
  sleep(1);
  STOP_BENCH_AND_PRINT_RESULTS_WITH_IO(one, 1025*1024);
  // or
  // STOP_BENCH(one);
  // PRINT_BENCH_RESULTS_WITH_IO_STAT(one, 1025*1024);

  RESTART_BENCH_WITH_DESC(one, "Factorial with memoization");
  for (int i = 0; i < ITERATIONS; i++) {
    int t = i % NUMBERS;
    uint64_t f = factorial_memo(t);
    assert(_factorial_table[t] == f);
  }
  STOP_BENCH_AND_PRINT_RESULTS(one);

  return 0;
}