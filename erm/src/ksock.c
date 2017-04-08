/**
 * ksocket.c
 *
 * Dave Taht
 * 2017-03-15
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <asm/types.h>
#include <linux/fib_rules.h>
#include <linux/if_bridge.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if_arp.h>
#include <sys/socket.h>

#include "preprocessor.h"
#include "erm_types.h"
#include "simd.h"

#include "linux_interface.h"

int socks[] = { RTNLGRP_IPV6_ROUTE, RTNLGRP_IPV6_IFADDR, RTNLGRP_IPV4_ROUTE,
                RTNLGRP_IPV4_IFADDR, RTNLGRP_LINK }; // ,RTNLGRP_RULE };

// The kernel keeps separate tables for everything.
// We do the same to reduce head of line blocking

static int netlink_socket_create(netlink_socket* nl, uint32_t groups)
{
  int rc;
  int rcvsize = 32 * 1024;

  nl->sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if(nl->sock < 0) return -1;

  memset(&nl->sockaddr, 0, sizeof(nl->sockaddr));
  nl->sockaddr.nl_family = AF_NETLINK;
  nl->sockaddr.nl_groups = groups;
  nl->socklen = sizeof(nl->sockaddr);

  nl->seqno = time(NULL);

  rc = fcntl(nl->sock, F_GETFL, 0);
  if(rc < 0) goto fail;

  rc = fcntl(nl->sock, F_SETFL, (rc | O_NONBLOCK));
  if(rc < 0) goto fail;

#ifdef SO_RCVBUFFORCE
  rc = setsockopt(nl->sock, SOL_SOCKET, SO_RCVBUFFORCE, &rcvsize, sizeof(rcvsize));
#else
  rc = -1;
#endif
  if(rc < 0) {
    rc = setsockopt(nl->sock, SOL_SOCKET, SO_RCVBUF, &rcvsize, sizeof(rcvsize));
    if(rc < 0) {
      perror("setsockopt(SO_RCVBUF)");
    }
  }

  rc = bind(nl->sock, (struct sockaddr*)&nl->sockaddr, nl->socklen);
  if(rc < 0) goto fail;

  rc = getsockname(nl->sock, (struct sockaddr*)&nl->sockaddr, &nl->socklen);
  if(rc < 0) goto fail;

  return 0;

fail : {
  int saved_errno = errno;
  close(nl->sock);
  nl->sock = -1;
  errno = saved_errno;
  return -1;
}
}

kernel_sockets kernel_socket_teardown(kernel_sockets k)
{
  for(int i = 6; i > 0; i--) {
    close(k.sockets[i - 1].sock);
    k.sockets[i].sock = 0;
  }
  k.status = 0;
  return k;
}

kernel_sockets kernel_socket_setup(kernel_sockets k)
{
  int rc = 0;
  for(int i = 0; i < 4; i++) {
    if((rc = netlink_socket_create(&k.sockets[i], socks[i]) > 0)) {
      k.status |= (1 << i);
    } else {
      perror("netlink_socket failed (_ROUTE | _LINK | _IFADDR | _RULE)");
      k = kernel_socket_teardown(k);
      break;
    }
  }
  return k;
}

struct plens {
  u16 slen;
  u16 dlen;
};

typedef struct {
  u8 flags;
  u8 proto;
  u8 slen;
  u8 dlen;
  u32 via;
  u32 src;
  u32 dst;
  u32 metric;
  u16 table;
  u16 ifindex;
} v4route;

/* area sort_v4_area(area a) */
/* { */
/* } */

void kernel_dump_v4(kernel_sockets k) {}

