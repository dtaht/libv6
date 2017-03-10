/**
 * native_neon_regs.h
 * This changes the caller API to be vectors and not pointers
 */

#ifndef NATIVE_NEON_H
#define NATIVE_NEON_H
#include <arm_neon.h>

#define MIN(a,b) a < b ? b : a

typedef uint32x4_t usimd;

const unsigned char fe80str[16] = { 0xfe, 0x80 };
const unsigned char v4prefixstr[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0, 0, 0, 0 };
const unsigned char bits96str[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 
				      0xFF, 0xFF, 0xFF, 0xFF,
				      0xFF, 0xFF, 0xFF, 0xFF,
				      0, 0, 0, 0 };
const unsigned int maskshiftstr[4] = { 32, 64, 96, 128 };


// Essential constants
// alternatively get_zeros, load top lane with 0xfe80

register usimd ll asm ("q7");

// alternatively get_zeros, load_16 bit lane with ones

register usimd v4_prefix asm ("q6");

usimd maskshift;

// Aarch64 has individual instructions for this that I guess
// stops stalling things but I don't get the intrinsic
// MOVI Vn.2D, #uimm64
// Move immediate (vector) : replicates a “byte mask immediate” consisting of 8 bytes, each byte having only
// the value 0x00 or 0xff, into each 64-bit element.

// I originally wrote this as get_ones/get_zeros. Might change back
// alternatively vdupq_n_u32(0x255, 0x255, 0x255, 0x255);

static inline usimd set_ones(usimd v) { return  v = vceqq_u32(v,v); }
static inline usimd set_zeros(usimd v) { return v = veorq_u32(v,v); }

// I think but am not sure we can "generate random register here", and not specify one

static inline usimd get_ones(const usimd v)  { return  vceqq_u32(v,v); }
static inline usimd get_zeros(const usimd v) { return  veorq_u32(v,v); }

// Calling convention

// These are call-clobberd - which is basically what we
// want, except that I haven't the faintest idea how to get
// them into these registers on calls? Just declare them
// as function parameters? What are the different APIs?

// I thought from the doc I could actually load 4 at a time,
// but that is actually an intrinsic that "makes things easier".
// It might help on structure return -
// src_prefix, prefix being returned.

// register usimd src_prefix asm ("q0");
// register usimd prefix asm ("q1");
// register usimd src_mask asm ("q2");
// register usimd prefix_mask asm ("q3");

register usimd cmp_src_prefix asm ("q8");
register usimd cmp_prefix asm ("q9");
register usimd cmp_src_mask asm ("q10");
register usimd cmp_prefix_mask asm ("q11");

// Hmm: Can I easily make this get96bits? (getones, load one lane with 0s?. Is there a rest of register
// clobbering load?

register usimd bits96 asm ("q12");

static inline int init_simd() {

/* Ideally this would also do a sanity check (are these regs
   allowed */
	
//   Load up our constant registers before we do anything else

     ll = vld1q_u32((const uint32_t *) fe80str);
     v4_prefix = vld1q_u32((const uint32_t *) v4prefixstr);
     bits96 = vld1q_u32((const uint32_t *) bits96str);
     maskshift = vld1q_u32((const uint32_t *) maskshiftstr);
     return 0;
}

static inline size_t is_not_zero(usimd v)
{
    // I keep thinking I should just be able to get the max from all
    // lanes and skip this step entirely. FIXME. UMAXV exists for
    // all lanes in aarch64, so we can return size_t. What's the
  // intrinsic? armv7?

  // The obvious   return vget_lane_u64(vpmax_u64(v,v), 0); 
  // doesn't work.
  // #ifdef AARCH64
  return vmaxvq_u32(v); // ?
  //uint32x2_t tmp = vorr_u32(vget_low_u32(v), vget_high_u32(v));
  //return vget_lane_u32(vpmax_u32(tmp, tmp), 0); 
}

static inline size_t v6_nequal (const usimd p1,
				const usimd p2) {
	return is_not_zero(veorq_u32(p1,p2));
}

static inline size_t v6_equal (const usimd p1,
                                   const usimd p2) {
	return !is_not_zero(veorq_u32(p1,p2));
}

