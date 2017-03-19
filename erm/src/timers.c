/**
 * timers.c
 *
 * Dave Taht
 * 2017-03-18
 */

#include "timers.h"
#include <sys/timerfd.h>

// Screw that. Just punt it all to the kernel

timerfd_create(int clockid, int flags);

int timerfd_settime(int fd, int flags, const struct itimerspec* new_value, struct itimerspec* old_value);

int timerfd_gettime(int fd, struct itimerspec* curr_value);

// I love fdtimers

fdtimer_create()

timeval_min(&tv, &check_interfaces_timeout);
timeval_min_sec(&tv, expiry_time);
timeval_min_sec(&tv, source_expiry_time);
timeval_min_sec(&tv, kernel_dump_time);
timeval_min(&tv, &resend_time);

timeval_min(&tv, &ifp->flush_timeout);
timeval_min(&tv, &ifp->hello_timeout);
timeval_min(&tv, &ifp->update_timeout);
timeval_min(&tv, &ifp->update_flush_timeout);

if(timeval_compare(&check_neighbours_timeout, &now) < 0) {
  int msecs;
  msecs = check_neighbours();
  /* Multiply by 3/2 to allow neighbours to expire. */
  msecs = MAX(3 * msecs / 2, 10);
  schedule_neighbours_check(msecs, 1);
}

if(timeval_compare(&check_interfaces_timeout, &now) < 0) {
  check_interfaces();
  schedule_interfaces_check(30000, 1);
}

int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);
