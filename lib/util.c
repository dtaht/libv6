#include <stdio.h>
#include <stdint.h>
#include "v6lib.h"

// Do a bunch of sanity checks to identify an address
// and deal with endianness

int v6_sanity(u128 p) {
	int *u32 = (u32 *) p;
        printf("Sanity check: %x %x %x %x\n", ipv6_cast[0], ipv6_cast[1],
                ipv6_cast[2],ipv6_cast[3]);
}
