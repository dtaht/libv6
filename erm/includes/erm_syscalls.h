/**
 * erm_syscalls.h
 *
 * Dave Taht
 * 2017-03-23
 */

#ifndef ERM_SYSCALLS_H
#define ERM_SYSCALLS_H

// FIXME: I'll need an internal and external version of these

#ifndef __epiphany__
#include "arch/syscalls_epiphany.h"
#else
#include "arch/syscalls_linux.h"
#endif

#endif