v4route parse_kernel_route_rta4(struct rtmsg* rtm, int len, v4route route)
{
  route.table = rtm->rtm_table;
  struct rtattr* rta = RTM_RTA(rtm);
  len -= NLMSG_ALIGN(sizeof(*rtm));

  route.proto = rtm->rtm_protocol;

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case RTA_DST:
      route.dlen = rtm->rtm_dst_len;
      route.dst = *(int*)RTA_DATA(rta);
      break;
    case RTA_SRC:
      route.slen = rtm->rtm_src_len;
      route.src = *(int*)RTA_DATA(rta);
      break;
    case RTA_GATEWAY:
      route.via = *(int*)RTA_DATA(rta);
      break;
    case RTA_OIF:
      route.ifindex = *(int*)RTA_DATA(rta);
      break;
    case RTA_PRIORITY:
      route.metric = *(int*)RTA_DATA(rta);
      break;
    case RTA_TABLE:
      route.table = *(int*)RTA_DATA(rta);
      break;
    default:
      break;
    }
    rta = RTA_NEXT(rta, len);
  }
  return route;
}

#ifndef HAVE_NEON

enum { vvia, vsrc, vdst, vplens };

typedef union {
  u32 p VECTOR(32);
  u32 a[8];
} V4;

typedef union {
  u32 p VECTOR(16);
  u32 a[4];
} V21;

register u32 v4_stuff asm("xmm9") VECTOR(16);
register u32 v4_stuff2 asm("xmm10") VECTOR(16);

void parse_kernel_route_vta4(struct rtmsg* rtm, int len)
{
  V21 route;
  V21 route2;
  route2.a[3] = rtm->rtm_table;
  struct rtattr* rta = RTM_RTA(rtm);
  len -= NLMSG_ALIGN(sizeof(*rtm));
  route2.a[2] = rtm->rtm_protocol;

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case RTA_DST:
      route2.a[3] |= rtm->rtm_dst_len << 16;
      route.a[vdst] = *(int*)RTA_DATA(rta);
      break;
    case RTA_SRC:
      route2.a[3] |= rtm->rtm_src_len;
      route.a[vsrc] = *(int*)RTA_DATA(rta);
      break;
    case RTA_GATEWAY:
      route.a[vvia] = *(int*)RTA_DATA(rta);
      break;
    case RTA_OIF:
      route2.a[0] = *(int*)RTA_DATA(rta);
      break;
    case RTA_PRIORITY:
      route2.a[1] = *(int*)RTA_DATA(rta);
      break;
    case RTA_TABLE:
      route2.a[3] = *(int*)RTA_DATA(rta);
      break;
    default:
      break;
    }
    rta = RTA_NEXT(rta, len);
  }
  v4_stuff = route.p;
  v4_stuff2 = route2.p;
}

typedef struct {
  u64 tp;
  u64 sd;
  u64 sl;
  u64 ls;
} f;

static volatile f f1;

void parse_kernel_route_ivta4(struct rtmsg* rtm, int len)
{
  u64 tp = rtm->rtm_table;
  u64 sd = 0;
  u64 sl = 0;
  u64 ls = 0;
  struct rtattr* rta = RTM_RTA(rtm);
  len -= NLMSG_ALIGN(sizeof(*rtm));
  tp |= ((u64)rtm->rtm_protocol) << 32;

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case RTA_DST:
      sl = rtm->rtm_dst_len << 16 | (sl & 0xFFFFFFFFLL);
      sd = *(int*)RTA_DATA(rta) | (sd & ~0xFFFFFFFFLL);
      break;
    case RTA_SRC:
      sl = rtm->rtm_src_len | (sl & ~0xFFFFFFFFLL);
      sd = (((u64) * (int*)RTA_DATA(rta)) << 32) | (sd & ~0xFFFFFFFFLL);
      break;
    case RTA_GATEWAY:
      ls = *(int*)RTA_DATA(rta) | (ls & ~0xFFFFFFFFLL);
      break;
    case RTA_OIF:
      ls = (((u64) * (int*)RTA_DATA(rta)) << 32) | (ls & 0xFFFFFFFFLL);
      break;
    case RTA_PRIORITY:
      tp = ((u64) * (int*)RTA_DATA(rta)) << 32 | (tp & 0xFFFFFFFFLL);
      break;
    case RTA_TABLE:
      tp = *(int*)RTA_DATA(rta) | (tp & ~0xFFFFFFFFLL);
      break;
    default:
      break;
    }
    rta = RTA_NEXT(rta, len);
  }
  f1.ls = ls;
  f1.tp = tp;
  f1.sd = sd;
  f1.sl = sl;
}

