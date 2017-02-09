# Widened endpoint identifiers for VPNs

The last major place where fq_codel's techniques fall down is on deeply
encapsulated traffic. We already do a huge level of DPI to extract the
innermost headers from GRE, 6in4, 4in6, and I forget what else, but it
falls down on all known forms of crypto traffic, reducing that to 
serial behavior.

Recently a nice mod to fq_codel landed that preserved the inner hash
on vpns that terminated on the router, thus making vpn traffic compete
evenly with all other traffic. It had [*marvellous* results on an ipsec
tunnel](http://www.taht.net/~d/ipsec_fq_codel/).

Still, though, the problem remains of an end to end VPN technology
experiencing congestion at some hop that cannot gain insight into the flows.

I beat this, personally, to a large extent by using a meshy vpn [tinc](fixme),
but that still doesn't solve it between two hosts end to end.

## Enter IPv6

Even the smallest dhcp-pd delegation tends to be a /60. If we use a dedicated
subnet for either client or server, we have 2^64 potential addresses to
play with. Why treat them as addresses? I don't really know why people
have not tried to just route and listen to *everything* on that subnet.

Why not map inner headers to a hash that sends stuff to - or sources stuff
from - up to 2^64 addresses?

## Flaws

### Exposing more per-flow information

Exposing per flow information this way bothers some people. Including me. 
I like the idea of using a restricted range to expose less of this information
to observers.

On the other hand, perhaps rotating the hash periodically is enough. Or 
perhaps it doesn't matter at all for some use cases.

### It's hell on stateful firewalls

Random packets spewing from everywhere will be a good way to run stateful
firewalls out of memory. I'm not huge on that problem. Also the hole
punching problem remains. 

### It's not been done before

The standard socket API does let you listen to "raw" packets on an interface.

So getting "everything" on a subnet seems totally easy, but I've not tried it.

# FIXME: Grab writeup from wireguard mailing list.
