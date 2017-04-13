
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

/* Originally, what I envisioned was "divvying up the work"
   and splitting the work at 64k entries. At the moment,
   I seem to be splitting up things by popcount, and/or
   with/without actual memory segmentation */

#define POPSHIFT 15 // 15 bit indexes, deal with it
#define POPMASK  ((1 << POPSHIFT+8) - 1)
#define PAGE_SIZE (4096)
// FIXME PAGESIZE from OS
#define V6CACHE (1024*1024*16)

// If we end up going with disjoint addresses, this grows
// to include a pointer to each space or more likely:

typedef struct {
  union {
    v64 *v6;
    u32 *v4;
  };
} popaddrs;

// Or maybe I'll squeeze this still further and use the pages
// instead of entries

typedef struct {
  u16 size;
  u16 entries;
} popentry;

popentry v6poptable[128]; // FIXME SECTION SHAREDHOT
popentry v4poptable[32];  // FIXME SECTION SHAREDHOT
v64 addr;

// This attempts to map the entire possible ipv6 route table into one
// big space, and then lazily allocate extra space beyond the first 4k
// segment for each pop area.

void * get_v6space() {
  static u32 mflags = MAP_SHARED | MAP_ANONYMOUS | MAP_NORESERVE | MAP_UNINITIALIZED;
  addr = mmap(NULL, V6CACHE, PROT_READ|PROT_WRITE, mflags, -1, 0);
  if(addr == MAP_FAILED) abort();
  // FIXME: write red zone at the end not the beginning
  for(int i; i < V6CACHE; i++) {
    addr[i * (1 << (POPSHIFT - 2))] = 0xFFFF0000;
    vpoptable[i].size = PAGE_SIZE/sizeof(v64);
  }
  return addr;
}

void *get_poptable() {
  //  static u32 mflags = MAP_SHARED | MAP_ANONYMOUS | MAP_NORESERVE | MAP_UNINITIALIZED;
  //  addr = mmap(NULL, 1024*1024*24, PROT_READ|PROT_WRITE, mflags, -1, 0);
}

int check_poptable(u32 index, popentry *p) {
  u16 check = (index & POPMASK) >> POPSHIFT;
  if(p[check].entries) { trap(segvio); touch(addrs[bla]); }  
}

static inline addr get6addr(u24 index) {
  return addr[index];
}

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

