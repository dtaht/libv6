/**
 * shared.h
 * I am sufficiently lazy to not worry about this at the moment
 * Dave Taht
 * 2017-03-12
 */

#ifndef SHARED_H
#define SHARED_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
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
#include <linux/filter.h>
#include <linux/bpf.h>
#include <linux/fib_rules.h>
#include <net/if_arp.h>
#include <netinet/in.h>

#include "tabeld.h"
#include "globals.h"
#include "simd.h"

#endif
