/**
 * simd.h
 *
 * Dave Taht
 * 2017-03-13
 */

#ifndef SIMD_H
#define SIMD_H

// This ultimately will try to hammer the neon and sse abstractions into submission

#ifdef HAVE_NEON
#include <arm_neon.h>
#endif

#ifdef HAVE_SSE4
#include <emmintrin.h>
#endif

typedef unsigned int usimd __attribute__ ((vector_size (16)));

typedef union Vec4 {
    usimd p;
    float e[4];
    int32_t i[4];
    uint32_t u[4];
    uint16_t s[8];
    uint8_t c[16];
} Vec4_t;


#endif
