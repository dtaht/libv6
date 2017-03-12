/**
 * interface.h
 *
 * Dave Taht
 * 2017-03-11
 */

#ifndef INTERFACE_H
#define INTERFACE_H

/* We re-use the ifindex directly rather than a linked list */

struct interface {
    struct interface_conf conf;
    unsigned short flags;
    unsigned short cost;
    int channel;
    struct timeval hello_timeout;
    struct timeval update_timeout;
    struct timeval flush_timeout;
    struct timeval update_flush_timeout;
    unsigned char *ipv4;
    int numll;
    address_t ll;
    int buffered;
    int bufsize;
    /* Relative position of the Hello message in the send buffer, or
       (-1) if there is none. */
    int buffered_hello;
    char have_buffered_id;
    char have_buffered_nh;
    char have_buffered_prefix;
    rid_t buffered_id;
    unsigned char buffered_nh[4];
    address_t buffered_prefix;
    unsigned char *sendbuf;
    struct buffered_update *buffered_updates;
    int num_buffered_updates;
    int update_bufsize;
    time_t bucket_time;
    unsigned int bucket;
    time_t last_update_time;
    time_t last_specific_update_time;
    unsigned short hello_seqno;
    unsigned hello_interval;
    unsigned update_interval;
    /* A higher value means we forget old RTT samples faster. Must be
       between 1 and 256, inclusive. */
    unsigned int rtt_decay;
    /* Parameters for computing the cost associated to RTT. */
    unsigned int rtt_min;
    unsigned int rtt_max;
    unsigned int max_rtt_penalty;
};


#endif
