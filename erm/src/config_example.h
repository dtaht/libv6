/*
 * config_example.h © 2017 Michael David Täht
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

/*
   As the number of optional pre-processor defines for erm has grown
   it's got hard to keep them in my head, or easily control them in a
   makefile.

   Besides, I have some funny ones.

   There are also a plethora of semi-optional ones defined in the preprocessor
   and simd headers.

*/

#ifndef CONFIG_EXAMPLE_H
#define CONFIG_EXAMPLE_H

// Compile a (nearly) standalone version of a file for tests and debugging
// so "make whatever.dbg" works.
// #define DEBUG_MODULE

// 64 bit architecture
// #define HAVE_64BIT_ARCH

// 128 bit architecture
// #define HAVE_128BIT_ARCH

// Use vectorizable code
// #define HAVE_VECTORS

// High speed interprocessor mcast
// #define HAVE_SPAM

// Have a cycles timer
// #define HAVE_CYCLES

// Compile without timers
// #define NO_CYCLES

// Use Linux perf events (interferes with ~NO_CYCLES)
// #define HAVE_PERF

// Worry harder about secure constructs
// #define PARANOID_CRYPTOGRAPHERS

// Take it to 11.
// - Compile with max optimism.
// - Use risky data structures.
// - Throw caution to the winds.
// #define WANT_ELEVEN

// Ludicrous mode
// A place for ideas that are in progress that may not work out
// - Eventually may migrate to 11 or the base machine
// #define LUDICROUS

#endif
