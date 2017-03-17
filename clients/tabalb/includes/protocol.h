/**
 * protocol.h
 *
 * Dave Taht
 * 2017-03-17
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#define default_protocol_v6only true
#define default_protocol_v6addr "ff02:0:0:0:0:0:1:6"
#define default_protocol_v4addr "ff02:0:0:0:0:0:1:6" // ?
#define default_protocol_v6port 6696
#define default_protocol_v4port 6696

#define default_protocol_v4transport1 IPPROTO_UDP
#define default_protocol_v4transport2 IPPROTO_UDPLITE
#define default_protocol_v6transport1 IPPROTO_UDP
#define default_protocol_v6transport2 IPPROTO_UDPLITE

#define default_protocol_proto_table  42
#define default_protocol_table  254
#define default_base_v4_metric  0
#define default_base_v6_metric  1024

#define default_config_port 33124

#define default_protocol_config_file "/etc/babeld.conf"

#define default_protocol_pid_file "/var/run/tabelb.pid"
#define default_protocol_state_file "/var/run/tabelb.pid"
#define default_protocol_id_file "/var/run/tabelb.id"
#define default_config_local_mux  "/tmp/tabelb.sock"
#define default_config_allowed_gid  0

// FIXME, insert my timebase here

#define default_protocol_hello_interval 4000
#define default_protocol_wired_hello_interval (4000*5)
#define default_protocol_half_time 4

// FIXME: Not sure what these are for yet

#define default_protocol_priority 0
#define	default_protocol_kernel_priority 0
#define default_protocol_duplicate_priority 0
#define	default_protocol_priority 0

#define default_config_address "::1"

#define default_protocol_traps 0xFFFF

#define default_protocol_config_keys_file "/etc/tabelb.conf.keys"
#define default_protocol_keys_file "/etc/tabelb.keys"
#define default_protocol_config_mykeys_file "/etc/tabelb.conf.pub"
#define default_protocol_mykeys_file "/etc/tabelb.pub"

// I dare not run this against mainline babeld so I'm running it over UDPLITE
// and throwing it into a table I'm not routing with.

#ifndef PRODUCTION
#undef default_protocol_v6transport1
#define default_protocol_v6transport1 IPPROTO_UDPLITE
#undef default_protocol_v4transport1
#define default_protocol_v4transport1 IPPROTO_UDPLITE
#undef default_protocol_proto_table
#define default_protocol_proto_table  84
#undef default_protocol_table
#define default_protocol_table  8484
#undef default_protocol_config_file
#define default_protocol_config_file "/etc/tabelb.conf"
#endif

#endif
