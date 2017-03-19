/**
 * init.h
 *
 * Dave Taht
 * 2017-03-12
 */

#ifndef INIT_H
#define INIT_H

typedef struct {
  void* data;
  ubase_t idx;
  ubase_t size;
} v6table;

extern v6addr_t* addresses;
extern addrflags_t* addrdata;

extern v6table routers;

#endif
