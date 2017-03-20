/**
 * tabeld.h
 *
 * Dave Taht
 * 2017-03-11
 */

#ifndef TABELD_H
#define TABELD_H

#include "align.h"
#include <stdint.h>

#ifndef INFINITY
#define INFINITY (0xFFFF)
#endif

// FIXME

#define CACHELINE_ALIGN

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

typedef uint8_t v6addr_t __attribute__((vector_size(16))); // v6 addresses
typedef uint8_t r6addr_t __attribute__((vector_size(8)));  // Router ID
typedef uint8_t emacaddr_t
__attribute__((vector_size(8))); // We waste 2 bytes on macs

// I would like to vectorize ipv4 but am not sure how to express it cleanly
// typedef uint8_t v4addr_t __attribute__ ((vector_size(4))); // v6 addresses

typedef uint32_t v4addr_t;

/* There is a hard limit of 64k anything in the system presently,
   but we can change that if we need to and/or fire off another
   vm to split it */

typedef uint16_t ubase_t;

typedef ubase_t address_t;
typedef ubase_t neigh_t;
typedef ubase_t route_t;
typedef ubase_t rid_t; /* router id */
typedef ubase_t seqno_t;
typedef ubase_t macaddr_t;

typedef uint16_t ifindex_t; // the spec says this is an int
typedef uint8_t proto_t;    // We pee on 255 for special stuff

typedef uint8_t plen_t;
typedef uint8_t popcnt_t;

typedef uint8_t gc_t; // Garbage collection could be 4 bits

typedef uint32_t ttime_t; // time in usec

// typedef ttable * address_p;
/* Unified metric table too, expressable as a vector */

typedef struct {
  ubase_t ref;
  ubase_t cost;
  ubase_t add;
  ubase_t user; // 0 ? infinity?
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
  uint16_t mcast : 1;
  uint16_t ll : 1;
  uint16_t martian : 1;
  uint16_t v4 : 1;
  uint16_t v6 : 1;

  uint16_t aggregating : 2;

  // This idea is around the concept of both
  // aggregating routes and watching for potential changes ahead
  // We only have 4 states:
  // 3 = stable. 10 = stablizing 01 = going unstable 00 = unstable
  // or the reverse 0s here. Dunno

  uint16_t stability : 2;

  // I could put in more state here - nexthop, dest, src...
  // hmm lets do that
  // and see if that helps the GC phase. Inverting the
  // sense of the logic saves on some initialization maybe
  // An address that has none of these bits unset is nearly
  // dead.

  uint16_t dead : 1; // Do the next bits suffice for GC
  uint16_t notspecific : 1;
  uint16_t notnexthop : 1;
  uint16_t notsrc : 1;
  uint16_t notdst : 1;
  uint16_t notaddress : 1;

} addrflags_t;

// However we do need to parse it on some occasions and/or pass it around
// in registers

typedef struct {
  v6addr_t address;
  addrflags_t flags;
  int32_t special;
} fulladdr_t;

// The original source table was 80 bytes
// This is 16 and I begrudge the time and route_count
// fields.

typedef struct {
  rid_t id;
  address_t src_prefix;
  address_t prefix;
  seqno_t seqno;
  ubase_t metric;
  ubase_t route_count; // wtf is this for?
  ttime_t time;        // gc tick instead?
  //	gc_t gc;
} sources_t;

typedef struct {
  rid_t id;
  address_t src_prefix;
  address_t prefix;
  unsigned char pad[2];
} buffered_update;

typedef struct {
  uint32_t debug : 2;
  uint32_t stubby : 1;
  uint32_t link_detect : 1;
  uint32_t all_wireless : 1;
  uint32_t has_ipv6_subtrees : 1;
  uint32_t ipv4 : 1;
  uint32_t ipv6 : 1;
  uint32_t do_daemonise : 1;
  uint32_t skip_kernel_setup : 1;
  uint32_t iff_running : 1;
  uint32_t have_id : 1;
  uint32_t random_id : 1;
  uint32_t deoptimize_wired : 1;
  uint32_t split_horizon : 1;
  uint32_t config_finalized : 1;
  uint32_t monitor : 1;
  uint32_t flush_invisible : 1;
  uint32_t use_unicast : 1;
  uint32_t use_unicast_ihu : 1;
  uint32_t default_metric : 2;
  uint32_t diversity : 2;

  // I might regret this as it changes

  uint32_t kernel_routes_changed : 1;
  uint32_t kernel_rules_changed : 1;
  uint32_t kernel_link_changed : 1;
  uint32_t kernel_addr_changed : 1;

  // I could see adding - compute_bloated, discarding, GC or
  // other states here. Maybe.

} global_flags_t;

// I don't know why xroute and babel_route are different.
// The are not going to be, for me

// parse_packet bugs me

// a src_prefix plen of 255 means this is not a src specific
// route. As an optimization this basically just is at 0 for the
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

/* Horizontal add with saturating arith ? */

static inline int route_metric(const metric_t metric)
{
  int m = (int)metric.ref + metric.cost + metric.add;
  return MIN(m, INFINITY);
}

// Some unconverted structures
// A neighbour is a router. It's shorter to type

typedef struct {
  rid_t id;
  metric_t m;
  neigh_t neigh; //
  ubase_t nexthop_idx;
  ttime_t time;
  int expires;
  ubase_t hold_time;       /* in seconds */
  ubase_t smoothed_metric; /* for route selection */
  ttime_t smoothed_metric_time;
  short installed;
  short channels_len;
  unsigned char* channels;
} routers_t CACHELINE_ALIGN;

#endif
