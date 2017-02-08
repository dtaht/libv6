# Coping with IPv6 better

It's not so much that lede's ipv6 support has regressed, it's just that
the ipv6 support in the world has moved forward, and changed.

Most of my problems stem from living in a highly dynamic ipv6
environment.

My 6rd connections change underlying IPs as much as 3 times
a day, with an underlying lease of 600 sec. These don't make it to the
delegated clients.

My comcast connections advertise absurdly long leases for PD by default,
but a reboot of either router or modem quite often loses those. (lack of
persistent store on the device?). A reboot of the device often fails
to get dhcp at all.

When testing multiple routers on the same link, reflashing them, etc, I
end up with persistent ULAs that don't expire.

My problems were compounded by an expectation of now (2017) always
seeing preferred_lft, valid_lft and expires on non-cc routers and their
clients in the kernel tables (even when not strictly needed), on the
non-openwrt oses I'd been using.

And... also compounded by user error - never, ever do a "ip -6 flush
addr dev whatevever scope global" without the scope global. Sure, that
might flush 40 unused, unexpiring, and unroutable ULAs accumulated for
all the testing you did that day - but it will also flush the link layer
fe80:: address until you bring the interface up and down or add it back
in manually. This got [PATCH: babel into kind of a wedged state]().

There are issues with usbnet interfaces needing an ipv4 address in order
to even acquire a ipv6 ll address, and their up/down and error recovery
survival also.

Most people in a *static* ipv6 configuration - (e.g geeks with dedicated
ipv6 addresses that never change), will be ok.

Given the headaches in identifying where routes came from I'd ended up
using "ip route proto" to identify where those came from. (basic proto
support, just landed in lede head)

Given the headaches in identifying where addresses came from, I'd ended
up using ipsets to track that (in another os). I think adding an
equivalent to "proto" support for addresses in the linux kernel will help.

# General thoughts

* Never announce a long lived route or address that's handed you by
  someone else. They are frequently lying. Scale back your announcement
  size to some multiple or divisor of what your advertisement is. IF
  you are getting refreshes, increase it to a multiple. Clamp it to
  no less than a minute, preferably 5 minutes to an hour.
* Refresh RAs early and often. dnsmasq (based on painful experience),
  will do an RA after a dhcpv4 request, and uses multicast to do so.
* Never flush a route table of anything you don't "own".
* Never deal with the ietf to design a protocol.

# Address splitting is probably the most painful thing out there.

Not one daemon gets this right (dibbler, wide, dnsmasq, etc)

Nothing will take a request for a /62 and break up the pool correctly.
IF you only want 3 prefixes you will get bumped to a /62. Unless you
don't have a /62 in which case you don't get anything.

Please note (the expectation was that everyone would have millions of
prefixes to choose from, which the ISPs promptly created artificial
scarcity from). I too had expected we'd just get /48s, and not getting
them, has led to endless headaches with all sorts of expections in every
OS and daemon.

Say you do this to yourself:

config interface 'usb0'
        option ifname 'usb0'
        option proto 'static'
        option ipaddr '172.26.97.1'
        option netmask '255.255.255.255'
        option ip6assign '64'
        option ip6class 'local'

# Add the above interface to your system.

Great. So your former /60 that got delegated to your lan becomes a /61.
Its interfaces get renumbered, without the renumbering passing onto the
infinite leases the daemon is handing out.

# Enter v6lib

So I gave up, and started writing [this library](v6lib) to manage ipv6 addresses. I am well aware that this is likely to become yet another incomplete
answer to the problem.

# On the dns front... Then there's DNAMES.

Saving that rant for later.

# Bringing up routing daemons

it makes no sense to bring up any routing deamons until static
configuration is complete - this includes creating a bridge, assigning
static addresses, getting the wifi up, etc.

lede starts them as soon as possible at boot, which causes unneeded churn
in the routing protocol.

In some cases it might make sense to wait even for certain other things
to complete (dhcp from upstream with a default route) before bothering,
but at least that much would avoid some churn in the network when, for
example, the power fails globally.

