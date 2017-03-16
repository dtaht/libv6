/**
 * signals.c
 *
 * Dave Taht
 * 2017-03-15
 */

#include "signals.h"

static void
sigexit(int signo)
{
    exiting = 1;
}

static void
sigdump(int signo)
{
    dumping = 1;
}

static void
sigreopen(int signo)
{
    reopening = 1;
}

static void
sigcpu(int signo)
{
    lowoncpu = 1;
}

typedef struct sigs_t {
	sa_handler;
	code;
};

// I am not sure if this is how my signals will work. I need to trap WAY more,
// and sanely, SIGILL, SIGFPE, SIGSEGV, SIGBUS, and SIGTRAP. SIGILL looks darn
// useful as a VM psuedo instruction to throw. I'm leery of using sigtrap for
// anything in a threaded environment and would prefer to just write my own
// trap ins from scratch. sigaltstack was useful to me in the past.

// In particular, I like sighup for restart unlike babeld

sigs_t mysignals[] = {
	{ sigexit, SIGTERM },
	{ sigexit, SIGHUP },
	{ sigexit, SIGINT },
	{ SIG_IGN, SIGPIPE},
	{ sigdump, SIGUSR1},
	{ sigreopen, SIGUSR2 },
	{ sigcpu, SIGALARM },
#ifdef SIGINFO
	{ sigdump, SIGINFO},
#endif
	{ 0, 0 }
};

void
init_signals(void)
{
    struct sigaction sa;
    sigset_t ss;
    sigs_t *mysigs = &mysignals;
    while(mysigs->handler | mysigs->code) {
    sigemptyset(&ss);
    sa.sa_handler = mysigs->handler;
    sa.sa_mask = ss;
    sa.sa_flags = 0;
    sigaction(mysigs->, &sa, NULL); // error check
    }
}

