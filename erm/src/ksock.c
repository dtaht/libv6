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

enum { vvia, vsrc, vdst, vplens };

typedef union {
  u32 p VECTOR(32);
  u32 a[8];
} V4;

register u32 v4_stuff VECTOR(16) asm("xmm9");
register u32 v4_stuff2 VECTOR(16) asm("xmm10");

void parse_kernel_route_vta4(struct rtmsg* rtm, int len)
{
  V4 route;
  route.p = v4_stuff;
  route.a[7] = rtm->rtm_table;
  struct rtattr* rta = RTM_RTA(rtm);
  len -= NLMSG_ALIGN(sizeof(*rtm));
  route.a[6] = rtm->rtm_protocol;

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case RTA_DST:
      route.a[3] |= rtm->rtm_dst_len << 16 ;
      route.a[vdst] = *(int*)RTA_DATA(rta);
      break;
    case RTA_SRC:
      route.a[3] |= rtm->rtm_src_len;
      route.a[vsrc] = *(int*)RTA_DATA(rta);
      break;
    case RTA_GATEWAY:
      route.a[vvia] = *(int*)RTA_DATA(rta);
      break;
    case RTA_OIF:
      route.a[4] = *(int*)RTA_DATA(rta);
      break;
    case RTA_PRIORITY:
      route.a[5] = *(int*)RTA_DATA(rta);
      break;
    case RTA_TABLE:
      route.a[7] = *(int*)RTA_DATA(rta);
      break;
    default:
      break;
    }
    rta = RTA_NEXT(rta, len);
  }
  v4_stuff = route.p;
}


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
