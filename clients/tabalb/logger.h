/**
 * logger.h
 * Dave Taht
 * 2017-03-13
 */

#ifndef LOGGER_H
#define LOGGER_H
#define tlogger(a,b,...) tlog(a,b,__LINE__,__func__, ...)
#define DONOTHING do { } while 0

extern int print(...);
extern int tlog(int errlevel, int err, int line, char *file, char *func, ...);

// Logging is done both on severity and on type or file basis

#ifdef DEBUG
#define debug_log(a, b, ...) a < DEBUG && b & ~DEBUG ? DONOTHING : logger(errlevel, err, ... )
#else
#define debug_log(a, b, ...) DONOTHING
#endif

#endif
