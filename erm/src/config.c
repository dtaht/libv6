/**
 * config.c
 *
 * Dave Taht
 * 2017-03-13
 */
#include "shared.h"

#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <regex.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"

#ifndef MAP_POPULATE
// OSX doesn't support MAP_POPULATE
#define MAP_POPULATE 0
#endif

/* get_file expects the filename to be filled out.
   An incoming fd overrides the filename.
   if you want it mapped to a specific place, assign f.map something other
   than
   NULL.
   The minimum side effect of this call is the size is filled out.
*/

fmap get_file(fmap f)
{
  if(f.fd < 1) {
    if(f.filename != NULL) {
      f.fd = open(f.filename, O_RDONLY);
      if(f.fd < 1) {
        fprintf(stderr, "Cannot open config file %s\n", f.filename);
      }
    }
  }
  struct stat stats;
  fstat(f.fd, &stats);
  f.size = stats.st_size;
  if(f.size > 0) {
    f.map = mmap(f.map, f.size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, f.fd, 0);
    if(f.map == MAP_FAILED) f.size = 0;
  }
  return f;
}

fmap close_file(fmap f)
{
  if(f.map) munmap(f.map, f.size);
  if(f.fd) close(f.fd);
  return f; // fixme do more
}

int main()
{
  int count;
  fmap f = { 0 };

  f.filename = "/etc/hosts";

  f = get_file(f);

  if(f.size) {
    char* data = f.map;
    for(int i = 0; i < f.size; i++) {
      if(data[i] == '\n') count++;
    }
  }
  printf("lines in /etc/hosts are: %d\n", count);
  return 0;
}
