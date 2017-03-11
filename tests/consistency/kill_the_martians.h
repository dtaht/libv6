/**
 * kill_the_martians.h
 * Dave Taht
 * 2017-03-10
 */

#ifndef KILL_THE_MARTIANS_H
#define KILL_THE_MARTIANS_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern const unsigned char v4prefix[16];
extern const unsigned char zeroes[16];

// The original routine was opaque

extern int v4mapped_orig(const unsigned char *address);

typedef struct {
	unsigned char p[16];
	int plen;
} prefix;

extern int
martian_prefix_new(const unsigned char *prefix, int plen);

extern int
martian_prefix_old(const unsigned char *prefix, int plen);

extern int count_martian_prefixes_new(prefix *p, int count);
extern int count_martian_prefixes_old(prefix *p, int count);
extern int count_martian_prefixes_new_dual(prefix *p, prefix *p2, int count);
extern int count_martian_prefixes_new_single(prefix *p, prefix *p2, int count);

extern int
martian_prefix_new_dual(const unsigned char *prefix, int plen,
			const unsigned char *prefix1, int plen2);

extern prefix * gen_random_prefixes(int count);

#ifdef __x86_64__
// x86_64 specific for now

static inline unsigned long get_clock(void) {
  unsigned a, d;
  asm volatile("rdtsc" : "=a" (a), "=d" (d));
  return ((unsigned long)a) | (((unsigned long)d) << 32);
}

#endif

// http://neocontra.blogspot.com/2013/05/user-mode-performance-counters-for.html

// from the bad old days http://hardwarebug.org/2010/01/14/beware-the-builtins/

#if 0
#ifdef __arm__
static inline unsigned long get_clock(void)
{
#if defined(__GNUC__) && defined(__ARM_ARCH_7A__)
        uint32_t r = 0;
        asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(r) );
        return r;
#endif
}
#endif
#endif

#if 1
#ifdef __arm__
extern int fddev;
static inline long long
get_clock(void)
{
 long long result = 0;
 if (read(fddev, &result, sizeof(result)) < sizeof(result)) return 0;
 return result;
}
#endif
#endif

extern void fool_compiler(prefix *p);

#endif
