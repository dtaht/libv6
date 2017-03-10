/**
 * kill_the_martians.h
 * Dave Taht
 * 2017-03-10
 */

#ifndef KILL_THE_MARTIANS_H
#define KILL_THE_MARTIANS_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern const unsigned char v4prefix[16];
extern const unsigned char zeroes[16];

typedef struct {
	unsigned char p[16];
	int plen;
} prefix;

extern int
martian_prefix_new(const unsigned char *prefix, int plen);

extern int
martian_prefix_old(const unsigned char *prefix, int plen);


extern int count_martian_prefixes_new(prefix *p, int count);
extern int count_martian_prefixes_old(prefix *p, int count);
extern prefix * gen_random_prefixes(int count);

extern int start_clock(void);
extern int stop_clock(void);
extern void fool_compiler(prefix *p);

#endif
