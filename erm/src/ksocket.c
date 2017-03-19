/**
 * ksocket.c
 *
 * Dave Taht
 * 2017-03-15
 */

#include "ksocket.h"

// If we aren't doing ipv4 or ipv6, don't do that
union {
};

// The kernel keeps separate tables for everything.
// We do the same to reduce head of line blocking

typedef struct {
  v6route, v6addr, v4route, v4addr, link
} kernel_sockets_t;

kernel_sockets_t kernel_setup_socket(int setup)
{
  int rc = 0;
  kernel_sockets_t k;
  if(setup) {
    rc |= netlink_socket(&k.link, RTNLGRP_LINK);
    rc |= netlink_socket(&k.v6route, RTNLGRP_IPV6_ROUTE);
    rc |= netlink_socket(&k.v6addr, RTNLGRP_IPV6_IFADDR);
    rc |= netlink_socket(&k.v4route, RTNLGRP_IPV4_ROUTE);
    rc |= netlink_socket(&k.v4addr, RTNLGRP_IPV4_IFADDR);

    if(rc < 0) {
      perror("netlink_socket(_ROUTE | _LINK | _IFADDR | _RULE)");
      kernel_socket = -1;
      return -1;
    }

    kernel_socket = nl_listen.sock;

    return 1;

  } else {

    close(nl_listen.sock);
    nl_listen.sock = -1;
    kernel_socket = -1;

    return 1;
  }
}

/* Since we have a socket for v4 and v6, we are less complicated, but we still
 * share code */

static int parse_kernel_route_rta4(struct rtmsg* rtm, int len,
				   struct kernel_route* route)
{
  int table = rtm->rtm_table;
  struct rtattr* rta = RTM_RTA(rtm);
  ;
  int i, is_v4;

  len -= NLMSG_ALIGN(sizeof(*rtm));

  memset(route, 0, sizeof(struct kernel_route));
  if(rtm->rtm_family == AF_INET) {
    /* if RTA_DST is not a TLV, that's a default destination */
    const unsigned char zeroes[4] = { 0, 0, 0, 0 };
    v4tov6(route->prefix, zeroes);
    route->plen = 96;
  }
  route->proto = rtm->rtm_protocol;

  is_v4 = rtm->rtm_family == AF_INET;

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case RTA_DST:
      route->plen = GET_PLEN(rtm->rtm_dst_len, is_v4);
      COPY_ADDR(route->prefix, rta, is_v4);
      break;
    case RTA_SRC:
      route->src_plen = GET_PLEN(rtm->rtm_src_len, is_v4);
      COPY_ADDR(route->src_prefix, rta, is_v4);
      break;
    case RTA_GATEWAY:
      COPY_ADDR(route->gw, rta, is_v4);
      break;
    case RTA_OIF:
      route->ifindex = *(int*)RTA_DATA(rta);
      break;
    case RTA_PRIORITY:
      route->metric = *(int*)RTA_DATA(rta);
      if(route->metric < 0 || route->metric > KERNEL_INFINITY)
        route->metric = KERNEL_INFINITY;
      break;
    case RTA_TABLE:
      table = *(int*)RTA_DATA(rta);
      break;
    default:
      break;
    }
    rta = RTA_NEXT(rta, len);
  }

  for(i = 0; i < import_table_count; i++)
    if(table == import_tables[i]) return 0;

  return -1;
}

static int filter_netlink(struct nlmsghdr* nh, struct kernel_filter* filter)
{
  int rc;
  union {
    struct kernel_route route;
    struct kernel_addr addr;
    struct kernel_link link;
    struct kernel_rule rule;
  } u;

  switch(nh->nlmsg_type) {
  case RTM_NEWROUTE:
  case RTM_DELROUTE:
    if(!filter->route) break;
    rc = filter_kernel_routes(nh, &u.route);
    if(rc <= 0) break;
    return filter->route(&u.route, filter->route_closure);
  case RTM_NEWLINK:
  case RTM_DELLINK:
    if(!filter->link) break;
    rc = filter_link(nh, &u.link);
    if(rc <= 0) break;
    return filter->link(&u.link, filter->link_closure);
  case RTM_NEWADDR:
  case RTM_DELADDR:
    if(!filter->addr) break;
    rc = filter_addresses(nh, &u.addr);
    if(rc <= 0) break;
    return filter->addr(&u.addr, filter->addr_closure);
  case RTM_NEWRULE:
  case RTM_DELRULE:
    if(!filter->rule) break;
    rc = filter_kernel_rules(nh, &u.rule);
    if(rc <= 0) break;
    return filter->rule(&u.rule, filter->rule_closure);
  default:
    kdebugf("filter_netlink: unexpected message type %d\n", nh->nlmsg_type);
    break;
  }
  return 0;
}

