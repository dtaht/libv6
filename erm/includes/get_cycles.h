/*
 * get_cycles.h © 2017 Michael David Täht
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __GET_CYCLES_H
#define __GET_CYCLES_H

#if !(defined(NO_CYCLES) | defined(USING_PERF)) &&                           \
(defined(__x86_64__) | defined(__arm__) | defined(__epiphany__) | defined(__aarch64__))

#ifdef __x86_64__

typedef u64 cycles_t;

#define CYCLES_FMT "%lu"

static inline int init_cycles(int events) { return 0; }

static inline int stop_cycles(int events) { return 0; }

// For out of order ops you need
// mfence;rdtsc on AMD platforms and lfence;rdtsc on Intel. If you don't want to
// bother with distinguishing between these, mfence;rdtsc works.

/** Get CPU timestep counter */
ALWAYS_INLINE static inline int get_bottom_cycles()
{
  register volatile u32 r asm("%eax");
  /** Not recommended to use rdtsc on old multicore machines */
  __asm__ volatile("rdtsc;"
                   : "=a"(r)
                   :
                   : "%rcx", "%rdx", "memory");

  return r;
}

/** Get CPU timestep counter */
ALWAYS_INLINE static inline cycles_t get_cycles()
{
  volatile u64 r;
  /** Not recommended to use rdtsc on old multicore machines */
  __asm__ volatile("rdtsc;"
                   "shl $32, %%rdx;"
                   "or %%rdx,%%rax"
                   : "=a"(r)
                   :
                   : "%rcx", "%rdx", "memory");

  return r;
}
#endif

#if(defined(__arm__) | defined(__arch64__))

typedef cycles_t u32;

// kernel module https://github.com/zhiyisun/enable_arm_pmu.git for something
// saner

#define CYCLES_FMT "%u"

#define ARMV8_PMEVTYPER_P (1 << 31) /* EL1 modes filtering bit */
#define ARMV8_PMEVTYPER_U (1 << 30) /* EL0 filtering bit */
#define ARMV8_PMEVTYPER_NSK                                                  \
  (1 << 29) /* Non-secure EL1 (kernel) modes filtering bit */
#define ARMV8_PMEVTYPER_NSU                                                  \
  (1 << 28) /* Non-secure User mode filtering bit                            \
                  */
#define ARMV8_PMEVTYPER_NSH                                                  \
  (1 << 27)                          /* Non-secure Hyp modes filtering bit   \
                                           */
#define ARMV8_PMEVTYPER_M (1 << 26)  /* Secure EL3 filtering bit */
#define ARMV8_PMEVTYPER_MT (1 << 25) /* Multithreading */
#define ARMV8_PMEVTYPER_EVTCOUNT_MASK 0x3ff

static inline void init_cycles(u32 events)
{
#if defined __aarch64__
  events &= ARMV8_PMEVTYPER_EVTCOUNT_MASK;
  asm volatile("isb");
  /* Just use counter 0 */
  asm volatile("msr pmevtyper0_el0, %0" : : "r"(events));
  /*   Performance Monitors Count Enable Set register bit 30:1 disable, 31,1
   * enable */
  u32 r = 0;

  asm volatile("mrs %0, pmcntenset_el0" : "=r"(r));
  asm volatile("msr pmcntenset_el0, %0" : : "r"(r | 1));
#endif
}

// Sigh - I have two different version of this code from the net

ALWAYS_INLINE static inline cycles_t get_cycles2(void)
{
  u32 r;
  __asm__ __volatile__("mrc p15, 0, %0, c15, c12, 1" : "=r"(r));
  return val;
}


static ALWAYS_INLINE cycles_t get_cycles(void)
{
  u32 r = 0;
#if defined __aarch64__
  asm volatile("mrs %0, pmccntr_el0" : "=r"(r));
#elif defined(__ARM_ARCH_7A__)
  asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(r));
#endif
  return r;
}


static inline u32 read_pmu(void)
{
  u32 r = 0;
#if defined __aarch64__
  asm volatile("mrs %0, pmevcntr0_el0" : "=r"(r));
#endif
  return r;
}

static inline void disable_pmu(u32 events)
{
/*   Performance Monitors Count Enable Set register: clear bit 0 */
#if defined __aarch64__
  u32 r = 0;
  asm volatile("mrs %0, pmcntenset_el0" : "=r"(r));
  asm volatile("msr pmcntenset_el0, %0" : : "r"(r && 0xfffffffe));
#endif
}

static inline int stop_cycles(int events)
{
  disable_pmu(events);
  return 0;
}

#endif

#ifdef __epiphany__
#define CYCLES_FMT "%u"
typedef u32 cycles_t;

static inline int init_cycles() { return 0; }
static inline int stop_cycles() { return 0; }
static inline cycles_t get_cycles() { return 0L; }
static inline cycles_t get_all_cycles() { return 0L; }

#endif

#else

#if defined(NO_CYCLES) | defined(USING_PERF)

#define CYCLES_FMT "%u"
typedef u32 cycles_t;

// I want to compile this out even harder than this

static inline int init_cycles(int events) { return 0; }
static inline int stop_cycles(int events) { return 0; }
static inline cycles_t get_cycles() { return 0L; }

#else
#warning no sane get_cycles on this arch so benchmarking is impossible
#define CYCLES_FMT "%u"

typedef u32 cycles_t;
static inline int init_cycles(u32 events)
{
  return 0;
} // Always fails on this arch
static inline int init_cycles(u32 events) { return 0; }

// Always fails on this arch
static inline cycles_t get_cycles() { return 0L; }

#endif
#endif

#endif
