/*
 *  Bitty Benchmark
 */

#ifndef BITTY_BENCHMARK_H
#define BITTY_BENCHMARK_H

#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#define USE_BENCH_STATE

/* dummies of the terminal color codes */
static char *BB_CKNRM  = "";
static char *BB_CKRED  = "";
static char *BB_CKGRN  = "";
static char *BB_CKYEL  = "";
static char *BB_CKBLU  = "";
static char *BB_CKMAG  = "";
static char *BB_CKCYN  = "";
static char *BB_CKWHT  = "";
static char *BB_CRESET = "";

/* we shoudn't put color codes if STDOUT is not a terminal */
static int
bb_should_colorize(void)
{
  return isatty(STDOUT_FILENO);
}

enum bb_opts {
  BB_OPT_INITIALIZED_BIT = 0,
  BB_OPT_INITIALIZED = (1 << BB_OPT_INITIALIZED_BIT),
};

static int _bb_opts = 0;
#define BB_IS_INITIALIZED (_bb_opts & BB_OPT_INITIALIZED)

static void
bb_init(void)
{
  if (bb_should_colorize()) {
    /* the real terminal color codes */
    BB_CKNRM  = "\x1B[0m" ;
    BB_CKRED  = "\x1B[31m";
    BB_CKGRN  = "\x1B[32m";
    BB_CKYEL  = "\x1B[33m";
    BB_CKBLU  = "\x1B[34m";
    BB_CKMAG  = "\x1B[35m";
    BB_CKCYN  = "\x1B[36m";
    BB_CKWHT  = "\x1B[37m";
    BB_CRESET = "\033[0m" ;
  }

  _bb_opts |= BB_OPT_INITIALIZED;
}


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
  if (!BB_IS_INITIALIZED) bb_init();                                           \
  struct bench_stat bench_ ## id = {                                           \
    .desc = description,                                                       \
    .elapsed_monotime_ms = 0,                                                  \
    .elapsed_pcputime_ms = 0,                                                  \
    .state = BENCH_STATE_STOPPED                                               \
  };                                                                           \
  do {} while(0)
#else
#define NEW_BENCH(id, description)                                             \
  if (!BB_IS_INITIALIZED) bb_init();                                           \
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
    printf("%s%12.2f %12.2f%s " "%25s   %s",                                   \
      BB_CKYEL,                                                                \
      GET_BENCH_ELAPSED_MONOTIME_MS(id),                                       \
      GET_BENCH_ELAPSED_PCPUTIME_MS(id),                                       \
      BB_CKCYN,                                                                \
      QUOTE(id),                                                               \
      BB_CRESET                                                                \
    );                                                                         \
    if (bench_ ## id .desc) {                                                  \
      printf("# %s", bench_ ## id .desc);                                      \
    }                                                                          \
    putchar('\n');                                                             \
  } while(0)

#define PRINT_BENCH_RESULTS_WITH_IO_STAT(id, size)                             \
  do {                                                                         \
    printf("%s%12.2f %12.2f%s" "%10.4f MB/s %s" "%10s   %s",                   \
      BB_CKYEL,                                                                \
      GET_BENCH_ELAPSED_MONOTIME_MS(id),                                       \
      GET_BENCH_ELAPSED_PCPUTIME_MS(id),                                       \
      BB_CKGRN,                                                                \
      GET_BENCH_IO_SPEED_MB(id, size),                                         \
      BB_CKCYN,                                                                \
      QUOTE(id),                                                               \
      BB_CRESET                                                                \
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