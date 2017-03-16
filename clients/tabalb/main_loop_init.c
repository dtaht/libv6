/**
 * main_loop_init.c
 *
 * Dave Taht
 * 2017-03-15
 */

#include "main_loop_init.h"

// fixme - need to pass more args or make them available on the stack
// the basic thing here is the "interface", which is defined as "up" somehow in the mask

void startup(int a) {
        mysleep(roughly(a));
        send_hello(ifp);
        send_wildcard_retraction(ifp);
}

void alive(int a) {
        mysleep(roughly(a)); // now could come from mysleep
        gettime(&now);
        send_hello(ifp); // FIXME ifp mutates into thread-local storage
        send_wildcard_retraction(ifp);
        send_self_update(ifp);
        send_request(ifp, NULL, 0, NULL, 0); // send_wildcard_request
        flushupdates(ifp);
        flushbuf(ifp);
}

/* Make some noise so that others notice us, and send retractions in
       case we were restarted recently.

The math here works out the same as the original code, just all run in
parallel. */

void main_loop_init() {
	int num_live_interfaces = get_num_live_interfaces();
	stagger(interfaces,10000 * num_live_interfaces, startup);
	a = stagger_ack(interfaces,10000*num_live_interfaces, alive);
	if(stagger_wait(a,10000*num_live_interfaces * 4) < 0) crashlog(thread_crash);
	exec_main_loop(); // chaining
}
