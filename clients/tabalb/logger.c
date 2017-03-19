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

#include "shared.h"
#include "logger.h"

int v4mapped_vec(usimd a) { return 0; }
int v4mapped_str(char * a) { return 0; }

#define format_prefix_t(X,v) _Generic((X),  \
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

#ifdef DEBUG_MODULE
#define TPREFIX1 "fd80::1"
#define TPREFIX2 "fd80::2"

int main() {
	printf("str: %s\n",format_prefix(TPREFIX1,12));
	return 0;
}
#endif
