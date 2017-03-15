/**
 * random_bytes.c
 *
 * Dave Taht
 * 2017-03-15
 */

#include "random_bytes.h"

#ifdef DEBUG_MODULE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	char buf[8];
	char buf2[4];
	char buf3[16];
	get_random_bytes(&buf,sizeof(buf));
	get_random_bytes(&buf,sizeof(buf));
	get_random_bytes(&buf,sizeof(buf));
        printf("How random is this: %x vs %llx vs %llx - %llx\n",
		(unsigned int) &buf2,
		(unsigned long long) &buf,
		(unsigned long long) &buf3[0],
		(unsigned long long) &buf3[8]);
	if(memcmp(buf,buf2,4) == 0 || memcmp(buf3,buf2,8) == 0) {
		printf("get_random is not random enough!");
                exit(-1);
	}
	return 0;
}
#endif
