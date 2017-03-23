/**
 * u_types.h
 *
 * Dave Taht
 * 2017-03-22
 */

#ifndef ERM_UTYPES_H
#define ERM_UTYPES_H

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

#ifdef HAVE_128BIT_INT

typedef __uint128 u128;

#else

// FIXME: endian

typedef struct {
	u64 low;
	u64 hi;
} u128;

#endif

// Logical. Register

typedef u128 L128 VECTOR(16);
typedef u128 R128 VECTOR(16);

#endif
