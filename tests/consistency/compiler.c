/* Is the compiler smart enough to load stuff into neon 
   regs and leave them there? */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arm_neon.h>

#define MIN(a,b) a < b ? b : a

#include "common.h"
#include "native_neon.h"

// http://stackoverflow.com/questions/38547082/arm-neon-assembly-proper-way-to-test-values?rq=1

typedef uint32x4_t usimd;

// register int *p2 asm ("r1") = …;

register usimd ll asm ("q15");
register usimd v4_mapped asm ("q14");
register usimd zeros asm ("q13");
register usimd ones asm ("q12");

static inline int compare(const unsigned char *src) {
        usimd tmp = vld1q_u32((const unsigned int *) src);
	if(is_not_zero(veorq_u32(zeros,tmp))) {
		return 1;
	} else {
	if(is_not_zero(veorq_u32(ones,tmp))) {
		return 2;
	} else {
	if(is_not_zero(veorq_u32(ll,tmp))) {
		return 3;
	} else {
		if(is_not_zero(veorq_u32(v4_mapped,tmp))) {
			return 4;
		}
	}}}
return -1;
}

int main() {
     unsigned char *myzeros = calloc(1,16);
     unsigned char myones[16] = { 1 };
     unsigned char myll[16] = { 0, 0xfe80 };
     unsigned char myv4mapped[16] = { 0 };
     printf("Zeros compare %d\n", compare(myzeros));
     printf("Ones compare %d\n", compare(myones));
     printf("Myll compare %d\n", compare(myll));
     printf("v4mapped compare %d\n", compare(myv4mapped));
     myzeros = calloc(2,16);
     myones[11] = 2;
     myll[2] = 0xfe80;
     myv4mapped[2] = 44;
     printf("Zeros compare %d\n", compare(myzeros));
     printf("Ones compare %d\n", compare(myones));
     printf("Myll compare %d\n", compare(myll));
     printf("v4mapped compare %d\n", compare(myv4mapped));

}
