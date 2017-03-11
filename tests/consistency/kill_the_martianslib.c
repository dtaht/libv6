/**
 * kill_the_martianslib.c
 *
 * Dave Taht
 * 2017-03-10
 */

#include "kill_the_martians.h"

// The original routine was NOT inlined

int
v4mapped_orig(const unsigned char *address)
{
    return memcmp(address, v4prefix, 12) == 0;
}


const unsigned char v4prefix[16] =
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0, 0, 0, 0 };

const unsigned char llprefix[16] =
    {0xFE, 0x80};

const unsigned char zeroes[16] = { 0 };

#ifdef __arm__
#include "perfevents.c"
#endif

unsigned char *random_prefix() {
	static __thread unsigned int a[4] = {0};
	a[0] = random();
	a[1] = random();
	a[2] = random();
	a[3] = random();
	return (unsigned char *) a;
}

enum common_prefixes {
	linklocal,
	v6multicast,
	v4map,
	allones,
	allzeroes,
	v6localhost,
	v4multicast,
	v4localhost,
	COMMON_PREFIXES_MAX
};

// What we need to do next is generate legal AND illegal prefixes in these ranges

/*
prefix * gen_common_prefixes(void) {
	prefix *p = calloc(COMMON_PREFIXES_MAX,sizeof(prefix));
        p[linklocal]     = { 0xfe, 0x80 }, 10;
        p[v6multicast]   = { { 0xff }, 8 };
        p[v4map]         = { { 0,0,0,0, 0xff, 0xff }, 96 };
        p[allzeros]      =   { { 0 }, 128 };
        p[allones]       =   { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }, 128 };
        p[v6localhost]   = p[allzeroes]; p[v6localhost].p[15] = 1;
        p[v4localhost]   = p[v4map]; p[v4localhost].p[12] = 127; p[v4localhost].plen = 100;
        p[v4multicast]   = p[v4map]; p[v4multicast].p[12] = 0xff; p[v4multicast].plen = 104;
	return p;
}
*/


prefix * gen_random_prefixes(int count) {
	prefix *p = calloc(count,sizeof(prefix));
	for(int i = 0; i < count; i++) {
		memcpy(p[i].p,random_prefix(),16);
		p[i].plen = random() % 128;
	}
	return p;
}

// Sometimes I think passing the additonal typing along (*[16]) universally might be of help

int count_martian_prefixes_old(prefix *p, int count) {
	int c = 0;
	for(int i=0;i<count;i++)
		c += martian_prefix_old((const unsigned char*)&p[i].p,p[i].plen);
	return c;
}

int count_martian_prefixes_new(prefix *p, int count) {
	int c = 0;
	for(int i=0;i<count;i++)
		c += martian_prefix_new((const unsigned char*)&p[i].p,p[i].plen);
	return c;
}

int count_martian_prefixes_new_single(prefix *p, prefix *p2, int count) {
	int c = 0;
	for(int i=0;i<count;i++)
		c += martian_prefix_new((const unsigned char*)&p[i].p,p[i].plen) ||
			martian_prefix_new((const unsigned char*)&p2[i].p,p2[i].plen);
	return c;
}

int count_martian_prefixes_new_dual(prefix *p, prefix *p2, int count) {
	int c = 0;
	for(int i=0;i<count;i++)
		c += martian_prefix_new_dual((const unsigned char*)&p[i].p,p[i].plen,
					(const unsigned char*)&p2[i].p,p2[i].plen);
	return c;
}

void fool_compiler(prefix *p) {}
