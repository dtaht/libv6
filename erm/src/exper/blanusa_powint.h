/**
 * The blanusa snarks
 * blanusa.h
 *
 * Dave Taht
 * 2017-03-21
 */

// rewritten to not use pow as we are raising things to a constant int
// This is one of those functions that is so totally obviously unsafe
// to have as a macro. I don't even trust mypow in this case.
// I wonder if I can still outsmart the compiler here?

#define pow2(a) ((a) * (a))
#define pow3(a) ((a) * (a) * (a))
#define pow4(a) ((a) * (a) * (a) * (a) )

generic blanusa_powint(generic x); // PURE;

generic blanusa_powint(generic x) {
return pow3((x-3)*(x-1)) * ((x+1)*(x+2)) *
	(pow4(x) + pow3(x) - 7 * pow2(x) - 5 * x + 6) *
	 pow2(pow4(x) + pow3(x) - 5 * (pow2(x) - 3 * x + 4));
}
