# 

# Dealing with where clients come from with ipset

We have a good in-kernel way of keeping track where routes come from
(proto facility). We do not have a good way of keeping track where
addresses come from. Nor expiring them properly (or renewing them). Nor
firewalling them properly.

ipset destroy dhcp_prefixes
ipset destroy ra_prefixes
ipset destroy static_prefixes

ipset create dhcp_prefixes hash:net,iface family ipv6 timeout 600
ipset create ra_prefixes hash:net,iface family ipv6 timeout 600
ipset create static_prefixes hash:net,iface family ipv6 timeout 600

ipset add static_prefixes fd75::/48,eth0 timeout 26000
ipset add ra_prefixes fd75::/64,eth1 timeout 600
ipset add ra_prefixes fd74::/60,pool timeout 6000
ipset add ra_prefixes fd74::/64,eth0 timeout 600
ipset add ra_prefixes fd74:0:0:1::/64,eth1 timeout 600
ipset add ra_prefixes fd74:0:0:2::/64,eth2 timeout 600
sleep 10
ipset -exist add ra_prefixes fd74:0:0:2::/64,eth2 timeout 1200

root@archer-atf:~# ipset list ra_prefixes
Name: ra_prefixes
Type: hash:net,iface
Revision: 6
Header: family inet6 hashsize 1024 maxelem 65536
Size in memory: 1956
References: 0
Members:
fd74::/64,eth0
fd74::/60,pool
fd74:0:0:1::/64,eth1
fd75::/64,eth1
fd74:0:0:2::/64,eth2

## The ipset list thing is dumb

I have a patch pending for ipset -m to just list the members.

## Notes

I have to pull together a whole lot of ipset glue and see what
I can do with it.
