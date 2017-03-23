/**
 * syscalls_linux.h
 *
 * Dave Taht
 * 2017-03-23
 */

#ifndef SYSCALLS_LINUX_H
#define SYSCALLS_LINUX_H

/* Need to wrap: https://filippo.io/linux-syscall-table/

   sendmmsg, recvmmsg, open, close, mmap, sbrk/calloc, free, abort, atexit, trap
   handler, set/getsockopt, posix_timers, mremap, various sigactions, msync,
   mprotect, read/writeev,shm*, alarm, sendfile, splice - it turned out to be
   quite a lot, and it seems better to just rip stuff out of the c lib to do it
   than do it here. I didn't know that nanosleep was a syscall! Not sure what
   really happens on alarms or if I can just use timerfd universally. It looks
   to be about 80-100 syscalls in total.

   didn't know a good half dozen calls existed - process_vm_rw_pages
*/

#endif