u32 onemasks[5][4] = { { -1, -1, -1, -1 },
                       { 0, 0, 0, -1 },
                       { 0, 0, -1, 0 },
                       { 0, -1, 0, 0 },
                       { -1, 0, 0, 0 } };

#endif

#ifdef HAVE_NEON
/*

 What I basically want is to skip enregisterization entirely. loading
 an individual lane directly from the possibly unaligned
 pointer. These are all 32 bit values being folded into two 128 bit
 registers. So I load a temporary in with all other bits
 cleared. andnot or

 loadhi
 loadlow

 OR:

 Hmm. I could just load up 8 128 bit registers and then or them together
 at the end.

 or, in the case of neon, 16 D registers, then to Q registers...

 Nah, I can do it with load lane on neon and *perfectly* fill the pipeline

 */

register volatile u32 v4_stuff asm("q10") VECTOR(16);
register volatile u32 v4_stuff2 asm("q11") VECTOR(16);

// register volatile int32x4_t v4_stuff asm("q10") VECTOR(16) ;
// register volatile int32x4_t v4_stuff2 asm("q11") VECTOR(16);

typedef struct {
  int32x4_t addrs;
  int32x4_t metric;
} ip4_addr;

ip4_addr parse_kernel_route4_neon(struct rtmsg* rtm, int len, ip4_addr a) COLD;

ip4_addr parse_kernel_route4_neon(struct rtmsg* rtm, int len, ip4_addr a)
{

  len -= NLMSG_ALIGN(sizeof(*rtm));
  struct rtattr* rta = RTM_RTA(rtm);

  a.metric = vld1q_lane_s32((int*)&rtm->rtm_protocol, a.metric, 1);
  a.metric = vld1q_lane_s32((int*)&rtm->rtm_table, a.metric, 3);

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case RTA_DST:
      a.metric = vld1q_lane_s16((short*)&rtm->rtm_dst_len, a.metric, 0);
      a.addrs = vld1q_lane_s32(RTA_DATA(rta), a.addrs, 0);
      break;
    case RTA_SRC:
      a.metric = vld1q_lane_s16((short*)&rtm->rtm_src_len, a.metric, 1);
      a.addrs = vld1q_lane_s32(RTA_DATA(rta), a.addrs, 1);
      break;
    case RTA_GATEWAY:
      a.addrs = vld1q_lane_s32(RTA_DATA(rta), a.addrs, 2);
      break;
    case RTA_OIF:
      a.addrs = vld1q_lane_s32(RTA_DATA(rta), a.addrs, 3);
      break;
    case RTA_PRIORITY:
      a.metric = vld1q_lane_s32(RTA_DATA(rta), a.metric, 2);
      break;
    case RTA_TABLE:
      a.metric = vld1q_lane_s32(RTA_DATA(rta), a.metric, 3);
      break;
    default:
      break;
    }
    rta = RTA_NEXT(rta, len);
  }
  return a;
}

typedef int32x4_t ip6_addr;

typedef struct {
  int32x4_t via;
  int32x4_t src;
  int32x4_t dst;
  int32x4_t metric;
} ip6_route;

// must be aligned

static ip6_addr *addrs6_table = 0;
static int size_6addrs = 0;
static int used_6addrs = 0;

static inline u32 is_not_zero(uint32x4_t v)
{
    uint32x2_t tmp = vorr_u32(vget_low_u32(v), vget_high_u32(v));
    return vget_lane_u32(vpmax_u32(tmp, tmp), 0);
}

#define VEQ(a,b) !(is_not_zero(veorq_u32(a,b)))
#define VNEQ(a,b) (is_not_zero(veorq_u32(a,b)))

// WIP - want to grossly check for inequality

// #define TVNEQ(a,b,c,d) (is_not_zero(vorr_u32((vorr_u32(veorq(a,b),veorq(a,c)),veorq(a,d)))))

// #define PEQ(a,b,c,d) vpmin?

