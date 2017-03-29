/*
 * bits.c © 2017 Michael David Täht
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
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

#include "bits.h"

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

u64 popcountb(const u64* buf, bit3 cnt)
{
  u64 t = 0;
  u64 scratch;
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

#ifdef DEBUG_MODULE
FIXME
#endif
