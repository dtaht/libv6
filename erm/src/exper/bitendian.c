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

#define REVERSEB(v) reverse_obvious(v)

///typedef struct {
// uint32_t a VECTOR(16);
// }v6addr_t;

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
// need my hooks for bits


#define CONVERT "%9x%9x%9x%9x"


// just need rbit for words

int32 *image;
for (i = 0; i < size / 4; h++) {
    asm("rbit %1,%0" : "=r" (image[i]) : "r" (image[i]));
    asm("rev %1,%0" : "=r" (image[i]) : "r" (image[i]));
}

size_t reverse(size_t n, unsigned int bytes)
    {
        __asm__("BSWAP %0" : "=r"(n) : "0"(n));
        n >>= ((sizeof(size_t) - bytes) * 8);
        n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);
        n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
        n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);
        return n;
    }

// Native ARM instruction "rbit" can do it with 1 cpu cycle and 1 extra cpu register, impossible to beat.

/*
bits 64
global bitflipbyte

bitflipbyte:    
        vmovdqa     ymm2, [rdx]
        add         rdx, 20h
        vmovdqa     ymm3, [rdx]
        add         rdx, 20h
        vmovdqa     ymm4, [rdx]
bitflipp_loop:
        vmovdqa     ymm0, [rdi] 
        vpand       ymm1, ymm2, ymm0 
        vpandn      ymm0, ymm2, ymm0 
        vpsrld      ymm0, ymm0, 4h 
        vpshufb     ymm1, ymm4, ymm1 
        vpshufb     ymm0, ymm3, ymm0         
        vpor        ymm0, ymm0, ymm1
        vmovdqa     [rdi], ymm0
        add     rdi, 20h
        dec     rsi
        jnz     bitflipp_loop
        ret
	*/


   n = reverse(n, sizeof(char));//only reverse 8 bits
    n = reverse(n, sizeof(short));//reverse 16 bits
    n = reverse(n, sizeof(int));//reverse 32 bits
    n = reverse(n, sizeof(size_t));//reverse 64 bits

/* Classic binary partitioning algorithm */
inline uint32_t brev_classic (uint32_t a)
{
    uint32_t m;
    a = (a >> 16) | (a << 16);                            // swap halfwords
    m = 0x00ff00ff; a = ((a >> 8) & m) | ((a << 8) & ~m); // swap bytes
    m = m^(m << 4); a = ((a >> 4) & m) | ((a << 4) & ~m); // swap nibbles
    m = m^(m << 2); a = ((a >> 2) & m) | ((a << 2) & ~m);
    m = m^(m << 1); a = ((a >> 1) & m) | ((a << 1) & ~m);
    return a;
}

uint64_t reverse_vectorized(const uint64_t n,
                 const uint64_t k)
{
        uint64_t r, i;
        for (r = 0, i = 0; i < k; ++i)
                r |= ((n >> i) & 1) << (k - i - 1);
        return r;
}

int new_main()
{
        const uint64_t size = 64;
        uint64_t sum = 0;
        uint64_t a;
        for (a = 0; a < (uint64_t)1 << 30; ++a)
                sum += reverse(a, size);
        printf("%" PRIu64 "\n", sum);
        return 0;
}

/* Knuth's algorithm from http://www.hackersdelight.org/revisions.pdf. Retrieved 8/19/2015 */
inline uint32_t brev_knuth (uint32_t a)
{
    uint32_t t;
    a = (a << 15) | (a >> 17);
    t = (a ^ (a >> 10)) & 0x003f801f; 
    a = (t + (t << 10)) ^ a;
    t = (a ^ (a >>  4)) & 0x0e038421; 
    a = (t + (t <<  4)) ^ a;
    t = (a ^ (a >>  2)) & 0x22488842; 
    a = (t + (t <<  2)) ^ a;
    return a;
}

unsigned int reverse_asm(unsigned int t)
    {
        size_t n = t;//store in 64 bit number for call to BSWAP
        __asm__("BSWAP %0" : "=r"(n) : "0"(n));
        n >>= ((sizeof(size_t) - sizeof(unsigned int)) * 8);
        n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);
        n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
        n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);
        return n;
    }

unsigned int reverse_builtin(unsigned int t)
    {
        size_t n = t;//store in 64 bit number for call to BSWAP
	n = __builtin_bswap(n)
        n >>= ((sizeof(size_t) - sizeof(unsigned int)) * 8);
        n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);
        n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
        n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);
        return n;
    }

int bitconvert16_plain(v6addr_t a)
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
  v6addr_t in;
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
