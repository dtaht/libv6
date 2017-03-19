/**
 * popvec.c
 *
 * Dave Taht
 * 2017-03-12
 */

#include "debug.h"
#include "shared.h"

static uint8_t asizes[128];  // current size of the table must be a power of
                             // two
static uint16_t atable[128]; // offsets? ptrs? to starting points?

/*

a = get_startpoint(popcnt_t p) { return atable[p]; }
index << 5; ? use index + offset + ?

*/

ubase_t popsorted[128];

// in all these functions 0 is failed

/*
basetype init(table,offset,value);
basetype insert(table,offset,value);
delete (table,offset,value);

lookup(plen_t);
lookup_pop(table,value);
lookup_plen(table,value)
lookup_addr(table,value)
return_set()

*/

/*
we can improve the basic search time by leveraging that the max(pop)
is relative to the min(plen)

solvers
min_cost
min_length

// need a union - popplen;

// r += src.pop xor dst[i].pop xor src.plen xor dst.plen;

// this is subtly garunteed to fail. r might - really rarely - overflow
// back to 0. The odds of this are many million to one, but in a 16 bit
// system it will produce a wrong result every 64000 times or so.

// But it's one of those things that just may be "correct enough".
// If we miss on a search once, oh well, things are immutable
// we'll garbage collect it eventually

// r += src.popplen xor dst[i].popplen;

// unroll = min(MAX_TABLE,8)

// with 256 routes it cuts the size of the search space by a factor of 8.
// r == 0 && src.popplen == dst.popplen when false = 0
// and doesn't increment when false, causing the backtrack to have to
// go to the beginning of the loop and re-run from there.

// r += && src.popplen == dst.popplen when false = 1
// has the pleasing property of giving you an exact figure on what to
// backtrack to. Seven times through the loop after a hit, r becomes 7.

// sadly it creates a carried dependency that you can't run stuff
// unrolled unless you do it by hand

insert(int offset, size) {
        int i = offset;
start:	for (i, i < size % MAX_TABLE, i++)	{
                r += src.pop == dst[i].pop && src.plen==dst.plen;
        }
        // no need to manually prefetch nowadays
        PREFETCH(dst[i+1],0,3); // check reads ?1
        if(r) { // some non-zero value but backtracking is cheap
                oldi = i - MAX_TABLE;
                for(i; i< oldi+MAX_TABLE; i++) { fixme
                                if (src.poplen == dst[i].popplen)
                                        search_harder();
                }
                // backtrack
        } else {
                if(++i > size - MAX_TABLE) grow_table(); backtrack a bit;
                insert(i, oldsize);
        } else
                goto start;
        }
}

search(int offset, size) {
        int i = offset;
        int r = 0;
start:	for (i, i < size % MAX_TABLE, i++)	{
                r += src.pop == dst[i].pop && src.plen == dst.plen;
        }
        if(r) {
                // backtrack
        } else {
                if(++i > size - MAX_TABLE) return 0; // notfound
                goto start;
        }

}

search
*/

#ifdef DEBUG_MODULE
MFIXME
#endif
