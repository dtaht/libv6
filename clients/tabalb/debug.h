/**
 * debug.h
 *
 * Dave Taht
 * 2017-03-15
 */

#ifndef DEBUG_H
#define DEBUG_H

#define BACKTRACE(a) debug_backtrace(a)

extern int debug_backtrace(char *msg);

#endif
