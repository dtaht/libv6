/**
 * native_neon.h
 *
 */

#ifndef NATIVE_NEON_H
#define NATIVE_NEON_H
#ifdef HAVE_NEON
#include <arm_neon.h>

static inline uint32_t is_not_zero(uint32x4_t v)
{
    uint32x2_t tmp = vorr_u32(vget_low_u32(v), vget_high_u32(v));
    return vget_lane_u32(vpmax_u32(tmp, tmp), 0);
}

static inline size_t v6_nequal (const unsigned char *p1,
				const unsigned char *p2) {
	uint32x4_t up1 = vld1q_u32((const unsigned int *) p1);
        uint32x4_t up2 = vld1q_u32((const unsigned int *) p2);
	return is_not_zero(veorq_u32(up1,up2));
}

static inline size_t v6_equal (const unsigned char *p1,
                                   const unsigned char *p2) {
	return !v6_nequal(p1,p2);
}

static inline int
v4mapped(const unsigned char *address)
{
    return memcmp(address, v4prefix, 12) == 0;
}


static inline void
v4tov6(unsigned char *dst, const unsigned char *src)
{
    my_memcpy(dst, v4prefix, 12);
    my_memcpy(dst + 12, src, 4);
}

static inline enum prefix_status
prefix_cmp(const unsigned char *p1, unsigned char plen1,
           const unsigned char *p2, unsigned char plen2)
{
    int plen = MIN(plen1, plen2);

    if(v4mapped(p1) != v4mapped(p2))
        return PST_DISJOINT;

    if(memcmp(p1, p2, plen / 8) != 0)
        return PST_DISJOINT;

    if(plen % 8 != 0) {
        int i = plen / 8 + 1;
	// FIXME - does the shift differ for endianness?
        unsigned char mask = (0xFF << (plen % 8)) & 0xFF;
        if((p1[i] & mask) != (p2[i] & mask))
            return PST_DISJOINT;
    }
    // FIXME: I don't see how we ever get here. Ah:
    // fd99::1:/60
    // fd99::1:/64

    if(plen1 < plen2)
        return PST_LESS_SPECIFIC;
    else if(plen1 > plen2)
        return PST_MORE_SPECIFIC;
    else
        return PST_EQUALS;
}

#endif

#endif