//static inline size_t v6_nequal (const unsigned char *p1,
//				const unsigned char *p2) {
//	uint32x4_t up1 = vld1q_u32((const unsigned int *) p1);
//        uint32x4_t up2 = vld1q_u32((const unsigned int *) p2);
//	return is_not_zero(veorq_u32(up1,up2));
//}

//static inline size_t v6_equal (const unsigned char *p1,
//                                  const unsigned char *p2) {
//	return !v6_nequal(p1,p2);
//}


short brute_insert_v6(ip6_addr a) {
  int i = 0;
  if(addrs6_table == NULL) {
    used_6addrs = 0;
    size_6addrs = 64;
    addrs6_table = calloc(size_6addrs,sizeof(ip6_addr));
  }

  // if only it was this easy
  
  for(; i < used_6addrs && (VNEQ(addrs6_table[i],a)); i++) ;

  if(i == used_6addrs) {
    addrs6_table[i] = a;
    if(++used_6addrs > size_6addrs - 4) {
      ip6_addr *temp = realloc(addrs6_table, 2 * size_6addrs * sizeof(ip6_addr));
      if(temp == NULL) abort() ; // out of memory
      size_6addrs *=2;
      addrs6_table = temp;
    }
  }
  return i;  
}

// enum haves { none, src = 1, dst = 2 , via = 4 };

typedef struct {
  short via;
  short src;
  short dst;
} v6_route_index;

// In the end this will become a merge sort of the list

v6_route_index brute_insert_v6_route(ip6_route a)
{
  int i = 0;
  if(addrs6_table == NULL) {
  used_6addrs = 0;
  size_6addrs = 64;
  addrs6_table = calloc(size_6addrs,sizeof(ip6_addr));
  }

// we can make this more efficient by cutting the compares in the loop
// as we acquire hits and also checking for joint inequality rather
// than equality before backtracking to find equality.
// Maybe.

// switch(haves) {
// case none:
  v6_route_index vr;
  for(; i < used_6addrs; i++) {
      ip6_addr temp = addrs6_table[i];
      if( VEQ(temp,a.src) ) vr.src = i;
      if( VEQ(temp,a.dst) ) vr.dst = i;
      if( VEQ(temp,a.via) ) vr.via = i;
  }

  if(i == used_6addrs) {
    if(++used_6addrs > size_6addrs - 4) {
      ip6_addr *temp = realloc((void *)addrs6_table, 2 * size_6addrs * sizeof(ip6_addr));
      if( temp == NULL) abort() ; // out of memory
      size_6addrs *=2;
      addrs6_table = temp;
    }
   }
  // By definition we cannot overrun the size of the table

  if(vr.via == 0) { addrs6_table[used_6addrs] = a.via; vr.via = used_6addrs++; }
  if(vr.src == 0) { addrs6_table[used_6addrs] = a.src; vr.src = used_6addrs++; }
  if(vr.dst == 0) { addrs6_table[used_6addrs] = a.dst; vr.dst = used_6addrs++; }
  return vr;
}


// thhis does all the work of the parallel compares for me in
// in the compiler, but for no good reason, stashes
// all the vectors on the stack on the way in.

v6_route_index brute2_insert_v6_route(ip6_route a)
{
  int i = 0;
//  Let's live dangerously and assume this was done already
//  if(addrs6_table == NULL) {
//  used_6addrs = 0;
//  size_6addrs = 64;
//  addrs6_table = calloc(size_6addrs,sizeof(ip6_addr));
//  }

// we can make this more efficient by cutting the compares in the loop
// with an unroll
// as we acquire hits and also checking for the most common
// joint inequality rather
// than equality before backtracking to find equality.
// However the compiler IS generating this out of order for me...
// But we're totally not filling the pipeline

// switch(haves) {
// case none:
  int src, dst, via;
  src=dst=via=0;
  for(; i < used_6addrs; i++) {
      v4_stuff = addrs6_table[i]; // compiler STILL insists on hitting the stack
      if( VEQ(a.src,v4_stuff) ) src = i;
      if( VEQ(a.dst,v4_stuff) ) dst = i;
      if( VEQ(a.via,v4_stuff) ) via = i;
  }

//  for(; i < used_6addrs; i++) {
//      if( VEQ(addrs6_table[i],a.src) ) src = i;
//      if( VEQ(addrs6_table[i],a.dst) ) dst = i;
//      if( VEQ(addrs6_table[i],a.via) ) via = i;
//  }

  if(i == used_6addrs) {
    if(++used_6addrs > size_6addrs - 4) {
      ip6_addr *temp2 = realloc((void *)addrs6_table, 2 * size_6addrs * sizeof(ip6_addr));
      if( temp2 == NULL) abort() ; // out of memory
      size_6addrs *=2;
      addrs6_table = temp2;
    }
   }
  // By definition we cannot overrun the size of the table

  if(via == 0) { addrs6_table[used_6addrs] = a.via; via = used_6addrs++; }
  if(src == 0) { addrs6_table[used_6addrs] = a.src; src = used_6addrs++; }
  if(dst == 0) { addrs6_table[used_6addrs] = a.dst; dst = used_6addrs++; }

  v6_route_index vr;
  vr.via = via;
  vr.src = src;
  vr.dst = dst;
  return vr;
}

