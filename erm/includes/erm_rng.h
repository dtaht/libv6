/*
 * erm_random.h © 2017 Michael David Täht
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

#ifndef ERM_RANDOM_H
#define ERM_RANDOM_H

/* FIXME: Since the concept of the hwrng is many - pulling from a different
   pool per functional unit is needed. Sometimes. Unless we do linker magic.
*/

extern u8 rngpool[ERM_RND_PAGE_SIZE / sizeof(u32)]; // SECTION("rng")

#if !(defined(PARANOID_CRYPTOGRAPHERS) | defined(HAVE_NO_CYCLES))

#define RNDTABLE_SIZE_MASK (255)

/*
   My simplest implementation. ... except that get_cycles is slow, buggy, or
   unavailable on some processors.

   We also have on-board time which we could use that + pc counter

   If time itself wasn't being emulated.

   FIXME: I am also reminded of the time that get_cycles was indeed compiled out
   and returning 0 to genuine crypto code in... was it arm? mips?
*/

ALWAYS_INLINE u8 static inline get_rng_bytes1()
{
  return *(u8 *) &rngpool[get_cycles() & (RNDTABLE_SIZE_MASK)];
}
ALWAYS_INLINE static inline u16 get_rng_bytes2()
{
  return *(u16 *) &rngpool[(get_cycles() << 1) & (RNDTABLE_SIZE_MASK)];
}
ALWAYS_INLINE static inline u32 get_rng_bytes4()
{
  return *(u32 *) &rngpool[(get_cycles() << 2) & (RNDTABLE_SIZE_MASK)];
}
ALWAYS_INLINE static inline u64 get_rng_bytes8()
{
  return *(u64* ) &rngpool[(get_cycles() << 3) & (RNDTABLE_SIZE_MASK >> 1)];
}
/*

ALWAYS_INLINE u128 static inline get_rng_bytes128()
{
  return (u128)rngpool[(get_cycles() << 4) & (RNDTABLE_SIZE_MASK >> 2)];
}

// And hopefully this will let me load up sse/neon sanely
ALWAYS_INLINE static inline v64 get_rng_bytesv8()
{
  return (v64)rngpool[(get_cycles() << 3) & (RNDTABLE_SIZE_MASK >> 1)];
}
ALWAYS_INLINE static inline v128 get_rng_bytesv16()
{
  return (v128)rngpool[(get_cycles() << 4) & (RNDTABLE_SIZE_MASK >> 2)];
}
*/
#else
/* TODO - real rng. Also one that always returns 0 for sims */
#warning better version of erm_rng not written yet
#endif

#endif
