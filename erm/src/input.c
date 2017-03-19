/**
 * input.c
 *
 * Dave Taht
 * 2017-03-13
 */

#include "shared.h"
#include "input.h"
#include "debug.h"

extern const unsigned char v4prefixstr[];

int
parse_address(const char *address, unsigned char *addr_r, int *af_r)
{
    struct in_addr ina;
    struct in6_addr ina6;
    int rc;

    rc = inet_pton(AF_INET, address, &ina);
    if(rc > 0) {
        memcpy(addr_r, v4prefixstr, 12);
        memcpy(addr_r + 12, &ina, 4);
        if(af_r) *af_r = AF_INET;
        return 0;
    }

    rc = inet_pton(AF_INET6, address, &ina6);
    if(rc > 0) {
        memcpy(addr_r, &ina6, 16);
        if(af_r) *af_r = AF_INET6;
        return 0;
    }

    return -1;
}

#ifdef DEBUG_MODULE
const unsigned char v4prefixstr[] = { 0, 0, 0, 0,
				      0, 0, 0, 0,
				      0, 0, 0xff, 0xff,
				      1, 1, 1, 1 };
MFIXME
#endif
