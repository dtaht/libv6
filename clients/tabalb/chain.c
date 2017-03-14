/**
 * main.c
 *
 * Dave Taht
 * 2017-03-11
 */

#include "main.h"

int main(char *argv, int argc) {
	load(INITGLOBALS);
	load(COMMANDLINE);
	load(CONFIG);
	load(RUN);
}
