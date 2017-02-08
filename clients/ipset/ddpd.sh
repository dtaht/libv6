#!/bin/sh

start() {
    ipset create lease hash:net,iface family ipv6 timeout 6000
}

create_pool() {
    ipset add lease fd66::/48,pool
}

get_prefix() {
:
}

export i=1

split_prefix() {
    P=$1 # 64
    IFACE=$2 # interface that wants it
    ipset -o members list lease | grep ",pool" | while read x
    do
        PF=`echo $x | cut -f2 -d/ | cut -f1 -d,`
        a=`printf "%x" $i`
        echo $a
        ipset add lease fd66:0:0:$a::$64,$IFACE #
        i=`expr $i + 1`
    done

}

join_prefix() {
:
}

end() {
    ipset destroy lease
}

start
create_pool
split_prefix 64 eth0
split_prefix 64 eth1
split_prefix 64 eth2
split_prefix 64 eth3
split_prefix 64 eth4
split_prefix 62 lo

ipset -o members list leases
