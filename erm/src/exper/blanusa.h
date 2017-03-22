/**
 * The blanusa snarks
 * blanusa.h
 *
 * Dave Taht
 * 2017-03-21
 */

// This is one of those functions that is so totally obviously unsafe
// to have as a macro. I don't even trust pow in this case.

generic blanusa(generic x); // PURE;

generic blanusa(generic x) {
return pow((x-3)*(x-1),3) * ((x+1)*(x+2)) *
	(pow(x,4) + pow(x,3) - 7 * pow(x,2) - 5 * x + 6) *
	 pow(pow(x,4) + pow(x,3) - 5 * (pow(x,2) - 3 * x + 4),2);
}
