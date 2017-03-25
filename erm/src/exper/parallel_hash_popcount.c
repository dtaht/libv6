/*
 * parallel_hash_popcount.c © 2017 Michael David Täht
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* The root of ERM's classification system is to *rapidly* get both a popcount
   of the value(s) and a usable hash. One of the things that irks me about
hash
   developers is that they all benchmark their hashes with these enormouse
loops
   against X amount of data - long strings often - each time. Big, beautiful
benchmarks.

   Running a hot cache for their ins. As if our cpus did nothing more than
hash
   stuff all day.

   I don't run the hash often enough to care about that! I need a quick hash
on
   some fairly small fixed length values, then to move on. Start time is
   important. Now, it happens that cake ended up using a similar hash on 3
   values at the same time ... and I'm wondering that can yield to running 4
   hashes in parallel in the simd unit, which would be pretty awesome...

... but I don't know anyone that has single hash, multiple data code. (I
should
look).

   The first problem, is actually popcount. Which is an SSE4 instruction that
actually runs in the main cpu. Awesome. 0 clock cycles, given all the other
overheads we got. Except, where we don't have popcount - which is on
everything
else. __builtin_popcount is like 20 instructions per *word*. Which also might
yield to a vectorized approach. Now, if we can hash and popcount 4 values at
exactly the same time... big win. Worth writing lots of code for.

Well, first benchmarking what straightline code does makes the most sense. :)

*/


#include <math.h>
#include <stdint.h>
#include <tgmath.h>

#include "debug.h"
#include "erm_types.h"
#include "preprocessor.h"
#include "simd.h"

typedef struct {
  u64 one;
  u64 two;
} twocount;

// http://www.felixcloutier.com/x86/POPCNT.html

// ZF is set if all zeros, btw

// https://www.cs.cmu.edu/~fp/courses/15213-s07/misc/asm64-handout.pdf
// r12 is unused for C? goodie

// %q
// %rdi = first argument
// pinsrq

/*    void kcopy (unsigned int src, unsigned int dst,
                unsigned int nbytes) {
        __asm__ __volatile__ (
        "cld \n"
        "rep \n"
        "movsb \n"
        :
        : "S"(src), "D"(dst), "c"(nbytes)
        : "%esi", "%edi", "%ecx" );
    }
*/

// popcnt + cmov?

/* Label:			; this is a label in assembly
   INX 	EAX   	; increment eax
   LOOP 	Label		; decrement ECX, loop if not 0
*/

//        leal    -4(%ebp), %ebx
// If your code modifies the condition code register, “cc”.
// So the code I pull from the web is wrong....

// asm("sidt %0\n" :
//   : "m"(loc));

/*
inline u64 popcount2thinpostinc1(const u64 *buf) {
    u64 t;
    const int i = 0;
    __asm__ __volatile__(
        // "lea %1, %%rbx \n\t"
            "popcnt (%1), %0  \n\t"
            "again: popcnt (%1), %%rax  \n\t"
            "add %%rax, %0     \n\t"
        "shr $8, %0 \n\t "
            "popcnt (%1), %%rax  \n\t"
            "or %%rax, %0     \n\t"
            "popcnt (%1), %%rax  \n\t"
            "add %%rax, %0     \n\t"
            : "=w" (t)
            : "o" (buf)
                );
  return t;
}
*/

/*

*/

/* The generic version with less branches */

u64 popcount2generic_lessbranchy(const u64* buf, int cnt) COLD;

u64 popcount2generic_lessbranchy(const u64* buf, int cnt)
{
  u64 t = 0;
  u64 scratch;
  int i = 0;
  do {
    t = t << 8;
    scratch = __builtin_popcountll(*buf++);
    scratch += __builtin_popcountll(*buf++);
    t += scratch;
  } while(++i < cnt);
  return t;
}

u64 popcount2generic(const u64* buf, int cnt) COLD;

u64 popcount2generic(const u64* buf, int cnt)
{
  u64 t = 0;
  u64 scratch;
  for(int i = 0; i < cnt; i++) {
    t = t << 8;
    scratch = __builtin_popcountll(*buf++);
    scratch += __builtin_popcountll(*buf++);
    t += scratch;
  }
  return t;
}

// We can rule this out.

u64 popcount2genericbackwards(const u64* buf, int cnt)
{
  u64 t = 0;
  u64 scratch;
  for(; cnt > 0; cnt--) {
    t = t << 8;
    scratch = __builtin_popcountll(*buf++);
    scratch += __builtin_popcountll(*buf++);
    t += scratch;
  }
  return t;
}