# Firewalling

Given all this complexity and how often interfaces and IPs could come
and go, I gave up and switched to using ipsets universally elsewhere.
The cerowrt design never had to reload a firewall. Regrettably it never
handled vlans, either. Nobody else uses ipsets comprehensively.

# Permanent identifiers

Given all this maddening dynamicism in an "modern" ipv6 network and lack
of dns integration (and even then), I adopted the concept of a permanent
device identifier - a dedicated ULA range announced network-wide by the
babel protocol. The way I do that is:

	ip -6 address add fdXX::YY/128 dev lo proto 44

and tell babel to always export that.

IPv6 has always had the ability to route over the link layer. There is,
honestly, generally, no need to actually number all your interfaces in
ipv6. There are people that dislike exposing core devices via ipv6 at all.

I should probably have done it using an entire ula/48 so I could
announce each /64 via an offlink RAs elsewhere, and I might switch to that.
... if I could figure out if offlink RAs worked.

````
config interface 'loopback'
        option ifname 'lo'
        option proto 'static'
        option ipaddr '127.0.0.1'
        option netmask '255.0.0.0'
        option ip6addr 'fd99::93/128'
````

# ULAs & netifd

netifd has a useful internal concept of "local" for its ULA
assignments - the above ip6class, for example, lets me not use up any
global public prefix on the ula interface (and get a public one when I
bring the wifi side or down). It doesn't map back to "protos" per se,
but could perhaps be extended (list ip6class local slaac homenet ?)

netifd doesn't renew or expire them, and advertises permanent leases
to the network.

# 6rd

This route makes it:

FIXME

This one, doesn't.

FIXME

# DHCPv6, SLAAC

# Privacy addresses

# Against the ::1 convention for routers

I am actually unfond of this as it means that on any given link, the
boxes you can most easily attack will be found on ::1 instead of 2^60 or
so alternatives. While this is a handy feature for a beleagured
sysadmin, it reduces the bad-guy search space by far too much, IMHO.

(same concern goes for other critical hardware, servers, etc). With
 solid DNS integration (hah!), finding stuff you need to find gets easier.

While this may be "security by obscurity", it's security by a lot of obscurity.

# Source specific routing

The implementation in lede works great for exterior routers, but installs
a lot of static routes that shouldn't be installed for 

# Default-deny firewalling

pcp is "not ready". IPv6 does have some nice new protocols in it.

# DNS integration

# Homenet

I have not even tried.

# Coping with multicast issues

For the past year lede has been carrying patches for converting multicast
to unicast.  After being submitted to kernel mainline at least one bug was found
concerning ipv6 multicast.

I turned it off by default, and slept better.

        option igmp_snooping '0'
        option multicast_to_unicast '0'

# USB

I have been using a variety of hackerboards to periodically test if the
wifi was working. They hang off the local usb port on their own battery
backup. I'd

Except that the new proto support doesn't extend to addresses that come
up statically.

# DDPD

# Chip

They also *really want to

accept_ra does not get announced for a long time

#
# if the usb0 gets rebooted, it IS reconfigured with the ipv4 address

but the local assignment from netifd's pool is NOT re-assigned.

Sat Feb  4 18:06:54 2017 daemon.notice netifd: Network device 'usb0' link is up
Sat Feb  4 18:06:54 2017 daemon.notice netifd: Interface 'usb0' has link connectivity

root@archer-atf:/etc/config# ifconfig usb0
usb0      Link encap:Ethernet  HWaddr 12:FD:2C:C4:59:24
          inet addr:172.26.97.200  Bcast:255.255.255.255  Mask:255.255.255.255
          inet6 addr: fe80::10fd:2cff:fec4:5924/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:126 errors:0 dropped:0 overruns:0 frame:0
          TX packets:87 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:13107 (12.7 KiB)  TX bytes:11651 (11.3 KiB)

root@archer-atf:/etc/config# ifdown usb0
root@archer-atf:/etc/config# ifup usb0
