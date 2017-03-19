/**
 * init.c
 *
 * Dave Taht
 * 2017-03-12
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "defaults.h"
#include "init.h"
#include "io.h"
#include "shared.h"
#include "traps.h"
// Meh. We should just keep the one and keep things to powers of two
// Why waste four bytes? :).
// ALWAYS treat addrdata as the canonical reference

v6table addrs;
v6table addrdatas;
v6table routes;
v6table routers;
v6table sources;

// Hmm. Maybe for our stronger typing we can do a cast thusly
// and hide the rest of the struct

v6addr_t* addresses;
addrflags_t* addrdata;

void* place_tables(void* mem)
{
  // do some mmap magic here
  return mem;
}

// FIXME - add interfaces and kill calloc as we know it

bool load_tables(void* mem)
{
  addrs.data = calloc(BASE, sizeof(v6addr_t));
  addrdatas.data = calloc(BASE, sizeof(addrflags_t));
  routes.data = calloc(BASE, sizeof(routes_t));
  sources.data = calloc(BASE, sizeof(sources_t));
  routers.data = calloc(NUM_ROUTERS, sizeof(routers_t));
  addresses = (v6addr_t*)addrs.data;
  if(addrs.data && addrdatas.data && routers.data) {
    addrs.size = addrdatas.size = routes.size = BASE;
    if(sources.data && routers.data) {
      sources.size = BASE;
      routers.size = NUM_ROUTERS;
      return true;
    }
  }
  return false;
}

/* pre-fill link local/mcast/v4mapped/etc/etc */

bool fill_tables(void* mem)
{
  addrflags_t temp = { 0 };
  addrflags_t* t = (addrflags_t*)addrdatas.data;
  temp.len = temp.pop = 255;
  temp.martian = temp.v4 = temp.v6 = true;
  t[addrdatas.idx++] = temp;

  // FIXME - add localhost, localhost6, unspec, link local, multicast v4, v6,
  // v4mapped
  // my protocol group is 1
  // protocol group ff02:0:0:0:0:0:1:6
  temp.len = temp.pop = 255;
  temp.martian = temp.v4 = false;
  t[addrdatas.idx++] = temp;

  // For conveinence, the first router id in the routerid table is me
  // wish I could just set the flags register and squeeze out a test
  return true;
}

// Need MAP_FIXED for the parallella
// probably should use MAP_HUGETLB
// /sys/kernel/mm/hugepages has a list of other page sizes
// MAP_LOCKED + mlock are correct ways to keep this in core
// MAP_POPULATE - zeros in the memory, I guess
// ftruncate?

#ifdef DEBUG_MODULE
#define MYMEM "/tabeld-test3"
#define babel_group 84
static int default_perms = (MAP_SHARED | MAP_HUGETLB);

int main(int argc, char **argv)
{
  char * shmem = argc == 2 ? argv[1] : MYMEM;
  int fd;
  int tsize = BASE * 16;
  uint32_t* mem = NULL;
  unsigned char* tables = NULL;
  TRAP_LT((fd = shm_open(shmem, O_CREAT | O_RDWR, 0)), 0,
          "Couldn't open shared memory - aborting");
  TRAP_WERR((fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP)),
            "Couldn't change shared memory mode"); // rw root, r group
  TRAP_WERR((fchown(fd, -1, babel_group)),
            "Couldn't change shared memory group");
  // hmm. if it exists we fail?
  ftruncate(fd, BASE * 16 * 4);
  TRAP_WEQ((mem = mmap(NULL, BASE * 16, PROT_READ | PROT_WRITE, default_perms, fd, 0)),
           (void*)-1, "Couldn't mmap shared huge page memory");
  if(mem == (void*)-1) {
    default_perms &= ~MAP_HUGETLB;
    TRAP_EQ((mem = mmap(NULL, BASE * 16, PROT_READ | PROT_WRITE, default_perms, fd, 0)),
            (void*)-1, "Couldn't mmap shared memory - aborting");
    //	        TRAP_WLEQ(mem = mmap(NULL,BASE*16,PROT_READ |
    // PROT_WRITE,default_perms , fd , 0)), (void *) -1, err, "Couldn't mmap
    // shared memory - aborting");
  }
  tables = place_tables(mem);
  load_tables(mem);
  fill_tables(mem);
  printf("success!\n");
  for(int i = 60; i > 0; --i) {
    mem[8] = i;
    usleep(333333);
  }
  mem[8] = 0;
  TRAP_WERR(munmap(mem, tsize), "Couldn't unmap shared memory");
  // err:
  TRAP_WERR(shm_unlink(shmem), "Couldn't close shared memory");
  printf("exiting\n");
}
#endif
