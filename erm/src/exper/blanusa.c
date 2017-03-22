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

#define generic uint16_t
#define blanusa blanusauint16_t
#define mypow(a,b) ((a) << (b))
#include "blanusa.h"

#undef generic
#undef blanusa
#define generic uint32_t
#define blanusa blanusauint32_t
#include "blanusa.h"

#undef generic
#undef blanusa
#define generic uint64_t
#define blanusa blanusauint64_t
#include "blanusa.h"

#undef generic
#undef blanusa
#define generic int16_t
#define blanusa blanusaint16_t
#include "blanusa.h"

#undef generic
#undef blanusa
#define generic int32_t
#define blanusa blanusaint32_t
#include "blanusa.h"

#undef generic
#undef blanusa
#define generic int64_t
#define blanusa blanusaint64_t
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
  double d;
  for(int i = 0; i < 0xffff; i++) {
    d = blanusadouble(i);
    printf("Blanusa %d: %g\n", i, d);
  }

  return 0;
}
#endif