int kernel_callback(struct kernel_filter* filter)
{
  int rc;

  kdebugf("\nReceived changes in kernel tables.\n");

  if(nl_listen.sock < 0) {
    rc = kernel_setup_socket(1);
    if(rc < 0) {
      perror("kernel_callback: kernel_setup_socket(1)");
      return -1;
    }
  }
  rc = netlink_read(&nl_listen, &nl_command, 0, filter);

  if(rc < 0 && nl_listen.sock < 0) kernel_setup_socket(1);

  return 0;
}

/* Routing table's rules */

static int filter_kernel_routes(struct nlmsghdr* nh, struct kernel_route* route)
{
  int rc, len;
  struct rtmsg* rtm;

  len = nh->nlmsg_len;

  if(nh->nlmsg_type != RTM_NEWROUTE && nh->nlmsg_type != RTM_DELROUTE)
    return 0;

  rtm = (struct rtmsg*)NLMSG_DATA(nh);
  len -= NLMSG_LENGTH(0);

  if(rtm->rtm_protocol == RTPROT_BABEL) return 0;

  /* Ignore cached routes, advertised by some kernels (linux 3.x). */
  if(rtm->rtm_flags & RTM_F_CLONED) return 0;

  rc = parse_kernel_route_rta(rtm, len, route);
  if(rc < 0) return 0;

  /* Ignore default unreachable routes; no idea where they come from. */
  if(route->plen == 0 && route->metric >= KERNEL_INFINITY) return 0;

  if(debug >= 2) {
    if(rc >= 0) {
      print_kernel_route(nh->nlmsg_type, rtm->rtm_protocol, rtm->rtm_type, route);
    }
  }

  return 1;
}

static int filter_kernel_routes(struct nlmsghdr* nh, struct kernel_route* route)
{
  int rc, len;
  struct rtmsg* rtm;

  len = nh->nlmsg_len;

  if(nh->nlmsg_type != RTM_NEWROUTE && nh->nlmsg_type != RTM_DELROUTE)
    return 0;

  rtm = (struct rtmsg*)NLMSG_DATA(nh);
  len -= NLMSG_LENGTH(0);

  if(rtm->rtm_protocol == RTPROT_BABEL) return 0;

  /* Ignore cached routes, advertised by some kernels (linux 3.x). */
  if(rtm->rtm_flags & RTM_F_CLONED) return 0;

  rc = parse_kernel_route_rta(rtm, len, route);
  if(rc < 0) return 0;

  /* Ignore default unreachable routes; no idea where they come from. */
  if(route->plen == 0 && route->metric >= KERNEL_INFINITY) return 0;

  if(debug >= 2) {
    if(rc >= 0) {
      print_kernel_route(nh->nlmsg_type, rtm->rtm_protocol, rtm->rtm_type, route);
    }
  }

  return 1;
}

