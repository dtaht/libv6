#ifndef __GET_CYCLES_H
#define __GET_CYCLES_H

#include <stdint.h>

#if !defined(NO_CYCLES) &&                                                   \
(defined(__x86_64__) | defined(__arm__) | defined(__epiphany__))

// haven't decided on this API YET. if doing anything with start or end title ?
// doonething do aother should compile out.

#define GET_CYCLES_INIT(title, name, start, end)                             \
  static char* __cyclestitle = name;                                         \
  cycles_t start;                                                            \
  cycles_t end;

#define GET_CYCLES_START(title, start, end)
#define GET_CYCLES_STOP(title, start, end)
#define GET_CYCLES_RESUME(title, start, end)
#define GET_CYCLES_CHECK(title, start, end)
#define GET_CYCLES_CLEAN(title, start, end)

#ifdef __x86_64__

typedef u64 cycles_t;
#define CYCLES_FMT "%lu"
static inline int init_get_cycles() { return 0; } // Always works on this arch

/** Get CPU timestep counter */
ALWAYS_INLINE static inline cycles_t get_cycles()
{
  u64 tsc;
  /** @todo not recommended to use rdtsc on old multicore machines */
  __asm__("rdtsc;"
          "shl $32, %%rdx;"
          "or %%rdx,%%rax"
          : "=a"(tsc)
          :
          : "%rcx", "%rdx", "memory");

  return tsc;
}
#endif
/** Get CPU timestep counter */

#ifdef __arm__
typedef cycles_t u32;

// Sometimes works on this arch. I need to try to hook the perf lib
// Also look over the right register for aarch64

static inline int init_get_cycles() { return 0; }
#define CYCLES_FMT "%u"

ALWAYS_INLINE static inline cycles_t get_cycles(void)
{
  u32 val;
  __asm__ __volatile__("mrc p15, 0, %0, c15, c12, 1" : "=r"(val));
  return val;
}
#endif

#ifdef __epiphany__
#define CYCLES_FMT "%u"
typedef u32 cycles_t;
static inline int init_get_cycles() { return 0; } // Always works on this arch
static inline cycles_t get_cycles() { return 0L; }
static inline cycles_t get_all_cycles() { return 0L; }
#endif

#else

#ifdef NO_CYCLES
#define CYCLES_FMT "%u"
typedef u32 cycles_t;

// I want to compile this out even harder than this

static inline int init_get_cycles() { return 0; }
static inline cycles_t get_cycles() { return 0L; }

#else
#warning no sane get_cycles on this arch so benchmarking is impossible
#define CYCLES_FMT "%u"
typedef u32 cycles_t;
static inline int init_get_cycles()
{
  return -1;
} // Always fails on this arch
static inline cycles_t get_cycles() { return 0L; }
#endif
#endif

#endif
