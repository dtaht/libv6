/**
 * kill_the_martianslib.c
 *
 * Dave Taht
 * 2017-03-10
 */

#include "kill_the_martians.h"

const unsigned char v4prefix[16] =
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0, 0, 0, 0 };

const unsigned char llprefix[16] =
    {0xFE, 0x80};

const unsigned char zeroes[16] = { 0 };

unsigned char *random_prefix() {
	static __thread unsigned int a[4] = {0};
	a[0] = random();
	a[1] = random();
	a[2] = random();
	a[3] = random();
	return (unsigned char *) a;
}

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

void fool_compiler(prefix *p) {}
