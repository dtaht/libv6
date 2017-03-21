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
