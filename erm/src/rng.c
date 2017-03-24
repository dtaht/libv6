/*
 * rng.c © 2017 Michael David Täht
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

/* ERM's emulation of a hardware rng is different from a real rng. It is
   entirely nonblocking, using a periodically regenerated pnrg from another
   thread. If you overrun the entropy pool, you start over from the beginning,
   which for all you know has been regenerated while you weren't looking but
   you
   can't be sure.

   By default it is one memory page in size, but can be compiled down to
   two cachelines and placed in hot or cold memory.

   You can have many hwrngs.

   Access to the entropy pool is strictly read-only, and can be generated
   uniquely per thread.

*/

#ifndef ERM_RND_PAGE_SIZE
#warning you should very much define ERM_RND_PAGE_SIZE
#define ERM_RND_PAGE_SIZE (256)
#endif

#define SECTION(a)

#include "shared.h"
#include "preprocessor.h"
#include "erm_types.h"
#include "get_cycles.h"
#include "erm_rng.h"

u8 rngpool[ERM_RND_PAGE_SIZE / sizeof(u32)] SECTION("rng");

// Simple stupid filler thread

void rng_fill()
{
	for(int i = 0; i < ERM_RND_PAGE_SIZE/sizeof(i); i++) {
        rngpool[i] = random();
  }
//    sleep(1);
}

#ifdef DEBUG_MODULE
#include <stdio.h>

#define PERF stdout
#define LOGGER_INFO(where, fmt, ... ) fprintf(where, fmt, __VA_ARGS__)

#include "erm_logger.h"
#include "get_cycles.h"

int main()
{
  rng_fill();
  LOGGER_INFO(PERF, "This will probably not be sufficiently random: "
                    " u8: %c u16: %d u32: %d u64: %ld\n",
              get_rng_bytes1(), get_rng_bytes2(), get_rng_bytes4(), get_rng_bytes8());
  return 0;
}

#endif
