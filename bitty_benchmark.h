/*
 *  Bitty Benchmark
 */

#ifndef BITTY_BENCHMARK_H
#define BITTY_BENCHMARK_H

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>

#define USE_BENCH_STATE
#define USE_COLOR_CODES

#ifdef USE_COLOR_CODES
  #define BB_CKNRM  "\x1B[0m"
  #define BB_CKRED  "\x1B[31m"
  #define BB_CKGRN  "\x1B[32m"
  #define BB_CKYEL  "\x1B[33m"
  #define BB_CKBLU  "\x1B[34m"
  #define BB_CKMAG  "\x1B[35m"
  #define BB_CKCYN  "\x1B[36m"
  #define BB_CKWHT  "\x1B[37m"
  #define BB_CRESET "\033[0m"
#else
  #define BB_CKNRM  ""
  #define BB_CKRED  ""
  #define BB_CKGRN  ""
  #define BB_CKYEL  ""
  #define BB_CKBLU  ""
  #define BB_CKMAG  ""
  #define BB_CKCYN  ""
  #define BB_CKWHT  ""
  #define BB_CRESET ""
#endif // USE_COLOR_CODES

/*
 *  Get the difference between finish value and start value in ms
 */
static inline double
bb_get_time_diff_ms(const struct timespec *start, const struct timespec *finish)
{
  if (!start || !finish) return 0;
  double rs = (finish->tv_sec - start->tv_sec) * 1e3;  /* sec to ms */
  rs += (finish->tv_nsec - start->tv_nsec) * 1e-6;     /* ns  to ms */
  return rs;
}

#ifdef USE_BENCH_STATE
enum bench_point_state {
  BENCH_STATE_STOPPED = 0,
  BENCH_STATE_STARTED
};
#endif

struct bench_stat {
  struct timespec monotime_start;
  struct timespec monotime_finish;
  struct timespec pcputime_start;
  struct timespec pcputime_finish;
  char *desc;
#ifdef USE_BENCH_STATE
  double elapsed_monotime_ms;
  double elapsed_pcputime_ms;
  unsigned char state;
#endif
};

#ifdef USE_BENCH_STATE
#define GET_BENCH_ELAPSED_PCPUTIME_MS(id)                                      \
  bench_ ## id .elapsed_pcputime_ms
#else
#define GET_BENCH_ELAPSED_PCPUTIME_MS(id)                                      \
  bb_get_time_diff_ms(                                                         \
    &bench_ ## id .pcputime_start,                                             \
    &bench_ ## id .pcputime_finish                                             \
  )
#endif

#ifdef USE_BENCH_STATE
#define GET_BENCH_ELAPSED_MONOTIME_MS(id)                                      \
  bench_ ## id .elapsed_monotime_ms
#else
#define GET_BENCH_ELAPSED_MONOTIME_MS(id)                                      \
  bb_get_time_diff_ms(                                                         \
    &bench_ ## id .monotime_start,                                             \
    &bench_ ## id .monotime_finish                                             \
  )
#endif

#define GET_BENCH_DESC(id)                                                     \
  (bench_ ## id .desc == NULL ? "(none)" : bench_ ## id .desc)

#define SET_BENCH_DESC(id, description)                                        \
  do {                                                                         \
    bench_ ## id .desc = description;                                          \
  } while(0)

#define GET_BENCH_IO_SPEED_MB(id, size)                                        \
  ((double)(size) / 1024 / 1024 / GET_BENCH_ELAPSED_MONOTIME_MS(id) * 1e3)


#ifdef USE_BENCH_STATE
#define NEW_BENCH(id, description)                                             \
  struct bench_stat bench_ ## id = {                                           \
    .desc = description,                                                       \
    .elapsed_monotime_ms = 0,                                                  \
    .elapsed_pcputime_ms = 0,                                                  \
    .state = BENCH_STATE_STOPPED                                               \
  };                                                                           \
  do {} while(0)
#else
#define NEW_BENCH(id, description)                                             \
  struct bench_stat bench_ ## id = {                                           \
    .desc = description                                                        \
  };                                                                           \
  do {} while(0)
#endif

#define NEW_BENCH1(id) NEW_BENCH(id, NULL)

