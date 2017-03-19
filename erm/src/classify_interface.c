/**
 * classify_interface.c
 * Lifted from code originally copyright multiple babel authors
 * Dave Taht
 * 2017-03-15
 */

#include "classify_interface.h"

static int get_old_if(const char* ifname)
{
  int i;
  for(i = 0; i < num_old_if; i++)
    if(strcmp(old_if[i].ifname, ifname) == 0) return i;
  if(num_old_if >= MAX_INTERFACES) return -1;
  old_if[num_old_if].ifname = strdup(ifname);
  if(old_if[num_old_if].ifname == NULL) return -1;
  old_if[num_old_if].rp_filter = -1;
  return num_old_if++;
}

int kernel_interface_operational(const char* ifname, int ifindex)
{
  struct ifreq req;
  int rc;
  int flags = link_detect ? (IFF_UP | IFF_RUNNING) : IFF_UP;

  memset(&req, 0, sizeof(req));
  strncpy(req.ifr_name, ifname, sizeof(req.ifr_name));
  rc = ioctl(dgram_socket, SIOCGIFFLAGS, &req);
  if(rc < 0) return -1;
  return ((req.ifr_flags & flags) == flags);
}

int kernel_interface_ipv4(const char* ifname, int ifindex, unsigned char* addr_r)
{
  struct ifreq req;
  int rc;

  memset(&req, 0, sizeof(req));
  strncpy(req.ifr_name, ifname, sizeof(req.ifr_name));
  req.ifr_addr.sa_family = AF_INET;
  rc = ioctl(dgram_socket, SIOCGIFADDR, &req);
  if(rc < 0) return -1;

  memcpy(addr_r, &((struct sockaddr_in*)&req.ifr_addr)->sin_addr, 4);
  return 1;
}

int kernel_interface_mtu(const char* ifname, int ifindex)
{
  struct ifreq req;
  int rc;

  memset(&req, 0, sizeof(req));
  strncpy(req.ifr_name, ifname, sizeof(req.ifr_name));
  rc = ioctl(dgram_socket, SIOCGIFMTU, &req);
  if(rc < 0) return -1;

  return req.ifr_mtu;
}

static int isbridge(const char* ifname, int ifindex)
{
  char buf[256];
  int rc, i;
  unsigned long args[3];
  int indices[256];

  rc = snprintf(buf, 256, "/sys/class/net/%s", ifname);
  if(rc < 0 || rc >= 256) goto fallback;

  if(access(buf, R_OK) < 0) goto fallback;

  rc = snprintf(buf, 256, "/sys/class/net/%s/bridge", ifname);
  if(rc < 0 || rc >= 256) goto fallback;

  if(access(buf, F_OK) >= 0)
    return 1;
  else if(errno == ENOENT)
    return 0;

fallback:
  args[0] = BRCTL_GET_BRIDGES;
  args[1] = (unsigned long)indices;
  args[2] = 256;

  rc = ioctl(dgram_socket, SIOCGIFBR, args);
  if(rc < 0) {
    if(errno == ENOPKG)
      return 0;
    else
      return -1;
  }

  for(i = 0; i < rc; i++) {
    if(indices[i] == ifindex) return 1;
  }

  return 0;
}

static int isbatman(const char* ifname, int ifindex)
{
  char buf[256];
  int rc;

  rc = snprintf(buf, 256, "/sys/devices/virtual/net/%s/mesh", ifname);
  if(rc < 0 || rc >= 256) return -1;

  if(access(buf, F_OK) >= 0) return 1;

  if(errno != ENOENT) return -1;

  return 0;
}
