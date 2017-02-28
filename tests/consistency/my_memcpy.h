#ifdef HAVE_NEON
#include <arm_neon.h>

#define my_memcpy(to,from,qty) qty == 16 ? my_memcpy16(to,from)\
                         : qty == 12 ? my_memcpy12(to,from) \
                         : qty == 8  ? my_memcpy8(to,from) \
                         : memcpy(to,from,qty);


//I haven't the faintest idea what the real difference between loading
// and storing here. Alignment? Endian? Speed?

static inline void * my_memcpy16(void *dest, const void *src) {
     vst1q_u64((uint64_t *) dest, vld1q_u64((uint64_t *) src));
     return dest;
}

// Avoid overwriting memory by using a temporary

static inline void * my_memcpy12(void *dest, const void *src) {
     uint64x2_t t1 = vld1q_u64((const uint64_t *) src);
     uint32x2_t t2 = vld1_u32((const uint32_t *) (src + 8)); // ? 8? 2?
     vst1q_u64((uint64_t *) dest, t1);
     vst1_u32(( uint32_t *) (dest+8), t2);
     return dest;
}

static inline void * my_memcpy8(void *dest, const void *src) {
     vst1_u64((uint64_t *) dest, vld1_u64((const uint64_t *) src));
     return dest;
}

#else
#define my_memcpy(a,b,c) memcpy(a,b,c)
#endif
