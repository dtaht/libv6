# DDPD: Dumb Distributed Prefix Daemon

* Runs over any pre-existing path on the network to get prefixes,
be they ULAs, IPv4 public or private addresses, or existing public
prefixes. If there's UDP connectivity of any sort, you can get a prefix.

* Routability is handled by the receiving router. It needs to announce
to the universe it has routes to this prefix. That's it.

* Is mutually authenticatable using first class crypto techniques,
and prefix assignments themselves are encrypted.

* Is dirt simple

Each side establishes a public key for multual authentication. The
grantor agrees to grant up to X prefixes, the grantee requests up to X
prefixes.

# Backstory

## DHCPv6

Is a confusing mess of options inherited from dhcp, with all the same
problems dhcp had (especially including "rogue" dhcp servers), and
requiring a complex per-hop relay infrastructure as if complexity
helped, and lousy support for both authentication and crypto.

The specification runs to 144+ badly implemented pages. Not one daemon I
know of gets dhcpv6 right - and it also interacts somewhat badly with
the RA based distribution mechanism.

## AHCPD

Did almost everything I needed except that it did not do IPv6 prefix
distribution. It also required that it run on every intermediate hop
on the network. It got some things very right, in getting a basic IPv6
address to everybody, fast - but only for p2p links.

## HNETD

HNET promised to fix all that. Millions were spent on developing it,
with a design by committee totally disinterested in actually testing it
in real environments. People wanted a link state routing protocol, that
got rejected...

Homenet then became an enormous link-state protocol that mandated not
only it run on every intervening router, but left authentication as a
handwave.

I had no desire to hand a globally routable prefix to every device in
my entire network.

... All I needed was to backhaul ipv6 to the pool, and the back 40,
over the existing transports and routers, which were all capable of
routing ipv6. In fact, they were already doing it, but they couldn't
obtain a global prefix to play with. And they all already ran ipv4.

One thing I'd used before was the "selfassign" script which leveraged
information from the routing protocol to give itself a prefix. This
had notable failure modes where the far end stopped routing, or was
rebooted, and required I be very careful about who I gave a prefix to.

# Enter DDPD.

## Use Case #1 - Meshy networks

Consider a topology like this, with multiple potential exit nodes. In a
natted world, any (stable) exit node will do. With IPv6, you need a
prefix range that can go out the correct gateway, always.

So you have the devices that need public prefixes - AND ONLY THOSE -
requesting them from master servers on those edge gateways.

Because of the mutual authentication requirement, this also allows folk
to charge for (or establish) an existing peer agreement. Everyone
agreeing to share a prefix on the network, is known.

## Use Case #2 - VPN networks

VPNs also tend to rely on distributed address assignment. DDPD allows
for selecting an IPv6 address and prefix from a pool without using DHCP,
out of band prior to actually starting up the VPN.

## Use Case #3 - Conventional networks

Assuming RA is working right, it seems possible to make this work 
in that case too.
