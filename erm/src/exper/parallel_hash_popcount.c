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
   of the value(s) and a usable hash. One of the things that irks me about hash
   developers is that they all benchmark their hashes with these enormouse loops
   against X amount of data - long strings often - each time. Big, beautiful benchmarks.

   Running a hot cache for their ins. As if our cpus did nothing more than hash
   stuff all day.

   Not, as if, I run the hash often enough to care about that! I need a quick
   hash on some fairly small fixed length values, then move on. Start time is
   important. Now, it happens that cake ended up using a similar hash on 3
   values at the same time ... and I'm wondering that can yield to running 4
   hashes in parallel in the simd unit, which would be pretty awesome...

... but I don't know anyone that has single hash, multiple data code. (I should
look).

   The first problem, is actually popcount. Which is an SSE4 instruction that
actually runs in the main cpu. Awesome. 0 clock cycles, given all the other
overheads we got. Except, where we don't have popcount - which is on everything
else. __builtin_popcount is like 20 instructions per *word*. Which also might
yield to a vectorized approach. Now, if we can hash and popcount 4 values at
exactly the same time... big win. Worth writing lots of code for.

Well, first benchmarking what straightline code does makes the most sense. :)

*/


#include "parallel_hash_popcount.h"

#ifdef DEBUG_MODULE
FIXME
#endif
