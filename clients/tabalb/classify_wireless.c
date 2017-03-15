/**
 * classify_wireless.c
 * lifted from code originally written by multiple babel authors
 * but is going to get completely rewritten to work with modern wifi
 * Dave Taht
 * 2017-03-15
 */

#include "classify_wireless.h"

int
kernel_interface_wireless(const char *ifname, int ifindex)
{
#ifndef SIOCGIWNAME
#define SIOCGIWNAME 0x8B01
#endif
    struct ifreq req;
    int rc;

    if(isbridge(ifname, ifindex) != 0 || isbatman(ifname, ifindex) != 0)
        return -1;

    memset(&req, 0, sizeof(req));
    strncpy(req.ifr_name, ifname, sizeof(req.ifr_name));
    rc = ioctl(dgram_socket, SIOCGIWNAME, &req);
    if(rc < 0) {
        if(errno == EOPNOTSUPP || errno == EINVAL)
            rc = 0;
        else {
            perror("ioctl(SIOCGIWNAME)");
            rc = -1;
        }
    } else {
        rc = 1;
    }
    return rc;
}

/* Sorry for that, but I haven't managed to get <linux/wireless.h>
   to include cleanly. */

#define SIOCGIWFREQ 0x8B05

struct iw_freq {
    int m;
    short e;
    unsigned char i;
    unsigned char flags;
};

struct iwreq_subset {
    union {
        char ifrn_name[IFNAMSIZ];
    } ifr_ifrn;

    union {
        struct iw_freq freq;
    } u;
};

static int
freq_to_chan(struct iw_freq *freq)
{
    int m = freq->m, e = freq->e;

    /* If exponent is 0, assume the channel is encoded directly in m. */
    if(e == 0 && m > 0 && m < 254)
        return m;

    if(e <= 6) {
        int mega, step, c, i;

        /* This encodes 1 MHz */
        mega = 1000000;
        for(i = 0; i < e; i++)
            mega /= 10;

        /* Channels 1 through 13 are 5 MHz apart, with channel 1 at 2412. */
        step = 5 * mega;
        c = 1 + (m - 2412 * mega + step / 2) / step;
        if(c >= 1 && c <= 13)
            return c;

        /* Channel 14 is at 2484 MHz  */
        if(c >= 14 && m < 2484 * mega + step / 2)
            return 14;

        /* 802.11a channel 36 is at 5180 MHz */
        c = 36 + (m - 5180 * mega + step / 2) / step;
        if(c >= 34 && c <= 165)
            return c;
    }

    errno = ENOENT;
    return -1;
}

int
kernel_interface_channel(const char *ifname, int ifindex)
{
    struct iwreq_subset iwreq;
    int rc;

    memset(&iwreq, 0, sizeof(iwreq));
    strncpy(iwreq.ifr_ifrn.ifrn_name, ifname, IFNAMSIZ);

    rc = ioctl(dgram_socket, SIOCGIWFREQ, &iwreq);
    if(rc >= 0)
        return freq_to_chan(&iwreq.u.freq);
    else
        return -1;
}

/* Return true if we cannot handle disambiguation ourselves. */


