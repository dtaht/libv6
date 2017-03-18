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

// trap_t tabled_trap(trap_t t); // true if handled successfully

// inline trap_t get_tabled_trap_mask(trap_t); // boolean to only see the traps you want

// should not be inline but I don't know what they do yet

// extern trap_t set_tabled_trap_mask(trap_t t);
// extern trap_t set_tabled_trap_cb(trap_t t, trap_callback_t c);

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

// I will compile these down differently later and come up with better semantics
// I'm tempted to add bitwise operators and __func__
// all of these need to be rewritten to take advantage of the trap register

#if !(defined (TRAP_BASIC) | defined(TRAP_BIG) | defined(TRAP_NONE))
#define TRAP_BIG
#endif

#ifdef TRAP_BASIC
#define TRAP_LT(operation,value,msg) if(unlikely((operation) < (value))) { perror(msg); exit(-1); }
#define TRAP_GT(operation,value,msg) if(unlikely((operation) > (value))) { perror(msg); exit(-1); }
#define TRAP_EQ(operation,value,msg) if(unlikely((operation) == (value)) ) { perror(msg); exit(-1); }

// In c true is any non zero value

#define TRAP_TRUE(operation,msg) if((operation)) { perror(msg); exit(-1); }
#define TRAP_FALSE(operation,msg) if(unlikely(!(operation))) perror(msg); exit(-1); }
#define TRAP_ERR(op,msg) TRAP_TRUE(op,msg)

#define TRAP_WTRUE(operation,msg) if((operation)) perror(msg)
#define TRAP_WFALSE(operation,msg) if(unlikely(!(operation))) perror(msg)
#define TRAP_WERR(op,msg) TRAP_WTRUE(op,msg)

// Synonyms

#define TRAP_NZ(a,b) TRAP_TRUE(a,b)
#define TRAP_ZERO(a,b) TRAP_FALSE(a,b)

#endif

#ifdef TRAP_BIG
// I learned something today. I thought errno was a tls variable that you just
// got. No. It's a subroutine. Yuck. I'm not doing that. To hell with errno as
// we know it... eventually.

//  400906:       e8 d5 fe ff ff          callq  4007e0 <__errno_location@plt>
//  40090b:       8b 38                   mov    (%rax),%edi

// incidentally, this is STILL buggy and scribbles on the stack in some undefined way
// where does __func__ come from?
// Not clear if this is the right way to get this right

void static inline errcmd (char *msg, const char *f) __attribute__ ((noreturn));
void static inline wrncmd (char *msg, const char *f);

static inline noreturn void errcmd(char *msg, const char *f) {
        int len = strlen(msg);
        int lenf = strlen(f);
	char buf[lenf + len + 8]; // FIXME: grab as fixed mem from elsewhere and this is not really an inline
	sprintf(buf,"%s in %s()",msg,f);
/*      it is not too early to stop using sprintf
	strncpy(buf,msg,len);
        buf[len++] = ':';
        buf[len++] = ' ';
	strncpy(&buf[len],f,strlen(f));
        buf[len++] = '(';
        buf[len++] = ')';
        buf[len++] = 0;
*/
	perror(buf); // where's my errno?
	exit(-1);
}

static inline void wrncmd(char *msg, const char *f) {
        int len = strlen(msg);
        int lenf = strlen(f);
	char buf[lenf + len + 8];
// FIXME: grab as fixed mem from elsewhere and this is not really an inline
	sprintf(buf,"warning: %s in %s()",msg,f);
	perror(buf); // where's my errno?
}

#define TRAP_LT(operation,value,msg) if(unlikely((operation) < (value))) errcmd(msg, __func__)
#define TRAP_GT(operation,value,msg) if(unlikely((operation) > (value))) errcmd(msg, __func__)
#define TRAP_EQ(operation,value,msg) if(unlikely((operation) == (value))) errcmd(msg, __func__)

// In c true is any non zero value
// in libraries an error return is usually -1 in other word - true. Sigh
// hmm, can builtin handle nonzero?

#define TRAP_TRUE(operation,msg) if((operation)) errcmd(msg,__func__)
#define TRAP_FALSE(operation,msg) if(unlikely(!(operation))) errcmd(msg,__func__)
#define TRAP_ERR(op,msg) TRAP_TRUE(op,msg)

#define TRAP_WTRUE(operation,msg) if((operation)) wrncmd(msg,__func__)
#define TRAP_WFALSE(operation,msg) if(unlikely(!(operation))) wrncmd(msg,__func__)
#define TRAP_WERR(op,msg) TRAP_WTRUE(op,msg)

// Synonyms

#define TRAP_NZ(a,b) TRAP_TRUE(a,b)
#define TRAP_ZERO(a,b) TRAP_FALSE(a,b)
#endif

#ifdef TRAP_NONE

// Go ahead, live dangerously.

#define TRAP_LT(operation,value,msg) operation
#define TRAP_GT(operation,value,msg) operation
#define TRAP_EQ(operation,value,msg) operation

// In c true is any non zero value

#define TRAP_TRUE(operation,msg) operation
#define TRAP_FALSE(operation,msg) operation
#define TRAP_ERR(op,msg) op

#define TRAP_WTRUE(operation,msg) operation
#define TRAP_WFALSE(operation,msg) operation
#define TRAP_WERR(op,msg) op

// Synonyms

#define TRAP_NZ(a,b) TRAP_TRUE(a,b)
#define TRAP_ZERO(a,b) TRAP_FALSE(a,b)

#endif

#endif
