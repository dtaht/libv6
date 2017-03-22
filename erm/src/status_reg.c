/**
 * status_reg.c
 *
 * Dave Taht
 * 2017-03-22
 */

#include "status_reg.h"

// Locking? Who needs locking when you have a dedicated register?

extern global_flags_t status_reg; // asm("q15");

// spill();
// merge();

// You don't need getters and setters per se'

// set();
// get();

reg_t status_spill() {
	status_save(commoncpuarea,getcpunum());
	// hmm get the last known global status here? next cpu up?
	return commoncpuarea[-1];
}

// global = maxcpus() is too dynamic

// for ints as opposed to doing it in sse

reg_t status_merge() {
	reg_t myreg = {0};
	for(int i = 0; i < maxcpus(); i++) {
		myreg |= commoncpuarea[i];
	}
	status_save(commoncpuarea[-1],myreg);
	return myreg;
}

do_something() {
	if(madechange = do_something_else())
	     status_reg.gc = 1;
}

moveon() {
// now you can always collapse back to some other well defined task
while(status_merge() == status_reg) {
	do_something();
}
return;
}

