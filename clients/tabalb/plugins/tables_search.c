/**
 * plugin_tests.c
 *
 * Dave Taht
 * 2017-03-18
 */
#include <stdint.h>
#include <sched.h>

#define YIELD sched_yield()
//#define tbl_a uint16_t
//#define tbl_b uint16_t
#define tbl_a uint32_t
#define tbl_b uint32_t

// FIXME: token pasting from hell and doesn't expand correctly yet.

#define PPASTE(t1,t2,v) t1##_##t2##_##v
#define PO(v) PPASTE(tbl_a,tbl_b,v)

#include "tables_search.h"

#ifdef DEBUG_MODULE
int main() {
	return 0;
}
#endif
