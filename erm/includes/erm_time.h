/**
 * erm_time.h
 *
 * Dave Taht
 * 2017-03-20
 */

#ifndef ERM_TIME_H
#define ERM_TIME_H

// Erm time is collected as the number of usec since the build date - a year
// (for wrap) in 124 bit values. (or 60 bit) There are 3.154e+13 us in a year.

// it is generational (so it can be adjusted forward or backwards)
// so when you need a consistent view of time you just look at your
// copy and update to the current gc if you want by taking deltas forward.
// which are kept around a few generations.

// One pleasing thing about this is that if an erm runs for that long
// without being recompiled we don't have a Y2038 problem. We have a
// problem that takes place smeared over the number of build dates and
// machines.

// we convert back to now only when needed.

// someone in 2080 will probably get mad at me for not using picoseconds.
// in fact, I'm tempted to use 10ns.

#endif
