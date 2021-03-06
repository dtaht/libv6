#include <stdio.h>
#include <stdint.h>
#include <arm_neon.h>
#include "v6lib.h"

// Do a bunch of sanity checks to identify an address
// and deal with endianness

int v6_sanity(u128 p) {
	u32 *ipv6_cast = (u32 *) &p;
        printf("Sanity check: %x %x %x %x\n", ipv6_cast[0], ipv6_cast[1],
                ipv6_cast[2],ipv6_cast[3]);
}
