/**
 * erm_client.c
 *
 * Dave Taht
 * 2017-03-17
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "c11threads.h"
#include "defaults.h"
#include "erm_client.h"
#include "init.h"
#include "io.h"
#include "shared.h"
#include "traps.h"

// yea, global memory. Shoot me

static int default_perms = (MAP_SHARED | MAP_HUGETLB);

static __thread int fd = 0;
static __thread int tsize = BASE * 16;
static __thread uint32_t* mem = NULL;
static __thread char file[255] = { 0 };

int erm_close(erm_t erm)
{
  printf("Deregistering erm instance\n"); // TRAP_NOTIFY
  TRAP_WERR(munmap(mem, tsize), "Couldn't unmap shared memory");
  //        TRAP_WERR(shm_unlink(file), "Couldn't close shared memory"); // I
  //        can still delete myself here. Yuck. What can I do about it?
  memset(&file, 0, sizeof(file));
  fd = 0;
  mem = NULL;
  tsize = BASE * 16;
  printf("erm instance detached\n"); // Hmm. I'm not seeing this on atexit
                                     // aborted with cntrl-c
  return 0;
}

void bye(void) { erm_close(mem); }

erm_t erm_attach_client(char* instance)
{
  TRAP_LT((fd = shm_open(instance, O_RDONLY, 0)), 0,
          "Couldn't open shared memory - aborting");
  // struct stats_t;
  // Hmm. Can we get the size from stat?
  TRAP_WEQ((mem = mmap(NULL, BASE * 16, PROT_READ, default_perms, fd, 0)),
           (void*)-1, "Couldn't mmap shared huge page memory");
  if(mem == (void*)-1) {
    default_perms &= ~MAP_HUGETLB;
    TRAP_EQ((mem = mmap(NULL, BASE * 16, PROT_READ, default_perms, fd, 0)),
            (void*)-1, "Couldn't mmap shared memory - aborting");
  }
  atexit(bye);
  strncpy(file, instance, strlen(instance));
  return mem;
}

uint32_t erm_status(erm_t erm) { return mem[8]; }

erm_t* erm_query(erm_t erm, uint32_t* buf, int size) { return &mem[9]; }

#ifdef DEBUG_MODULE
#define MYMEM "/tabeld-test3"
int main()
{
  erm_t e;
  TRAP_WEQ(e = erm_attach_client(MYMEM), (void*)-1,
           "Couldn't attach to erm instance");
  printf("success! reading from engine...\n");
  for(int i = 60; i > 0; i--) {
    printf("erm_status: %d\n", erm_status(e));
    usleep(555555);
  }
  exit(0);
}

#endif
