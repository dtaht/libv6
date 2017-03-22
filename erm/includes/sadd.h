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

static inline u16 sadd16(us16 a, uint16_t b)
{
  u16 c = a + b;
  if(c < a) /* Can only happen due to overflow */
    c = -1;
  return c;
}

// Saturating arithmetic courtesy of:
// http://stackoverflow.com/questions/121240/how-to-do-saturating-addition-in-c
// Sorted in order of least bad on x86, with gcc5

u16 adds16_msalters(us16 a, uint16_t b)
{
  u16 c = a + b;
  if(c < a) /* Can only happen due to overflow */
    c = -1;
  return c;
}
u32 adds32_msalters(us32 a, uint32_t b)
{
  u32 c = a + b;
  if(c < a) /* Can only happen due to overflow */
    c = -1;
  return c;
}

#if defined(__i386__) || defined(__x86_64__)
// TODO: let the compiler know that the operands are interchangeable

// ideally check __i686__ for cmov, but that's only defined for an actual i686
// target, not e.g. haswell
u32 adds32_x86_pjc_mov(us32 a, uint32_t b)
{ // shorter critical-path latency by one cycle than the sbb version
  u32 tmp = -1;
  // { AT&T    | Intel }  syntax alternatives.  The other versions without
  // this  will break with -masm=intel
  asm("add     { %[b],%[a]   | %[a],  %[b] }\n\t"
      "cmovnc  { %[a],%[dst] | %[dst],%[a] }"
      : [dst] "+r"(tmp), [a] "+&r"(a)
      : [b] "g"(b) // use "rme" for a 64bit version, since it still uses 32bit
                   // sign-extended immediate
      // no clobbers
      );
  return tmp;
}
u32 adds32_x86_pjc_sbb(us32 a, uint32_t b)
{ // smaller code-size, but longer critical-path latency (4c on Haswell, 3c on
  // AMD and Broadwell/Skylake)
  // sbb is on the critical path, unlike mov -1.  sbb and cmov are the same
  // latency on all CPUs
  // sbb same,same doesn't break the dependency on the old value of dst on any
  // uarch except AMD
  u32 tmp;
  asm("add     %[b], %[a]\n\t"
      "sbb     %[dst], %[dst]\n\t"
      "or      %[a], %[dst]"
      : [dst] "=r"(tmp), [a] "+&r"(a)
      : [b] "g"(b));
  return tmp;
}
u32 adds32_x86_pjc_inplace(us32 a, uint32_t b)
{ // can reuse the same -1 register, but can't produce the result in a new
  // register
  asm("add  %[b], %[a]\n\t"
      "cmovc %[sat], %[a]"
      : [a] "+&r"(a)
      : [b] "g"(b), [sat] "r"(-1) // could use an "rm" constraint on [sat],
                                  // but loading -1 from memory is dumb, so
                                  // stop clang from shooting itself in the
                                  // foot
      );
  return a;
}
#endif

unsigned saturate_add_uint_kevin(unsigned x, unsigned y)
{
  if(y > UINT32_MAX - x) return UINT32_MAX;
  return x + y;
}
u16 sadd16_remo(us16 a, uint16_t b)
{
  return (a > 0xFFFF - b) ? 0xFFFF : a + b;
}

u32 sadd32_remo(us32 a, uint32_t b)
{
  return (a > 0xFFFFFFFF - b) ? 0xFFFFFFFF : a + b;
}


u32 sadd32_r(us32 a, uint32_t b)
{ // from R.
  uint64_t s = (uint64_t)a + b;
  return -(s >> 32) | (u32)s;
}

#define sadd16(a, b)                                                         \
  (u16)(((u32)(a) + (us32)(b)) > 0xffff ? 0xffff : ((a) + (b)))
u32 sadd32_op(us32 a, uint32_t b)
{
  return (u32)(((uint64_t)(a) + (uint64_t)(b)) > 0xffffffff ? 0xffffffff :
                                                                   ((a) + (b)));
}


// unfortunately compiles to two conditional branches or cmoves, because
// compilers don't grok this
u32 saturate_add32_dgentry(us32 a, uint32_t b)
{
  u32 sum = a + b;
  if((sum < a) || (sum < b))
    return ~((u32)0);
  else
    return sum;
} /* saturate_add32 */
u16 saturate_add16_dgentry(us16 a, uint16_t b)
{
  u16 sum = a + b;
  if((sum < a) || (sum < b))
    return ~((u16)0);
  else
    return sum;
} /* saturate_add16 */


// These generate horrible code :(
// SWAR 8 in 32
u32 SatAddUnsigned8_SWAR_nils(us32 x, uint32_t y)
{
  u32 signmask = 0x80808080;
  u32 t0 = (y ^ x) & signmask;
  u32 t1 = (y & x) & signmask;
  x &= ~signmask;
  y &= ~signmask;
  x += y;
  t1 |= t0 & x;
  t1 = (t1 << 1) - (t1 >> 7);
  return (x ^ t0) | t1;
}
u32 SatAddUnsigned32_nils(us32 x, uint32_t y)
{
  u32 signmask = 0x80000000;
  u32 t0 = (y ^ x) & signmask;
  u32 t1 = (y & x) & signmask;
  x &= ~signmask;
  y &= ~signmask;
  x += y;
  t1 |= t0 & x;
  t1 = (t1 << 1) - (t1 >> 31);
  return (x ^ t0) | t1;
}


// signed saturation
s32 signed_sadd_Hannodje(int32_t a, int32_t b)
{
  s32 sum = a + b;
  s32 overflow = ((a ^ sum) & (b ^ sum)) >> 31;
  return (overflow << 31) ^ (sum >> (overflow & 31));
}

int main() {}

#endif
