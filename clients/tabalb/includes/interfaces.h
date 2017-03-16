/**
 * interfaces.h
 *
 * Dave Taht
 * 2017-03-16
 */

#ifndef INTERFACES_H
#define INTERFACES_H

typedef struct {
	char *ifname; // tables, tables, tables
	unsigned hello_interval; // why these in a different base
    unsigned update_interval;
	unsigned short cost; // belong in a vector
	char type;  // bits in a flag
    char split_horizon;
    char lq;
    char faraway;
	int channel; // agggh
    int enable_timestamps;
	unsigned int rtt_decay; // to me these belong in a vector
    unsigned int rtt_min;
    unsigned int rtt_max;
    unsigned int max_rtt_penalty;
} interface_conf_t;

// This is the hairiest structure in all of babel
// and can get the crap optimized out of it

typedef struct {
	unsigned short flags; // why these flags and not the later flags
    unsigned short cost;
	int channel; // agggh - way more channels available
	ttime_t hello_timeout; // are these absolute or relative?
	ttime_t update_timeout; // Can we just garbage collect these
    ttime_t flush_timeout;
    ttime_t update_flush_timeout;
	char name[IF_NAMESIZE]; // AGGHH
    unsigned char *ipv4;
	int numll; // more than one ll is possible
	unsigned char (*ll)[16]; // aggh
	int buffered; // why?
	int bufsize; // Why?
    /* Relative position of the Hello message in the send buffer, or
       (-1) if there is none. */
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
	ttime_t bucket_time; // we are going to toss the htb limiter
	unsigned int bucket; // and use deficits as per cake
    ttime_t last_update_time;
    ttime_t last_specific_update_time;
	seqno_t hello_seqno; // isn't this essentially shared data?
	unsigned hello_interval; // why is this in a different timebase?
    unsigned update_interval;
    /* A higher value means we forget old RTT samples faster. Must be
       between 1 and 256, inclusive. */
    unsigned int rtt_decay;
    /* Parameters for computing the cost associated to RTT. */
    unsigned int rtt_min;
    unsigned int rtt_max;
    unsigned int max_rtt_penalty;
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
