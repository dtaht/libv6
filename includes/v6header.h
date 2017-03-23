#ifndef _V6HEADER
#define _V6HEADER
#include <netinet/in.h>
#include <netinet/in6.h>

// ipv6 was designed long before C11.

typedef struct in6_addr in6_addr_t;
typedef struct sockaddr_in6 sockaddr_in6_t; // sigh in6_addr here
typedef struct ipv6_mreq inv6_mreq_t;
typedef struct in6_flowlabel_req in6_flowlabel_req_t;
typedef struct in6_u in6_u_t;

// 

// #define IN6_IS_ADDR_ULA

// need moral equivalents of in6_addr_any - which is in network byte order
// https://en.wikipedia.org/wiki/Endianness
#endif
