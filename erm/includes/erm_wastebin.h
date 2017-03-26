/*
 * erm_wastebin.h © 2017 Michael David Täht
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
 * The wastebin is a write only memory structure. It cannot be read, only
 * written to, and by default is a 4K memory page mapped on itself a few
 * hundred times.
*/


#ifndef ERM_WASTEBIN_H
#define ERM_WASTEBIN_H

extern void* wastebin_mem; // With per CPU linker magic

// register void * WasteBin REGISTER(waste);

void* wastebin_create(erm_t instance);
void* wastebin_get(erm_t instance);

#endif
