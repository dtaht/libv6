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


#endif
