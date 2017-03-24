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
#include "cycles_bench.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

float bvals[4];

int main()
{
  GET_CYCLES_INIT(0, collect, start, end);
  GET_CYCLES_AVAIL(start, end);
  float f;
  for(int i = -0xff; i < 0xff; i++) {
    f = blanusa_powintdouble(i);
    printf("Blanusa %d: %g: %g\n", i, f, blanusa_powintfloat(f));
  }
  GET_CYCLES_STOP(collect, start, end);

  double d;
  for(int i = -0xff; i < 0xff; i++) {
    d = blanusa_powintdouble(i);
    printf("Blanusa %d: %g: %g\n", i, d, blanusa_powintdouble(d));
  }

  u16 c;
  for(int i = -0xff; i < 0xff; i++) {
    c = blanusa_powintu16(i);
    printf("Blanusa16trunc %d: %d\n", i, c);
  }

  // FIXME: saturating

  // Parallel?

  float* vals __attribute__((aligned(16)));
  vals = calloc(0xffff, sizeof bvals);
  float* res __attribute__((aligned(16)));
  res = calloc(0xffff, sizeof bvals);

  for(int i = 0; i < 0xff * 4; i++) {
    vals[i] = i * .03124 * (random() % 32);
  }

  // No workie
  GET_CYCLES_START(collect, start, end);
#pragma simd
  for(int i = 0; i < 0xffff; i += 4) {
    res[i] = blanusa_powintfloat(vals[i]);
    res[i + 1] = blanusa_powintfloat(vals[i + 1]);
    res[i + 2] = blanusa_powintfloat(vals[i + 2]);
    res[i + 3] = blanusa_powintfloat(vals[i + 3]);
  }
  GET_CYCLES_STOP(collect, start, end);
  u64 last = collect;

  GET_CYCLES_PRINT("vectorized", collect);

  for(int i = 0; i < 0xff; i += 4) {
    printf("%g %g %g %g\n", res[i], res[i + 1], res[i + 2], res[i + 3]);
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
  double l1 = last;
  double l2 = collect;
  double ratio = l2 / l1;

  GET_CYCLES_PRINT("simple vectorized", collect);
  for(int i = 0; i < 0xff; i += 4) {
    printf("%g %g %g %g\n", res[i], res[i + 1], res[i + 2], res[i + 3]);
  }
  printf("difference: %ld speedup: %.30f\n", last - collect, ratio);
  return 0;
}
#endif
