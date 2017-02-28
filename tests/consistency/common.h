/**
 * common.h
 *
 */

#ifndef COMMON_H
#define COMMON_H

enum prefix_status {
    PST_EQUALS = 0,
    PST_DISJOINT,
    PST_MORE_SPECIFIC,
    PST_LESS_SPECIFIC
};

typedef struct {
	unsigned char prefix[16];
	unsigned char plen;
} prefix_table; // Fixme, try aligned and unaligned


static inline int
linklocal(const unsigned char *address)
{
    const unsigned short *up1 = (const unsigned short *) address;
    return up1[0] == htobe16(0xfe80);
}

// These make sense as inlines but we want to fool the compiler

extern const unsigned char v4prefix[16];
extern const unsigned char llprefix[16];

#endif
