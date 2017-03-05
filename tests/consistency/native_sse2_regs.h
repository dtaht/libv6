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

static inline int v6_equal (const usimd p1,
                            const usimd p2) {
       return p1 == p2 ;
}

static inline int v6_nequal (const usimd p1,
                            const usimd p2) {
       return p1 != p2
}

static inline int
v4mapped(const usimd address)
{
    return memcmp(address, v4prefix, 12) == 0;
}


#endif
