/* The martian check is one the nost inefficient pieces of code
   I can imagine. This is an attempt to speed it up */

#include "kill_the_martians.h"

static inline size_t
v4mapped(const unsigned char *address)
{
#ifdef  HAVE_64BIT_ARCH
    const unsigned long *up1 = (const unsigned long *) address;
    const unsigned int *up2 = (const unsigned int *) (&address[8]);
    // Fixme Address extend?
    return ((up1[0] ^ 0) | (up2[0] ^ htobe32(0xffff))) == 0UL;
#else
    const unsigned int *up1 = (const unsigned int *) address;
    return ((up1[0] ^ 0) | (up1[1] ^ 0) | (up1[2] ^ htobe32(0xffff))) == 0;
#endif
}

/* The problem with the original routine is that it can essentially
   execute all paths, and the v4mapped call was in a separate library entirely */

int
martian_prefix_old(const unsigned char *prefix, int plen)
{
        if((plen >= 8 && prefix[0] == 0xFF) ||
        (plen >= 10 && prefix[0] == 0xFE && (prefix[1] & 0xC0) == 0x80) ||
        (plen >= 128 && memcmp(prefix, zeroes, 15) == 0 &&
         (prefix[15] == 0 || prefix[15] == 1)) ||
        (plen >= 96 && v4mapped(prefix) &&
         ((plen >= 104 && (prefix[12] == 127 || prefix[12] == 0)) ||
          (plen >= 100 && (prefix[12] & 0xE0) == 0xE0))))
        return true;
	return false;
}


int
martian_prefix_older(const unsigned char *prefix, int plen)
{
        if((plen >= 8 && prefix[0] == 0xFF) ||
        (plen >= 10 && prefix[0] == 0xFE && (prefix[1] & 0xC0) == 0x80) ||
        (plen >= 128 && memcmp(prefix, zeroes, 15) == 0 &&
         (prefix[15] == 0 || prefix[15] == 1)) ||
        (plen >= 96 && v4mapped_orig(prefix) &&
         ((plen >= 104 && (prefix[12] == 127 || prefix[12] == 0)) ||
          (plen >= 100 && (prefix[12] & 0xE0) == 0xE0))))
        return true;
	return false;
}

/* This (usually) halves the search space by starting with the most unlikely
   match (0xFFFF in the 10th and 11th bytes), and fanning out from there

   The only case where this is slower than the original code is when the ipv6
address has a 0xFF 0xFF in the 10th and 11th places. It uses more native address
comparisons where feasible independent of the endian-ness.

*/

// This does the logical compare first then carries it

int
martian_prefix_new_string(const unsigned char *prefix, int plen)
{
	// The compiler should automatically defer or interleave this load
	// until it is actually needed

	size_t is_zero = (*(const long long *) &prefix[0]) == 0LL;

/* Is it possibly a v4prefix? */

	if(prefix[10] == 0xFF && prefix[11] ==0xFF) {
		// Likely v4mapped but is it a martian?
		if (plen >= 96) {
			if((plen >= 104 &&
					(prefix[12] == 127 || prefix[12] == 0))
				|| (plen >= 100 && (prefix[12] & 0xE0) == 0xE0))
				/* is it also v4mapped? */
				if(is_zero)
					return true;

	                if(is_zero) return false; /* v4mapped but not a martian */
		}

	/* Definately not v4mapped and must be IPv6 at this point, but we know
           for sure it's not going to be a localhost or localnet due to the 0xFF
           but might be multicast or link local. This is a pretty pointless
           optimization. */

//        return( (plen >= 8 && prefix[0] == 0xFF) ||
//		(plen >= 10 && prefix[0] == 0xFE && (prefix[1] & 0xC0) == 0x80));

	}

        /* Definately not v4mapped at this point. Is it multicast or link local? */

	if(!is_zero) {

        return( (plen >= 8 && prefix[0] == 0xFF) ||
		(plen >= 10 && prefix[0] == 0xFE && (prefix[1] & 0xC0) == 0x80));
	}

        /* Crap. It's got lots of zeros. */
	/* false = Not a martian and generally, unreachable in normal ipv6 data sets */

        return((plen >= 128 && (prefix[15] == 0 || prefix[15] == 1) && memcmp(prefix + 8, zeroes, 7) == 0));


}