static inline size_t
v4nmapped(const usimd address)
{
    usimd up1 = vandq_u32(bits96,address);
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

  // That loads a lane from a literal
  //  d = vset_lane_u32(val,d,whichlane); // 3? vld?

// This could do a simd return instead and probably should

static inline void
v4tov6(usimd *dst, const char *src)
{
  usimd d = v4_prefix;
  d = vld1q_lane_u32((uint32_t *) src + 12,d,0); // 3? vld?
  *dst = d;
}


// Memset things 1,2,3,4,5,8,16
// void  vst1q_lane_u64(__transfersize(1) uint64_t * ptr, uint64x2_t val, __constrange(0,1) int lane); // VST1.64 {d0}, [r0]

// Get a single lane from the vector
// uint8 = vgetq_lane_u8(uint8x16_t vec, __constrange(0,15) int lane); plen >> 8

// A range of comparison intrinsics are provided. If the comparison is true for a lane, the result in that lane is all bits set to one. If the comparison is false for a lane, all bits are set to zero. The return type is an unsigned integer type. This means that you can use the result of a comparison as the first argument for the vbsl intrinsics.//
// vcltrq will return all ones in the register if true

//uint32x4_t vcltq_u32(uint32x4_t a, uint32x4_t b);

// Not really sure what vector test bits does

// uint32x4_t vtstq_u32(uint32x4_t a, uint32x4_t b); // VTST.32 q0, q0, q0

// vector shift right with insert?

// uint32x4_t vsriq_n_u32(uint32x4_t a, uint32x4_t b, __constrange(1,32) int c); // VSRI.32 q0,q0,#32
// uint64x2_t vsriq_n_u64(uint64x2_t a, uint64x2_t b, __constrange(1,64) int c); // VSRI.64 q0,q0,#64

// vreinterpret{q}_dsttype_srctype
// uint64x1x4_t  vld4_dup_u64(__transfersize(4) uint64_t const * ptr); // VLD1.64 {d0, d1, d2, d3}, [r0]

/* The old getones trick for sse2. compare a reg to itself, you get all ones. xor one you get all zeros

__m128 junk; return _mm_cmpeq_epi8(junk,junk);

*/

// This is completely wrong I just wanted to see how much code was generated

static inline enum prefix_status
prefix_cmp(const usimd p1, unsigned char plen1,
           const usimd p2, unsigned char plen2)
{
    int plen = MIN(plen1, plen2);
    
    asm("nop; /* prefix_cmp really start */");
    usimd ones = get_ones(p1);
    // usimd shift2 = vload_dup_lanes_u32(&plen);
    // saturate to 0 subtract
    // plen to actual shift { abs(plen - 96), 
    int32x4_t shift2 = vdupq_n_s32(plen);
    usimd shift1 = vqsubq_u32(maskshift,shift2);
    usimd mask1 = vshlq_u32(ones, shift1);
    
    shift2 = vdupq_n_s32(plen2);
    shift1 = vqsubq_u32(maskshift,shift2);
    
    usimd mask2 = vshlq_u32(ones, shift1);
    /* pulling this back into the main processor is stupid */
    /* Worse, the first argument is usually an invariant and can be hoisted */
    usimd t1 = vandq_u32(bits96,p1);
    usimd t2 = vandq_u32(bits96,p2);
    t1 = veorq_u32(t1,v4_prefix); // '0' = it is a v4 prefix
    t2 = veorq_u32(t2,v4_prefix); // random values it is not
    usimd t3 = vorrq_u32(t1,t2);  // all zeroes we are both v4 prefixes
    
    // Still incorrect
    if (is_not_zero(t3))
		    return PST_DISJOINT;
    // any ones we could be both non v4_prefixes

    // 0 and 0 eq
    // 1 and 0 gt
    // 0 and 1 lt
    // 1 and 1 potentially eq
    
    // At this point 
    //    if(v4mapped(p1) != v4mapped(p2))
    //    return PST_DISJOINT;

    // FIXME this is incorrect - need to generate the mask
    usimd t4 = vandq_u32(mask1,p1);
    usimd t5 = vandq_u32(mask2,p2);
    
    if(is_not_zero(t4) != is_not_zero(t5))
        return PST_DISJOINT;

    if(plen1 < plen2)
        return PST_LESS_SPECIFIC;
    else if(plen1 > plen2)
        return PST_MORE_SPECIFIC;
    else
        return PST_EQUALS;
    asm("nop; /* prefix_cmp end */");
}

#endif

