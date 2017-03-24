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

#ifndef CYCLES_BENCH_H
#define CYCLES_BENCH_H
#include "get_cycles.h"

// haven't decided on this API YET. if doing anything with start or end title
// doonething do aother should compile out.

#ifndef LOGGER
#warning "no logger defined, using printf"
#define LOGGER(fmt, ...) printf(fmt, __VA_ARGS__)
#endif

#define GET_CYCLES_INIT(mask, collect, start, end)                           \
  cycles_t start = 0;                                                        \
  cycles_t end = 0;                                                          \
  u64 collect = 0;                                                           \
  init_cycles(mask)

#define GET_CYCLES_AVAIL(start, end)                                         \
  do {                                                                       \
    start = get_cycles();                                                    \
    sleep(1);                                                                \
    end = get_cycles();                                                      \
    LOGGER("cycle counter%s active\n", start == end ? " not" : " ");         \
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
#define GET_CYCLES_PRINT(title, collect)                                     \
  LOGGER("%s Cycles: %ld\n", title, collect)

// #define GET_CYCLES_ELAPSED_TIME()
// #define GET_CYCLES_PUSH (don't print, store it somewhere)
// POP? PUSH? FIXUP? 32 bits is not enough

#endif
