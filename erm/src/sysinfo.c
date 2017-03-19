/**
 * sysinfo.c
 * based on code lifted from the original authors of babel
 * Dave Taht
 * 2017-03-15
 */

#include "sysinfo.h"

int kernel_older_than(const char* sysname, int version, int sub_version)
{
  struct utsname un;
  int rc;
  int v = 0;
  int sub_v = 0;
  if((rc = uname(&un)) < 0 || strcmp(sysname, un.sysname) != 0) return -1;
  if((rc = sscanf(un.release, "%d.%d", &v, &sub_v)) < 2) return -1;
  return (v < version || (v == version && sub_v < sub_version));
}

/* Like gettimeofday, but returns monotonic time.  If POSIX clocks are not
   available, falls back to gettimeofday but enforces monotonicity. */

int gettime(struct timeval* tv)
{
  int rc;
  static time_t offset = 0, previous = 0;

#if defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0 && defined(CLOCK_MONOTONIC)
  static int have_posix_clocks = -1;

  if(UNLIKELY(have_posix_clocks < 0)) {
    struct timespec ts;
    rc = clock_gettime(CLOCK_MONOTONIC, &ts);
    if(rc < 0) {
      have_posix_clocks = 0;
    } else {
      have_posix_clocks = 1;
    }
  }

  if(have_posix_clocks) {
    struct timespec ts;
    int rc;
    rc = clock_gettime(CLOCK_MONOTONIC, &ts);
    if(rc < 0) return rc;
    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
    return rc;
  }
#endif

  rc = gettimeofday(tv, NULL);
  if(rc < 0) return rc;
  tv->tv_sec += offset;
  if(UNLIKELY(previous > tv->tv_sec)) {
    offset += previous - tv->tv_sec;
    tv->tv_sec = previous;
  }
  previous = tv->tv_sec;
  return rc;
}

static int read_proc(char* filename)
{
  char buf[100];
  int fd, rc;
  fd = open(filename, O_RDONLY);
  if(fd < 0) return -1;
  rc = read(fd, buf, 99);
  if(rc < 0) {
    int saved_errno = errno;
    close(fd);
    errno = saved_errno;
    return -1;
  }
  close(fd);

  if(rc == 0) return -1;

  buf[rc] = '\0';
  return atoi(buf);
}

static int write_proc(char* filename, int value)
{
  char buf[100];
  int fd, rc, n;
  n = snprintf(buf, 100, "%d", value);

  fd = open(filename, O_WRONLY);
  if(fd < 0) return -1;

  rc = write(fd, buf, n);
  if(rc < n) {
    int saved_errno = errno;
    close(fd);
    errno = saved_errno;
    return -1;
  }

  close(fd);
  return 1;
}

/* Determine an interface's hardware address, in modified EUI-64 format */
int if_eui64(char* ifname, int ifindex, unsigned char* eui)
{
  int s, rc;
  struct ifreq ifr;

  s = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  if(s < 0) return -1;
  memset(&ifr, 0, sizeof(ifr));
  strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
  rc = ioctl(s, SIOCGIFHWADDR, &ifr);
  if(rc < 0) {
    int saved_errno = errno;
    close(s);
    errno = saved_errno;
    return -1;
  }
  close(s);

  switch(ifr.ifr_hwaddr.sa_family) {
  case ARPHRD_ETHER:
#ifndef NO_ARPHRD
  case ARPHRD_FDDI:
  case ARPHRD_IEEE802_TR:
  case ARPHRD_IEEE802:
#endif
  {
    unsigned char* mac;
    mac = (unsigned char*)ifr.ifr_hwaddr.sa_data;
    /* Check for null address and group and global bits */
    if(memcmp(mac, zeroes, 6) == 0 || (mac[0] & 1) != 0 || (mac[0] & 2) != 0) {
      errno = ENOENT;
      return -1;
    }
    memcpy(eui, mac, 3);
    eui[3] = 0xFF;
    eui[4] = 0xFE;
    memcpy(eui + 5, mac + 3, 3);
    eui[0] ^= 2;
    return 1;
  }
#ifndef NO_ARPHRD
  case ARPHRD_EUI64:
  case ARPHRD_IEEE1394:
  case ARPHRD_INFINIBAND: {
    unsigned char* mac;
    mac = (unsigned char*)ifr.ifr_hwaddr.sa_data;
    if(memcmp(mac, zeroes, 8) == 0 || (mac[0] & 1) != 0 || (mac[0] & 2) != 0) {
      errno = ENOENT;
      return -1;
    }
    memcpy(eui, mac, 8);
    eui[0] ^= 2;
    return 1;
  }
#endif
  default:
    errno = ENOENT;
    return -1;
  }
}

int kernel_disambiguate(int v4) { return !v4 && has_ipv6_subtrees; }

int kernel_has_ipv6_subtrees(void)
{
  return (kernel_older_than("Linux", 3, 11) == 0);
}

static void print_kernel_route(int add, int protocol, int type, struct kernel_route* route)
{
  char ifname[IFNAMSIZ];
  char addr_prefix[INET6_ADDRSTRLEN];
  char src_addr_prefix[INET6_ADDRSTRLEN];
  char addr_gw[INET6_ADDRSTRLEN];

  if(!inet_ntop(AF_INET6, route->prefix, addr_prefix, sizeof(addr_prefix)) ||
     !inet_ntop(AF_INET6, route->gw, addr_gw, sizeof(addr_gw)) ||
     !if_indextoname(route->ifindex, ifname)) {
    kdebugf("Couldn't format kernel route for printing.");
    return;
  }

  if(route->src_plen >= 0) {
    if(!inet_ntop(AF_INET6, route->src_prefix, src_addr_prefix, sizeof(src_addr_prefix))) {
      kdebugf("Couldn't format kernel route for printing.");
      return;
    }

    kdebugf("%s kernel route: dest: %s/%d gw: %s metric: %d if: %s "
            "(proto: %d, type: %d, from: %s/%d)",
            add == RTM_NEWROUTE ? "Add" : "Delete", addr_prefix, route->plen, addr_gw,
            route->metric, ifname, protocol, type, src_addr_prefix, route->src_plen);
    return;
  }

  kdebugf("%s kernel route: dest: %s/%d gw: %s metric: %d if: %s "
          "(proto: %d, type: %d)",
          add == RTM_NEWROUTE ? "Add" : "Delete", addr_prefix, route->plen,
          addr_gw, route->metric, ifname, protocol, type);
}
