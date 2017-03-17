/**
 * interfaces.h
 *
 * Dave Taht
 * 2017-03-16
 */

#ifndef INTERFACES_H
#define INTERFACES_H

typedef struct {
	unsigned hello_interval; // why these in a different base
        unsigned update_interval;
} intervals_t;

typedef struct {
	int ifindex;
	char name[IF_NAMESIZE];
} ifnames_t;

// This will grow so let's leave it at a 16.

typedef struct {
	uint16_t up:1; // Interface up
	uint16_t wireless:1; // Interface is wireless
	uint16_t split_horizon:1; // Apply split horizon
	uint16_t lq:1; // Do link quality estimation
	uint16_t faraway:1; // Nodes on the far end don't interfere
	uint16_t timestamps:1; // Use timestamps
	uint16_t wedged:1;
	uint16_t dead:1;
	// needs attention? Has garbage? Just went down/up and is coming back?
	// doesn't have an address? Wedged? (I've wedged a device driver a lot)
} interface_flags_t;

typedef struct {
    unsigned int rtt_decay; // to me these belong in a vector
    unsigned int rtt_min;
    unsigned int rtt_max;
    unsigned int max_rtt_penalty;
} rtt_metrics_t;

/* A higher value means we forget old RTT samples faster. Must be
      between 1 and 256, inclusive. */

typedef struct {
	char *ifname; // tables, tables, tables
	interface_flags_t flags;
	unsigned short cost; // belong in a vector ? part of something else?
	unsigned short channel; // agggh no such thing as a channel
	rtt_metrics_t metrics;
} interface_conf_t;

typedef struct {
	int ifindex;
	char name[IF_NAMESIZE]; // AGGHH
} ifnames_t;

typedef struct {
    ttime_t hello_timeout; // are these absolute or relative?
    ttime_t update_timeout; // Can we just garbage collect these
    ttime_t flush_timeout;
    ttime_t update_flush_timeout;
} iface_timeouts_t;

// This is the hairiest structure in all of babel
// and can get the crap optimized out of it

typedef struct {
        interface_flags_t flags;
	seqno_t hello_seqno; // isn't this essentially shared data?
        unsigned short cost; // where does this come from?
	int channel; // agggh - way more channels available
        unsigned char *ipv4; // ?
	int numll; // more than one ll is possible but unhandled
	address_t ll; // now offset into the address table

    /* Relative position of the Hello message in the send buffer, or
       (-1) if there is none. */

	// This is another phase entirely now

	int buffered; // why?
	int bufsize; // Why?
        int buffered_hello;
	char have_buffered_id; // sigh. A flag
	char have_buffered_nh; // flag
	char have_buffered_prefix; // flag
	unsigned char buffered_id[8]; // agggh
	unsigned char buffered_nh[4]; // aggggggh
	unsigned char buffered_prefix[16]; //AGGGGGGH
        unsigned char *sendbuf;
	struct buffered_update *buffered_updates; // huh?
	int num_buffered_updates; // aggh
        int update_bufsize;
        ttime_t last_update_time; // what do these mean?
        ttime_t last_specific_update_time;
	unsigned hello_interval; // why is this in a different timebase?
        unsigned update_interval;
	rtt_metrics_t rtt_metric;
} interface_t;

interface_t add_interface(char *ifname, interface_conf_t if_conf);
int flush_interface(char *ifname);
unsigned jitter(interface_t ifp, int urgent);
unsigned update_jitter(interface_t ifp, int urgent);
void set_timeout(ttime_t timeout, int usecs);
int interface_up(interface_t ifp, int up);
int interface_ll_address(interface_t ifp, const unsigned char *address);
void check_interfaces(void);

#endif
