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
   which for all you know has been regenerated while you weren't looking but you
   can't be sure.

   By default it is one memory page in size, but can be compiled down to a
   cacheline and placed in hot or cold memory.

   You can have many hwrngs.

   Access to the entropy pool is strictly read-only, and can be generated
   uniquely per thread.

*/

#include "rng.h"

// My simplest implementation of this would be get_cycles & (TOP_MASK | ALIGNMENT)
// Toss all randomness into one memory area
// whatever SECTION HWRNG

#ifdef DEBUG_MODULE
FIXME
#endif
