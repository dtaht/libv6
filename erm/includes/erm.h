/**
 * erm.h
 *
 * Dave Taht
 * 2017-03-20
 */

#ifndef ERM_H
#define ERM_H

// I sure wish I could mangle c++ go or lua syntax here
// erm.open(). Anyway, note that then erm_t type is
// a 28 bit index into a table, with 4 bits of gc tag. It's
// not quite an opaque handle. And for all I know I'll make
// it 64 bits or even 128. With that tag.

// Pointers? Who needs pointers when you can make virtual
// memory do everything you want? (famous last words)

extern erm_t erm_create(char *instance, int perms) COLD;
extern erm_t erm_open(char *instance, int perms) COLD;
extern erm_t erm_close(erm_t handle) COLD;
extern erm_t erm_stat(erm_t handle) COLD;
extern erm_t erm_poll(erm_t handle) HOT;

extern erm_conn_t erm_conn_setup(erm_conn_setup_t vals) COLD;
extern erm_conn_t erm_conn_status(erm_conn_t vals) COLD;
extern erm_conn_t erm_conn_close(erm_conn_setup_t vals) COLD;

// I am tempted to not even expose a non-packet-flags-exposing API

extern erm_packet_t erm_packet_setup(erm_setup_t vals) COLD;
extern erm_packet_t erm_packet_get(erm_packet_t handle) HOT;
extern erm_packet_t erm_packet_send(erm_packet_t handle) HOT;

// Because I am by god going to send/get flowinfo and dscp and ecn vals

extern erm_packet_full_t erm_packet_send_with_flags(erm_packet_full_t handle) HOT;
extern erm_packet_full_t erm_packet_get_with_flags(erm_packet_full_t handle) HOT;

// Note there are usually two possible "generations" of time
// And that we let time get a bit out of sync between processors

extern erm_time_t erm_interval_create(erm_time_setup_t t) COLD;
extern erm_time_t erm_interval_timer_set(erm_time_setup_t t) HOT;
extern erm_time_t erm_time_get(erm_time_setup_t t) HOT;

extern erm_string_t erm_string_stash(char *text) HOT;
extern char * erm_string_get(erm_string_t val) HOT;

// No, these are natural 32 or 128 bit values

// That also get a 32 bit set of flags attached of popcnt | plen | type

extern erm_ip_t erm_ip_stash(erm_t handle, char *text) WARM;
extern char * erm_ip_get(erm_ip_t val) HOT;
extern erm_ip_flags_t erm_ip_flags_get(erm_ip_t val) VERYHOT;

// No these are natural 48 bit (round up to 64) bit values

extern erm_mac_t erm_mac_stash(char *text) HOT;
extern char * erm_mac_get(erm_mac_t val) HOT;

#endif
