#!/bin/sh

IFACE=eip0
P=44 # I use this for oddball tests and export it to babel
ADDR=fd11::
ADDRESSES=1024

create() {
	ip link add name $IFACE type dummy
	ip -6 route add $ADDR/64 dev $IFACE proto $P expires 600
	for i in `seq 1 $ADDRESSES`
	do
		ip -6 addr add ${ADDR}`printf %x $i`/64 dev $IFACE noprefixroute valid_lft 600 preferred_lft 300
	done
	ip link set $IFACE up
}

destroy() {
	ip link set $IFACE down
	ip -6 addr flush dev $IFACE scope global
	ip -6 route del $ADDR::/64 dev $IFACE proto $P
}

create
#sleep 60
#destroy
