/**
 * popcount.h
 *
 * Dave Taht
 * 2017-03-16
 */

#ifndef POPCOUNT_H
#define POPCOUNT_H

// Several core routines are heavily dependent on popcount and there are more
// than a few ways to do it. One way patches the code at compile time. Not huge
// on that. The other way is to rely on the generated code by the
// __builtin_popcount to "do the right thing". the first problem is finding the
// right popcount to call... I didn't know, until I coded this, that the SSE4
// version does not actually run in the SSE regs! So use the C11 generics
// extension to first map the call to the type

#define popcount(X) _Generic((X),                         \
                                uint128_t : popcount16,   \
                                uint64_t : popcount8,     \
                                uint32_t : popcount4,     \
                                uint16_t : popcount2,     \
                                usimd    : popcount_vec16,\
        default: popcount8                                \
                )(X)
x

// Next: figure out if we have the right arch for the extension
// for regular SSE - harley-seal is the winner
// for SSE4 the built in instruction wins by a mile
// FIXME and this is all wrong

#if !((defined (HAVE_SSE4) | !defined(HAVE_NEON)))
#ifdef HAVE_64BIT_ARCH
static inline uint8_t popcount16(uint32x4_t i) { return popcountll(i) + popcountll(i); }
static inline uint8_t popcountvec16(uint32x4_t i) { return __builtin_popcountll(i) + __builtin_popcountll(i); }
#else
static inline uint8_t popcount16(uint32x4_t i) { return popcountll(i) + popcountll(i); }
static inline uint8_t popcountvec16(uint32x4_t i) { return __builtin_popcountll(i) + __builtin_popcountll(i); }
#endif

static inline uint8_t popcount8(uint64_t i) { return __builtin_popcountl(i); }
static inline uint8_t popcountvec8(uint64_t i) { return __builtin_popcountl(i); }
static inline uint8_t popcount4(uint32_t i) { return __builtin_popcount(i); }
static inline uint8_t popcount2(uint16_t i) { return __builtin_popcount(i); }
static inline uint8_t popcount1(uint8_t i) { return __builtin_popcount(i); }

#else
#if defined(HAVE_NEON)
#error FIXME - write neon version
#else
#if defined(HAVE_SSE4)
#ifdef HAVE_64BIT_ARCH
static inline uint8_t popcount16(uint32x4_t i) { return popcountll(i) + popcountll(i); }
static inline uint8_t popcountvec16(uint32x4_t i) { return __builtin_popcountll(i) + __builtin_popcountll(i); }
#else
static inline uint8_t popcount16(uint32x4_t i) { return popcountll(i) + popcountll(i); }
static inline uint8_t popcountvec16(uint32x4_t i) { return __builtin_popcountll(i) + __builtin_popcountll(i); }
#endif

static inline uint8_t popcount8(uint64_t i) { return __builtin_popcountl(i); }
static inline uint8_t popcountvec8(uint64_t i) { return __builtin_popcountl(i); }
static inline uint8_t popcount4(uint32_t i) { return __builtin_popcount(i); }
static inline uint8_t popcount2(uint16_t i) { return __builtin_popcount(i); }
static inline uint8_t popcount1(uint8_t i) { return __builtin_popcount(i); }
#endif

// And: Now things get hairy - to use this right - and interleave instructions - we
// need to start one or more operations and collect them later. Probably. So we
// fling the start of things here, where you can use a named variable to collect
// things still in the vector unit.

// I am probably massively overthinking this

#if !defined(HAVE_NEON)
// But there seems to be no point in using vectors for this with the builtin
#define POPCOUNT_START(type,result,v) type result = popcount(v);
#define POPCOUNT_START4(type,result,v) type result#_0 = popcount(v); \
	                               type result#_1 = popcount(v); \
				       type result#_2 = popcount(v); \
                                       type result#_3 = popcount(v);
#define POPCOUNT_COLLECT1(r,result)
#define POPCOUNT_COLLECT4(r,result)

#else
// wrong. I need to think about this further

#define POPCOUNT_START(type,result,v) type result = popcount(v);
#define POPCOUNT_START4(type,result,v) type result#_0 = popcount(v); \
	                               type result#_1 = popcount(v); \
				       type result#_2 = popcount(v); \
                                       type result#_3 = popcount(v);
static inline int get_result(uint32_4t a) return get_lane(result);

#define POPCOUNT_COLLECT1(v,result) v = get_result
#define POPCOUNT_COLLECT4(v,result)
#endif

#endif
