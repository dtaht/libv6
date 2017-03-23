// Test structure assignment code generation capabilities

#include <stdio.h>
#include <stdint.h>

typedef struct {
	u32 a;
} global_flags_t;

global_flags_t tflags;

#include "preprocessor.h"
#include "simd.h"

typedef struct {
	u32 flags;
} flags_t;

typedef struct {
	u8 a VECTOR(16);
} ip_addr;

typedef struct {
	ip_addr a;
	flags_t f;
} compound_struct_t;

// These should do the right thing.

#define REGCALL __attribute__((regcall))

#define R REGCALL

// __m128, __m128i, __m128d

extern compound_struct_t R fool_compiler1(compound_struct_t a, compound_struct_t b);
extern compound_struct_t R fool_compiler2(flags_t af, ip_addr av,
				        flags_t bf, ip_addr bv);

extern ip_addr R fool_compiler3(flags_t af, ip_addr av,
			       flags_t bf, ip_addr bv);

extern flags_t R fool_compiler4(flags_t af, ip_addr av,
			      flags_t bf, ip_addr bv);

extern ip_addr R fool_compiler5(flags_t af, ip_addr av,
			       flags_t bf, ip_addr bv);

extern compound_struct_t fool_compiler_compound_struct;
extern ip_addr fool_compiler_addr;

int check_flags(flags_t f) {
	if(f.flags | 1) printf("Got 1\n");
	return f.flags;
}

R int check_addr(ip_addr a) {
	printf("hooked %Q", a);
	return 1;
}

#define TESTS1(n,a,b,result) result = fool_compiler##n(a,b); check_flags(r.f)
#define TESTS2(n,g,g1,k,k1,result) result = fool_compiler##n(g.f,g1.a,k.f,k1.a); check_flags(r.f)
#define TESTS3(n,g,g1,k,k1,result) result = fool_compiler##n(g.f,g1,k.f,k1.a); check_flags(r.f)

int main() {
	compound_struct_t c = {0};
	compound_struct_t d = {0};
	compound_struct_t r;
	ip_addr l = d.a;
	c.f.flags = 1;
	d.f.flags = 2;
	TESTS1(1,c,d,r); // does not generate SSE instructions
	TESTS1(1,c,d,r); // does not generate SSE instructions
	TESTS2(2,c,c,d,d,r); // does generate SSE instructions
	TESTS2(2,c,c,d,d,r); // does generate SSE instructions
	TESTS2(3,c,c,d,d,r.a); // does generate SSE instructions
	TESTS2(3,c,c,d,d,r.a); // does generate SSE instructions
	TESTS2(4,c,c,d,d,r.f); // does generate SSE instructions
	TESTS2(4,c,c,d,d,r.f); // does generate SSE instructions
	asm("nop; /* keep registration */");
// can we fool the compiler harder?
	TESTS3(5,c,l,d,d,r.a); // does generate SSE instructions
// can we fool the compiler harder or will it spill l?
	TESTS3(5,c,l,d,d,r.a); // does generate SSE instructions

	return 0;
}
