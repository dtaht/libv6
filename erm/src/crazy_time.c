/*
 * crazy_time.c © 2017 Michael David Täht
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

// #include "crazy_time.h"

#include <stdint.h>
#include <stdbool.h>

#include "preprocessor.h"
#include "erm_types.h"
#include "simd.h"
#include "get_cycles.h"

/* This is not intended to be serious code. It is just me trying to find
   ways to portably use up the vector unit. In this example, time
   is a 120 bit number. */

#define SOURCE_DATE_EPOCH 0

#define TAGBITS (1<<31 | 1<<30)

extern usimd ttov(long v);
extern bool is_not_zero(usimd a);
extern void time_adj();

u32 svtags[4] = { TAGBITS, TAGBITS, TAGBITS, TAGBITS };

register usimd erm_time asm("xmm15");
register usimd vtags asm("xmm14");

// SOURCE_DATE_EPOCH is needed for reproducable builds
// and is defined to be a string of seconds since the regular epoch
// e.g - the output of date +%s. As a string, it's a PITA, and we need
// to get time fast...

void erm_normalize_time() {
	Vec4_t t;
	t.p = erm_time;
}

void init_time() {
	vtags = * (usimd *) svtags;
	erm_time = ttov(SOURCE_DATE_EPOCH);
}

// This is actually fired on an interrupt

void update_time() {
	u32 c = get_bottom_cycles(); // get timer actually
	Vec4_t v = {0};
	v.c[3] = (c & ( (1<<30) - 1));
	v.c[2] = (c >> 28) & ~3;
	erm_time += v.p;
	if(is_not_zero(erm_time & vtags)) {
		time_adj();
	}
}

#ifdef DEBUG_MODULE
#include <stdio.h>
int main() {
	init_time();
}

#endif
