/* The martian check is one the nost inefficient pieces of code
   I can imagine. This is an attempt to speed it up */

#include "kill_the_martians.h"

static inline int
v4mapped_orig(const unsigned char *address)
{
    return memcmp(address, v4prefix, 12) == 0;
}


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
   execute all paths */

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

/* This (usually) halves the search space by starting with the most unlikely
   match (0xFFFF in the 10th and 11th bytes), and fanning out from there

   The only case where this is slower than the original code is when the ipv6
address has a 0xFF 0xFF in the 10th and 11th places. It uses more native address
comparisons where feasible independent of the endian-ness.

*/

int
martian_prefix_new(const unsigned char *prefix, int plen)
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

        if( (plen >= 8 && prefix[0] == 0xFF) ||
		(plen >= 10 && prefix[0] == 0xFE && (prefix[1] & 0xC0) == 0x80))
		return true;

	return false;
	}

        /* Definately not v4mapped at this point. Is it multicast or link local? */

	if(!is_zero) {

        if( (plen >= 8 && prefix[0] == 0xFF) ||
		(plen >= 10 && prefix[0] == 0xFE && (prefix[1] & 0xC0) == 0x80))
		return true;

	return false;
	}

        /* Crap. It's got lots of zeros. */

        if((plen >= 128 && (prefix[15] == 0 || prefix[15] == 1) && memcmp(prefix + 8, zeroes, 7) == 0))
	        return true;

	/* Not a martian and generally, unreachable in normal ipv6 data sets */

	return false;

}

//#define PREFIXES 512 /* don't stress the dcache overmuch */
//#define PREFIXES 512 /* don't stress the dcache overmuch */
#define PREFIXES 6400000 /* don't stress the dcache overmuch */

int main() {
	int a,b,c,d,v1,v2;
	prefix *prefixes = gen_random_prefixes(PREFIXES);
	a = start_clock();
        fool_compiler(prefixes);
        v1 = count_martian_prefixes_old(prefixes,PREFIXES);
	b = stop_clock();
        fool_compiler(prefixes);
	c = start_clock();
        v2 = count_martian_prefixes_new(prefixes,PREFIXES);
	d = stop_clock();
	printf("Timestamp difference: %d vs %d\n", b-a, d-c);
	printf("The two algorithms are %s\n", v1 == v2 ? "equivalent" : "incorrect");
	return 0;
}
