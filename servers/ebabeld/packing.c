/**
 * test_packing.c
 */

#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include "preprocessor.h"
#include "erm_types.h"
#include "arch.c"
#include "babel_tlv.h"
#include "babel_msg.h"

#define P(a)    printf("%s %s %ld\n", argv[0], #a, sizeof(a))

int main(int argc, char ** argv)
{
#ifdef __GNUC__
    printf("Compiled with gcc %d.%d.%d for the %s "
        "architecture in %s mode\n",
        __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__,
        arch, __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ ?
        "little endian" : "big endian");
#endif
//   These are enums, but get down to bits
//   P(babel_tlv_type);
//   P(babel_iface_type);
//   P(babel_ae_type);
    P(babel_msg_ack_req);
    P(babel_msg_hello);
    P(babel_msg_ihu);
    P(babel_msg_update);
    P(babel_msg_route_request);
    P(babel_msg_seqno_request);
    printf("Union of all types: ");
    P(babel_msg);

    printf("Babel TLVs\n");
    P(babel_pkt_header);
    P(babel_tlv);
    P(babel_tlv_ack_req);
    P(babel_tlv_ack);
    P(babel_tlv_hello);
    P(babel_tlv_ihu);
    P(babel_tlv_router_id);
    P(babel_tlv_next_hop);
    P(babel_tlv_update);
    P(babel_tlv_route_request);
    P(babel_tlv_seqno_request);

    /* have not go so far as internal data structures yet
    P(filter_result);
    P(filter);
    P(buffered_update);
    P(interface_conf);
    P(interface);
    P(kernel_route);
    P(kernel_rule);
    P(kernel_filter);
    P(local_socket);
    P(neighbour);
    P(resend);
    P(babel_route);
    P(source);
    P(xroute);
    P(zone);
    */
    return 0;
}
