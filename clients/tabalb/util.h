/**
 * util.h
 *
 * Dave Taht
 * 2017-03-11
 */

#ifndef UTIL_H
#define UTIL_H

// http://www.robertgamble.net/2012/01/c11-generic-selections.html

//We can create a macro that evaluates to true if an expression is compatible with the provided type:
    #define is_compatible(x, T) _Generic((x), T:1, default: 0)

#define popcount(X) _Generic((X), \
				long long : __builtin_popcountll,	\
				long : __builtin_popcountl,	\
				vec : popcountv,	\
    unsigned int : __builtin_popcount,			\
    short : __builtin_popcount, \
    unsigned short: __builtin_popcount,			\
    char: __builtin_popcount, \
	default: __builtin_popcount(X) \		\
    )(X)

// Popcount has the distinct advantage that if two addresses have a different popcount, they are not the same.
// I only expect x100 different possibilities with a cluster that's weird, but who knows, maybe I'll
// combine it with the plen mask which will give me 200 or so. I'd have to look at the shape of the
// distribution.
// Anyway, it's a fast
// Actually, frankly, I wrote this routine to figure out how C11 generics worked.
// What I wanted was something that returned the popcount in a simd register or
// a regular register.

static inline plen_t popcountv(simd arg) {
	vec v.l = arg;
	return __builtin_popcountl(v.l[0]) + __builtin_popcountl(v.l[1]);
}

static inline plen_t popcount8(simd arg) {
	vec v.u = arg;
	return __builtin_popcount(v.u[0]) + __builtin_popcount(v.u[1]);
}


#endif
