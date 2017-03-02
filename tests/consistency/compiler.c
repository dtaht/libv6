/* Is the compiler smart enough to load stuff into neon 
   regs and leave them there? */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arm_neon.h>

#define MIN(a,b) a < b ? b : a

#include "common.h"
#include "my_memcpy.h"
#include "native_neon.h"

// http://stackoverflow.com/questions/38547082/arm-neon-assembly-proper-way-to-test-values?rq=1

// s16–s31 (d8–d15, q4–q7) must be preserved according to this

/*
http://stackoverflow.com/questions/261419/arm-to-c-calling-convention-registers-to-save
*/

/* but the arm 64bit arch is different. I've read this three times and I still don't understand it.

The ARM 64-bit architecture also has a further thirty-two registers, v0-v31, which can be used by SIMD and Floating-Point operations. The precise name of the register will change indicating the size of the access.

Note: Unlike in AArch32, in AArch64 the 128-bit and 64-bit views of a SIMD and Floating-Point register do not overlap multiple registers in a narrower view, so q1, d1 and s1 all refer to the same entry in the register bank.

The first eight registers, v0-v7, are used to pass argument values into a subroutine and to return result values from a function. They may also be used to hold intermediate values within a routine (but, in general, only between subroutine calls).


Registers v8-v15 must be preserved by a callee across subroutine calls; the remaining registers (v0-v7, v16-v31) do not need to be preserved (or should be preserved by the caller). Additionally, only the bottom 64-bits of each value stored in v8-v15 need to be preserved; it is the responsibility of the caller to preserve larger values.

*/



// Then we have q13-q15 as scratch

// Typical pre-amble
// vld4 cmp q8 // typically preserved
// vld4 cmp q10 // typically preserved

// vld4 src q0
// vld1 src+32 q2 // d? and then convert to mask 

// This is the naive version of a test

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
		if(is_not_zero(veorq_u32(v4_prefix,tmp))) {
			return 4;
		}
	}}}
return -1;
}

int main() {

     unsigned char *myzeros = calloc(1,16);
     unsigned char myones[16] = { 1 };
     unsigned char myll[16] = { 0xfe, 0x80 };
     unsigned char myv4mapped[16] = { 0 };
     init_simd();

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
