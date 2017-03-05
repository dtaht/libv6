/*
src_prefix
prefix
src_mask
mask
bit96

gate

not_v4mapped() {
t1 = and(dest,bits96)
t1 = xor(t1,v4_mapped)
}

int
v4mapped(const unsigned char *address)
{
    return memcmp(address, v4prefix, 12) == 0;
}

*/

   /* Check that the protocol family is consistent. */
/*
    if(plen >= 96 && v4mapped(dest)) {
        if(!v4mapped(gate) ||
           (src_plen > 0 && (!v4mapped(src) || src_plen < 96))) {
            errno = EINVAL;
            return -1;
        }
    } else {
        if(v4mapped(gate)|| (src_plen > 0 && v4mapped(src))) {
            errno = EINVAL;
            return -1;
        }
    }
}

int
linklocal(const unsigned char *address)
{
    return memcmp(address, llprefix, 8) == 0;
}

}

tmp = v4prefix
tmp = vld(src+12)
str(dst)

void
v4tov6(unsigned char *dst, const unsigned char *src)
{
    memcpy(dst, v4prefix, 12);
    memcpy(dst + 12, src, 4);
}

enum prefix_status
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
        unsigned char mask = (0xFF << (plen % 8)) & 0xFF;
        if((p1[i] & mask) != (p2[i] & mask))
            return PST_DISJOINT;
    }

    if(plen1 < plen2)
        return PST_LESS_SPECIFIC;
    else if(plen1 > plen2)
        return PST_MORE_SPECIFIC;
    else
        return PST_EQUALS;
}

*/

