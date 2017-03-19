// string of tests to test correctness and speed

#include "tabeld.h"
#include "bench.h"

ipv6addr get_random_addr() {
	ipv6 addr a;
	b.u[0] = random();
	b.u[1] = random();
	b.u[2] = random();
	b.u[3] = random();
	a = b;
	return a;
}

addr_t a[MAX_ADDRS]
address_t s[MAX_ADDRS]

// What I need is a basic timing and testing
// framework that is not assert based
// and lets me quickly generate tests.
// I also want to feed results into a ringbuffer 
// rather than call a print routine directly

// * c? &c 

test_insertion_speed() {
	int r = 0;
	cycle_t c = get_cycles();
	for (int i=0; i<MAX_ADDRS; i++) {
		a[i] = classify(get_random_addr());
		s[i] = insert_ip(a[i]);
	}
	speed("Insert:", c, get_cycles());
	for (int i=0; i<MAX_ADDRS; i++) {
		r += s[i] == insert_ip(a[i]);
	speed("Search:", c, get_cycles());
	for (int i=0; i<MAX_ADDRS; i++) {
		r += s[i] == search_ip(a[i]);
	}
	speed("Update:", c, get_cycles());
	for (int i=0; i<MAX_ADDRS; i++) {
		r += s[i] == update_ip(a[i]);
	}
	// FIXME reset r to saner values, print result

	int lpm, nom, match = 0;

	speed("FindBest:", c, get_cycles());
	for (int i=0; i<MAX_ADDRS; i++) {
		lpm += s[i] == compare_ip_gt(a[i],a[i+512 & MAX_ADDRS]);
	}
	speed("CountDisjoint:", c, get_cycles());
	for (int i=0; i<MAX_ADDRS; i++) {
		nom += s[i] == compare_ip_lt(a[i],a[i+512 & MAX_ADDRS]);
	}
	speed("CountEqual:", c, get_cycles());
	for (int i=0; i<MAX_ADDRS; i++) {
		match += s[i] == compare_ip_eq(a[i],a[i+512 & MAX_ADDRS]);
	}

// These should get "marked" gone

	speed("Delete:", c, get_cycles());
	for (int i=0; i<MAX_ADDRS; i++) {
		r += s[i] == delete_ip(a[i]);
	}

// And then be missing

	speed("Delete check:", c, get_cycles());
	for (int i=0; i<MAX_ADDRS; i++) {
		r += 0 == delete_addrindx(s[i]);
	}
}