// Can I flip this to not_martian_prefix somehow?

// This carries around the whole prefix til the end

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

int
martian_prefix_new(const unsigned char *prefix, int plen)
{
	// The compiler should automatically defer or interleave this load
	// until it is actually needed

	unsigned long long p = (*(const unsigned long long *) &prefix[0]);

//	if(p) seemed to be a lose
/* Is it possibly a v4prefix? */

	if(prefix[10] == 0xFF && prefix[11] ==0xFF) {
		// Likely v4mapped but is it a martian?
		if (plen >= 96) {
			if((plen >= 104 &&
					(prefix[12] == 127 || prefix[12] == 0))
				|| (plen >= 100 && (prefix[12] & 0xE0) == 0xE0))
				/* is it also v4mapped? */
				if(p == 0LL)
					return true;

	                if(p == 0LL) return false; /* v4mapped but not a martian */
		}

	/* Definately not v4mapped and must be IPv6 at this point, but we know
           for sure it's not going to be a localhost or localnet due to the 0xFF
           but might be multicast or link local. This is a pretty pointless
           optimization. */

//        return( (plen >= 8 && prefix[0] == 0xFF) ||
//		(plen >= 10 && prefix[0] == 0xFE && (prefix[1] & 0xC0) == 0x80));

	}

        /* Definately not v4mapped at this point. Is it multicast or link local? */

	// likely? Flip these two? Not sure this is correct for alternate endians
// Does this need to be the logical operations? Can I just return size_t?
	// Can I just do the booleans? What happens to my thinking if I move
	// ALL this bit to the top?
// does likely do the right thing on the non-zero true?
		
//	if(likely(p)) {
	if(p) {
	unsigned short p1 = p & 0xFFFF;
//		return( (plen >= 8 && ((unsigned char)p & 0xFF) == 0xFF) ||
//			(plen >= 10 && ((unsigned short)p & 0xFE80) == 0xFE80 ));
		return( (plen >= 8 && (p1 & 0xFF) == 0xFF) ||
			(plen >= 10 && (p1 & 0xFE80) == 0xFE80 ));
	}

        /* Crap. It's got lots of zeros. */
	/* false = Not a martian and generally, unreachable in normal ipv6 data sets */

        return((plen >= 128 && (p - 2) & *(unsigned long long *) (prefix + 8)));

}

inline int
martian_prefix_new2(const unsigned char *prefix, int plen)
{
	// The compiler will automatically defer or interleave this load
	// until it is actually needed

	size_t is_zero = (*(const long long *) &prefix[0]) == 0LL;

/* Is it possibly a v4prefix? */

	if(prefix[10] == 0xFF && prefix[11] ==0xFF) {
		// Likely v4mapped but is it a martian?
		if (plen >= 96) {
			if((plen >= 104 &&
					(prefix[12] == 127 || prefix[12] == 0))
				|| (plen >= 100 && (prefix[12] & 0xE0) == 0xE0))
				/* is it also v4mapped? */
				if(is_zero)
					return true;

	                if(is_zero) return false; /* v4mapped but not a martian */
		}

	/* Definately not v4mapped and must be IPv6 at this point, but we know
           for sure it's not going to be a localhost or localnet due to the 0xFF
           but might be multicast or link local. This is a pretty pointless
           optimization. */

        return( (plen >= 8 && prefix[0] == 0xFF) ||
		(plen >= 10 && prefix[0] == 0xFE && (prefix[1] & 0xC0) == 0x80));

	}

        /* Definately not v4mapped at this point. Is it multicast or link local? */

	if(!is_zero) {

        return( (plen >= 8 && prefix[0] == 0xFF) ||
		(plen >= 10 && prefix[0] == 0xFE && (prefix[1] & 0xC0) == 0x80));
	}

        /* Crap. It's got lots of zeros. */
	/* false = Not a martian and generally, unreachable in normal ipv6 data sets */

        return((plen >= 128 && (prefix[15] == 0 || prefix[15] == 1) && memcmp(prefix + 8, zeroes, 7) == 0));


}

// If we are lucky the compiler will find a place to optimize and wedge these two together

