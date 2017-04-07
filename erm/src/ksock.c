/**
 * ksocket.c
 *
 * Dave Taht
 * 2017-03-15
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/route.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_bridge.h>
#include <linux/fib_rules.h>
#include <net/if_arp.h>

#include "preprocessor.h"
#include "erm_types.h"
#include "simd.h"

#include "linux_interface.h"

int socks[] = { RTNLGRP_IPV6_ROUTE, RTNLGRP_IPV6_IFADDR,
		RTNLGRP_IPV4_ROUTE, RTNLGRP_IPV4_IFADDR,
		RTNLGRP_LINK } ; // ,RTNLGRP_RULE };

// The kernel keeps separate tables for everything.
// We do the same to reduce head of line blocking

static int
netlink_socket_create(netlink_socket *nl, uint32_t groups)
{
    int rc;
    int rcvsize = 32 * 1024;

    nl->sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if(nl->sock < 0)
        return -1;

    memset(&nl->sockaddr, 0, sizeof(nl->sockaddr));
    nl->sockaddr.nl_family = AF_NETLINK;
    nl->sockaddr.nl_groups = groups;
    nl->socklen = sizeof(nl->sockaddr);

    nl->seqno = time(NULL);

    rc = fcntl(nl->sock, F_GETFL, 0);
    if(rc < 0)
        goto fail;

    rc = fcntl(nl->sock, F_SETFL, (rc | O_NONBLOCK));
    if(rc < 0)
        goto fail;

#ifdef SO_RCVBUFFORCE
    rc = setsockopt(nl->sock, SOL_SOCKET, SO_RCVBUFFORCE,
                    &rcvsize, sizeof(rcvsize));
#else
    rc = -1;
#endif
    if(rc < 0) {
        rc = setsockopt(nl->sock, SOL_SOCKET, SO_RCVBUF,
                        &rcvsize, sizeof(rcvsize));
        if(rc < 0) {
            perror("setsockopt(SO_RCVBUF)");
        }
    }

    rc = bind(nl->sock, (struct sockaddr *)&nl->sockaddr, nl->socklen);
    if(rc < 0)
        goto fail;

    rc = getsockname(nl->sock, (struct sockaddr *)&nl->sockaddr, &nl->socklen);
    if(rc < 0)
        goto fail;

    return 0;

 fail:
    {
        int saved_errno = errno;
        close(nl->sock);
        nl->sock = -1;
        errno = saved_errno;
        return -1;
    }
}

kernel_sockets kernel_socket_teardown(kernel_sockets k)
{
  for(int i = 6 ;i>0; i--) {
    close(k.sockets[i-1].sock);
    k.sockets[i].sock = 0;
  }
  k.status= 0;
  return k;  
}

kernel_sockets kernel_socket_setup(kernel_sockets k)
{
  int rc = 0;
  for(int i = 0; i < 4; i++) {
    if((rc = netlink_socket_create(&k.sockets[i],socks[i]) > 0)) {
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

void kernel_dump_v4(kernel_sockets k)
{

}

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

register u32 v4_stuff asm("xmm9") VECTOR(16) ;
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
      route2.a[3] |= rtm->rtm_dst_len << 16 ;
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
  tp |= ((u64) rtm->rtm_protocol) << 32;

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case RTA_DST:
      sl = rtm->rtm_dst_len << 16 | (sl &  0xFFFFFFFFLL);
      sd = *(int*)RTA_DATA(rta) | (  sd & ~0xFFFFFFFFLL);
      break;
    case RTA_SRC:
      sl = rtm->rtm_src_len | (sl &                     ~0xFFFFFFFFLL);
      sd = (((u64) *(int*)RTA_DATA(rta)) << 32) | (sd & ~0xFFFFFFFFLL);
      break;
    case RTA_GATEWAY:
      ls = *(int*)RTA_DATA(rta) | (ls &                 ~0xFFFFFFFFLL);
      break;
    case RTA_OIF:
      ls = (((u64) *(int*)RTA_DATA(rta)) << 32) | (ls & 0xFFFFFFFFLL);
      break;
    case RTA_PRIORITY:
      tp = ((u64) *(int*)RTA_DATA(rta)) << 32 | (tp & 0xFFFFFFFFLL);
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

u32 onemasks[5][4] =
  {
  { -1, -1, -1, -1 },
  { 0, 0, 0, -1 },
  { 0, 0, -1, 0 },
  { 0, -1, 0, 0 },
  { -1, 0, 0, 0 }
};

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

register u32 v4_stuff asm("q10") VECTOR(16) ;
register u32 v4_stuff2 asm("q11") VECTOR(16);

void parse_kernel_route4_neon(struct rtmsg* rtm, int len)
{
  usimd addrs = {0};
  usimd metric = {0};

  len -= NLMSG_ALIGN(sizeof(*rtm));
  struct rtattr* rta = RTM_RTA(rtm);

  vld1q_lane_s32(&rtm->rtm_table, metric,1);
  vld1q_lane_s32(&rtm->rtm_protocol, metric,2);

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case RTA_DST:
      vld1q_lane_s16(&rtm->rtm_dst_len,addrs,0);
      vld1q_lane_s32(*(int*)RTA_DATA(rta),addrs,0);
      break;
    case RTA_SRC:
      vld1q_lane_s16(&rtm->rtm_src_len,addrs,1);
      vld1q_lane_s32(*(int*)RTA_DATA(rta),addrs,1);
      break;
    case RTA_GATEWAY:
      vld1q_lane_s32(*(int*)RTA_DATA(rta),addrs,2);
      break;
    case RTA_OIF:
      vld1q_lane_s32(*(int*)RTA_DATA(rta),addrs,3);
      break;
    case RTA_PRIORITY:
      vld1q_lane_s32(*(int*)RTA_DATA(rta),metric,0);
      break;
    case RTA_TABLE:
      vld1q_lane_s32(*(int*)RTA_DATA(rta),metric,1);
      break;
    default:
      break;
    }
    rta = RTA_NEXT(rta, len);
  }
  v4_stuff = addrs;
  v4_stuff2 = addrs;
}

#endif

#ifdef DEBUG_MODULE
int main() {
  kernel_sockets k = {0};
  k = kernel_socket_setup(k);
  if(k.status > 0) {
    printf("Getting dumps\n");
    return 0;
  }
  return -1;
}
#endif
