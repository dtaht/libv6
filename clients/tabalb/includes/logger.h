/**
 * logger.h
 * Dave Taht
 * 2017-03-13
 */

#ifndef LOGGER_H
#define LOGGER_H
#define tlogger(a,b,...) tlog(a,b,__LINE__,__func__, ...)
#define DONOTHING do { } while 0

extern int tprint(int dest, ...);
extern int tlog(int errlevel, int err, int line, char *file, char *func, ...);

// Logging is done both on severity and on type or file basis
#ifdef DEBUG
#define debug_log(a, b, ...) a < DEBUG && b & ~DEBUG ? DONOTHING : tlogger(errlevel, err, ... )
#define debug_log_address(a, b, ...) a < DEBUG && b & ~DEBUG ? DONOTHING : tlogger(errlevel, err, ... )
#else
#define debug_log(a, b, ...) DONOTHING
#define debug_log_address(a, b, ...) DONOTHING
#endif

#endif
