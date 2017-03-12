/**
 * tabeld.h
 *
 * Dave Taht
 * 2017-03-11
 */

#ifndef TABELD_H
#define TABELD_H

#include <stdint.>
#include <simd.h>

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
*/

typedef uint16_t ubase_t;

typedef ubase_t address_t;
typedef ubase_t rid_t; /* router id */
typedef ubase_t toffset;
typedef ubase_t seqno_t;
typedef uint8_t plen_t;
typedef uint8_t popcnt_t;
typedef uint32_t ttime_t;
typedef ubase_t gc_t; // Garbage collection

typedef ttable * address_p;

/* Unified metric table too, expressable as a vector */

typedef struct  {
    ubase_t ref;
    ubase_t cost;
    ubase_t add;
    ubase_t seqno; // 0 ? infinity?
} metric_t;

/* Horizontal add ? */

// I like the idea of somehow vectorizing the route metric
// calculation
/*

static inline int
route_metric(const struct babel_route *route)
{
    int m = (int)route->refmetric + route->cost + route->add_metric;
    return MIN(m, INFINITY);
}
*/

/* martian detector needs to pass a much larger struct on the stack,
   and can further classify things from there */

// address_configure

/* struct {
	union {
		vec a;
	}
}
*/

//typedef tchar  * taddress;

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
} addrflags_t;

// However we do need to parse it on some occasions and/or pass it around
// in registers

typedef struct {
	addrflags_t flags;
	paddr_t address;
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
} source;

typedef struct {
    rid_t id;
    address_t src_prefix;
    address_t prefix;
    unsigned char pad[2];
} buffered_update;

// I don't know why xroute and babel_route are different
//

struct xroute {
    address_t prefix;
    address_t src_prefix;
    ubase_t metric; // FIXME metric is a short here?
    ifindex_t ifindex;
    proto_t proto;
} CACHELINE_ALIGN;


// Some unconverted structures

struct babel_route {
    struct babel_route *next;
    struct source *src;
    ubase_t refmetric;
    ubase_t cost;
    ubase_t add_metric;
    ubase_t seqno;
    struct neighbour *neigh;
    ubase_t nexthop_idx;
    time_t time;
    int expires;
    ubase_t hold_time;    /* in seconds */
    ubase_t smoothed_metric; /* for route selection */
    time_t smoothed_metric_time;
    short installed;
    short channels_len;
    unsigned char *channels;
} CACHELINE_ALIGN;


#endif
