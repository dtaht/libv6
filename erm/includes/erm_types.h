/**
 * erm_types.h
 *
 * Dave Taht
 * 2017-03-11
 */

#ifndef ERM_TYPES_H
#define ERM_TYPES_H

#include <stdint.h>
#include "align.h"
#include "erm_utypes.h"

#ifndef INFINITY
#define INFINITY (0xFFFF)
#endif

// All addresses are kept as vector types to start with

typedef u8 ip_addr VECTOR(16); // v6 addresses
typedef u8 r6addr_t VECTOR(8); // Router ID
typedef u8 emacaddr_t VECTOR(8); // We waste 2 bytes on macs

// I would like to vectorize ipv4 but am not sure how to express it cleanly
// typedef u8 v4addr_t VECTOR(4); // v6 addresses

typedef u32 v4addr_t;

/* There is a hard limit of 64k anything in the system presently,
   but we can change that if we need to and/or fire off another
   vm to split it */

typedef u16 ubase_t;

typedef ubase_t address_t;
typedef ubase_t neigh_t;
typedef ubase_t route_t;
typedef ubase_t rid_t; /* router id */
typedef ubase_t seqno_t;
typedef ubase_t macaddr_t;

typedef u16 ifindex_t; // the spec says this is an int
typedef u8 proto_t;    // We pee on 255 for special stuff

typedef u8 plen_t;
typedef u8 popcnt_t;

typedef u8 gc_t; // Garbage collection could be 4 bits

typedef u32 ttime_t; // time in usec

// this is a bird-ism

typedef struct net_addr {
  u8 type;
  u8 pxlen;
  u16 length;
  u8 data[16];
  u64 align[0];
} net_addr;

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
  u16 mcast : 1;
  u16 ll : 1;
  u16 martian : 1;
  u16 v4 : 1;
  u16 v6 : 1;

  u16 aggregating : 2;

  // This idea is around the concept of both
  // aggregating routes and watching for potential changes ahead
  // We only have 4 states:
  // 3 = stable. 10 = stablizing 01 = going unstable 00 = unstable
  // or the reverse 0s here. Dunno

  u16 stability : 2;

  // I could put in more state here - nexthop, dest, src...
  // hmm lets do that
  // and see if that helps the GC phase. Inverting the
  // sense of the logic saves on some initialization maybe
  // An address that has none of these bits unset is nearly
  // dead.

  u16 dead : 1; // Do the next bits suffice for GC
  u16 notspecific : 1;
  u16 notnexthop : 1;
  u16 notsrc : 1;
  u16 notdst : 1;
  u16 notaddress : 1;

} addrflags_t;

// However we do need to parse it on some occasions and/or pass it around
// in registers

typedef struct {
  ip_addr address;
  addrflags_t flags;
  s32 special;
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
  u32 debug : 2;
  u32 stubby : 1;
  u32 link_detect : 1;
  u32 all_wireless : 1;
  u32 has_ipv6_subtrees : 1;
  u32 ipv4 : 1;
  u32 ipv6 : 1;
  u32 do_daemonise : 1;
  u32 skip_kernel_setup : 1;
  u32 iff_running : 1;
  u32 have_id : 1;
  u32 random_id : 1;
  u32 deoptimize_wired : 1;
  u32 split_horizon : 1;
  u32 config_finalized : 1;
  u32 monitor : 1;
  u32 flush_invisible : 1;
  u32 use_unicast : 1;
  u32 use_unicast_ihu : 1;
  u32 default_metric : 2;
  u32 diversity : 2;

  // I might regret this as it changes

  u32 kernel_routes_changed : 1;
  u32 kernel_rules_changed : 1;
  u32 kernel_link_changed : 1;
  u32 kernel_addr_changed : 1;

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
