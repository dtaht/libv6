/**
 * interfaces.c
 *
 * Dave Taht
 * 2017-03-15
 */

#include "interfaces.h"
#include "c11threads.h"
#include "debug.h"
#include "shared.h"
#include "tabeld.h"

extern interface_t ifptable[];

// __thread is a way better name than thread_local

__thread interface_t ifp = { 0 };
__thread int fds = 0;
__thread int notfailed = 0;

int interface_handler(uint16_t offset)
{
  ifp = ifptable[offset];
  //       int fds = construct_fds(ifp.ifacefd,ifp.v4routefd,ifp.v6routefd,
  //       msg,
  //       timer);
  do {
    //		select(fds,bla,bla,bla);
    // do_stuff();
    YIELD(0);
  } while(notfailed == 0);

  return (notfailed);
}

#ifdef DEBUG_MODULE
interface_t ifptable[8];
MFIXME
#endif
