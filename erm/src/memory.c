
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

