/**
 * init.h
 *
 * Dave Taht
 * 2017-03-12
 */

#ifndef INIT_H
#define INIT_H
#include <stdint.h>
#include "preprocessor.h"
#include "erm_types.h"
#define ERM_SHARED_MEM_PATTERN "%s-machine"
#define ERM_SHARED_DIR_PATTERN "/dev/shm%s"

typedef struct {
  void* data;
  ubase_t idx;
  ubase_t size;
} v6table;

extern ip_addr* addresses;
extern addrflags_t* addrdata;

extern v6table routers;

#endif
