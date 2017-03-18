/**
 * print_global_t.c
 *
 * Dave Taht
 * 2017-03-18
 */

#include <stdio.h>
#include <time.h>
#include "tabeld.h"

// In other abuses of printf, I wonder if I can abuse it to
// let me use captn proto or protobufs? a
// \s flag as a separator instead of \n?

// FIXME:
// Need to define %T and %B as truth values for printf
// maybe use a Tb - boolean Tf flag or some other abuse of printf
// "v is " v.x ? "enabled" : "disabled";
// "v is " v.x ? "true" : "false";
/*
#define TI(v) #v ": %d\n"
#define TF(v) #v ": %B\n"
#define TB(v) #v ": %T\n"

// Just to keep the distinctions clear in my head

#define PI(v) a.v
#define PF(v) a.v
#define PB(v) a.v

PA for address
PP for prefix
M for mac

*/


#define TS(v) #v ": %s\n"
#define TI(v) #v ": %d\n"
#define TF(v) #v ": %s\n"
#define TB(v) #v ": %s\n"

// Just to keep the distinctions clear in my head

#define PI(v) a.v
#define PF(v) a.v ? "enabled" : "disabled"
#define PB(v) a.v ? "true" : "false"

static char * stability(int v) {
	switch(v & 3) {
	case 3: return "stable";
	case 2: return "stabilizing";
	case 1: return "destabilizing";
	default: return "unstable";
	}
}

// I think "folding" more adequately describes merging adjacent routes
// but right now it's "aggregating""

static char * folded(int v) {
	switch(v & 3) {
	case 3: return "folded";
	case 2: return "folding";
	case 1: return "unfolding";
	default: return "split";
	}
}

int human_print_addrflags_t(addrflags_t a) {
	printf(
	        TI(len)
		TI(pop)
		TB(mcast)
		TB(ll)
		TB(martian)
		TB(v4)
		TB(v6)
		TS(aggregating)
		TS(stability)
		TF(dead)
		TF(notspecific)
		TB(notnexthop)
		TB(notsrc)
		TB(notdst)
		TB(notaddress),

	        PI(len),
		PI(pop),
		PB(mcast),
		PB(ll),
		PB(martian),
		PB(v4),
		PB(v6),
		folded(a.aggregating),
		stability(a.stability),
		PF(dead),
		PF(notspecific),
		PB(notnexthop),
		PB(notsrc),
		PB(notdst),
		PB(notaddress)
		);
	return 0;
}


#ifdef DEBUG_MODULE
int main() {
	addrflags_t t = {0};
	human_print_addrflags_t(t);
}
#endif
