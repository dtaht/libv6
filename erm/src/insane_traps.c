/**
 * insane_traps.c
 *
 * Dave Taht
 * 2017-03-22
 */

// #include "insane_traps.h"

#include <stdio.h>
#include <stdint.h>
#include "shared.h"

void trap0(){
	printf("trap 0\n");
}
void trap1(){
	printf("trap 1\n");
}
void trap2(){
	printf("trap 2\n");
}
void trap3(){
	printf("trap 3\n");
}
void trap4(){
	printf("trap 4\n");
}
void trap5(){
	printf("trap 5\n");
}
void trap6(){
	printf("trap 6\n");
}
void trap7(){
	printf("trap 7\n");
}
void trap8(){
	printf("trap 8\n");
}
void trap9(){
	printf("trap 9\n");
}
void trapa(){
	printf("trap a\n");
}
void trapb(){
	printf("trap b\n");
}
void trapc(){
	printf("trap c\n");
}
void trapd(){
	printf("trap d\n");
}
void trape(){
	printf("trap e\n");
}
void trapf(){
	printf("trap f\n");
}

typedef void (*traps_t)();

traps_t traps[] = {
	trap0, trap1, trap2, trap3,
	trap4, trap5, trap6, trap7,
	trap8, trap9, trapa, trapb,
        trapc, trapd, trape, trapf
};

#ifdef __arm__
register usimd traptbl0 asm("q11");
register usimd traptbl1 asm("q14");
register usimd traptbl2 asm("q13");
register usimd traptbl3 asm("q12");
#endif

#ifdef __arch64__
register traptbl0 asm("W15");
register traptbl1 asm("W14");
register traptbl2 asm("W13");
register traptbl3 asm("W12");
#endif

#ifdef __x86_64__
register traptbl0 asm("Y15");
register traptbl1 asm("Y14");
register traptbl2 asm("Y13");
register traptbl3 asm("Y12");
#endif

#if !(defined(__arm__) | defined(__x86_64__) | defined(__aarch64__))
#error you are crazy to run this code in the first place.
#endif

#if defined(__arm__)
void jmp(void *p) { longjump(p); };

void * gethigh(usimd v) { return (void *) vget_lane_u32(v,1); };
void * getlow(usimd v) { return (void *) vget_lane_u32((uint32x2_t)v,0); };

//void * getmlow(usimd v) { return vget_lane_u32((uint32x2_t)v,1); };
//void * getvhigh(usimd v) { return vget_lane_u32((uint32x2_t)v,3); };

void load_traps() {
// I am under the impression I can load up everything with one ins
	traptbl0 = vld1q_u32((u32 *) &traps[0]);
	traptbl1 = vld1q_u32((u32 *) &traps[4]);
	traptbl2 = vld1q_u32((u32 *) &traps[8]);
	traptbl3 = vld1q_u32((u32 *) &traps[12]);
}
#endif

#if defined(__x86_64__)
void jmp(void *p) { return p; };
void * gethigh(usimd v) { return NULL; };
void * getlow(usimd v) { return NULL; };

void load_traps() {
	traptbl0 = vld1((u32 *) &traps[0]);
	traptbl1 = vld1((u32 *) &traps[4]);
	traptbl2 = vld1((u32 *) &traps[8]);
	traptbl3 = vld1((u32 *) &traps[12]);
}
#endif

#if defined(__aarch64__)

// 64 bits wide so we use up more regs fix me

void * getvhigh(usimd v) { return vget_lane_u32(v,3); };
void * gethigh(usimd v) { return vget_lane_u32(v,2); };
void * getmlow(usimd v) { return vget_lane_u32(v,1); };
void * getlow(usimd v) { return vget_lane_u32(v,0); };

void load_traps() {
	traptbl0 = vld1((u32 *) &traps[0]);
	traptbl1 = vld1((u32 *) &traps[4]);
	traptbl2 = vld1((u32 *) &traps[8]);
	traptbl3 = vld1((u32 *) &traps[12]);
}
#endif


void inline trapon(u8 t) {
	switch(t&7) {
	case 0: jmp(getlow(traptbl0));
	case 1: jmp(getlow(traptbl1));
	case 2: jmp(getlow(traptbl2));
	case 3: jmp(getlow(traptbl3));
	case 4: jmp(gethigh(traptbl0));
	case 5: jmp(gethigh(traptbl1));
	case 6: jmp(gethigh(traptbl2));
	case 7: jmp(gethigh(traptbl3));
	}
}

int main() {
	load_traps();
	trapon(0);
	trapon(4);
	trapon(7);
	trapon(1);
	trapon(2);
	trapon(3);
	trapon(4);
	return(0);
}
