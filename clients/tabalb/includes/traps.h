/**
 * traps.h
 *
 * Dave Taht
 * 2017-03-13
 */

#ifndef TRAPS_H
#define TRAPS_H

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

// trap_t tabled_trap(trap_t t); // true if handled successfully

// inline trap_t get_tabled_trap_mask(trap_t); // boolean to only see the traps you want

// should not be inline but I don't know what they do yet

// extern trap_t set_tabled_trap_mask(trap_t t);
// extern trap_t set_tabled_trap_cb(trap_t t, trap_callback_t c);

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

// I will compile these down differently later and come up with better semantics

#define TRAP_LT(operation,value,msg) if(unlikely((operation) < (value))) { perror(msg); exit(-1); }
#define TRAP_GT(operation,value,msg) if(unlikely((operation) > (value))) { perror(msg); exit(-1); }
#define TRAP_EQ(operation,value,msg) if(unlikely((operation) == (value)) ) { perror(msg); exit(-1); }

#define TRAP_NZ(operation,msg) if((operation)) { perror(msg); exit(-1); }
#define TRAP_ZERO(operation,msg) if((!operation) { perror(msg); exit(-1); }

#endif
