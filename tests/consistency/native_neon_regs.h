/**
 * native_neon_regs.h
 * This changes the caller API to be vectors and not pointers
 */

#ifndef NATIVE_NEON_H
#define NATIVE_NEON_H
#include <arm_neon.h>

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

static inline int init_simd() {

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

static inline uint32_t is_not_zero(usimd v)
{
    // I keep thinking I should just be able to get the max from all
    // lanes and skip this step entirely. FIXME. UMAXV exists for
    // all lanes in aarch64. armv7?
  
    uint32x2_t tmp = vorr_u32(vget_low_u32(v), vget_high_u32(v));
    return vget_lane_u32(vpmax_u32(tmp, tmp), 0); 
}

static inline size_t v6_nequal (const usimd p1,
				const usimd p2) {
	return is_not_zero(veorq_u32(p1,p2));
}

static inline size_t v6_equal (const usimd p1,
                                   const usimd p2) {
	return !is_not_zero(veorq_u32(p1,p2));
}

static inline int
v4nmapped(const usimd address)
{
    usimd up1 = vandq_u32(bits96,address);
    return is_not_zero(veorq_u32(up1,v4_prefix));
}

static inline size_t v4mapped (const usimd address) {
	return !v4nmapped(address);
}

// This could do a simd return instead
static inline void
v4tov6(usimd *dst, const char *src)
{
  usimd d = v4_prefix;
  // incorrect also:
  // FIXME my_memcpy(dst + 12, src, 4); // load the lane
  // FIXME save everything to dst;
  *dst = d;
}

static inline enum prefix_status
prefix_cmp(const usimd p1, unsigned char plen1,
           const usimd p2, unsigned char plen2)
{
    int plen = MIN(plen1, plen2);

    if(v4mapped(p1) != v4mapped(p2))
        return PST_DISJOINT;

    // FIXME this is incorrect - need to generate the mask
    
    if(is_not_zero(p2) != is_not_zero(p1))
        return PST_DISJOINT;

    if(plen1 < plen2)
        return PST_LESS_SPECIFIC;
    else if(plen1 > plen2)
        return PST_MORE_SPECIFIC;
    else
        return PST_EQUALS;
}

#endif

