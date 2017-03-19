/**
 * neighbor.h
 *
 * Dave Taht
 * 2017-03-12
 */

#ifndef NEIGHBOR_H
#define NEIGHBOR_H

struct neighbour {
    struct neighbour *next;
    /* This is -1 when unknown, so don't make it unsigned */
    int hello_seqno;
    unsigned char address[16];
    unsigned short reach;
    unsigned short txcost;
    struct timeval hello_time;
    struct timeval ihu_time;
    unsigned short hello_interval; /* in centiseconds */
    unsigned short ihu_interval;   /* in centiseconds */
    /* Used for RTT estimation. */
    /* Absolute time (modulo 2^32) at which the Hello was sent,
       according to remote clock. */
    unsigned int hello_send_us;
    struct timeval hello_rtt_receive_time;
    unsigned int rtt;
    struct timeval rtt_time;
    struct interface *ifp;
};

#endif
