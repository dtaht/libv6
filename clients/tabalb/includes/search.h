/**
 * search.h
 *
 * Dave Taht
 * 2017-03-14
 */

#ifndef SEARCH_H
#define SEARCH_H

// Some simple searches via preprocessor magic
// my hope is to be able to express a complete filtering language this way
// op can only be ! or eq? or can it? search would be an inline function declaration

#define IP6search(addr, op, opt) search(ip6table, op addr.opt, addr.address)

IP6search(addr,, plen)
IP6search(addr,, pop)
IP6search(addr,!,dead)
IP6search(addr,,martian)
IP6search(addr,!,stable)

#endif
