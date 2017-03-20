/**
 * plugin_tests.c
 *
 * Dave Taht
 * 2017-03-18
 */
#include <sched.h>
#include <stdint.h>

#define tbl_a uint16_t
#define tbl_b uint16_t

// FIXME: token pasting from hell and doesn't expand correctly yet.

#define PPASTE(t1, t2, v) t1##_##t2##_##v
#define PO(v) PPASTE(tbl_a, tbl_b, v)

// This set of queries are aligned against 16 byte boundaries

#define B_ALIGNED_YES

#include "tables_search.h"

#ifdef DEBUG_MODULE
MIGNORE
#endif