// Popcount up to 8 128 bit values and stores the result in the return
// (doing more than that is doable - but futile)
// Lets you use any dest
// Uses a counter reg (ecx)
// Use a temp reg (any)
// Return result in a register

u64 popcount2anydest(const u64* buf, int cnt)
{
  u64 t = 0;
  u64* b2 = buf; // I don't want the !@#! %sp
  u64 scratch;
  __asm__ __volatile__("lea %5, %1 \n\t"
                       "popcnt_again%=:\n\t"
                       "shl $8, %0 \n\t "
                       "popcnt (%1), %2  \n\t"
                       "add %2, %0     \n\t"
                       "popcnt -0x8(%1), %2  \n\t"
                       "add $16, %1\n\t "
                       "add %2, %0     \n\t"
                       "LOOP popcnt_again%="
                       : "+r"(t), "=r"(*b2), "=r"(scratch), "=c"(cnt)
                       : "3"(cnt), "m"(*b2)
                       : "cc");
  return t;
}

// FIXME: Doesn't load up ecx
//        Not sure if it is doing the right thing with lea
//        (the rsp should be enough?)
//        I don't want to specify rax, or for that matter ecx
//        can just us a classic loop instead, but...

inline u64 popcount2thinpostinc1(const u64* buf, int cnt)
{
  u64 t;
  u64* b2 = buf; // I don't want the !@#! %sp
  __asm__ __volatile__(
  //	    "lea %2, %%rcx \n\t"
  "lea %3, %1 \n\t"
  "again:\n\t"
  "shl $8, %0 \n\t "
  "popcnt (%1), %%rax  \n\t"
  "add %%rax, %0     \n\t"
  "popcnt 8(%1), %%rax  \n\t"
  "add $16, %1\n\t "
  "add %%rax, %0     \n\t"
  "LOOP again"
  : "=r"(t), "=r"(b2)
  : "r"(cnt), "m"(buf)
  : "cc", "%rcx", "%rax");
  return t;
}

/*
inline u64 popcount2thinpostinc(const u64 *buf) {
    u64 t;
    const int i = 0;
    __asm__ __volatile__(
            "popcnt %1, %0  \n\t"
            "popcnt %2, %%rax  \n\t"
            "add %%rax, %0     \n\t"
        "shl $8, %0 \n\t "
            "popcnt %3, %%rax  \n\t"
            "or %%rax, %0     \n\t"
            "popcnt %4, %%rax  \n\t"
            "add %%rax, %0     \n\t"
            : "=r" (t) // , // "=a" (addr) ??
            : "o>"  (buf[i]), "o>"  (buf[i+1]), "o>"  (buf[i+2]), "o>"
(buf[i+3])
                );
  return t;
}

*/

inline u64 popcount2thin(const u64* buf)
{
  u64 t;
  const int i = 0;
  asm("nop; /* dave was here*/");
  __asm__ __volatile__("popcnt %1, %0  \n\t"
                       "popcnt %2, %%rax  \n\t"
                       "add %%rax, %0     \n\t"
                       "shl $8, %0 \n\t "
                       "popcnt %3, %%rax  \n\t"
                       "add %%rax, %0     \n\t"
                       "popcnt %4, %%rax  \n\t"
                       "add %%rax, %0     \n\t"
                       : "=r"(t)
                       : "o>"(buf[i]), "o>"(buf[i + 1]), "o>"(buf[i + 2]),
                         "o>"(buf[i + 3]));
  return t;
}


// The %q puts it anywhere it feels like

inline twocount popcount2wrong(const u64* buf)
{
  u64 cnt[1] = { 0 };
  const int i = 0;
  __asm__ __volatile__("popcnt %q, %2  \n\t"
                       "add %q, %0     \n\t"
                       "popcnt %q, %3  \n\t"
                       "add %q, %0     \n\t"
                       "popcnt %q, %4  \n\t"
                       "add %q, %1     \n\t"
                       "popcnt %q, %5  \n\t"
                       "add %q, %1     \n\t"
                       : "+r"(cnt[0]), "+r"(cnt[1])
                       : "r"(buf[i]), "r"(buf[i + 1]), "r"(buf[i + 2]), "r"(buf[i + 3]));
  twocount t;
  t.one = cnt[0];
  t.two = cnt[1];
  return t;
}

inline twocount popcount2cheaper(const u64* buf)
{
  u64 cnt[1] = { 0 };
  const int i = 0;
  __asm__ __volatile__("popcnt %2, %2  \n\t"
                       "add %2, %0     \n\t"
                       "popcnt %4, %4  \n\t"
                       "add %4, %1     \n\t"
                       "popcnt %3, %3  \n\t"
                       "add %3, %0     \n\t"
                       "popcnt %5, %5  \n\t"
                       "add %5, %1     \n\t"
                       : "+r"(cnt[0]), "+r"(cnt[1])
                       : "r"(buf[i]), "r"(buf[i + 1]), "r"(buf[i + 2]), "r"(buf[i + 3]));
  twocount t;
  t.one = cnt[0];
  t.two = cnt[1];
  return t;
}


