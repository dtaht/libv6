#ifndef _V6LIB
#define _V6LIB
#include "arch-neon.h"

// ipv6 was designed long before C11.

typedef struct in6_addr in6_addr_t;

const u128 v6_ones = {-1};
const u128 v6_zeros = {0};

typedef struct {
        u64 sha;
        u64 ihave;
        u128 prefix;
        u8 mask;
} v6_prefix_table;

// it is not clear if the ABIs can handle anything but pointers yet

extern int v6_find_prefix(v6_prefix_table *p, int size, u128 prefix, u8 mask);
extern int v6_store_prefix(u128 prefix, u128 prefix2);
extern int v6_sanity(u128 prefix);

u128 v6_gen_random_prefix(int subnet);

#endif