#ifdef USE_BENCH_STATE
#define START_BENCH(id)                                                        \
  do {                                                                         \
    assert(bench_ ## id .state == BENCH_STATE_STOPPED);                        \
    bench_ ## id .state = BENCH_STATE_STARTED;                                 \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &bench_ ## id .pcputime_start);    \
    clock_gettime(CLOCK_MONOTONIC, &bench_ ## id .monotime_start);             \
  } while(0)
#else
#define START_BENCH(id)                                                        \
  do {                                                                         \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &bench_ ## id .pcputime_start);    \
    clock_gettime(CLOCK_MONOTONIC, &bench_ ## id .monotime_start);             \
  } while(0)
#endif

#ifdef USE_BENCH_STATE
#define STOP_BENCH(id)                                                         \
  do {                                                                         \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &bench_ ## id .pcputime_finish);   \
    clock_gettime(CLOCK_MONOTONIC, &bench_ ## id .monotime_finish);            \
    assert(bench_ ## id .state == BENCH_STATE_STARTED);                        \
    bench_ ## id .state = BENCH_STATE_STOPPED;                                 \
    bench_ ## id .elapsed_monotime_ms +=                                       \
      bb_get_time_diff_ms(                                                     \
        &bench_ ## id .monotime_start,                                         \
        &bench_ ## id .monotime_finish                                         \
      );                                                                       \
    bench_ ## id .elapsed_pcputime_ms +=                                       \
      bb_get_time_diff_ms(                                                     \
        &bench_ ## id .pcputime_start,                                         \
        &bench_ ## id .pcputime_finish                                         \
      );                                                                       \
  } while(0)
#else
#define STOP_BENCH(id)                                                         \
  do {                                                                         \
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &bench_ ## id .pcputime_finish);   \
    clock_gettime(CLOCK_MONOTONIC, &bench_ ## id .monotime_finish);            \
  } while(0)
#endif

#ifdef USE_BENCH_STATE
#define RESET_BENCH(id)                                                        \
  do {                                                                         \
    assert(bench_ ## id .state == BENCH_STATE_STOPPED);                        \
    bench_ ## id .elapsed_pcputime_ms = 0;                                     \
    bench_ ## id .elapsed_monotime_ms = 0;                                     \
  } while(0)
#else
#define RESET_BENCH(id)                                                        \
  do {                                                                         \
  } while(0)
#endif

#define RESTART_BENCH(id)                                                      \
  do {                                                                         \
    RESET_BENCH(id);                                                           \
    START_BENCH(id);                                                           \
  } while(0)

#define RESTART_BENCH_WITH_DESC(id, description)                               \
  do {                                                                         \
    SET_BENCH_DESC(id, description);                                           \
    RESTART_BENCH(id);                                                         \
  } while(0)

#define START_NEW_BENCH(id, description)                                       \
  NEW_BENCH(id, description);                                                  \
  START_BENCH(id)

#define START_NEW_BENCH1(id) START_NEW_BENCH(id, NULL)


#define Q(x) #x
#define QUOTE(x) Q(x)

#define PRINT_BENCH_RESULTS(id)                                                \
  do {                                                                         \
    printf(BB_CKYEL "%12.2f %12.2f " BB_CKCYN "%25s   " BB_CRESET,             \
      GET_BENCH_ELAPSED_MONOTIME_MS(id),                                       \
      GET_BENCH_ELAPSED_PCPUTIME_MS(id),                                       \
      QUOTE(id)                                                                \
    );                                                                         \
    if (bench_ ## id .desc) {                                                  \
      printf("# %s", bench_ ## id .desc);                                      \
    }                                                                          \
    putchar('\n');                                                             \
  } while(0)

#define PRINT_BENCH_RESULTS_WITH_IO_STAT(id, size)                             \
  do {                                                                         \
    printf(BB_CKYEL "%12.2f %12.2f" BB_CKGRN "%10.4f MB/s "                    \
           BB_CKCYN "%10s   " BB_CRESET,                                       \
      GET_BENCH_ELAPSED_MONOTIME_MS(id),                                       \
      GET_BENCH_ELAPSED_PCPUTIME_MS(id),                                       \
      GET_BENCH_IO_SPEED_MB(id, size),                                         \
      QUOTE(id)                                                                \
    );                                                                         \
    if (bench_ ## id .desc) {                                                  \
      printf("# %s", bench_ ## id .desc);                                      \
    }                                                                          \
    putchar('\n');                                                             \
  } while(0)

#define STOP_BENCH_AND_PRINT_RESULTS(id)                                       \
  do {                                                                         \
    STOP_BENCH(id);                                                            \
    PRINT_BENCH_RESULTS(id);                                                   \
  } while(0)

#define STOP_BENCH_AND_PRINT_RESULTS_WITH_IO(id, size)                         \
  do {                                                                         \
    STOP_BENCH(id);                                                            \
    PRINT_BENCH_RESULTS_WITH_IO_STAT(id, size);                                \
  } while(0)

#endif // BITTY_BENCHMARK_H