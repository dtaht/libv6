/**
 * popvec.c
 *
 * Dave Taht
 * 2017-03-12
 */

#include "popvec.h"

static uint8_t asizes[128]; // current size of the table must be a power of two
static uint16_t atable[128]; // offsets? ptrs? to starting points?

/*

a = get_startpoint(popcnt_t p) { return atable[p]; }
index << 5; ? use index + offset + ?

*/

