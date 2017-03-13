/**
 * tabeld.h
 *
 * Dave Taht
 * 2017-03-11
 */

#ifndef TABELD_H
#define TABELD_H

#include <stdint.h>
#include "align.h"
#include "simd.h"

#ifndef INFINITY
#define INFINITY (0xFFFF)
#endif

// FIXME

#define CACHELINE_ALIGN

#define MIN(a,b) (a > b ? a : b)

/*
  24 = sizeof (filter_result)
 112 = sizeof (filter)
  44 = sizeof (buffered_update)
  56 = sizeof (interface_conf)
 272 = sizeof (interface)
  68 = sizeof (kernel_route)
  28 = sizeof (kernel_rule)
  64 = sizeof (kernel_filter)
  24 = sizeof (local_socket)
  88 = sizeof (resend)

 I don't understand why these are so different

 120 = sizeof (neighbour)
  80 = sizeof (babel_route)
  56 = sizeof (source)
  44 = sizeof (xroute)

  24 = sizeof (zone)

What we are doing here is extremely similar to the zone concept
but applied everywhere.

*/

// All addresses are kept as vector types to start with

typedef uint8_t v6addr_t __attribute__ ((vector_size(16))); // v6 addresses
typedef uint8_t r6addr_t __attribute__ ((vector_size(8)));  // Router ID

/* There is a hard limit of 64k anything in the system presently,
   but we can change that if we need to */

typedef uint16_t ubase_t;

typedef ubase_t address_t;
typedef ubase_t neigh_t;
typedef ubase_t route_t;
typedef ubase_t rid_t;   /* router id */
typedef ubase_t seqno_t;

typedef uint16_t ifindex_t; // the spec says this is an int
typedef uint8_t proto_t;    // We pee on 255 for special stuff

typedef uint8_t plen_t;
typedef uint8_t popcnt_t;
typedef ubase_t gc_t; // Garbage collection (arguably could be 8)

typedef uint32_t ttime_t; // time in usec
// typedef ttable * address_p;

/* Unified metric table too, expressable as a vector */

typedef struct  {
    ubase_t ref;
    ubase_t cost;
    ubase_t add;
    ubase_t pad; // 0 ? infinity?
} metric_t;

/* martian detector needs to pass a much larger struct on the stack,
   and can further classify things from there */

// address_configure

/* struct {
	union {
		vec a;
	}
}
*/

// A given address has these characteristics
// and is kept in parallel with another table
// of the actual address itself. This gives
// pleasing structure alignment

typedef struct {
	plen_t len;
	popcnt_t pop;
	uint16_t mcast:1;
	uint16_t ll:1;
	uint16_t martian:1;
	uint16_t v4:1;
	uint16_t v6:1;
	uint16_t aggregated:1;
} addrflags_t;

// However we do need to parse it on some occasions and/or pass it around
// in registers

typedef struct {
	int32_t pad;
	addrflags_t flags;
	v6addr_t address;
} fulladdr_t;

// The original source table was 80 bytes
// This is 16

typedef struct {
	rid_t id;
	address_t src_prefix;
	address_t prefix;
        seqno_t seqno;
        ubase_t metric;
        ubase_t route_count; // wtf is this for?
        ttime_t time; // gc instead?
//	gc_t gc;
} sources_t;

typedef struct {
    rid_t id;
    address_t src_prefix;
    address_t prefix;
    unsigned char pad[2];
} buffered_update;

typedef struct {
	uint32_t debug:2;
	uint32_t stubby:1;
	uint32_t link_detect:1;
	uint32_t all_wireless:1;
	uint32_t has_ipv6_subtrees:1;
	uint32_t do_daemonise:1;
	uint32_t skip_kernel_setup:1;
	uint32_t have_id:1;
	uint32_t random_id:1;
	uint32_t config_finalized:1;
	uint32_t use_unicast_hello:1;
	uint32_t use_unicast_ihu:1;
	uint32_t default_metric:2;

// I might regret this as it changes

	uint32_t kernel_routes_changed:1;
	uint32_t kernel_rules_changed:1;
        uint32_t kernel_link_changed:1;
	uint32_t kernel_addr_changed:1;

	// I could see adding - compute_bloated, discarding, GC or
	// other states here. Maybe.

} global_flags;

// I don't know why xroute and babel_route are different.
// The are not going to be, for me

// parse_packet bugs me

// a src_prefix plen of 255 means this is not a src specific
// route. As an optimization this basically just is 0 for the
// the offset table

typedef struct {
    address_t src_prefix;
    address_t prefix;
    address_t dest;
    ifindex_t ifindex;
    metric_t metric; // FIXME metric is a short here?
    proto_t proto;
} routes_t CACHELINE_ALIGN;

// I like the idea of somehow vectorizing the route metric
// calculation

/* Horizontal add ? */

static inline int
route_metric(const metric_t metric)
{
    int m = (int)metric.ref + metric.cost + metric.add;
    return MIN(m, INFINITY);
}

// Some unconverted structures
// A neighbour is a router. It's shorter to type

typedef struct {
    rid_t id;
    metric_t m;
    neigh_t neigh;  //
    ubase_t nexthop_idx;
    time_t time;
    int expires;
    ubase_t hold_time;    /* in seconds */
    ubase_t smoothed_metric; /* for route selection */
    time_t smoothed_metric_time;
    short installed;
    short channels_len;
    unsigned char *channels;
} routers_t CACHELINE_ALIGN;

#endif
