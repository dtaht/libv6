/**
 * preprocessor.h
 * This file contains the preprocessor macros used throughout erm,
 * and documentation for what they do. They are "sane" defaults otherwise.
 *
 * Dave Taht
 * 2017-03-19
 */

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

// I will probably end up wrapping sched_yield to do more stuff
// and force callers to check status. "Why am I back here?"
// sched_yield always returns 0 on linux.

#ifndef YIELD
#define YIELD(mask) sched_yield()
#endif

// FIXME: discuss DEBUG & NDEBUG and assert usage

// There are a multitude of TRAP handlers in the virtual machine and a multitude
// of ways to run them - ranging from a FPGA system with its own opcode and
// register to handle it - to a full blown operating system. The default is full
// logging, with line numbers of functions.

#ifndef TRAP_MODEL
#define TRAP_MODEL big
#define TRAP_BIG
// #define TRAP_TRACE
// #define TRAP_DEBUG
// #define TRAP_CRASH
// #define TRAP_BASIC
// #define TRAP_NONE
#endif

// Any given bit of code can be built standalone with make "whatever.dbg"
// This option must be handled within that module to compile, but the
// makefile will generate the code using it.
// #define DEBUG_MODULE

// build module for profiling: make "whatever.prof"
//
// #define PROFILE_MODULE

// RUNAHEAD: defines how far any given operation can overrun its data
// This - in a vector machine - can be as high as FIXME.

#ifndef RUNAHEAD
#define RUNAHEAD 4
#endif

// SOMETIMES_INLINE: used as a pre-pre processor macro for stuff you
// can include more than once and only sometimes want to inline.

// DATA_BARRIER: FIXME
// is a std sequence of 1111s and 0000s at the end of all data structures
// presently that is defined as RUNAHEAD * datatype size string of
// 0xffff0000 always on an aligned boundary.

// DONOTHING
// I really wish C had standardized on a means to do nothing

#define DONOTHING do {} while (0)
#define MIN(a,b) (a > b ? a : b)
#define MAX(a,b) (a < b ? a : b)

// FIXME: actually I think returning something other than one
// makes sense to find obscure bugs in people's error handling.
// maybe make these generic, take varargs, and typeless?

static inline int donothing_true() { return 1; }
static inline int donothing_false() { return 0; }

// Standard hints to the compiler for the likelihood of a branch

#define likely(x)       __builtin_expect(!!(x),1)
#define unlikely(x)     __builtin_expect(!!(x),0)

// Common additional function attributes in GCC and LLVM

#define CONST __attribute__((const))
#define PURE __attribute__((pure))
#define HOT  __attribute__((hot))
#define COLD __attribute__((cold))
#define NORETURN __attribute__((noreturn))
#define INTERRUPT __attribute__((interrupt))

// FIXME: We occasionally have to do linker magic with SECTIONS
//

// PREFETCH - is generally a lose nowadays. Except when going backwards.
// I can never remember the direction of read or write, but do remember the
// locality argument ranges from 0 to 3. Never know which of those to use
// either!
// So go right ahead, put 'em in there. But test with them left out!

#ifndef NO_PREFETCH

#define RPREFETCH(v) __builtin_prefetch(0,v)
#define WPREFETCH(v) __builtin_prefetch(1,v)
#define PREFETCH(v) RPREFETCH(v)

#else

#define RPREFETCH(v) DONOTHING
#define WPREFETCH(v) DONOTHING
#define PREFETCH(v) DONOTHING

#endif

#endif
