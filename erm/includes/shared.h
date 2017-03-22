/**
 * shared.h
 * I am sufficiently lazy to not worry about this at the moment
 * Dave Taht
 * 2017-03-12
 */

#ifndef SHARED_H
#define SHARED_H

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <asm/types.h>
#include <linux/bpf.h>
#include <linux/fib_rules.h>
#include <linux/filter.h>
#include <linux/if_bridge.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "globals.h"
#include "preprocessor.h"
#include "simd.h"
#include "erm_types.h"

#endif
