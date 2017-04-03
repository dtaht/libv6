/*
 *	ERM -- The Babel protocol defaults
 *
 *	© 2017 Michael David Täht
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PROTOCOL_DEFAULTS_H
#define PROTOCOL_DEFAULTS_H

#define default_protocol_v6only true
#define default_protocol_v6addr "ff02:0:0:0:0:0:1:6"
#define default_protocol_v4addr "ff02:0:0:0:0:0:1:6" // ?
#define default_protocol_v6port 6696
#define default_protocol_v4port 6696

#define default_protocol_v4transport1 IPPROTO_UDP
#define default_protocol_v4transport2 IPPROTO_UDPLITE
#define default_protocol_v6transport1 IPPROTO_UDP
#define default_protocol_v6transport2 IPPROTO_UDPLITE

#define default_protocol_proto_table 42
#define default_protocol_table 254
#define default_base_v4_metric 0
#define default_base_v6_metric 1024

#define default_protocol_config_port 33124

#define default_protocol_config_file "/etc/babeld.conf"

#define default_protocol_pid_file "/var/run/tabelb.pid"
#define default_protocol_state_file "/var/run/tabelb.pid"
#define default_protocol_id_file "/var/run/tabelb.id"
#define default_config_local_mux "/tmp/tabelb.sock"
#define default_config_allowed_gid 0

// FIXME, insert my timebase here

#define default_protocol_hello_interval 4000
#define default_protocol_wired_hello_interval (4000 * 5)
#define default_protocol_half_time 4

// FIXME: Not sure what these are for yet

#define default_protocol_kernel_priority 0
#define default_protocol_duplicate_priority 0

#define default_config_address "::1"

#define default_protocol_traps 0xFFFF

#define default_protocol_config_keys_file "/etc/ebabeld.conf.keys"
#define default_protocol_keys_file "/etc/ebabeld.keys"
#define default_protocol_config_mykeys_file "/etc/ebabeld.conf.pub"
#define default_protocol_config_mypub_file "/etc/ebabeld.pub"

// I dare not run this against mainline babeld so I'm running it over UDPLITE
// and throwing it into a table I'm not routing with.

#ifndef PRODUCTION
#undef default_protocol_v6transport1
#define default_protocol_v6transport1 IPPROTO_UDPLITE
#undef default_protocol_v4transport1
#define default_protocol_v4transport1 IPPROTO_UDPLITE
#undef default_protocol_proto_table
#define default_protocol_proto_table 84
#undef default_protocol_table
#define default_protocol_table 8484
#undef default_protocol_config_file
#define default_protocol_config_file "/etc/ebabeld.conf"
#endif

#endif
