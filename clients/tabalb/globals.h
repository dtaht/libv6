/**
 * globals.h
 *
 * Dave Taht
 * 2017-03-11
 */

#ifndef GLOBALS_H
#define GLOBALS_H

struct globals {
	address_indx;
	address_size,
	addr_cache *addrs;
	addr_cache *routes;
};

register globals *g asm("r2");

#define mem(a) (g->a[a])

// No use offsets

// address = mem(address_idx);

#endif
