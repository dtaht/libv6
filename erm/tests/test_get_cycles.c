#include <stdio.h>
#include "get_cycles.h"

// Can I trap the invalid instruction and make the syscall instead?

int main() {
	cycles_t s = 0;
	cycles_t e = 0;
	s = get_cycles();
	printf("Start ticks:" CYCLES_FMT "\n", s);
	e = get_cycles();
	printf("End ticks: " CYCLES_FMT "\n", e);
	printf("Difference: " CYCLES_FMT "\n", e-s);
	return 0;
}

