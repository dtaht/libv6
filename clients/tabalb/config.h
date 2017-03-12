/**
 * config.h
 *
 * Dave Taht
 * 2017-03-12
 */

#ifndef CONFIG_H
#define CONFIG_H

// I'm not sure why or even if I need this
// Can't I just write the thing directly?

struct interface_conf {
    unsigned hello_interval;
    unsigned update_interval;
    unsigned short cost;
    char type;
    char split_horizon;
    char lq;
    char faraway;
    int channel;
    int enable_timestamps;
    unsigned int rtt_decay;
    unsigned int rtt_min;
    unsigned int rtt_max;
    unsigned int max_rtt_penalty;
    char ifname[IF_NAMESIZE];
};


#endif
