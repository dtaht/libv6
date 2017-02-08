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

typedef unsigned char u8;
typedef unsigned long long u64;
typedef uint64x2_t u128;

#define PAND(a,b) vandq_u64(a,b)
#define PANDN(a,b) vbicq_u64(a,b)
#define PXOR(a,b) veorq_u64(a,b)
#define PXORN(a,b) vornq_u64(a,b)
#define POR(a,b) vorrq_u64(a,b)
#define PSHL(a,b) vqrshlq_u64(a,b)
#define PSHR(a,b) vqrshrq_u64(a,b)

// these are nice because you can shift by a constant int
// but tricky because they shift one 64 bit value left.
// It's saner to just load a constant 128 bit value and use
// the regular shift instructions above
// #define PSHRL(a,b) vqrshlq_u64(a,b)
// #define PSHRC(a,b) vqrshrq_u64(a,b)

// While this is a sane way to do it on x86
//static const u128 u128_zero = {0};
//static const u128 u128_ones = {-1};

// this is probably saner on neon til I figure out
// how to zero allocate a register (xor itself?)

#define u128_zero vdup_n_u32(0)
#define u128_ones vdup_n_u32(-1)

typedef struct {
	u64 sha;
	u64 ihave;
        u128 prefix;
	u8 mask;
} pref_table;

pref_table *assigned;

size_t pos = 0;
size_t size = 127;
size_t begin = 0;
size_t end = 0;

extern int find_prefix(pref_table *p, u128* prefix, u8 mask);

int find_prefix(pref_table *p, u128 *prefix, u8 mask) {
	int i;
	u128 m = (PAND(u128_ones, PMASK(mask),u128_ones));
      	u128 cmp = PAND(*prefix,m);
	for(i = 0; i<size &&
		    PISZERO(PXOR(PAND(p[i].prefix,m),cmp)); i++);
	return i;
}

#ifdef TEST
void main(int argc, char* argv[]) {
	char *prog = basename(argv[0]);
        assigned = calloc(size,sizeof(pref_table));
	assigned[4].prefix = u128_ones;
	if(strcmp(prog,"ddpd") == 0) server(argc,argv);
      	else if(strcmp(prog,"ddpc") == 0) client(argc,argv);
	else {
		printf("Must be invoked as either ddpd or ddpc\n");
		exit(-1);
	}
}
#endif

