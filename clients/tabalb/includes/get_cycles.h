#ifndef __GET_CYCLES_H
#define __GET_CYCLES_H

#include <stdint.h>

#if defined (__x86_64__) | defined (__arm__) | defined (__epiphany__)

#ifdef __x86_64__

typedef uint64_t cycles_t;
#define CYCLES_FMT "%lu"

/** Get CPU timestep counter */
__attribute__((always_inline)) static inline cycles_t get_cycles()
{
        uint64_t tsc;
        /** @todo not recommended to use rdtsc on old multicore machines */
        __asm__ ("rdtsc;"
                 "shl $32, %%rdx;"
                 "or %%rdx,%%rax"
                : "=a" (tsc)
                :
                : "%rcx", "%rdx", "memory");

        return tsc;
}
#endif
/** Get CPU timestep counter */

#ifdef __arm__
typedef cycles_t uint32_t;
#define CYCLES_FMT "%u"

__attribute__((always_inline)) static inline cycles_t get_cycles(void)
{
	u32 val;
	__asm__ __volatile__ ("mrc p15, 0, %0, c15, c12, 1" : "=r" (val));
	return val;
}
#endif

#ifdef __epiphany__
#define CYCLES_FMT "%u"
typedef uint32_t cycles_t;

#endif

#else

#warning no sane get_cycles on this arch so benchmarking is impossible

#define CYCLES_FMT "%u"
typedef uint32_t cycles_t;
static inline cycles_t get_cycles() { return 0L; }
#endif

#endif

