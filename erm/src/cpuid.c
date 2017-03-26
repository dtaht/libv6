/*
 * cpuid.c © 2017 Michael David Täht
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

#include <cpuid.h>
#include <stdio.h>

/* Need something a bit more elaborate than this but... */

#ifdef DEBUG_MODULE
void
main () {
  unsigned int eax, ebx, ecx, edx;

 __get_cpuid(1, &eax, &ebx, &ecx, &edx);

  if (ecx & bit_SSE4_2)
    printf ("SSE4.2 is supported\n");
  return;
}

#endif
