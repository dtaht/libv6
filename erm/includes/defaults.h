/**
 * defaults.h
 *
 * Dave Taht
 * 2017-03-17
 */

#ifndef DEFAULTS_H
#define DEFAULTS_H

#if !(defined(SYSTEM_SMALL) | defined(SYSTEM_BIG))
#define SYSTEM_SMALL
#endif

// Reasonable defaults for a small system

#ifdef SYSTEM_SMALL
#define BASE 64
#define NUM_ROUTERS 4
#define NUM_INTERFACES 4
#endif

// 16 thousand routes is the biggest babel-ish network I know of
// and 2k is where babeld falls over today. So... hit it with a hammer

#ifdef SYSTEM_BIG
#define BASE (1024 * 16)
#define NUM_ROUTERS 64
#define NUM_INTERFACES 32
#endif


#endif
