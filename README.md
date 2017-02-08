# v6lib - a saner library for dealing with v4/v6 network addresses

This was written because [I was going mad](ipv6-rant). However
it is not anywhere near started, this repo exists merely because
I was accumulating rants and not code.

## Objectives

* Manage ipv4 and ipv6 addresses in cpu optimized fashion
** Use SIMD instructions where available
** Use cpu efficient data structures 
** Cpu efficent means to find/insert/delete/update/expire ipv6 addresses

* Pull and push sanely from:
** interface address tables
** route tables
** ipset

* Manage address and prefix pools
* Manage RAs in sane ways
* Manage dhcp in sane ways

* Integrate into other daemons
** odhcp6c
** odhcpd
** dhclient
** dibbler
** udhcpc
** dnsmasq
** ddpd
** hnetd

* Integrate into other busses
** dbus
** ubus
** netlink

* Supply a few sample clients
** be SMALL and busybox-like

## Non-objectives

* ipv6calc functionality

Use ipv6calc!

** Might write a client to do some of that (6rd calcs)


