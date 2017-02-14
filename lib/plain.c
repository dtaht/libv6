/**
 * neon attempt
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <arm_neon.h>
#include "v6lib.h"

static u128 u128_zero = {0};
static u128 u128_ones = {-1};

// vget_lane_u64 

typedef uint64x2_t u128;

// FIXME - how does the ABI handle this order? Should we put the last first?

int find_prefix(v6_prefix_table *p, int size, u128 prefix, u8 mask) {
	int i;
	i128 s = (vld1q_u64(&mask));
	u128 m = PMASK(s);
      	u128 cmp = PAND(prefix,m);
	for(i = 0; i<size &&
	      PISZERO(PXOR(PAND(p[i].prefix,m),cmp)); i++) ;
	return i;
}
