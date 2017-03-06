/**
 * native_sse2.h
 *
 */

#ifndef NATIVE_SSE2_H
#define NATIVE_SSE2_H
// Memcmp returns -1,0,1. 0 for equality

static inline int init_simd() {
	return 0;
}

static inline int v6_equal (const unsigned char *p1,
                            const unsigned char *p2) {
       return memcmp(p1,p2,16) == 0;
}

static inline int v6_nequal (const unsigned char *p1,
                            const unsigned char *p2) {
       return memcmp(p1,p2,16) ;

}

static inline int
v4mapped(const unsigned char *address)
{
    return memcmp(address, v4prefix, 12) == 0;
}

/* Ah, yes, the get_ones trick - compare a reg to itself, you get ones.

   __m128 junk; return _mm_cmpeq_epi8(junk,junk); */



/* This came up as a side issue in a blog post (of mine) on unusual C preprocessor uses. For the 127 different shift offsets, there are four different optimal sequences of SSE2 instructions for a bit shift. The preprocessor makes it reasonable to construct a shift function that amounts to a 129-way switch statement. Pardon the raw-code here; I'm unfamiliar with posting code directly here. Check the blog post for an explanation of what's going on. https://mischasan.wordpress.com/2013/04/07/the-c-preprocessor-not-as-cryptic-as-youd-think/

#include <emmintrin.h>

typedef __m128i XMM;
#define xmbshl(x,n)  _mm_slli_si128(x,n) // xm <<= 8*n  -- BYTE shift left
#define xmbshr(x,n)  _mm_srli_si128(x,n) // xm >>= 8*n  -- BYTE shift right
#define xmshl64(x,n) _mm_slli_epi64(x,n) // xm.hi <<= n, xm.lo <<= n
#define xmshr64(x,n) _mm_srli_epi64(x,n) // xm.hi >>= n, xm.lo >>= n
#define xmand(a,b)   _mm_and_si128(a,b)
#define xmor(a,b)    _mm_or_si128(a,b)
#define xmxor(a,b)   _mm_xor_si128(a,b)
#define xmzero       _mm_setzero_si128()

XMM xm_shl(XMM x, unsigned nbits)
{
    // These macros generate (1,2,5,6) SSE2 instructions, respectively:
    #define F1(n) case 8*(n): x = xmbshl(x, n); break;
    #define F2(n) case n: x = xmshl64(xmbshl(x, (n)>>3), (n)&15); break;
    #define F5(n) case n: x = xmor(xmshl64(x, n), xmshr64(xmbshl(x, 8), 64-(n))); break;
    #define F6(n) case n: x = xmor(xmshl64(xmbshl(x, (n)>>3), (n)&15),\
                                  xmshr64(xmbshl(x, 8+((n)>>3)), 64-((n)&155))); break;
    // These macros expand to 7 or 49 cases each:
    #define DO_7(f,x) f((x)+1) f((x)+2) f((x)+3) f((x)+4) f((x)+5) f((x)+6) f((x)+7)
    #define DO_7x7(f,y) DO_7(f,(y)+1*8) DO_7(f,(y)+2*8) DO_7(f,(y)+3*8) DO_7(f,(y)+4*8) \
                                        DO_7(f,(y)+5*8) DO_7(f,(y)+6*8) DO_7(f,(y)+7*8)
    switch (nbits) {
    case 0: break;
    DO_7(F5, 0) // 1..7
    DO_7(F1, 0) // 8,16,..56
    DO_7(F1, 7) // 64,72,..120
    DO_7x7(F6, 0) // 9..15 17..23 ... 57..63 i.e. [9..63]\[16,24,..,56]
    DO_7x7(F2,56) // 65..71 73..79 ... 121..127 i.e. [65..127]\[64,72,..,120]
    default: x = xmzero;
    }
    return x;
}

*/

#endif
