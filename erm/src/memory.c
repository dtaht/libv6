
/* Erm memory generally always needs to be aligned to the page.  There
   are private memory spaces, spaces that are read or write only or
   placed in special places, or seen in two places, and so on, to
   handle, that needs a bigger API than what I'm outlining below.  The
   concept of "heap" is not enough. Ideally underrunning or over running
   any space should cause a trap, with a trap handler for it.

   Abstractions for "oncpu", "offcpu", etc are needed.

   I'll probably end up calling sbrk directly in some cases.

   growing an allocation needs to be atomically swapped.

   A big grow needs to be interruptable.

   or pausable.

   I forget what else.

Anyway, the simplest possible implementation uses 32k (15 bit) indexes
offset by 7.1 bits of popcount, of the size of the ipv6 address.

A popcount of 0 or 128 are special cases.

Assuming you had a perfect distribution, (we don't), this leaves room
for for a lot of addresses/prefixes. Still assuming that distribution,
given projected rates of growth for the BGP route table, that should
take us into 2050 or so. I think.

Each 4k block for addresses can hold up to 256 addresses, but we
define the end 4 addresses as a red zone before we expand.

Ipv4 uses 6 bits of popcount, of the size of the ipv4 address

This leaves lookup tables that look like this:


   mmap 8 * 32k * 16

   pop | 

   addrindex & ((1 << 23) - 1)

*/

// reallocation is always a copy and may resort to dma

erm_realloc_double(mem,size,where)
{
}

void *erm_realloc(mem,size,where)
{
  void *tmp = mmap();
  memcpy(mem);
#ifdef PARANOID_CRYPTOGRAPHERS
  memzero(mem,oldsize);
#endif
  munmap(mem);
}

void * erm_malloc(size,where,prot)
{
  return malloc(size);
}

void * erm_calloc(mem,size,where,prot)
{
  return calloc(mem,size);
}

void * erm_free(mem)
{
#ifdef PARANOID_CRYPTOGRAPHERS
  memzero(mem,oldsize);
#endif
  munmap(mem);
}

// A helper for a mmapped rungbuffer

void * erm_map_on_self(times)
{
}

// various functions for better inlining memmov
// for short bits of data

