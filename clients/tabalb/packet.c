/**
 * packet.c
 *
 * Dave Taht
 * 2017-03-13
 */

#include "shared.h"
#include "packet.h"

/* Parse an address, encoded in the somewhat baroque compressed
   representation used by Babel.  Return a struct containing the 
   classified address. 

ae == 0 full address ?
ae == 1 ipv4 address
ae == 2
ae == 3 ll address

I imagined that ae == 4 will one day 
I have another ae compression method in mind - a "aggregated route"
*/

// A goto in a macro? What could go wrong? How I am going to get the
// addr over right? As a string? what?

#define packet_err(e) { debug_log_address(1,1, e, addr); goto err; }

//  extern fulladdr_t bogus_addr; ?

fulladdr_t classify_address(int ae, int plen, unsigned int omitted,
               const unsigned char *p, const unsigned char *dp,
               unsigned int len, unsigned char *p_r)
{
    unsigned pb;

/*  fulladdr_t addr = bogus_addr;
    actually bogus is an index into the first entry of the addr table
    eventually. And that will probably be a register.
    fulladdr_t addr;
    addr.addr  = addrs[0];
    addr.flags = addrflags[0];
*/
    fulladdr_t addr = {0};
    addr.flags.len = addr.flags.pop = 255; // bogus packet is 255s

    if(plen >= 0)
	    pb = (plen + 7) / 8; // Mask artifact?
    else if(ae == 1) {
        pb = 4;
	addr.flags.v4 = 1;
        } else {
        pb = 16;
	addr.flags.v6 = 1;
    }

    if(pb > 16) packet_err("Packet encoding too long");

    switch(ae) {
    case 0: // ?? unencoded packet? what?
        break;
    case 1:
        if(omitted > 4 || pb > 4 || (pb > omitted && len < pb - omitted))
		packet_err("Bogus ipv4 packet encoding");
	addr.address = v4prefix;
        if(omitted) {
            if(dp == NULL || !v4mapped(dp))
		packet_err("Bogus ipv4 packet encoding");

	    addr.address.u[3] = dp & 0xFF << omitted; // wrong: pull bytes here
        }
        if(pb > omitted) memcpy(prefix + 12 + omitted, p, pb - omitted);
        ret = pb - omitted; // not sure why this exists
        break;
    case 2:
	    if(omitted > 16 || (pb > omitted && len < pb - omitted))
		    packet_err("Packet omitted offset wrong");

	    if(omitted) {
		if(dp == NULL || v4mapped(dp))
		    packet_err("Packet omitted offset wrong");

		memcpy(prefix, dp, omitted);
        }
        if(pb > omitted) memcpy(prefix + omitted, p, pb - omitted);
        ret = pb - omitted;
        break;
    case 3:
	    if(pb > 8 && len < pb - 8)
		    packet_err("LL Packet omitted offset wrong"); 
        prefix[0] = 0xfe;
        prefix[1] = 0x80;
        addr.flags.ll = 1;
        if(pb > 8) memcpy(prefix + 8, p, pb - 8);
        ret = pb - 8;
        break;
    default: goto err;
    }
    addr.flags.popcnt = popcount(addr.address);
//?     normalize_prefix(p_r, prefix, plen < 0 ? 128 : ae == 1 ? plen + 96 : plen);
    addr.flags.len = plen;

err:
    return addr;
}



}
