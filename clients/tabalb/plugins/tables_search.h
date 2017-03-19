/**
 * tables_search.h
 *
 * Dave Taht
 * 2017-03-18
 */

#ifndef COUNTER1
#define COUNTER1
#endif

#ifndef STACKMEM
#define STACKMEM
#endif

#ifndef SOMETIMES_INLINE
#define SOMETIMES_INLINE
// #define SOMETIMES_INLINE static inline
#endif

// FIXME: trying preinc for gcc in this version

// There are two operations done here
// one is an search, where we return a result:
// b < size of b table - possible hit
// b > size of b table - miss

// the other is a merge sort insert
// b < size of the b table - no need to insert
// b < size of table and a != b miss, must continue
// b is inserted, must switch to next a

// my intent was to bound a via some other means
// but haven't got there yet

// #define OP2(result,bpointer,next)
// #define OP(bpointer,next)

//#define OP(b, match = *a++;)
//#define OP2(r, b, match = *a++;) if((r = 

//		 match = *a++;

// we constrain the loop to no more than RUNAHEAD+1 searches
// because we have to yield to other threads at some point
// or fetch more memory into the cache
// RUNAHEAD must be x^2-1

#ifndef RUNAHEAD
#define RUNAHEAD 7
#endif

#ifndef RESULT
extern tbl_b my_result(tbl_a a, tbl_b b, int r);
#define RESULT(r,a,b) my_result(*a,*b, r)
#endif

// I don't want to return pointers here but structures in the right regs
// also I have a reg already defined with -1 in it on the
// parallella - will that get picked up?
// and I want the compiler to consider both as non-overlapping

SOMETIMES_INLINE tbl_b PO(roar_match)(tbl_a *a, tbl_b *b, unsigned int c) {
        unsigned int r = -1;
	tbl_a match = *a;
	--b;
	do {
		while(c++ & RUNAHEAD)
			r += match == *++b;
	} while(r = RESULT(r,a,b));

	return *b;
}

SOMETIMES_INLINE tbl_b PO(roar_match_freerun)(tbl_a *a, tbl_b *b, unsigned int c) {
	unsigned char d = c & 255;
        unsigned char r = -1;
	tbl_a match = *a;
	--b;
	do {
		while (d++)
			r += match == *++b ;
	} while(r = RESULT(r,a,b));

	return *b;
}

SOMETIMES_INLINE tbl_b PO(roar_match_firsthit)(tbl_a *a,tbl_b *b, unsigned int c) {
        unsigned int r = -1;
	tbl_a match = *a;
	--b;
	while(r) {
	         while (c++ & RUNAHEAD & r)
			 r += match == *++b ;
	} while(r = RESULT(r,a,b));

	return *b;
}

SOMETIMES_INLINE tbl_b PO(roar_match_freerun_firsthit)(tbl_a *a,tbl_b *b, unsigned int c) {
	unsigned char d = c & 255;
        unsigned char r = -1;
	tbl_a match = *a;
	--b;
	do {
	         while (d++ & r)
			 r += match == *++b ;
	} while(r = RESULT(r,a,b));

	return *b;
}
