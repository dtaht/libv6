/**
 * interfaces.c
 *
 * Dave Taht
 * 2017-03-15
 */

#include "interfaces.h"

int interface_handler(offset) {
	__thread interface_t ifp = ifptable[offset];
	__thread int fds = 0;
	__thread int notfailed = 0;
	fds = construct_fds(ifp.ifacefd,ifp.v4routefd,ifp.v6routefd, msg, timer);
	do {
		select(fds,bla,bla,bla);
		do_stuff();
		YIELD;
	} while(notfailed == 0);

	return(notfailed);
}
