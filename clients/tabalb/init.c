/**
 * init.c
 *
 * Dave Taht
 * 2017-03-12
 */

#include "shared.h"
#include "init.h"
#include "io.h"

// Reasonable defaults for a small system

#define BASE 64
#define NUM_ROUTERS 4
#define NUM_INTERFACES 4

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

v6addr_t *addresses;
addrflags_t *addrdata;


// FIXME - add interfaces and kill calloc as we know it

bool load_tables() {
	addrs.data      = calloc(BASE,sizeof(v6addr_t));
	addrdatas.data  = calloc(BASE,sizeof(addrflags_t));
	routes.data     = calloc(BASE,sizeof(routes_t));
	sources.data    = calloc(BASE,sizeof(sources_t));
	routers.data    = calloc(NUM_ROUTERS,sizeof(routers_t));
        addresses  = (v6addr_t *) addrs.data;
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

bool fill_tables() {
	addrflags_t temp = {0};
	addrflags_t *t = (addrflags_t *) addrdatas.data;
	temp.len = temp.pop = 255; temp.martian = temp.v4 = temp.v6 = true;
	t[addrdatas.idx++] = temp;

// FIXME - add localhost, localhost6, unspec, link local, multicast v4, v6, v4mapped
// my protocol group is 1
	// protocol group ff02:0:0:0:0:0:1:6
	temp.len = temp.pop = 255; temp.martian = temp.v4 = false;
	t[addrdatas.idx++] = temp;

// For conveinence, the first router id in the routerid table is me

	return true;
}

int main() {
	load_tables();
	fill_tables();
}