inline twocount popcount2(const u64* buf)
{
  u64 cnt[4] = { 0 };
  const int i = 0;
  __asm__ __volatile__("popcnt %4, %4  \n\t"
                       "add %4, %0     \n\t"
                       "popcnt %5, %5  \n\t"
                       "add %5, %1     \n\t"
                       "popcnt %6, %6  \n\t"
                       "add %6, %2     \n\t"
                       "popcnt %7, %7  \n\t"
                       "add %7, %3     \n\t"
                       : "+r"(cnt[0]), "+r"(cnt[1]), "+r"(cnt[2]), "+r"(cnt[3])
                       : "r"(buf[i]), "r"(buf[i + 1]), "r"(buf[i + 2]), "r"(buf[i + 3]));
  twocount t;
  t.one = cnt[0] + cnt[1];
  t.two = cnt[2] + cnt[3];
  return t;
}


#ifdef DEBUG_MODULE
#define LOGGER_INFO(where, fmt, ...)

#ifndef DONOTHING
#define DONOTHING                                                            \
  do {                                                                       \
  } while(0)
#endif

#ifndef CALLOCA
#define CALLOCA(type, dest, size, num)                                       \
  type dest __attribute__((aligned(16)));                                    \
  dest = calloc(size, num)
#endif

#include "align.h"
#include "cycles_bench.h"
#include "erm_logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

u64 test1[4] = { 0 };
u64 test2[4] = { 0xff, 0xff, 0xff, 0x7f };
u64 test3[4] = { 0xff7f, 0x7fffffff, 0x7f, 0xff };
u64 test4[4] = { 0xff7f, 0x7fffffff, 0xff, 0x7f };
u64 test5[4] = { 0xff, 0x7f, 0xff7f, 0x7fffffff };
u64 test6[16] = { 0xff,   0x7f,       0xff7f, 0x7fffffff, 0xff, 0x7f,
                  0xff7f, 0x7fffffff, 0xff7f, 0x7fffffff, 0xff, 0x7f,
                  0xff7f, 0x7fffffff, 0x7f,   0xff };

int main()
{
  twocount t = popcount2(&test1);
  printf("popzero: %d %d\n", t.one, t.two);
  t = popcount2(&test2);
  printf("popzero: %d %d\n", t.one, t.two);
  t = popcount2(&test3);
  printf("popzero: %d %d\n", t.one, t.two);
  t = popcount2cheaper(&test4);
  printf("pop2cheaper: %d %d\n", t.one, t.two);
  short s = popcount2thin(&test5);
  printf("pop2cheapest: %d %d\n", s >> 8, s & 255);
  //	s = popcount2thinpostinc(&test5);
  //	printf("pop2postinc: %d %d\n",s >> 8, s & 255);
  s = popcount2anydest(&test5, 2);
  printf("pop2anydest-5: %d %d\n", s >> 8, s & 255);
  s = popcount2generic(&test5, 2);
  printf("pop2generic-5: %d %d\n", s >> 8, s & 255);
  s = popcount2anydest(&test4, 2);
  printf("pop2anydest-4: %d %d\n", s >> 8, s & 255);
  s = popcount2generic(&test4, 2);
  printf("pop2generic-4: %d %d\n", s >> 8, s & 255);
  s = popcount2generic_lessbranchy(&test4, 2);
  printf("pop2generic_lessbranch-4: %d %d\n", s >> 8, s & 255);
  s = popcount2anydest(&test3, 2);
  printf("pop2anydest-3: %d %d\n", s >> 8, s & 255);
  s = popcount2generic(&test3, 2);
  printf("pop2generic-3: %d %d\n", s >> 8, s & 255);
  s = popcount2anydest(&test2, 2);
  printf("pop2anydest-2: %d %d\n", s >> 8, s & 255);
  s = popcount2generic(&test2, 2);
  printf("pop2generic-2: %d %d\n", s >> 8, s & 255);
  // What would be the best logic? Low bits? high bits?
  s = popcount2anydest(&test6, 8);
  printf("pop2anydest8: %d %d\n", s >> 8, s & 255);
  // Generic test
  s = popcount2generic(&test6, 8);
  printf("pop2generic8: %d %d\n", s >> 8, s & 255);

  return 0;
}

#endif
