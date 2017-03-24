/*
 * cycles_bench.h © 2017 Michael David Täht
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// FIXME: Make work with the perf related system calls
// FIXME: Make work with simulated time
// FIXME: For all I know I'm just going to use -pg and perf

// #define GET_CYCLES_ELAPSED_TIME()
// #define GET_CYCLES_PUSH (don't print, store it somewhere)
// POP? PUSH? FIXUP? 32 bits is not enough

#ifndef CYCLES_BENCH_H
#define CYCLES_BENCH_H

#ifdef NO_PERF
#define GET_CYCLES_INIT(mask, collect, start, end) DONOTHING
#define GET_CYCLES_DECLARE_ACC(v) DONOTHING
#define GET_CYCLES_ASSIGN(a, b) DONOTHING
#define GET_CYCLES_AVAIL(start, end) DONOTHING
#define GET_CYCLES_START(collect, start, end) DONOTHING
#define GET_CYCLES_STOP(collect, start, end) DONOTHING
#define GET_CYCLES_PAUSE(collect, start, end) DONOTHING
#define GET_CYCLES_RESUME(collect, start, end) DONOTHING
#define GET_CYCLES_COLLECT(collect, start, end) DONOTHING
#define GET_CYCLES_CLEAN(collect, start, end) DONOTHING
#define GET_CYCLES_CANCEL(mask) DONOTHING
#define GET_CYCLES_PRINT(where, title, collect) DONOTHING
#define GET_CYCLES_PRINT_DIFFERENCE(where, title, v1, v2) DONOTHING

#ifndef PERF
#define PERF stdout
#endif

#ifndef PERFINFO
#define PERFINFO stdout
#endif

#ifndef LOGGER
#warning "no logger defined, compiling out"
#define LOGGER(where, fmt, ...) DONOTHING
#endif

#ifndef LOGGER_INFO
#warning "no info logger defined, compiling out"
#define LOGGER_INFO(where, fmt, ...) DONOTHING
#endif

#else
#include "get_cycles.h"

#ifndef PERF
#define PERF stdout
#endif

#ifndef PERFINFO
#define PERFINFO stdout
#endif

#ifndef LOGGER
#warning "no logger defined, using fprintf"
#define LOGGER(where, fmt, ...) fprintf(where, fmt, __VA_ARGS__)
#endif

#ifndef LOGGER_INFO
#warning "no info logger defined, using fprintf"
#define LOGGER_INFO(where, fmt, ...) fprintf(where, fmt, __VA_ARGS__)
#endif

#define GET_CYCLES_INIT(mask, collect, start, end)                           \
  cycles_t start = 0;                                                        \
  cycles_t end = 0;                                                          \
  u64 collect = 0;                                                           \
  init_cycles(mask)

#define GET_CYCLES_DECLARE_ACC(v) u64 v
#define GET_CYCLES_ASSIGN(a, b) a = b

#define GET_CYCLES_AVAIL(start, end)                                         \
  do {                                                                       \
    start = get_cycles();                                                    \
    sleep(1);                                                                \
    end = get_cycles();                                                      \
    LOGGER(PERF, "cycle counter:%sactive\n", start == end ? " in" : " ");    \
  } while(0)

#define GET_CYCLES_START(collect, start, end)                                \
  do {                                                                       \
    collect = start = end = get_cycles();                                    \
  } while(0)

#define GET_CYCLES_STOP(collect, start, end)                                 \
  do {                                                                       \
    end = get_cycles();                                                      \
    collect += end > start ? end : start - end;                              \
  } while(0)

#define GET_CYCLES_PAUSE(collect, start, end)                                \
  do {                                                                       \
    end = get_cycles();                                                      \
    collect += end > start ? end : start - end;                              \
  } while(0)

#define GET_CYCLES_RESUME(collect, start, end)                               \
  do {                                                                       \
    end = start = get_cycles();                                              \
  } while(0)

#define GET_CYCLES_COLLECT(collect, start, end)                              \
  do {                                                                       \
    end = get_cycles();                                                      \
    collect += end > start ? end : start - end;                              \
    start = end;                                                             \
  } while(0)

#define GET_CYCLES_CLEAN(collect, start, end) collect = start = end = 0
#define GET_CYCLES_CANCEL(mask) stop_cycles(mask)
#define GET_CYCLES_PRINT(where, title, collect)                              \
  LOGGER(where, title ": Cycles: %ld\n", collect)

#define GET_CYCLES_PRINT_DIFFERENCE(where, title, v1, v2)                    \
  LOGGER(where, title ": Cycles: %ld Improvement: %20.20g\n", v2 - v1,       \
         (double)(v2) / (double)(v1))
#endif

#endif