/*      All the popcount code on the web is built on benchmarking
large streams of values. I just need 3 (via, src, dst).

Pulling the core functions from the github sse-popcount repo I see
stuff that doesn't make sense. The demo code uses up 8 regs for data,
does them all in parallell and tosses the results out to memory, then
uses a lookup table, so... transliterating....

        uint8x16_t t0   = vcntq_u8(a.via);
        t0 = vpaddlq_u8(t0,t0); // long pairwise add
        t0 = vpaddlq_u16(t0,t0); // add the 16s
//        t0 = vpaddlq_u32(t0,t0); // Add the 32s
                                 // great, now I have two 64s
that are essentially 2 pairs of 8 bits wide. And need to do the above
steps yet again... twice more. Or shift them in... hmm: maybe

        uint8x16_t t1   = vcntq_u8(a.src);
        t1 = vpaddlq_u8(t1,t1); // long pairwise add

        t1 = shift_u16(t1,8)
        t1 = vpaddlq_u16(t1,t0); // add the 16s - top t1, bottom t0

        uint8x16_t t2   = vcntq_u8(a.dst);
        t2 = vpaddlq_u8(t2,t2); // long pairwise add
        t2 = shift_u16(t2,16) // don't think I can do this

        t1 = vpaddlq_u32(t2,t1) // but then we can add 32s

and now I have 3 sets of 8 bit values that need to get added together

        t0 = gethi(t1) ; revert to 64 bits
        t0 = vpaddl_u8(t0,t1); 64 bit add

leaving those 3 sums in the bottom half.

My hope was kind of that I'd do popcnt inline rather than at the
end of the routine. I'm under the impression I can issue, like
16 neon instructions....

... which will all basically complete by the time we get around
to the rta routine finishing the loop. Still, basically "free".

And the other thought was that I'd be flipping addresses to little
endian to speed up comparisons later, but I'm no longer sure that
is a win.

Need to write the shortest and longest prefix match routines
I guess.

// Now what?


...

    uint32_t scalar = 0;
    uint32_t tmp[4];

    vst1q_u32(tmp, sum);
    for (int i=0; i < 4; i++) {
        scalar += tmp[i];
    }

    for (size_t j=0; j < k; j++) {
        scalar += lookup8bit[ptr[j]];
    }

    return scalar;


 */

ip6_route parse_kernel_route6_neon(struct rtmsg* rtm, int len, ip6_route a) COLD;

