/*
 * thread_per_cpu.c © 2017 Michael David Täht
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

/* Create one dedicated thread per cpu only */

#include "thread_per_cpu.h"

//	pthread_attr_init(attr);

Threads thread_per_cpu(void *thread) {
	pthread_attr_t attr;
	int c;
	size_t cpusetsize;
	for (c = getcpus(); c > -1; --c) {
		pthread_attr_setaffinity_np(attr, sizeof(cpu_set), cpuset);
  	        pthread_create(thread, attr, start, arg);
	}
}

#ifdef DEBUG_MODULE
stagger(timers,30);
FIXME
#endif
