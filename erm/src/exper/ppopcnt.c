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

/* The root of ERM's classification system is to *rapidly* get both a
   popcount of the value(s) and a usable hash. One of the things that
   irks me about hash developers is that they all benchmark their
   hashes with these enormous loops against X amount of data - long
   strings often - each time. Big, beautiful benchmarks.

   Running a hot cache for their ins. As if our cpus did nothing more
than hash stuff all day.

   I don't run the hash often enough to care about that! I need a
quick hash on some fairly small fixed length values, then to move
on. Start time is important. Now, it happens that cake ended up using
a similar hash on 3 values at the same time ... and I'm wondering that
can yield to running 4 hashes in parallel in the simd unit, which
would be pretty awesome...

... but I don't know anyone that has single hash, multiple data
code. (I should look).

   The first problem, is actually popcount. Which is an SSE4
instruction that actually runs in the main cpu. Awesome. 0 clock
cycles, given all the other overheads we got. Except, where we don't
have popcount - which is on everything else. __builtin_popcount is
like 20 instructions per *word*. Which also might yield to a
vectorized approach. Now, if we can hash and popcount 4 values at
exactly the same time... big win. Worth writing lots of code for.

Well, first benchmarking what straightline code does makes the most
sense. :)

*/


#include <math.h>
#include <stdint.h>
#include <tgmath.h>

#include "debug.h"
#include "erm_types.h"
#include "preprocessor.h"
#include "simd.h"

// Most arches don't have a native 128 bit type

typedef struct {
  u64 one;
  u64 two;
} twocount;

typedef struct {
 	u8 v:1;
} bit1;
typedef struct {
 	u8 v:2;
} bit2;
typedef struct {
 	u8 v:3;
} bit3;
typedef struct {
 	u8 v:4;
} bit4;

u16 popcountv6inlinefold(const u64* buf)
{
  u16 t = 0;
  u16 scratch;
    scratch  = __builtin_popcountll(*buf++);
    scratch += __builtin_popcountll(*buf++);
    t += scratch;
    t = t << 7;
    scratch  = __builtin_popcountll(*buf++);
    scratch += __builtin_popcountll(*buf++);
    t |= scratch;
  return t;
}

u16 popcountv6inline(const u64* buf)
{
  u16 t = 0;
  u16 scratch;
    scratch  = __builtin_popcountll(*buf++);
    scratch += __builtin_popcountll(*buf++);
    t += scratch;
    t = t << 8;
    scratch  = __builtin_popcountll(*buf++);
    scratch += __builtin_popcountll(*buf++);
    t |= scratch;
  return t;
}

u16 popcountb(const u64* buf, bit2 cnt)
{
  u16 t = 0;
  u16 scratch;
  do {
    t = t << 8;
    scratch = __builtin_popcountll(*buf++);
    scratch += __builtin_popcountll(*buf++);
    t += scratch;
  } while(cnt.v--);
  return t;
}

u64 popcountb2(const u64* buf, bit3 cnt)
{
  u64 t = 0;
  u64 scratch;
  for(;cnt.v;cnt.v--) {
    t = t << 8;
    scratch = __builtin_popcountll(*buf++);
    scratch += __builtin_popcountll(*buf++);
    t += scratch;
  }
  return t;
}

#ifdef __epiphany__

// WIP

#include "e_lib.h"

// Use up 4 cores to do popcount in parallel

u64 lv __attribute__ ((section (“.data_bank3”)));
u64 pv __attribute__ ((section (“.data_bank3”)));

int eppoppr2(u32 *x)
{
int c = 0;
while(1) {
	u32 c[4] = {0};
        e_barrier();
	for(y=0; y<4; y++) {
               for (; x[y] > 0; x[y] &= x[y] -1) c[y]++;
	}
        send(c);
	c[]={0};
        e_barrier();
        }
}

u64 epoppr(wg) {
	while(1) {
	e_barrier(wg);
	pv = __builtin_popcountll(lv);
	e_barrier(wg);
	}
}

u64 epopcount64(const u32* buf, int cnt) __attribute__ ((section (“.text_bank0”)));

u64 epopcount64(const u32* buf, int cnt) __attribute__ ((section (“.text_bank0”)));
{
  u64 t = 0;
  u64 scratch;
  for(int i = 0; i < cnt; i++) {
	  epopmsg(core, *buf++);
  }
  barrier(wg);
  for(int)
  for(int i = 0; i < 2; i++) {
	  
  return t;
}

#endif

#ifdef __x86_64__

// return two different crc values and merge them

/*
u64 crc128asm2(const u64* buf, int cnt) COLD;

u64 crc128asm2(const u64* buf, int cnt) {
  register u64 counter asm("%rax");
  __asm__ __volatile__(
  "xor    %%eax,%%eax\n\t"
  "mov    %%esi,%%ecx\n\t"
  "crcloop%=:\n\t"
  "shl    $0x20,%%rax\n\t"
  "crc32 (%%rdi),%%r8\n\t"
  "add    $0x10,%%rdi\n\t"
  "or     %%r8,%%rax\n\t"
  "crc32 -0x8(%%rdi),%%r8\n\t"
  "crc32  %%r8,%%rax\n\t"
  "LOOP crcloop%=\n\t"
  : // "+r" (counter)
  :
  : "cc", "%rdi", "%rax", "%ecx", "%r8"
  );
  return counter;
}

*/
  
u64 popcount128asm(const u64* buf, int cnt) COLD;

u64 popcount128asm(const u64* buf, int cnt) {
  register u64 counter asm("%rax");
  __asm__ __volatile__(
  "xor    %%eax,%%eax\n\t"
  "mov    %%esi,%%ecx\n\t"
  "poploop%=:\n\t"
  "shl    $0x8,%%rax\n\t"
  "popcnt (%%rdi),%%r8\n\t"
  "add    $0x10,%%rdi\n\t"
  "or     %%r8,%%rax\n\t"
  "popcnt -0x8(%%rdi),%%r8\n\t"
  "add    %%r8,%%rax\n\t"
  "LOOP poploop%=\n\t"
  : // "+r" (counter)
  :
  : "cc", "%rdi", "%rax", "%ecx", "%r8"
  );
  return counter;
}
#else
#define popcount128asm(a,b) popcount128(a,b)
#endif

u64 popcount128(const u64* buf, int cnt) HOT;

u64 popcount128(const u64* buf, int cnt)
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

// Tighter loop for 32 bit arches (I don't want to unroll this!)
// But I do want it to go in a hot section. Hmm.

u64 popcount128x32(const u32* buf, int cnt) COLD;

u64 popcount128x32(const u32* buf, int cnt)
{
  u64 t = 0;
  u64 scratch;
  for(int c = 0; c < cnt; c++) {
  t = t << 8;
  for(int i = 0; i < 4; i++) {
    scratch = __builtin_popcount(*buf++);
    t += scratch;
  }
  }
  return t;
}

/* vector version

u64 struct popcount128(const u64* buf)
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
*/

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
  u64 s;
  s = popcount128x32(&test6, 2);
  printf("pop128x32: %d %d\n", s >> 8, s & 255);
  s = popcount128(&test6, 2);
  printf("pop128: %d %d\n", s >> 8, s & 255);
  s = popcount128asm(&test6, 2);
  printf("popasm: %d %d\n", s >> 8, s & 255);

  return 0;
}

#endif
