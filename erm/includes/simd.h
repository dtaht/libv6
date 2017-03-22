/**
 * simd.h
 *
 * Dave Taht
 * 2017-03-13
 */

#ifndef SIMD_H
#define SIMD_H

// This ultimately will try to hammer the neon and sse abstractions and API
// into submission

// FIXME - I need to wedge 32 bytes into a register correctly. Somehow

#ifdef HAVE_NEON
register global_flags_t tflags asm("q15");
#define FFFFFFFF (0xFFFFFFFF)
#include <arm_neon.h>
static inline void push_regs() {}
static inline void pop_regs() {}
typedef uint32x4_t usimd VECTOR(16);
#endif

#ifdef HAVE_SSE4
register global_flags_t tflags asm("r15");
#define FFFFFFFF (0xFFFFFFFF)
#include <immintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
static inline void push_regs() {}
static inline void pop_regs() {}
typedef unsigned int usimd VECTOR(16);
#endif

#ifdef HAVE_ADAPTEVA
register global_flags_t tflags asm("r41");
register u32 FFFFFFFF asm("r63");
#include <bsp.h>
static inline void push_regs() {}
static inline void pop_regs() {}
typedef unsigned int usimd VECTOR(16);
#endif

#if !(defined(HAVE_NEON) | defined(HAVE_SSE4) | defined(HAVE_ADAPTEVA))
extern global_flags_t tflags;
#define FFFFFFFF (0xFFFFFFFF)
static inline void push_regs() {}
static inline void pop_regs() {}
typedef unsigned int usimd VECTOR(16);
#endif

typedef union Vec4 {
  v6addr_t a;
  usimd p;
  float e[4];
  s32 i[4];
  u32 u[4];
  u16 s[8];
  u8 c[16];
} Vec4_t;

#endif
