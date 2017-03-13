/**
 * simd.h
 *
 * Dave Taht
 * 2017-03-13
 */

#ifndef SIMD_H
#define SIMD_H

// This ultimately will try to hammer the neon and sse abstractions and API into submission

// FIXME - I need to wedge 32 bytes into a register correctly. Somehow

#ifdef HAVE_NEON
#include <arm_neon.h>
static inline void push_regs() { }
static inline void pop_regs() { }
typedef uint32x4_t usimd __attribute__ ((vector_size (16)));
#endif

#ifdef HAVE_SSE4
#include <xmmintrin.h>
#include <emmintrin.h>
static inline void push_regs() { }
static inline void pop_regs() { }
typedef unsigned int usimd __attribute__ ((vector_size (16)));
#endif

#ifdef HAVE_ADAPTEVA
#include <xmmintrin.h>
#include <emmintrin.h>
static inline void push_regs() { }
static inline void pop_regs() { }
typedef unsigned int usimd __attribute__ ((vector_size (16)));
#endif

#if !(defined (HAVE_NEON) | defined(HAVE_SSE4) | defined(HAVE_ADAPTEVA))
static inline void push_regs() { }
static inline void pop_regs() { }
typedef unsigned int usimd __attribute__ ((vector_size (16)));
#endif

typedef union Vec4 {
    usimd p;
    float e[4];
    int32_t i[4];
    uint32_t u[4];
    uint16_t s[8];
    uint8_t c[16];
} Vec4_t;

#endif
