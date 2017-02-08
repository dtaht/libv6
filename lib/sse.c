/**
 * sse.c
 * This doesn't even compile. I didn't realize until after I started that
 * shifts had to be embedded in the instruction.
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
#include <smmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>

typedef unsigned char u8;
typedef unsigned long long u64;

#if 1
typedef  __m128i u128;
// SSE4
//#define isAllZero(a) _mm_testz_si128(a,a);

static inline int isAllZero(u128 a) {
	return _mm_testz_si128(a,a);
}

#define PAND(a,b) _mm_and_si128(a,b)
#define PANDN(a,b) _mm_andnot_si128(a,b)
#define PXOR(a,b) _mm_xor_si128(a,b)
#endif

static const __m128i zero = {0};

/* inline bool compare128(__m128i a, __m128i b) {
    __m128i c = _mm_xor_si128(a, b);
    return _mm_testc_si128(zero, c);
}
*/

//typedef __uint128_t u128;

// PXOR(PAND(a,b),c) gives me a zero if they are the same but not sure how to do that...

//if(isAllZero(PAND(a,b),c))

typedef struct {
	u64 sha;
	u64 ihave;
        u128 prefix;
	u8 mask;
} pref_table;

// const u128 u128_ones = {0}; // FIXME - needs to be zeros
pref_table *assigned;

size_t pos = 0;
size_t size = 127;
size_t begin = 0;
size_t end = 0;
// _mm_set1_epi16(1)
extern int find_prefix(pref_table *p, u128* prefix, u8 mask);

int find_prefix(pref_table *p, u128 *prefix, u8 mask) {
	int i;
//	u128 zero = _mm_setzero_si128();
//	u128 ones = _mm_cmpeq_epi32(zero, zero);
	u128 ones = _mm_set1_epi16(1);
//	u128 m = _mm_load_ps(mask);
	u128 m = (PAND(ones, (u128) 1<<mask-1));
      	u128 cmp = PAND(*prefix,m);
//	for(i = 0; i<size && p[i].prefix & m != cmp ; i++) ; 
	for(i = 0; i<size &&
		    _mm_movemask_epi8(PXOR(PAND(p[i].prefix,m),cmp)); i++);
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

