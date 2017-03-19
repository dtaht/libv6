/**
 * traps.h
 *
 * Dave Taht
 * 2017-03-13
 */

#ifndef TRAPS_H
#define TRAPS_H

#include <stdnoreturn.h>

// Well... setting is one thing, storage another
// Resize notice? There will be a bunch of std traps
// that we will just ignore in the normal case.
// Some are internal. Some are related to protocol processing
// invalid packet. Hmm. Internal vs external traps?
// per thread traps?

#define TRAP_IGNORE 0x0
#define TRAP_ALL 0xffff

// enums are ints usually. They might not be in the future

typedef enum {
  TRAP_SUCCESS,
  INVALID_ADDRESS_ENCODING = 1,
  OUT_OF_SPACE = 2,
  BAD_ROUTE = 4,
  BAD_INTERFACE = 8,
  INSERT_FAILED = 16,
  IMPORT_FAILED = 32,
  EXPORT_FAILED = 64,
  OUT_OF_CPU = 128,
  LATE_HELLO = 256,
  MAX_TRAPS = 0xffff
} trap_t;

// needs to be inline, too. Don't know what it does yet
// (t & trap_mask) ? trap(t) : 0;
// trap_t erm_trap(trap_t t); // true if handled successfully
// inline trap_t erm_get_trap_mask(trap_t); // boolean to only see the traps
// you want
// extern trap_t erm_set_trap_mask(trap_t t) HOT;
// extern trap_t erm_set_trap_cb(trap_t t, trap_callback_t c) COLD;

// I will compile these down differently later and come up with better
// semantics
// all of these need to be rewritten to take advantage of the trap register

#if !(defined(TRAP_MODEL))
#define TRAP_MODEL big
#define TRAP_BIG
#endif

#if defined(TRAP_BIG)
extern noreturn void errcmd(const char* msg, const char* f) COLD;
extern void wrncmd(const char* msg, const char* f) COLD;
#endif

#if defined(TRAP_BASIC)
extern noreturn void errcmd(const char* msg) COLD;
extern void wrncmd(const char* msg) COLD;
#endif

// Note I am not using likely or unlikely on a few operations because of the
// extra
// check of the flags register it implies (sometimes).
// FIXME: It seems probable we can just use hot and cold
// instead of likely/unlikely, in the future

#ifdef TRAP_BASIC

#define TRAP_LT(op, value, msg)                                              \
  if(unlikely((op) < (value))) errcmd(msg);
#define TRAP_GT(op, value, msg)                                              \
  if(unlikely((op) > (value))) errcmd(msg);
#define TRAP_EQ(op, value, msg)                                              \
  if(unlikely((op) == (value))) errcmd(msg);

// In c true is any non zero value. Remember that.

#define TRAP_TRUE(op, msg)                                                   \
  if((op)) errcmd(msg)
#define TRAP_FALSE(op, msg)                                                  \
  if(unlikely(!(op))) errcmd(msg)

#define TRAP_WTRUE(op, msg)                                                  \
  if((op)) wrncmd(msg)
#define TRAP_WFALSE(op, msg)                                                 \
  if(unlikely(!(op))) wrncmd(msg)

#endif

#ifdef TRAP_BIG
#define TRAP_LT(op, value, msg)                                              \
  if(unlikely((op) < (value))) errcmd(msg, __func__)
#define TRAP_GT(op, value, msg)                                              \
  if(unlikely((op) > (value))) errcmd(msg, __func__)
#define TRAP_EQ(op, value, msg)                                              \
  if(unlikely((op) == (value))) errcmd(msg, __func__)

// In c true is any non zero value
// in libraries an error return is usually -1 in other word - true. Sigh
// hmm, can builtin handle nonzero?

#define TRAP_TRUE(op, msg)                                                   \
  if((op)) errcmd(msg, __func__)
#define TRAP_FALSE(op, msg)                                                  \
  if(unlikely(!(op))) errcmd(msg, __func__)

#define TRAP_WTRUE(op, msg)                                                  \
  if((op)) wrncmd(msg, __func__)
#define TRAP_WFALSE(op, msg)                                                 \
  if(unlikely(!(op))) wrncmd(msg, __func__)
#define TRAP_WEQ(op, value, msg)                                             \
  if((op) == (value)) wrncmd(msg, __func__)

#endif

#ifdef TRAP_NONE

// Go ahead, live dangerously.

#define TRAP_LT(op, value, msg) op
#define TRAP_GT(op, value, msg) op
#define TRAP_EQ(op, value, msg) op

#define TRAP_TRUE(op, msg) op
#define TRAP_FALSE(op, msg) op

#define TRAP_WTRUE(op, msg) op
#define TRAP_WFALSE(op, msg) op

#endif

// Synonyms
// In c true is any non zero value. NOT JUST 1.
// many libraries invert the logic, do special stuff for -1, etc

#define TRAP_NZ(op, msg) TRAP_TRUE(op, msg)
#define TRAP_ZERO(op, msg) TRAP_FALSE(op, msg)

#define TRAP_WERR(op, msg) TRAP_WTRUE(op, msg)
#define TRAP_ERR(op, msg) TRAP_TRUE(op, msg)

#endif
