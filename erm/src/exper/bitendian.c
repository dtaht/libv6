/**
 * bitendian.c conversions in various co-processors
 *
 * Dave Taht
 * 2017-03-20
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "preprocessor.h"
#include "tabeld.h"
#include "simd.h"

// What I want is to popcount and reverse bits at the same time
// popcount doesn't care about endian. It seems silly (but benchmarking awaits)
// to not do all this on one pass
// pop = popcount128(result);
// result = flip128(result);
// vs
// pop = popflip128(&result);
// This is made hairy because in some cases popcount is a 20 ins call.
// vs a single asm instruction.
// and some cases bitreversal is also
// and in both cases, they use a lot of magic constants

//#define REVERSEB(v) reverse_obvious(v)
#define REVERSEB(v) reverse_asm_const(v)

///typedef struct {
// u32 a VECTOR(16);
// }ip_addr;

unsigned char littleendian[] = { 0, 1, 2,  3,  4,  5,  6,  7,
                                 8, 9, 10, 11, 12, 13, 14, 3 };

// https://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious

#define CHAR_BIT 8

unsigned int reverse_obvious(int v)
{
  unsigned int r = v; // r will be reversed bits of v; first get LSB of v
  int s = sizeof(v) * CHAR_BIT - 1; // extra shift needed at end

  for(v >>= 1; v; v >>= 1) {
    r <<= 1;
    r |= v & 1;
    s--;
  }
  r <<= s;
  return r;
}

// FIXME: need my hooks for printf bits

#define CONVERT "%9x%9x%9x%9x"

// Native ARM instruction "rbit" can do it with 1 cpu cycle and 1 extra cpu register, impossible to beat. Can it be done right in neon?
// just need rbit for words

#ifdef __arm__

return u32 *reverse_arm(u32 *image, int size) {

	for (i = 0; i < size / 4; h++) {
    asm("rbit %1,%0" : "=r" (image[i]) : "r" (image[i]));
    asm("rev %1,%0" : "=r" (image[i]) : "r" (image[i]));
}
}

#endif

#ifdef __x86_64__

/*
    n = reverse(n, sizeof(char));//only reverse 8 bits
    n = reverse(n, sizeof(short));//reverse 16 bits
    n = reverse(n, sizeof(int));//reverse 32 bits
    n = reverse(n, sizeof(size_t));//reverse 64 bits
*/

size_t reverse_bswap_asm(size_t n, unsigned int bytes)
    {
        __asm__("BSWAP %0" : "=r"(n) : "0"(n));
        n >>= ((sizeof(size_t) - bytes) * 8);
        n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);
        n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
        n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);
        return n;
    }

 int c1 = 0xaaaaaaaa;
 int c2 = 0x55555555;
 int c3 = 0xcccccccc;
 int c4 = 0x33333333;
 int c5 = 0xf0f0f0f0;
 int c6 = 0x0f0f0f0f; // it irks me that these are just basically shifted

//  40088a:       81 e5 f0 f0 f0 f0       and    $0xf0f0f0f0,%ebp
//  400890:       25 0f 0f 0f 0f          and    $0xf0f0f0f,%eax

// vs the equivalent shorter instruction sequence (in bytes)
// That I don't really want to inline, either.
/*
  400834:       48 0f cb                bswap  %rbx
  400837:       48 c1 eb 20             shr    $0x20,%rbx
  40083b:       49 89 d5                mov    %rdx,%r13
  40083e:       48 89 d9                mov    %rbx,%rcx
  400841:       48 21 f3                and    %rsi,%rbx
  400844:       4d 21 c5                and    %r8,%r13
  400847:       4c 21 c9                and    %r9,%rcx
  40084a:       48 21 c2                and    %rax,%rdx
  40084d:       48 01 db                add    %rbx,%rbx
  400850:       48 d1 e9                shr    %rcx
  400853:       48 c1 e2 04             shl    $0x4,%rdx
  400857:       49 c1 ed 04             shr    $0x4,%r13
  40085b:       48 09 d9                or     %rbx,%rcx
  40085e:       49 09 d5                or     %rdx,%r13
  400861:       48 89 ca                mov    %rcx,%rdx
  400864:       48 21 f9                and    %rdi,%rcx
  400867:       4c 21 e2                and    %r12,%rdx
  40086a:       48 c1 ea 02             shr    $0x2,%rdx
  40086e:       48 c1 e1 02             shl    $0x2,%rcx
*/

