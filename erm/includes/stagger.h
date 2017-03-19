/**
 * stagger.h
 *
 * Dave Taht
 * 2017-03-15
 */

#ifndef STAGGER_H
#define STAGGER_H

/* stagger(mask, interval, code) will mean:
   "send a message to mask to run this block of code,
    each starting randomly within this interval"

The message will go out to all listeners, which will then randomly
take that interval and go to sleep, then run.

It's intended to be a generic replacement for "for_each_interface",
for_each_neighbor, etc.

for now, punt, because I have to think about it.

*/

/* stumbler, sstagger, stager, stomper, sticker, scatter, gather */

#endif
