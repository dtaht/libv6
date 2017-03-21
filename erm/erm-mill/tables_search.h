/**
 * tables_search.h
 *
 * Dave Taht
 * 2017-03-18
 */

// we constrain the loop to no more than RUNAHEAD+1 searches
// because we have to yield to other threads at some point
// or fetch more memory into the cache
// RUNAHEAD must be a power of two - 1

#ifndef RUNAHEAD
#define RUNAHEAD 8
#endif

#ifndef COUNTER1
#define COUNTER1
#endif

#ifndef STACKMEM
#define STACKMEM
#endif

#ifndef SOMETIMES_INLINE
#define SOMETIMES_INLINE
// #define SOMETIMES_INLINE static inline
#endif

// FIXME: trying preinc for gcc in this version

// There are two operations done here
// one is an search, where we return a result:
// b < size of b table - possible hit
// b > size of b table - miss

// the other is a merge sort insert
// b < size of the b table - no need to insert
// b < size of table and a != b miss, must continue
// b is inserted, must switch to next a

// We have to give up and hand control back to the
// garbage collector in order to keep hard R/T after a miss.

// #define OP2(result,bpointer,next)
// #define OP(bpointer,next)

//#define OP(b, match = *a++;)
//#define OP2(r, b, match = *a++;) if((r =

//		 match = *a++;


#ifndef RESULT
extern tbl_b my_result(tbl_a a, tbl_b b, int r);
#define RESULT(r, a, b) my_result(*a, *b, r)
#endif

#ifdef B_ALIGNED_YES
#define B_ALIGNED restrict
#endif

// I don't want to return pointers here but structures in the right regs
// also I have a reg already defined with -1 in it on the
// parallella - will that get picked up?
// and I want the compiler to consider both as non-overlapping

REGCALL SOMETIMES_INLINE tbl_b PO(roar_match_preindex)(tbl_a* a, tbl_b* B_ALIGNED b, unsigned int c)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned int r = -1;
  tbl_a match = *a;
  --b;
  do {
    while(c++ & (RUNAHEAD - 1)) r += match == *++b;
  } while((r = RESULT(r, a, b)));

  return *b;
}

REGCALL SOMETIMES_INLINE tbl_b PO(roar_match_preindex_bool)(tbl_a* a, tbl_b* B_ALIGNED b)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned int r = -1;
  unsigned int c = 0;
  tbl_a match = *a;
  --b;
  do {
    while(c++ < RUNAHEAD) r |= match == *++b;
  } while((r = RESULT(r, a, b)));

  return *b;
}

#if RUNAHEAD == 8
REGCALL SOMETIMES_INLINE tbl_b PO(roar_match_unroll)(tbl_a* a, tbl_b* B_ALIGNED b)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned int r = 0;
  tbl_a match = *a;
  do {
    r += match == *b;
    r += match == *++b;
    r += match == *++b;
    r += match == *++b;
    r += match == *++b;
    r += match == *++b;
    r += match == *++b;
    r += match == *++b;
  } while((r = RESULT(r, a, b)));

  return *b;
}
#endif

REGCALL SOMETIMES_INLINE tbl_b PO(roar_match_freerun)(tbl_a* a, tbl_b* B_ALIGNED b, unsigned int c)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned char d = c & 255;
  unsigned char r = -1;
  tbl_a match = *a;
  --b;
  do {
    while(d++) r += (match == *++b);
  } while((r = RESULT(r, a, b)));

  return *b;
}

// like so much here, this is not correct code
REGCALL
SOMETIMES_INLINE tbl_b PO(roar_match_freerun_vvectorno)(tbl_a* a, tbl_b* B_ALIGNED b)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned char d;
  unsigned char r = -1;
  tbl_a match = *a;
  b--;
  for(d = 1; d;
      d++) // I love writing infinite looking loops even tho icc will complain
    r += (match == b[d]);
  r = RESULT(r, a, b);

  return *b;
}

// like so much here, this is not correct code
REGCALL
SOMETIMES_INLINE tbl_b PO(roar_match_freerun_vvectorno_zoomon)(tbl_a* a, tbl_b* B_ALIGNED b)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned char d;
  unsigned char r = -1;
  tbl_a match = *a;
  b--; // FIXME: not correct - b-=(255-16)?
  d = 255 - 16;
  // this variant - if it strikes out, will zoom on for another 255 iterations
  // at a time
  // which lets me warn the main cpu it's going to be busier (yield/prefetch,
  // whatever)
  do {
    for(; d & r; d++) // I love writing infinite looking loops even tho icc
                      // will complain
      r += (match == b[d]);
  } while(d++ & (r = RESULT(r, a, b)));

  return *b;
}

// like so much here, this is not correct code. But at least it vectorized
REGCALL
SOMETIMES_INLINE tbl_b PO(roar_match_freerun_vvectoryes)(tbl_a* a, tbl_b* B_ALIGNED b)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned int d;
  unsigned char r = -1;
  tbl_a match = *a;
#pragma simd

  for(d = 0; d < (RUNAHEAD); d++) r += (match == b[d]);
  r = RESULT(r, a, b);

  return *b;
}

// like so much here, this is not correct code. But at least it vectorized
// and unrolls and seems to be the best of

REGCALL
SOMETIMES_INLINE tbl_b PO(roar_match_vvectoryesmorecorrect)(tbl_a* a, tbl_b* B_ALIGNED b)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned int d;
  unsigned char r = -1;
  tbl_a match = *a;
#pragma simd
retry:
  for(d = 0; d < (RUNAHEAD); d++) r += (match == b[d]);

  if((r = RESULT(r, a, b))) return *b;
  b = &b[RUNAHEAD + 1];
  goto retry;

  return *b;
}

REGCALL
SOMETIMES_INLINE tbl_b PO(roar_match_firsthit)(tbl_a* a, tbl_b* B_ALIGNED b, unsigned int c)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned int r = -1;
  tbl_a match = *a;
  --b;
  while(r) {
    while(c++ & (RUNAHEAD - 1) & r) r += match == *++b;
  }

  return *b;
}

REGCALL
SOMETIMES_INLINE tbl_b PO(roar_match_firsthit_nosimd_vvvector)(tbl_a* a, tbl_b* B_ALIGNED b)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned int r = -1;
  tbl_a match = *a;
  do {
    for(int c = 0; c < (RUNAHEAD); c++) {
      r += (match == b[c]);
    }
  } while((r = RESULT(r, a, b)));
  return *b;
}


REGCALL
SOMETIMES_INLINE tbl_b PO(roar_match_firsthit_vvvector)(tbl_a* a, tbl_b* B_ALIGNED b)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned int r = -1;
  tbl_a match = *a;
  do {
#pragma simd
    for(int c = 0; c < (RUNAHEAD); c++) {
      r += (match == b[c]);
    }
  } while((r = RESULT(r, a, b)));

  return *b;
}

REGCALL
SOMETIMES_INLINE
tbl_b PO(roar_match_freerun_firsthit)(tbl_a* a, tbl_b* B_ALIGNED b, unsigned int c)
{
  b = ASSUME_ALIGNED(b, 16);
  unsigned char d = c & 255;
  unsigned char r = -1;
  tbl_a match = *a;
  --b;
  do {
    while(d++ & r) r += match == *++b;
  } while((r = RESULT(r, a, b)));

  return *b;
}