inline unsigned int reverse_asm_const(unsigned int t)
    {
        size_t n = t;//store in 64 bit number for call to BSWAP
        __asm__("BSWAP %0" : "=r"(n) : "0"(n));
        n >>= ((sizeof(size_t) - sizeof(unsigned int)) * 8);
        n = ((n & c1) >> 1) | ((n & c2) << 1);
        n = ((n & c3) >> 2) | ((n & c4) << 2);
        n = ((n & c5) >> 4) | ((n & (c5 >> 4)) << 4);
        return n;
    }

inline unsigned int reverse_asm(unsigned int t)
    {
        size_t n = t;//store in 64 bit number for call to BSWAP
        __asm__("BSWAP %0" : "=r"(n) : "0"(n));
        n >>= ((sizeof(size_t) - sizeof(unsigned int)) * 8);
        n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);
        n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
        n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);
        return n;
    }

#endif

/* Classic binary partitioning algorithm */
inline u32 brev_classic (u32 a)
{
    u32 m;
    a = (a >> 16) | (a << 16);                            // swap halfwords
    m = 0x00ff00ff; a = ((a >> 8) & m) | ((a << 8) & ~m); // swap bytes
    m = m^(m << 4); a = ((a >> 4) & m) | ((a << 4) & ~m); // swap nibbles
    m = m^(m << 2); a = ((a >> 2) & m) | ((a << 2) & ~m);
    m = m^(m << 1); a = ((a >> 1) & m) | ((a << 1) & ~m);
    return a;
}

u64 reverse_vectorized(const u64 n,
                 const u64 k)
{
        u64 r, i;
        for (r = 0, i = 0; i < k; ++i)
                r |= ((n >> i) & 1) << (k - i - 1);
        return r;
}

/*
int new_main()
{
        const u64 size = 64;
        u64 sum = 0;
        u64 a;
        for (a = 0; a < (u64)1 << 30; ++a)
                sum += reverse(a, size);
        printf("%" PRIu64 "\n", sum);
        return 0;
}

*/

/* Knuth's algorithm from http://www.hackersdelight.org/revisions.pdf. Retrieved 8/19/2015 */

inline u32 brev_knuth (u32 a)
{
    u32 t;
    a = (a << 15) | (a >> 17);
    t = (a ^ (a >> 10)) & 0x003f801f;
    a = (t + (t << 10)) ^ a;
    t = (a ^ (a >>  4)) & 0x0e038421;
    a = (t + (t <<  4)) ^ a;
    t = (a ^ (a >>  2)) & 0x22488842;
    a = (t + (t <<  2)) ^ a;
    return a;
}

unsigned int reverse_builtin_32(unsigned int t)
    {
        size_t n = t;//store in 64 bit number for call to BSWAP
	n = __builtin_bswap32(n);
        n >>= ((sizeof(size_t) - sizeof(unsigned int)) * 8);
        n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);
        n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
        n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);
        return n;
    }

// I don't trust myself here

unsigned int reverse_builtin_64(size_t t)
    {
        size_t n = t;//store in 64 bit number for call to BSWAP
	n = __builtin_bswap64(n);
        n >>= ((sizeof(size_t) - sizeof(unsigned int)) * 8);
        n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);
        n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
        n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);
        return n;
    }

int bitconvert16_plain(ip_addr a)
{
  Vec4_t v;
  Vec4_t c;
  Vec4_t o;

  v.p = a;
  printf("IN:  " CONVERT "\n", v.u[0], v.u[1], v.u[2], v.u[3]);

  c.u[0] = REVERSEB(v.u[3]);
  c.u[1] = REVERSEB(v.u[2]);
  c.u[2] = REVERSEB(v.u[1]);
  c.u[3] = REVERSEB(v.u[0]);

  printf("OUT: " CONVERT "\n", c.u[0], c.u[1], c.u[2], c.u[3]);

  o.u[0] = REVERSEB(c.u[3]);
  o.u[1] = REVERSEB(c.u[2]);
  o.u[2] = REVERSEB(c.u[1]);
  o.u[3] = REVERSEB(c.u[0]);

  printf("RET: " CONVERT "\n", o.u[0], o.u[1], o.u[2], o.u[3]);

// if(c == a) return true;

  return false;

}

int main()
{
  ip_addr in;
  Vec4_t a;
  memcpy(&a.c,littleendian,sizeof(a));
  in = a.a;
  if(bitconvert16_plain(in)) {
    printf("success!\n");
    exit(0);
  }
  printf("fail!\n");
  return -1;
}
