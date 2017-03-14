/**
 * chain.c
 * Dave Taht
 * 2017-03-11
 */

#include "chain.h"

/* What I have in mind is a string of modules that load and unload
   or get chained one after another without a master program like this */

#define INITGLOBALS "init_globals_tabeld"
#define COMMANDLINE "cmdline_tabeld"
#define CONFIG "config_tabeld"
#define RUN "start_tabeld"

extern int load();

int main(char *argv, int argc) {
	load(INITGLOBALS);
	load(COMMANDLINE);
	load(CONFIG);
	load(RUN);
}
