/**
 * native_neon_regs.h
 * This changes the caller API to be vectors and not pointers
 */

#ifndef NATIVE_NEON_H
#define NATIVE_NEON_H

typedef uint32x4_t usimd;

const unsigned char fe80str[16] = { 0xfe, 0x80 };
const unsigned char v4prefixstr[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0, 0, 0, 0 };
const unsigned char bits96str[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 
				      0xFF, 0xFF, 0xFF, 0xFF,
				      0xFF, 0xFF, 0xFF, 0xFF,
				      0, 0, 0, 0 };


// Essential constants

register usimd ll asm ("q7");
register usimd v4_prefix asm ("q6");

// These can go long term, just here for simplicity

register usimd zeros asm ("q5");
register usimd ones asm ("q4");

// Calling convention

// These are call-clobberd - which is basically what we
// want, except that I haven't the faintest idea how to get
// them into these registers on calls? Just declare them
// as function parameters? What are the different APIs?

// register usimd src_prefix asm ("q0");
// register usimd prefix asm ("q1");
// register usimd src_mask asm ("q2");
// register usimd prefix_mask asm ("q3");

register usimd cmp_src_prefix asm ("q8");
register usimd cmp_prefix asm ("q9");
register usimd cmp_src_mask asm ("q10");
register usimd cmp_prefix_mask asm ("q11");
register usimd bits96 asm ("q12");

int init_simd() {

/* Ideally this would also do a sanity check (are these regs
   allowed */
	
//   Load up our constant registers before we do anything else

     ll = vld1q_u32((const uint32_t *) fe80str);
     v4_prefix = vld1q_u32((const uint32_t *) v4prefixstr);
     bits96 = vld1q_u32((const uint32_t *) bits96str);

//   It is generally cheaper to just generate these on the fly

     zeros = veorq_u32(ll,ll);
     ones = vmvnq_u32(zeros);
     return 0;
}

static inline uint32_t is_not_zero(uint32x4_t v)
{
    uint32x2_t tmp = vorr_u32(vget_low_u32(v), vget_high_u32(v));
    return vget_lane_u32(vpmax_u32(tmp, tmp), 0);
}

static inline size_t v6_nequal (const usimd p1,
				const usimd p2) {
	return is_not_zero(veorq_u32(p1,p2));
}

static inline size_t v6_equal (const usimd p1,
                                   const usimd p2) {
	return !v6_nequal(p1,p2);
}

static inline int
v4nmapped(const usimd address)
{
    uint32x4_t up1 = vandq_u32(bits96,address));
    return is_not_zero(veorq_u32(up1,v4_prefix));
}

static inline int
v4nmapped_dual(const usimd address1, const usimd address2)
{
    uint32x4_t up1 = vandq_u32(bits96,address1));
    uint32x4_t up2 = vandq_u32(bits96,address2));
    up1 = veorq_u32(up1,v4_prefix); 
    up2 = veorq_u32(up2,v4_prefix);
    /* truth table: 0 0  both are v4_mapped
                    1234 5432 both are not v4_mapped
                    0 1234 one is not v4_mapped and the other is

       the result we want is true when both are or are not v4_mapped
       they are disjoint. And my brain crashes. AND-NOT?.

       an eor gets me 1111s. 
       an or gets me a random value. 
       a not gets me 1111s or another random value. Hmm... lets try another
       way.
       a successful eq against the v4_prefix gets me 1111s across the board...
       nand of bits 48? not and? what?

       moving further down to where this is called we care if they are
       fully equal and if fully disjoint...
       perhaps some of my problem is that my representation of a v4
       prefix is wrong. We cannot have a v4 prefix that is all ones or
       all zeros, by definition of ipv4. , so in a true comparison for
       is this a v4 prefix, we need all zeroes high, the ones indicating
       and random bits in the lowest 4 bytes. But is a default route 
	a zero? Aggh.. brain crash.*/

     // fixme, wrong 
     return is_not_zero(veorq_u32(vorq_u32(up1,up2),v4_prefix));
}

static inline size_t v4mapped (const usimd address) {
	return !v4nmapped(address);
}

static inline void
v4tov6(usimd *dst, const usimd *src)
{
    my_memcpy(dst, v4prefix, 12);
    my_memcpy(dst + 12, src, 4);
}

static inline enum prefix_status
prefix_cmp(const usimd p1, unsigned char plen1,
           const usimd p2, unsigned char plen2)
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

