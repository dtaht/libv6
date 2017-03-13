/**
 * logger.h
 * Dave Taht
 * 2017-03-13
 */

#ifndef LOGGER_H
#define LOGGER_H

#define logger(a,b,...) log(a,b,__LINE__,__func__, ...)

extern int print(...);
extern int log(int errlevel, int err, int line, char *file, char *func, ...);

#define DONOTHING do { } while 0

// Logging is done both on severity and on type or file basis

#ifdef DEBUG
#define debug_log(a, b, ...) a < DEBUG && b & ~DEBUG ? DONOTHING : logger(errlevel, err, ... )
#else
#define debug_log(a, b, ...) DONOTHING
#endif

#endif
