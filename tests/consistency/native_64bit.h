/**
 * native_64bit.h
 *
 */

#ifndef NATIVE_64BIT_H
#define NATIVE_64BIT_H

static inline size_t v6_nequal (const unsigned char *p1,
                                   const unsigned char *p2)
{
        const unsigned long *up1 = (const unsigned long *)p1;
        const unsigned long *up2 = (const unsigned long *)p2;

        return ((up1[0] ^ up2[0]) | (up1[1] ^ up2[1]));
}

static inline size_t v6_equal (const unsigned char *p1,
                                   const unsigned char *p2) {
	return !v6_nequal(p1,p2);
}

static inline size_t
v4mapped(const unsigned char *address)
{
    const unsigned long *up1 = (const unsigned long *) address;
    const unsigned int *up2 = (const unsigned int *) (&address[8]);
    // Fixme Address extend?
    return ((up1[0] ^ 0) | (up2[0] ^ htobe32(0xffff))) == 0UL;
}

static inline void
v4tov6(unsigned char *dst, const unsigned char *src)
{
    memcpy(dst, v4prefix, 12);
    memcpy(dst + 12, src, 4);
}

static inline enum prefix_status
prefix_cmp(const unsigned char *p1, unsigned char plen1,
           const unsigned char *p2, unsigned char plen2)
{
    int plen = MIN(plen1, plen2);

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