/* Theoretically, we can lift the src_plen check to here and do both in parallel

Thing is, we want to reverse the logic = NOT martian prefix - because that's
the most common case.

    if(martian_prefix(prefix, plen)) {
        fprintf(stderr, "Rejecting martian route to %s through %s.\n",
                format_prefix(prefix, plen), format_address(nexthop));
        return NULL;
    }
    if(src_plen != 0 && martian_prefix(src_prefix, src_plen)) {
        fprintf(stderr, "Rejecting martian route to %s from %s through %s.\n",
                format_prefix(prefix, plen),
                format_prefix(src_prefix, src_plen), format_eui64(id));
        return NULL;
    }

 */
// src_plen == 1?
int
martian_prefix_new_dual(const unsigned char *prefix, int plen,
			const unsigned char *prefix1, int plen2) {
	return(martian_prefix_new2(prefix,plen) ||
		martian_prefix_new2(prefix1,plen2));
}

//#define PREFIXES 64 // A real micro-microbenchmark that's actually how this is used
//#define PREFIXES 64 /* don't stress the dcache overmuch */
//#define PREFIXES 512 /* don't stress the dcache overmuch */
#define PREFIXES 64000 /* don't stress the dcache overmuch */

int main() {
        unsigned long a,b,c,d;
	int v1,v2;
	double fp1, fp2;
	prefix *prefixes = gen_random_prefixes(PREFIXES);
	prefix *prefixes1 = gen_random_prefixes(PREFIXES);
        fool_compiler(prefixes);
	a = get_clock();
        v1 = count_martian_prefixes_old(prefixes,PREFIXES);
	b = get_clock();
        fool_compiler(prefixes);
	c = get_clock();
        v2 = count_martian_prefixes_new(prefixes,PREFIXES);
	d = get_clock();
	printf("The two algorithms are %s\n", v1 == v2 ? "equivalent" : "incorrect");
	printf("Difference between old and new: %ld vs %ld\n", b-a, d-c);
	fp1 = b-a;
	fp2 = d-c;
	printf("                       speedup: %g compares/prefix: %g\n", fp1/fp2, (fp1/fp2)/PREFIXES);
	// And just to make sure I'm not fooling myself, run it backwards which is hotter
        fool_compiler(prefixes);
	c = get_clock();
        v2 = count_martian_prefixes_new(prefixes,PREFIXES);
	d = get_clock();
        fool_compiler(prefixes);
	a = get_clock();
        v1 = count_martian_prefixes_old(prefixes,PREFIXES);
	b = get_clock();
	printf("Second difference between old and new: %ld vs %ld\n", b-a, d-c);
	fp1 = b-a;
	fp2 = d-c;
	printf("                              speedup: %g compares/prefix: %g\n", fp1/fp2, (fp1/fp2)/PREFIXES);

	// Hot cache test
	{ prefix p1 = prefixes[1];
        fool_compiler(prefixes);
	c = get_clock();
	v2 = martian_prefix_new(p1.p, p1.plen);
	d = get_clock();
        fool_compiler(prefixes);
	a = get_clock();
        v1 = martian_prefix_old(p1.p, p1.plen);
	b = get_clock();
	printf("Hot cache old and new: %ld vs %ld\n", b-a, d-c);
	fp1 = b-a;
	fp2 = d-c;
	printf("                              speedup: %g compares/prefix: %g\n", fp1/fp2, (fp1/fp2)/PREFIXES);
	}
	// Trying the dual code

        fool_compiler(prefixes);
        fool_compiler(prefixes1);
	c = get_clock();
        v2 = count_martian_prefixes_new_single(prefixes,prefixes1,PREFIXES);
	d = get_clock();
        fool_compiler(prefixes);
        fool_compiler(prefixes1);
	a = get_clock();
        v1 = count_martian_prefixes_new_dual(prefixes, prefixes1, PREFIXES);
	b = get_clock();
	printf("Difference between single and dual: %ld vs %ld\n", d-c, b-a);
	fp2 = b-a;
	fp1 = d-c;
	printf("                           speedup: %g compares/prefix: %g\n", fp1/fp2, (fp1/fp2)/PREFIXES);
	printf("The two algorithms are %s\n", v1 == v2 ? "equivalent" : "incorrect");

	return 0;
}
