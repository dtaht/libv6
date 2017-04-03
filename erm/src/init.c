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

ip_addr* addresses;
addrflags_t* addrdata;

//static int default_perms = (MAP_SHARED | MAP_HUGETLB);
static int default_perms = (MAP_SHARED | MAP_HUGETLB | MAP_ANONYMOUS);

#define MACHINE_MEMORY (1024*1024) // massive overallocation
#define MACHINE_LOC NULL // stick it anywhere for now (not the case for parallella)
#define ERM_GROUP 84

void* place_tables(void* mem)
{
  // do some mmap magic here
  return mem;
}

// FIXME - add interfaces and kill calloc as we know it

bool load_tables(void* mem)
{
  addrs.data = calloc(BASE, sizeof(ip_addr));
  addrdatas.data = calloc(BASE, sizeof(addrflags_t));
  routes.data = calloc(BASE, sizeof(routes_t));
  sources.data = calloc(BASE, sizeof(sources_t));
  routers.data = calloc(NUM_ROUTERS, sizeof(routers_t));
  addresses = (ip_addr*)addrs.data;
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

#define perms (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)
#define dperms (perms | S_IRWXG | S_IXUSR | S_IRWXO)

typedef struct {
  char* name;
  int mode;
} dirs_t;

dirs_t default_dirs[] = {
  { "cpu", dperms },        { "self", dperms },    { "rules", dperms },
  { "interfaces", dperms }, { "routes", dperms },  { "addresses", dperms },
  { "daemons", dperms },    { "stats", dperms },   { "formats", dperms },
  { "ocpu", dperms },       { "machine", dperms }, { NULL, 0 },
};

dirs_t topfiles[] = {
  { "flags", perms },  { "mach", dperms }, { "arch", perms },
  { "memory", perms }, { NULL, 0 },
};

dirs_t ddirs[] = {
  { "static", dperms }, { "boot", dperms },    { "kernel", dperms },
  { "tabeld", dperms }, { "odhcpd", dperms },  { "dnsmasq", dperms },
  { "udhcp", dperms },  { "udhcp6c", dperms }, { "hnet", dperms },
  { NULL, 0 },
};

int create_files(char* base, dirs_t* p)
{
  char buf[255];
  int fd;
  while(p->name != NULL) {
    sprintf(buf, "%s/%s", base, p->name);
    TRAP_WEQ((fd = open(buf, O_CREAT, p->mode)), -1, buf);
    fchown(fd,-1,ERM_GROUP);
    p++;
    close(fd);
  }
  return 0;
}

int create_dirs(char* base, dirs_t* p)
{
  char buf[255];
  while(p->name != NULL) {
    sprintf(buf, "%s/%s", base, p->name);
    TRAP_WEQ(mkdir(buf, p->mode), -1, buf);
    p++;
  }
  return 0;
}

int create_dirnum(char* base, dirs_t* files, int mode, int count)
{
  char buf[255];
  while(--count > -1) {
    sprintf(buf, "%s/%d", base, count);
    TRAP_WEQ(mkdir(buf, mode), -1, buf);
    if(errno && errno != EEXIST) create_files(buf, topfiles);
  }
  return 0;
}


int create_default_dirs(char* instance)
{
  char buf[1024];
  TRAP_WEQ(mkdir(instance, dperms), -1, instance);
  create_dirs(instance, default_dirs);
  create_files(instance, topfiles);
  sprintf(buf, "%s/cpu", instance);
  printf("creating cpu %s\n", buf);
  create_dirnum(buf, topfiles, perms, 4);
  sprintf(buf, "%s/ocpu", instance);
  printf("creating ocpu %s\n", buf);
  create_dirnum(buf, topfiles, perms, 16);
  sprintf(buf, "%s/daemons", instance);
  printf("creating daemons %s\n", buf);
  create_dirs(buf, ddirs);
  sprintf(buf, "%s/routes", instance);
  printf("creating routes %s\n", buf);
  create_dirs(buf, ddirs);
  return 0;
}

int erm_fs(char *instance) {
	return create_default_dirs(instance);
}

void * erm_start(char * shmem) {
  char buf[255];
  u32 *mem;
  u32 memsize;

  sprintf(buf, ERM_SHARED_DIR_PATTERN, shmem);
  TRAP_WERR(setegid(ERM_GROUP), "Can't switch to erm group");
  erm_fs(buf);

  sprintf(buf, ERM_SHARED_MEM_PATTERN, shmem);
  int fd = shm_open(buf, O_CREAT | O_RDWR, 0);
  if(fd < 0) {
    if((fd = shm_open(buf, O_RDWR, 0)) == -1) {
      perror("Couldn't attach shared memory");
      goto err;
    }
    struct stat s;
    fstat(fd,&s);
    memsize = s.st_size;
  } else {
	  memsize = ftruncate(fd, MACHINE_MEMORY); // allocate a mb
  }
  printf("Machine Attached: %s\n", buf);

  TRAP_WERR((fchmod(fd, perms)),
            "Couldn't change shared memory mode"); // rw root, r group
  TRAP_WERR((fchown(fd, -1, ERM_GROUP)),
            "Couldn't change shared memory group");
  TRAP_WEQ((mem = mmap(MACHINE_LOC, MACHINE_MEMORY, PROT_READ | PROT_WRITE, default_perms, fd, 0)),
           (void*)-1, "Couldn't mmap shared huge page memory");
  if(mem == (void*)-1) {
    default_perms &= ~MAP_HUGETLB;
    TRAP_EQ((mem = mmap(MACHINE_LOC, MACHINE_MEMORY, PROT_READ | PROT_WRITE, default_perms, fd, 0)),
            (void*)-1, "Couldn't mmap shared memory - aborting");
  }
  return mem;

err:
  TRAP_WERR(munmap(mem, MACHINE_MEMORY), "Couldn't unmap shared memory");
  sprintf(buf, ERM_SHARED_DIR_PATTERN, shmem);
  TRAP_WERR(shm_unlink(buf), "Couldn't close shared memory");
  printf("exiting\n");
  return NULL;
}

#ifdef DEBUG_MODULE
#define MYMEM "/erm-test"

int main(int argc, char** argv)
{
  char* shmem = argc == 2 ? argv[1] : MYMEM;
  int fd;
  unsigned char* tables = NULL;
  u32* mem = erm_start(shmem);

  tables = place_tables(mem);
  load_tables(mem);
  fill_tables(mem);
  printf("Machine initialized: !\n");
  for(int i = 60; i > 0; --i) {
    mem[8] = i;
    usleep(333333);
  }
  mem[8] = 0;

}
#endif
