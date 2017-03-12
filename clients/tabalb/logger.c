/**
 * logger.c
 *
 * Dave Taht
 * 2017-03-11
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

#include "logger.h"

//typedef uint32x4_t usimd;

typedef unsigned int usimd __attribute__ ((vector_size (16)));

int v4mapped_vec(usimd a) { return 0; }
int v4mapped_str(char * a) { return 0; }

const char * format_prefix_vec(const usimd prefix, unsigned char plen);
const char * format_prefix_str(const unsigned char *prefix, unsigned char plen);

typedef union Vec4 {
    usimd p;
    float e[4];
    int i[4];
    unsigned int u[4];
    unsigned char c[16];
} Vec4_t;

#define format_prefix(X,v) _Generic((X),  \
        char * : format_prefix_str,       \
        usimd  : format_prefix_vec,       \
        default: format_prefix_vec,	  \
		)(X,v)

#define v4mapped(X) _Generic((X),		      \
                                const unsigned char * : v4mapped_str,   \
                                unsigned char * : v4mapped_str,   \
                                char * : v4mapped_str,   \
                                usimd  : v4mapped_vec,    \
        default: v4mapped_vec			      \
		)(X)

const char *
format_prefix_vec(const usimd prefix, unsigned char plen)
{
    static __thread char buf[4][INET6_ADDRSTRLEN + 4] = {0};
    static __thread int i = 0;
    int n;
    Vec4_t p2;
    p2.p = prefix;
    i = (i + 1) % 4; // WTF in a 64 bit arch? Let you call this 4 times?
    if(plen >= 96 && v4mapped(prefix)) {
            inet_ntop(AF_INET, &p2.c[12], buf[i], INET6_ADDRSTRLEN); // ??
        n = strlen(buf[i]);
        snprintf(buf[i] + n, INET6_ADDRSTRLEN + 4 - n, "/%d", plen - 96);
    } else {
      inet_ntop(AF_INET6, &p2.c, buf[i], INET6_ADDRSTRLEN);
        n = strlen(buf[i]);
        snprintf(buf[i] + n, INET6_ADDRSTRLEN + 4 - n, "/%d", plen);
    }
    return buf[i];
}

const char *
format_address(const unsigned char *address)
{
    static char buf[4][INET6_ADDRSTRLEN];
    static int i = 0;
    i = (i + 1) % 4;
    if(v4mapped(address))
        inet_ntop(AF_INET, address + 12, buf[i], INET6_ADDRSTRLEN);
    else
        inet_ntop(AF_INET6, address, buf[i], INET6_ADDRSTRLEN);
    return buf[i];
}

const char *
format_prefix_str(const unsigned char *prefix, unsigned char plen)
{
    static char buf[4][INET6_ADDRSTRLEN + 4];
    static int i = 0;
    int n;
    i = (i + 1) % 4;
    if(plen >= 96 && v4mapped(prefix)) {
        inet_ntop(AF_INET, prefix + 12, buf[i], INET6_ADDRSTRLEN);
        n = strlen(buf[i]);
        snprintf(buf[i] + n, INET6_ADDRSTRLEN + 4 - n, "/%d", plen - 96);
    } else {
        inet_ntop(AF_INET6, prefix, buf[i], INET6_ADDRSTRLEN);
        n = strlen(buf[i]);
        snprintf(buf[i] + n, INET6_ADDRSTRLEN + 4 - n, "/%d", plen);
    }
    return buf[i];
}

const char *
format_eui64(const unsigned char *eui)
{
    static char buf[4][28];
    static int i = 0;
    i = (i + 1) % 4;
    snprintf(buf[i], 28, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
             eui[0], eui[1], eui[2], eui[3],
             eui[4], eui[5], eui[6], eui[7]);
    return buf[i];
}

const char *
format_thousands(unsigned int value)
{
    static char buf[4][15];
    static int i = 0;
    i = (i + 1) % 4;
    snprintf(buf[i], 15, "%d.%.3d", value / 1000, value % 1000);
    return buf[i];
}

#ifdef TEST
#define TPREFIX1 "fd80::1"
#define TPREFIX2 "fd80::2"

int main() {
	printf("str: %s\n",format_prefix(TPREFIX1,12));
	return 0;
}
#endif
