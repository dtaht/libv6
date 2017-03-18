/**
 * plugin_tests.h
 *
 * Dave Taht
 * 2017-03-18
 */

// In order to get better code generation tbl_p gets redefined with the
// actual stride(s) involved and included multiple times to do so.

// I really have to formalize these and abstract them
// #define COUNTER __asm__("q1");
// #define REGI bla elsewhere

#ifndef COUNTER1
#define COUNTER1
#define STACKMEM
#define SOMETIMES_INLINE
// #define SOMETIMES_INLINE static inline
#endif

extern int yield_current(int r);
extern int yield_current2(int r,tbl_b *ptr);

#define YIELDR(r) yield_current(r)
#define YIELDR2(r,v) yield_current2(r,v)
// FIXME: token pasting from hell and doesn't expand correctly yet.

#define PPASTE(t1,t2,v) t1##_##t2##_##v
#define PO(v) PPASTE(tbl_a,tbl_b,v)

// Your basic infinite loops. Hope you aren't paying for compute time!

 SOMETIMES_INLINE int PO(warm_reg)(tbl_a *a,tbl_b *b, unsigned int count) {
	register volatile int d COUNTER1;
	while(1) d+=count;
	return 0;
}

 SOMETIMES_INLINE int PO(warm_mem)(tbl_a *a,tbl_b  *b, unsigned int count) {
	volatile int d STACKMEM;
	while(1) b +=count;
	return 0;
}

 SOMETIMES_INLINE int PO(roar_warm_overrun_mem)(tbl_a *a,tbl_b *b, unsigned int count) {
	register volatile int d COUNTER1;
	d = count;
	while(d) *a++ = *b++ ;
	return 0;
}

 SOMETIMES_INLINE int PO(roar_warm_dont_overrun_mem)(tbl_a *a,tbl_b *b, unsigned int count) {
	register volatile int d COUNTER1;
	d = count;
	while(d--) *a++ = *b++ ;
	return 0;
}

 SOMETIMES_INLINE int PO(roar_warm_bounded_overrun_mem)(tbl_a *a,tbl_b *b, unsigned int count) {
	register volatile int d COUNTER1;
	d = count;
	while(d++ & 255) *a++ = *b++ ;
	return 0;
}

 SOMETIMES_INLINE int PO(roar_warm_bounded_overrun_mem_yield)(tbl_a *a,tbl_b *b, unsigned int count) {
	register volatile int d COUNTER1;
	d = count;
	while(1) {
	         while(d++ & 255) *a++ = *b++ ;
	         YIELD;
	}
	return 0;
}

 SOMETIMES_INLINE int PO(roar_warm_bounded_overrun_match_yield)(tbl_a *a,tbl_b *b, unsigned int count) {
	register volatile int d COUNTER1;
	d = count;
	int r = 0;
	tbl_a match = *a;
	// I do hope this can be vectorized
	while(1) {
	         while(d++ & 255) r += match == *b++ ;
	         YIELD;
		 match = *a++;
	}
	return 0;
}

