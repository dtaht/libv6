/**
 * debug.h
 *
 * Dave Taht
 * 2017-03-15
 */

#ifndef DEBUG_H
#define DEBUG_H
#include <stdnoreturn.h>
#include "preprocessor.h"

#define BACKTRACE(a) debug_backtrace(a)
#define UNFINISHED unfinished(__FILE__)
#define MFIXME int main(void) { UNFINISHED; return 0; }
#define MIGNORE(why) int main(void) { mignore(why); return 0; }
#define FIXME UNFINISHED
#define TODO(msg) unfinished(msg)

extern noreturn void mignore(const char *msg) COLD;
extern noreturn void unfinished(const char *msg) COLD;
extern int debug_backtrace(char *msg);

#endif
