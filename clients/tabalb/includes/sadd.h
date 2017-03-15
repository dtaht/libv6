/**
 * sadd.h
 *
 * Dave Taht
 * 2017-03-14
 */

#ifndef SADD_H
#define SADD_H

#include <stdint.h>

// The winner on x86 is:

static inline uint16_t sadd16(uint16_t a, uint16_t b)
{
  uint16_t c = a + b;
  if (c<a) /* Can only happen due to overflow */
    c = -1;
  return c;
}

// Saturating arithmetic courtesy of:
// http://stackoverflow.com/questions/121240/how-to-do-saturating-addition-in-c

#if 1   // icc avoids cmov with these in msalters' version
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       x
#define unlikely(x)     x
#endif

// Sorted in order of least bad on x86, with gcc5

uint16_t adds16_msalters(uint16_t a, uint16_t b) {
  uint16_t c = a + b;
  if (c<a) /* Can only happen due to overflow */
    c = -1;
  return c;
}
uint32_t adds32_msalters(uint32_t a, uint32_t b) {
  uint32_t c = a + b;
  if (c<a) /* Can only happen due to overflow */
    c = -1;
  return c;
}

#if defined (__i386__) || defined (__x86_64__)
// TODO: let the compiler know that the operands are interchangeable

// ideally check __i686__ for cmov, but that's only defined for an actual i686 target, not e.g. haswell
uint32_t adds32_x86_pjc_mov(uint32_t a, uint32_t b)
{  // shorter critical-path latency by one cycle than the sbb version
    uint32_t tmp = -1;
                   // { AT&T    | Intel }  syntax alternatives.  The other versions without this  will break with -masm=intel
    asm ("add     { %[b],%[a]   | %[a],  %[b] }\n\t"
         "cmovnc  { %[a],%[dst] | %[dst],%[a] }"
         : [dst] "+r" (tmp), [a] "+&r" (a)
         : [b] "g" (b) // use "rme" for a 64bit version, since it still uses 32bit sign-extended immediate
         // no clobbers
         );
    return tmp;
}
uint32_t adds32_x86_pjc_sbb(uint32_t a, uint32_t b)
{  // smaller code-size, but longer critical-path latency (4c on Haswell, 3c on AMD and Broadwell/Skylake)
   // sbb is on the critical path, unlike mov -1.  sbb and cmov are the same latency on all CPUs
   // sbb same,same doesn't break the dependency on the old value of dst on any uarch except AMD
    uint32_t tmp;
    asm ("add     %[b], %[a]\n\t"
         "sbb     %[dst], %[dst]\n\t"
         "or      %[a], %[dst]"
         : [dst] "=r" (tmp), [a] "+&r" (a)
         : [b] "g" (b)
         );
    return tmp;
}
uint32_t adds32_x86_pjc_inplace(uint32_t a, uint32_t b)
{   // can reuse the same -1 register, but can't produce the result in a new register
    asm ("add  %[b], %[a]\n\t"
         "cmovc %[sat], %[a]"
         : [a] "+&r" (a)
         : [b] "g" (b), [sat] "r" (-1) // could use an "rm" constraint on [sat], but loading -1 from memory is dumb, so stop clang from shooting itself in the foot
         );
    return a;
}
#endif

unsigned saturate_add_uint_kevin(unsigned x, unsigned y) {
    if (y>UINT32_MAX-x) return UINT32_MAX;
    return x+y;
}
uint16_t sadd16_remo(uint16_t a, uint16_t b)
    { return (a > 0xFFFF - b) ? 0xFFFF : a + b; }

uint32_t sadd32_remo(uint32_t a, uint32_t b)
    { return (a > 0xFFFFFFFF - b) ? 0xFFFFFFFF : a + b;}



uint32_t sadd32_r(uint32_t a, uint32_t b) {  // from R.
    uint64_t s = (uint64_t)a+b;
    return -(s>>32) | (uint32_t)s;
}

#define sadd16(a, b)  (uint16_t)( ((uint32_t)(a)+(uint32_t)(b)) > 0xffff ? 0xffff : ((a)+(b)))
uint32_t sadd32_op(uint32_t a, uint32_t b) { return (uint32_t)( ((uint64_t)(a)+(uint64_t)(b)) > 0xffffffff ? 0xffffffff : ((a)+(b))); }


// unfortunately compiles to two conditional branches or cmoves, because compilers don't grok this
uint32_t saturate_add32_dgentry(uint32_t a, uint32_t b) {
    uint32_t sum = a + b;
    if ((sum < a) || (sum < b))
        return ~((uint32_t)0);
    else
        return sum;
} /* saturate_add32 */
uint16_t saturate_add16_dgentry(uint16_t a, uint16_t b) {
    uint16_t sum = a + b;
    if ((sum < a) || (sum < b))
        return ~((uint16_t)0);
    else
        return sum;
} /* saturate_add16 */



// These generate horrible code :(
// SWAR 8 in 32
uint32_t SatAddUnsigned8_SWAR_nils(uint32_t x, uint32_t y) {
  uint32_t signmask = 0x80808080;
  uint32_t t0 = (y ^ x) & signmask;
  uint32_t t1 = (y & x) & signmask;
  x &= ~signmask;
  y &= ~signmask;
  x += y;
  t1 |= t0 & x;
  t1 = (t1 << 1) - (t1 >> 7);
  return (x ^ t0) | t1;
}
uint32_t SatAddUnsigned32_nils (uint32_t x, uint32_t y) {
  uint32_t signmask = 0x80000000;
  uint32_t t0 = (y ^ x) & signmask;
  uint32_t t1 = (y & x) & signmask;
  x &= ~signmask;
  y &= ~signmask;
  x += y;
  t1 |= t0 & x;
  t1 = (t1 << 1) - (t1 >> 31);
  return (x ^ t0) | t1;
}


// signed saturation
int32_t signed_sadd_Hannodje(int32_t a, int32_t b){
    int32_t sum = a+b;
    int32_t overflow = ((a^sum)&(b^sum))>>31;
    return (overflow<<31)^(sum>> (overflow&31));
}

int main(){}

#endif