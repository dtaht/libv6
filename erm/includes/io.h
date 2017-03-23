/**
 * io.h
 *
 * Dave Taht
 * 2017-03-13
 */

#ifndef IO_H
#define IO_H
#include "shared.h"

extern char* format_address(const ubase_t a);
extern const char* format_prefix_ref(const ubase_t a);
extern const char* format_prefix_str(const char *a);
extern const char* format_prefix_vec(v128 a);
extern const char* format_eui64(const unsigned char* eui);
extern const char* format_thousands(unsigned int value);

#endif
