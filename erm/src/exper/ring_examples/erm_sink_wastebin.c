/*
 * erm_sink_wastebin.c © 2017 Michael David Täht
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

// https://en.wikipedia.org/w/index.php?title=Circular_buffer&oldid=616721206#Optimization

#include "erm_sink_wastebin.h"

#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>

#define report_exceptional_condition() abort ()

enum watermark(dead, empty, quarter, half, threequarter, ninety, full);

typedef struct
{
  void *address;
  // *read_hook;
  // *write_hook;
  // *get_watermark(enum watermark);
  unsigned long count_bytes;
  unsigned long write_offset_bytes;
  unsigned long read_offset_bytes;
} ring_buffer;

// Warning order should be at least 12 for Linux

ring_buffer
ring_buffer_create (unsigned long order)
{
  char path[] = "/dev/shm/ring-buffer-XXXXXX";
  int file_descriptor;
  void *address;
  int status;
  ring_buffer buffer = {0};

  file_descriptor = mkstemp (path);
  if (file_descriptor < 0)
    report_exceptional_condition ();

  status = unlink (path);
  if (status)
    report_exceptional_condition ();

  buffer.count_bytes = 1UL << order;
  buffer.write_offset_bytes = 0;
  buffer.read_offset_bytes = 0;

  status = ftruncate (file_descriptor, buffer.count_bytes);
  if (status)
    report_exceptional_condition ();

  buffer.address = mmap (NULL, buffer.count_bytes << 1, PROT_NONE,
                          MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  if (buffer.address == MAP_FAILED)
    report_exceptional_condition ();

  address =
    mmap (buffer.address, buffer.count_bytes, PROT_WRITE,
          MAP_FIXED | MAP_SHARED, file_descriptor, 0);

  if (address != buffer.address)
    report_exceptional_condition ();

  address = mmap (buffer.address + buffer.count_bytes,
                  buffer.count_bytes, PROT_WRITE,
                  MAP_FIXED | MAP_SHARED, file_descriptor, 0);

  if (address != buffer.address + buffer.count_bytes)
    report_exceptional_condition ();

  // Create a 1MB wastebin mapped onto 4k of memory

  for(int i=0; i <256; i++) {
  void * newaddress = mmap (address + buffer.count_bytes,
                  buffer.count_bytes, PROT_WRITE,
                  MAP_FIXED | MAP_SHARED, file_descriptor, 0);

  if (newaddress != address + buffer.count_bytes)
    report_exceptional_condition ();
  address = newaddress;
  }

  status = close (file_descriptor);
  if (status)
    report_exceptional_condition ();

  return buffer;
}

void
ring_buffer_free (ring_buffer buffer)
{
  int status;

  status = munmap (buffer.address, buffer.count_bytes << 1);
  if (status)
    report_exceptional_condition ();
}

void *
ring_buffer_write_address (ring_buffer buffer)
{
  /*** void pointer arithmetic is a constraint violation. ***/
  return buffer.address + buffer.write_offset_bytes;
}

void
ring_buffer_write_advance (ring_buffer buffer,
                           unsigned long count_bytes)
{
  buffer.write_offset_bytes += count_bytes;
}

void *
ring_buffer_read_address (struct ring_buffer *buffer)
{
  return buffer.address + buffer.read_offset_bytes;
}

void
ring_buffer_read_advance (ring_buffer buffer,
                          unsigned long count_bytes)
{
  buffer.read_offset_bytes += count_bytes;

  if (buffer.read_offset_bytes >= buffer.count_bytes)
    {
      buffer.read_offset_bytes -= buffer.count_bytes;
      buffer.write_offset_bytes -= buffer.count_bytes;
    }
}

unsigned long
ring_buffer_count_bytes (ring_buffer buffer)
{
  return buffer.write_offset_bytes - buffer.read_offset_bytes;
}

unsigned long
ring_buffer_count_free_bytes (ring_buffer buffer)
{
  return buffer.count_bytes - ring_buffer_count_bytes (buffer);
}

void
ring_buffer_clear (ring_buffer buffer)
{
  buffer.write_offset_bytes = 0;
  buffer.read_offset_bytes = 0;
}

#ifdef DEBUG_MODULE
#include <stdio.h>

int main() {
	ringbuffer r = create_wastebin(where, power, mmap_options);
	if(r.address == NULL) perror("no wastebin");
	int *p = r.address;
	for(int i = 0; i < 1024*4; i++) {
		p[i] = i;
	}
	for(int i = 0; i < 1024*4; i++) {
		if(p[i] == 0) {
			// should not happen
			printf("i is 0 at: %i\n", i);
			abort();
		};
	}
	for(int i = 0; i < 1024*1024; i++) {
		if(p[i] == 0) {
			printf("i is 0 at: %i\n", i);
		};
	}
	printf("overrun: memory access error expected\n");
	for(int i = 1024*1024; i < 1024*1024+256; i++) {
		if(p[i] == 0) {
			printf("i is 0 at: %i\n", i);
		};
	}
}
#endif
