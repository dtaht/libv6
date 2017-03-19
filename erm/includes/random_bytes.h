/**
 * random_bytes.h
 *
 * Dave Taht
 * 2017-03-15
 */

#ifndef RANDOM_BYTES_H
#define RANDOM_BYTES_H

// FIXME: do better PNRG
// For most of babel's current purposes that I can think of,
// just randomize off the cycle counter and pnrg
// off of that. This could be better than it is but
// we only call this once, currently, for 8 bytes....
// and we have an issue in that get_cycles is not always defined
// and is (sigh) a system call on arm.

#ifdef __arm__
int
get_random_bytes(void *buf, int len)
{
    int fd, rc;
    if((fd = open("/dev/urandom", O_RDONLY)) < 0) return -1;
    if((rc = read(fd, buf, len)) < len) rc = -1;
    close(fd);
    return rc;
}
#else
#include "get_cycles.h"
#warning FIXME do better than get_cycles for read_random
int
get_random_bytes(void *buf, int len)
{
      cycles_t *buf2 = buf;
      int len2 = sizeof(cycles_t)/len;
      for(int i = 0; i < len2 ; i++) {
           buf2[i] = get_cycles();
      }
    return 0;
}

#endif

#endif
