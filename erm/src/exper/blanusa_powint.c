/**
 * blanusa_powint.c
 *
 * Dave Taht
 * 2017-03-21
 */

#include <stdint.h>
#include <math.h>
#include <tgmath.h>

#include "preprocessor.h"
#include "debug.h"
#include "erm_types.h"
#include "simd.h"

// Worse, we need saturating arith

#define generic u16
#define blanusa_powint blanusa_powintu16
#define mypow(a,b) ((a) << (b))
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

// Floating point is different. I can improve it but I don't care at the moment

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
#include <stdio.h>
#include <stdlib.h>

float bvals[4];
 
int main()
{
  float f;
  for(int i = -0xff; i < 0xff; i++) {
    f = blanusa_powintdouble(i);
    printf("Blanusa %d: %g: %g\n", i, f, blanusa_powintfloat(f));
  }

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

  float *vals = calloc(0xffff,sizeof bvals);
  float *res  = calloc(0xffff,sizeof bvals);

#pragma simd
  for (int i = 0; i < 0xff; i+=4) {
	res[i] = blanusa_powintfloat(vals[i]); 
	res[i+1] = blanusa_powintfloat(vals[i+1]); 
	res[i+2] = blanusa_powintfloat(vals[i+2]); 
	res[i+3] = blanusa_powintfloat(vals[i+3]); 
  }
  for (int i = 0; i < 0xff; i+=4) {
	printf("%g %g %g %g\n", res[i], res[i+1], res[i+2], res[i+3]);
  }

  return 0;
}
#endif