/* This function should not return routes installed by us. */
int kernel_dump(int operation, struct kernel_filter* filter)
{
  int i, rc;
  int families[2] = { AF_INET6, AF_INET };
  struct rtgenmsg g;

  if(!nl_setup) {
    fprintf(stderr, "kernel_dump: netlink not initialized.\n");
    errno = EIO;
    return -1;
  }

  if(nl_command.sock < 0) {
    rc = netlink_socket(&nl_command, 0);
    if(rc < 0) {
      int save = errno;
      perror("kernel_dump: netlink_socket()");
      errno = save;
      return -1;
    }
  }

  for(i = 0; i < 2; i++) {
    memset(&g, 0, sizeof(g));
    g.rtgen_family = families[i];
    if(operation & CHANGE_ROUTE) {
      rc = netlink_send_dump(RTM_GETROUTE, &g, sizeof(g));
      if(rc < 0) return -1;

      rc = netlink_read(&nl_command, NULL, 1, filter);
      if(rc < 0) return -1;
    }

    memset(&g, 0, sizeof(g));
    g.rtgen_family = families[i];
    if(operation & CHANGE_RULE) {
      rc = netlink_send_dump(RTM_GETRULE, &g, sizeof(g));
      if(rc < 0) return -1;

      rc = netlink_read(&nl_command, NULL, 1, filter);
      if(rc < 0) return -1;
    }
  }

  if(operation & CHANGE_ADDR) {
    memset(&g, 0, sizeof(g));
    g.rtgen_family = AF_UNSPEC;
    rc = netlink_send_dump(RTM_GETADDR, &g, sizeof(g));
    if(rc < 0) return -1;

    rc = netlink_read(&nl_command, NULL, 1, filter);
    if(rc < 0) return -1;
  }

  return 0;
}

static char* parse_ifname_rta(struct ifinfomsg* info, int len)
{
  struct rtattr* rta = IFLA_RTA(info);
  char* ifname = NULL;

  len -= NLMSG_ALIGN(sizeof(*info));

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case IFLA_IFNAME:
      ifname = RTA_DATA(rta);
      break;
    default:
      break;
    }
    rta = RTA_NEXT(rta, len);
  }
  return ifname;
}

static int parse_addr_rta(struct ifaddrmsg* addr, int len, struct in6_addr* res)
{
  struct rtattr* rta;
  len -= NLMSG_ALIGN(sizeof(*addr));
  rta = IFA_RTA(addr);

  while(RTA_OK(rta, len)) {
    switch(rta->rta_type) {
    case IFA_LOCAL:
    case IFA_ADDRESS:
      switch(addr->ifa_family) {
      case AF_INET:
        if(res) v4tov6(res->s6_addr, RTA_DATA(rta));
        break;
      case AF_INET6:
        if(res) memcpy(res->s6_addr, RTA_DATA(rta), 16);
        break;
      default:
        kdebugf("ifaddr: unexpected address family %d\n", addr->ifa_family);
        return -1;
        break;
      }
      break;
    default:
      break;
    }
    rta = RTA_NEXT(rta, len);
  }
  return 0;
}

static int filter_link(struct nlmsghdr* nh, struct kernel_link* link)
{
  struct ifinfomsg* info;
  int len;
  int ifindex;
  unsigned int ifflags;

  len = nh->nlmsg_len;

  if(nh->nlmsg_type != RTM_NEWLINK && nh->nlmsg_type != RTM_DELLINK) return 0;

  info = (struct ifinfomsg*)NLMSG_DATA(nh);
  len -= NLMSG_LENGTH(0);

  ifindex = info->ifi_index;
  ifflags = info->ifi_flags;

  link->ifname = parse_ifname_rta(info, len);
  if(link->ifname == NULL) return 0;
  kdebugf("filter_interfaces: link change on if %s(%d): 0x%x\n", link->ifname,
          ifindex, (unsigned)ifflags);
  return 1;
}

/* If data is null, takes all addresses.  If data is not null, takes
   either link-local or global addresses depending of the value of
   data[4]. */

static int filter_addresses(struct nlmsghdr* nh, struct kernel_addr* addr)
{
  int rc;
  int len;
  struct ifaddrmsg* ifa;
  char ifname[IFNAMSIZ];

  len = nh->nlmsg_len;

  if(nh->nlmsg_type != RTM_NEWADDR && nh->nlmsg_type != RTM_DELADDR) return 0;

  ifa = (struct ifaddrmsg*)NLMSG_DATA(nh);
  len -= NLMSG_LENGTH(0);

  rc = parse_addr_rta(ifa, len, &addr->addr);
  if(rc < 0) return 0;
  addr->ifindex = ifa->ifa_index;

  kdebugf("found address on interface %s(%d): %s\n",
          if_indextoname(ifa->ifa_index, ifname), ifa->ifa_index,
          format_address(addr->addr.s6_addr));

  return 1;
}

#ifdef DEBUG_MODULE
MFIXME
#endif
