/**
 * blanusa_powint.c
 *
 * Dave Taht
 * 2017-03-21
 */

#include <math.h>
#include <stdint.h>
#include <tgmath.h>

#include "debug.h"
#include "erm_types.h"
#include "preprocessor.h"
#include "simd.h"

// Worse, we need saturating arith

#define generic u16
#define blanusa_powint blanusa_powintu16
#define mypow(a, b) ((a) << (b))
#include "blanusa_powint.h"

#undef generic
#undef blanusa_powint
#define generic u32
#define blanusa_powint blanusa_powintu32
#include "blanusa_powint.h"

#undef generic
#undef blanusa_powint
#define generic u64
#define blanusa_powint blanusa_powintu64
#include "blanusa_powint.h"

#undef generic
#undef blanusa_powint
#define generic s16
#define blanusa_powint blanusa_powints16
#include "blanusa_powint.h"

#undef generic
#undef blanusa_powint
#define generic s32
#define blanusa_powint blanusa_powints32
#include "blanusa_powint.h"

#undef generic
#undef blanusa_powint
#define generic s64
#define blanusa_powint blanusa_powints64
#include "blanusa_powint.h"

// Floating point is different. I can improve it but I don't care at the
// moment

#undef generic
#undef blanusa_powint
#undef mypow
#define mypow powf
#define generic float
#define blanusa_powint blanusa_powintfloat
#include "blanusa_powint.h"
#undef generic
#undef blanusa_powint
#undef mypow
#define mypow pow
#define generic double
#define blanusa_powint blanusa_powintdouble
#include "blanusa_powint.h"
#undef generic
#undef blanusa_powint

#ifdef DEBUG_MODULE
// #define NO_PERF

#define LOGGER_INFO(where, fmt, ...)

#ifndef DONOTHING
#define DONOTHING                                                            \
  do {                                                                       \
  } while(0)
#endif

#ifndef CALLOCA
#define CALLOCA(type, dest, size, num)                                       \
  type dest __attribute__((aligned(16)));                                    \
  dest = calloc(size, num)
#endif

#include "align.h"
#include "cycles_bench.h"
#include "erm_logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

float bvals[4];

int main()
{
  GET_CYCLES_INIT(0, collect, start, end);
  GET_CYCLES_AVAIL(start, end);
  GET_CYCLES_DECLARE_ACC(simdversion);
  GET_CYCLES_DECLARE_ACC(nonsimdversion);

  float f;

  // without LOGGER_INFO these should all just compile out to a single
  // statement

  for(int i = -0xff; i < 0xff; i++) {
    f = blanusa_powintfloat(i);
    LOGGER_INFO(PERF, "Blanusa %d: %g: %g\n", i, f, blanusa_powintfloat(f));
  }

  double d = 0.0;
  asm("nop; /* start of test; */");
  for(double i = -0xffffff; i < 0xffffff; i++) {
    d += blanusa_powintdouble(i);
    //printf("Start: %d Val: %g \n", i, d);
    //LOGGER_INFO(PERF, "Blanusa %d: %g: %g\n", i, d, blanusa_powintdouble(d));
  }
  printf("d = %g\n", d);

  u16 c;
  for(int i = -0xff; i < 0xff; i++) {
    c = blanusa_powintu16(i);
    LOGGER_INFO(PERF, "Blanusa16trunc %d: %d\n", i, c);
  }

  // FIXME: add saturating arith

  // Parallel or not?

  CALLOCA(float*, vals, 0xffff, sizeof bvals);
  CALLOCA(float*, res, 0xffff, sizeof bvals);

  for(int i = 0; i < 0xff * 4; i++) {
    vals[i] = i * .03124 * (random() % 32);
  }

  GET_CYCLES_START(collect, start, end);

#pragma simd
  for(int i = 0; i < 0xffff; i += 4) {
    res[i] = blanusa_powintfloat(vals[i]);
    res[i + 1] = blanusa_powintfloat(vals[i + 1]);
    res[i + 2] = blanusa_powintfloat(vals[i + 2]);
    res[i + 3] = blanusa_powintfloat(vals[i + 3]);
    //printf("Val: %g %g %g %g\n", res[i],res[i+1], res[i+2],res[i+3]);
  }
  GET_CYCLES_STOP(collect, start, end);
  GET_CYCLES_ASSIGN(simdversion, collect);
  GET_CYCLES_PRINT(PERF, "vectorized", collect);

  for(int i = 0; i < 0xff; i += 4) {
    LOGGER_INFO(PERF, "%g %g %g %g\n", res[i], res[i + 1], res[i + 2], res[i + 3]);
  }

  for(int i = 0; i < 0xff * 4; i++) {
    vals[i] = i * .03124 * (random() % 32);
  }

  GET_CYCLES_START(collect, start, end);

#pragma simd
  for(int i = 0; i < 0xffff; i++) {
    res[i] = blanusa_powintfloat(vals[i]);
  }
  GET_CYCLES_STOP(collect, start, end);
  GET_CYCLES_ASSIGN(nonsimdversion, collect);
  GET_CYCLES_PRINT(PERF, "simple vectorized", collect);
  for(int i = 0; i < 0xff; i += 4) {
    LOGGER_INFO(PERF, "%g %g %g %g\n", res[i], res[i + 1], res[i + 2], res[i + 3]);
  }
  GET_CYCLES_PRINT_DIFFERENCE(PERF, "simd vs nonsimd version", simdversion, nonsimdversion);
  return 0;
}
#endif
