/**
 * search_tests.c
 *
 * Dave Taht
 * 2017-03-18
 */

#include <stdint.h>

// This set of queries are aligned against 16 byte boundaries

#define B_ALIGNED_YES

#include "preprocessor.h"
#include "simd.h"

#ifndef GEN_TYPE
#define tbl_a u16
#define tbl_b u16
#else
#define tbl_a GEN_TYPE
#define tbl_b GEN_TYPE
#endif

// FIXME: token pasting from hell and doesn't expand correctly yet.

#define PPASTE(t1, t2, v) t1##_##t2##_##v
#define PO(v) PPASTE(tbl_a, tbl_b, v)

#include "tables_search.h"

#ifdef DEBUG_MODULE
MIGNORE
#endif
