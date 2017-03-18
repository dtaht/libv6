/**
 * plugin_tests.c
 *
 * Dave Taht
 * 2017-03-18
 */
#include <sched.h>

#define YIELD sched_yield()
#define tbl_a int
#define tbl_b int

#include "plugin_tests.h"

#ifdef DEBUG_MODULE
int main() {

	return 0;
}
#endif
