/**
 * blanusa.c
 *
 * Dave Taht
 * 2017-03-21
 */

#include <stdint.h>
#include <math.h>

#include "preprocessor.h"
#include "debug.h"
#include "erm_types.h"
#include "simd.h"

// Worse, we need saturating arith

#define generic u16
#define blanusa blanusau16
#define mypow(a,b) ((a) << (b))
#include "blanusa.h"

#undef generic
#undef blanusa
#define generic u32
#define blanusa blanusau32
#include "blanusa.h"

#undef generic
#undef blanusa
#define generic u64
#define blanusa blanusau64
#include "blanusa.h"

#undef generic
#undef blanusa
#define generic s16
#define blanusa blanusas16
#include "blanusa.h"

#undef generic
#undef blanusa
#define generic s32
#define blanusa blanusas32
#include "blanusa.h"

#undef generic
#undef blanusa
#define generic s64
#define blanusa blanusas64
#include "blanusa.h"

// Floating point is different. I can improve it but I don't care at the moment

#undef generic
#undef blanusa
#undef mypow
#define mypow pow
#define generic float
#define blanusa blanusafloat
#include "blanusa.h"
#undef generic
#undef blanusa
#define generic double
#define blanusa blanusadouble
#include "blanusa.h"
#undef generic
#undef blanusa

#ifdef DEBUG_MODULE
#include <stdio.h>
int main()
{
/*
  double d;
  for(int i = -0xffff; i < 0xffff; i++) {
    d = blanusadouble(i);
    printf("Blanusa %d: %g: %g\n", i, d, blanusadouble(d));
  }

*/
  u16 c;
  for(int i = -0xff; i < 0xff; i++) {
    c = blanusau16(i);
//    printf("Blanusa16 %d: %d: %g\n", i, c, blanusadouble(c));
    printf("Blanusa16trunc %d: %d\n", i, c);
    }

  // saturating
  
  return 0;
}
#endif