ip6_route parse_kernel_route6_neon(struct rtmsg* rtm, int len, ip6_route a)
{

  len -= NLMSG_ALIGN(sizeof(*rtm));
  struct rtattr* rta = RTM_RTA(rtm);

  // The only thing I can't wedge in right is the protocol
  // Perhaps that could be known apriori?
  
  //  a.metric = vld1q_lane_s32((int*)&rtm->rtm_protocol, a.metric, );

  a.metric = vld1q_lane_s32((int*)&rtm->rtm_table, a.metric, 1);

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case RTA_DST:
      a.metric = vld1q_lane_s16((short*)&rtm->rtm_dst_len, a.metric, 0);
      a.dst = vld1q_s32(RTA_DATA(rta));
      break;
    case RTA_SRC:
      a.metric = vld1q_lane_s16((short*)&rtm->rtm_src_len, a.metric, 1);
      a.src = vld1q_s32(RTA_DATA(rta));
      break;
    case RTA_GATEWAY:
      a.via = vld1q_s32(RTA_DATA(rta));
      break;
    case RTA_OIF:
      a.metric = vld1q_lane_s32(RTA_DATA(rta), a.metric, 3);
      break;
    case RTA_PRIORITY:
      a.metric = vld1q_lane_s32(RTA_DATA(rta), a.metric, 2);
      break;
    case RTA_TABLE:
      a.metric = vld1q_lane_s32(RTA_DATA(rta), a.metric, 1);
      break;
    default:
      break;
    }
    rta = RTA_NEXT(rta, len);
  }
  return a;
}

int32x4_t parse_kernel_route4_neon_old(struct rtmsg* rtm, int len)
{
  int32x4_t addrs = { 0 };
  int32x4_t metric = { 0 };

  len -= NLMSG_ALIGN(sizeof(*rtm));
  struct rtattr* rta = RTM_RTA(rtm);

  metric = vld1q_lane_s32((int*)&rtm->rtm_protocol, metric, 1);
  metric = vld1q_lane_s32((int*)&rtm->rtm_table, metric, 3);

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case RTA_DST:
      metric = vld1q_lane_s16((short*)&rtm->rtm_dst_len, metric, 0);
      addrs = vld1q_lane_s32(RTA_DATA(rta), addrs, 0);
      break;
    case RTA_SRC:
      metric = vld1q_lane_s16((short*)&rtm->rtm_src_len, metric, 1);
      addrs = vld1q_lane_s32(RTA_DATA(rta), addrs, 1);
      break;
    case RTA_GATEWAY:
      addrs = vld1q_lane_s32(RTA_DATA(rta), addrs, 2);
      break;
    case RTA_OIF:
      addrs = vld1q_lane_s32(RTA_DATA(rta), addrs, 3);
      break;
    case RTA_PRIORITY:
      metric = vld1q_lane_s32(RTA_DATA(rta), metric, 2);
      break;
    case RTA_TABLE:
      metric = vld1q_lane_s32(RTA_DATA(rta), metric, 3);
      break;
    default:
      break;
    }
    rta = RTA_NEXT(rta, len);
  }
  v4_stuff = addrs;
  v4_stuff2 = metric;
  return addrs;
}

struct test;

struct test {
  u32 rta_type;
  s32 data;
  struct test* next;
} test;


int32x4_t simpler_kernel_route4_neon(struct test* rtm, int len)
{
  int32x4_t addrs = { 0 };
  int32x4_t metric = { 0 };
  struct test* rta = rtm;
  do {
    switch(rta->rta_type) {
    case RTA_DST:
      //      vld1q_lane_s16((short *)&rtm->rtm_dst_len,metric,0);
      addrs = vld1q_lane_s32(&rta->data, addrs, 0);
      break;
    case RTA_SRC:
      // vld1q_lane_s16((short *)&rtm->rtm_src_len,metric,1);
      addrs = vld1q_lane_s32(&rta->data, addrs, 1);
      break;
    case RTA_GATEWAY:
      addrs = vld1q_lane_s32(&rta->data, addrs, 2);
      break;
    case RTA_OIF:
      addrs = vld1q_lane_s32(&rta->data, addrs, 3);
      break;
    case RTA_PRIORITY:
      metric = vld1q_lane_s32(&rta->data, metric, 2);
      break;
    case RTA_TABLE:
      metric = vld1q_lane_s32(&rta->data, metric, 3);
      break;
    default:
      break;
    }
  } while(rta = rta->next);

  return addrs;
}

#endif

#ifdef DEBUG_MODULE
int main()
{
  kernel_sockets k = { 0 };
  k = kernel_socket_setup(k);
  if(k.status > 0) {
    printf("Getting dumps\n");
    return 0;
  }
  return -1;
}
#endif
