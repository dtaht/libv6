/**
 * search_prefixes.c
 *
 * Dave Taht
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <arm_neon.h>

#include "v6lib.h"

const char *test_prefixes[] = { "fe80::", "2001::", "fd27::", NULL };

static void populate_prefixes(v6_prefix_table *p) {
	in6_addr_t dst;
	int i = 0;
	for(i; i < size - 1; i++) {
	  inet_pton(AF_INET6,test_prefixes[i],(void *)&p[i].prefix);
	}
	p[i].prefix = u128_ones;
}

int conversions() {
}

void search(int argc, char *argv[]) {
}

void scan(int argc, char *argv[]) {
}

void usage(int argc, char *argv[], char *msg) {
  printf("\n");
  if(msg != NULL) fprintf(stderr,"%s\n",msg);
}

void main(int argc, char* argv[]) {
	v6_prefix_table *assigned = calloc(size,sizeof(v6_prefix_table));
	char *prog = basename(argv[0]);
	populate_prefixes(assigned);
	if(strcmp(prog,"search") == 0) search(argc,argv);
      	else if(strcmp(prog,"scan") == 0) scan(argc,argv);
	else usage(argc, argv,"Must be invoked as search or scan\n");
	free(assigned);
}