// this does indeed generate shorter code on x86 by killing the explicit free running variable to the natural width
// is it better code? dunno

 SOMETIMES_INLINE int PO(roar_warm_bounded_overrun_explicit_match_yield)(tbl_a *a,tbl_b *b, unsigned int count) {
	register volatile unsigned char d COUNTER1;
	d = count;
        int r = 0; // char? It's just a result. I mean flags << or whatever...
	tbl_a match = *a;
	// I do hope this can be vectorized automagically as it is key to the backtracker
	while(1) {
	         while(d++) r += match == *b++ ;
	         YIELD;
		 match = *a++;
	}
	return 0;
}

 SOMETIMES_INLINE int PO(roar_warm_bounded_overrun_explicit_match_yield_post)(tbl_a *a,tbl_b *b, unsigned int count) {
	register volatile unsigned char d COUNTER1;
	d = count;
        int r = 0; // char? It's just a result. I mean flags << or whatever...
	tbl_a match = *a;
	// I do hope this can be vectorized automagically as it is key to the backtracker
	while(1) {
	         while(d++) r += match == *b++ ;
	         YIELDR(r);
		 match = *a++;
	}
	return 0;
}

 SOMETIMES_INLINE int PO(roar_warm_bounded_overrun_explicit_match_yield_xor_char_post)(tbl_a *a,tbl_b *b, unsigned int count) {
	register unsigned char d COUNTER1;
	d = count;
        unsigned char r = 0; // char? It's just a fuzzy result. I mean flags << or whatever...
	tbl_a match = *a;
	// I do hope this can be vectorized automagically as it is key to the backtracker
	while(1) {
	         while(d++) r += match ^ *b++ ;
	         YIELDR(r);
		 match = *a++;
	}
	return 0;
}

 SOMETIMES_INLINE int PO(roar_warm_bounded_overrun_explicit_match_yield_xor_post)(tbl_a *a,tbl_b *b, unsigned int count) {
	register unsigned char d COUNTER1;
	d = count;
        int r = 0; // char? It's just a fuzzy result. I mean flags << or whatever...
	tbl_a match = *a;
	// I do hope this can be vectorized automagically as it is key to the backtracker
	while(1) {
	         while(d++) r += match ^ *b++ ;
	         YIELDR(r);
		 match = *a++;
	}
	return 0;
}

 SOMETIMES_INLINE int PO(roar_warm_bounded_overrun_explicit_match_yield_xor_post_array)(tbl_a *a,tbl_b *b, unsigned int count) {
	unsigned char d COUNTER1;
	d = count;
        int r = 0; // char? It's just a fuzzy result. I mean flags << or whatever...
	tbl_a match = *a;
	int i = 0;
	// I do hope this can be vectorized automagically as it is key to the backtracker
	while(1) {
	         while(d++) r += match ^ b[i++] ;
	         YIELDR(r);
		 match = *a++;
	}
	return 0;
}

// Anyway this is closer the real routine hopefully, and we constrain the loop
// to no more than 15 searches

 SOMETIMES_INLINE int PO(roar_real_unrolled_match)(tbl_a *a,tbl_b *b, unsigned int c) {
        unsigned int r = -1;
	tbl_a match = *a;
	while(1) {
	         while(c++ & 15) r += match == *b++ ;
	         YIELDR2(r,b);
		 r = -1;
		 match = *a++;
	}
	return 0;
}

// in this case we don't need to pass r

SOMETIMES_INLINE int PO(roar_real_match)(tbl_a *a,tbl_b *b, unsigned int c) {
        unsigned int r = -1;
	tbl_a match = *a;
	while(1) {
	         while (c++ & 15 & r) r += match == *b++ ;
	         YIELDR2(r,b);
		 r = -1;
		 match = *a++;
	}
	return 0;
}

// or in this case, 255

SOMETIMES_INLINE int PO(roar_real_match_freerun)(tbl_a *a,tbl_b *b, unsigned int c) {
	unsigned char d = c & 255;
        unsigned char r = -1;
	tbl_a match = *a;
	while(1) {
	         while (d++) r += match == *b++ ;
	         YIELDR2(r,b);
		 r = -1;
		 match = *a++;
	}
	return 0;
}

// or first hit. For all I know I should switch to pre-inc.
// in this case, we don't need to pass r
// I have tried r as an int too. Need to look closer

SOMETIMES_INLINE int PO(roar_real_match_freerun_firsthit)(tbl_a *a,tbl_b *b, unsigned int c) {
	unsigned char d = c & 255;
        unsigned char r = -1;
	tbl_a match = *a;
	while(1) {
	         while (d++ & r) r += match == *b++ ;
	         YIELDR2(r,b);
		 r = -1;
		 match = *a++;
	}
	return 0;
}

// FIXME: Add CRC and memory error checks and so forth
// One good consistency check would be to verify that the table flags matched the data
// match and matchthese fall from this, also

