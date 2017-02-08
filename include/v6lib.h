#ifndef _V6LIB
#define _V6LIB

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef __uint128_t u128;
u128 v6_ones = {-1};
u128 v6_zeros = {0};

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
