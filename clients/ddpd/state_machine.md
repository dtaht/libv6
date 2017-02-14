The classic dhcp state machine is a little weak when compared against
ipv6 requirements. All addresses everywhere in the system can expire.

They can change.

Getting DNS right is a pita. So is routing. Externally, while getting
an address a classic dhcp client looks more or less like this.

- boot

- request

- confirm - new step!

- renew

- release

- review

Most don't have a "sanity check" phase.

While from a server perspective it looks more like this
-------------------------------------------------------

- boot & review existing assign leases
  confirm those are in use

- acquire prefixes

- assign prefixes in response to requests

- confirm those prefixes work

- monitor prefix table for obsolecense

- make sure renews are sent otherwise remove people from the table

The server really wants persistent state.

Thoughts
--------

I am tempted to use ipsets as that requires no file writing. Also,
using a dummy device to keep the prefix pool on. Lede does not ship
with dummy kmod by default, but it's cheap to add.

The sanity check phase is different on ddpc
-------------------------------------------

- Do I have a route towards the desired server (ipv4 or ipv6)?

- when yes, do I have an address (ipv4 or ipv6?)

- when yes, can I send a packet that way?

Ooh! I got a response!

- sanity check - advertise I have that route. Assign myself
  an address out of that pool on each interface I'm going to assign
  stuff to. SOURCE a confirm packet from
  those addresses to the server. When confirmed, advertise that prefix
  to other clients on the link.

There's a potential tighter sanity check, in that we can monitor the
underlying routing table, and if we lose apparent connectivity,
start withdrawing prefixes and disabling preferrered lifetime sooner.
