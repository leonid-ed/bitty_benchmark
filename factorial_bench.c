#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "bitty_benchmark.h"

static uint64_t factorial_table[] = {
  1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800,
  479001600, 6227020800, 87178291200, 1307674368000, 20922789888000,
  355687428096000, 6402373705728000, 121645100408832000, 2432902008176640000
};

static uint64_t
factorial(int i)
{
  return factorial_table[i];
}

static uint64_t
factorialr(int i)
{
  if (i == 0)
    return 1;
  else
    return i * factorialr(i - 1);
}


int main(int argc, char const *argv[])
{
  NEW_BENCH(one, "Factorial measurement");
  START_BENCH(one);
  // or
  // START_NEW_BENCH(one, "Factorial measurement");

  for (int i = 0; i < 21 * 1 << 16; i++) {
    int t = i % 21;
    long f = factorialr(t);

    // STOP_BENCH(one);
    // fprintf(stderr, "%d: factorial(%d) = %ld\n", i, t, f);
    // START_BENCH(one);
  }

  STOP_BENCH_AND_PRINT_RESULTS(one);
  // or
  // STOP_BENCH(one);
  // PRINT_BENCH_RESULTS(one);

  RESTART_BENCH_WITH_DESC(one, "test IO speed");
  // or
  // SET_BENCH_DESC(one, "test IO speed");
  // RESTART_BENCH(one)
  sleep(1);
  STOP_BENCH_AND_PRINT_RESULTS_WITH_IO(one, 1025*1024);
  // or
  // STOP_BENCH(one);
  // PRINT_BENCH_RESULTS_WITH_IO_STAT(one, 1025*1024);

  return 0;
}