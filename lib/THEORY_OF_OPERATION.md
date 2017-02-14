# IPv6 Addresses, Addresses everywhere.

All interfaces with ipv6 tend to have at LEAST 3 addresses
associated with them:

Link Local Address: fe80::/64
ULA Address: fc::/7
Public Address: 2::/something

Optionally they may contain one or more of

DHCP addresses
Privacy address
SLAAC address
statically assigned addresses
HNETD addresses
VPN addresses

... as well as potentially hundreds of expiring addresses.

The same goes for routes, only worse.

The way these are conventionally handled is via a linked list, which
is already inefficient to start with.

# Alternate idea:

libv6 deals with these in chunks, where the smallest possible chunk
is 4 addresses. Why 4? Because that fits nicely into 4 extra wide
registers and lets us do a fully parallel search for them in SSE or
arm neon instructions.

If we need more addresses, we allocate more space, copy the
existing address set, and store them in a bigger chunk.

No linked lists here. Just bigger chunks. You need to then be careful
about slinging pointers to these chunks around elsewhere, but using rcu
on some things we can get much higher performance that way, and retain
an "offset" logic to let us sling them around.

We can also hash based on offset and still do rcu on things.

# Goals

## Keep single address lookup as fast as a linked list
## Win on 2 or more addresses
