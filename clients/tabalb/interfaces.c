/**
 * interfaces.c
 *
 * Dave Taht
 * 2017-03-15
 */

#include "c11threads.h"
#include "shared.h"
#include "tabeld.h"
#include "interfaces.h"
#define YIELD sched_yield()

extern interface_t ifptable[];

// __thread is a way better name that thread_local

__thread interface_t ifp = {0};
__thread int fds = 0;
__thread int notfailed = 0;

int interface_handler(offset) {
         ifp = ifptable[offset];
//       int fds = construct_fds(ifp.ifacefd,ifp.v4routefd,ifp.v6routefd, msg, timer);
	 do {
//		select(fds,bla,bla,bla);
		do_stuff();
		YIELD;
	} while(notfailed == 0);

	return(notfailed);
}

#ifdef DEBUG_MODULE
int main() {

	return 0;
}
#endif
