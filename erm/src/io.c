/**
 * io.c
 *
 * Dave Taht
 * 2017-03-12
 */

#include "shared.h"
#include "io.h"
#include "debug.h"
#include "init.h"

bool is_v4mapped(const ubase_t u) {
return (addrdata[u].v4);
}

bool is_v4mapped_str(const char *u) {
	return 1;
}

// I will probably regret this unless I make it atomic or per cpu or per
// thread

static char netbuf[8][INET6_ADDRSTRLEN];
static int neti = 0;
#define getnetbuf(cpu) netbuf[++neti % 8];

// Why do we not have the plen check here?

const char *format_address_str(const char *a) {
  char* buf = getnetbuf(NULL); // const is a lie
  inet_ntop(is_v4mapped_str(a) ? AF_INET : AF_INET6,
  a + 12 * is_v4mapped_str(a), buf, INET6_ADDRSTRLEN);
  return buf;
}

char* format_address_ref(const ubase_t a)
{
  char* address = (char*)&addresses[a];
  char* buf = getnetbuf(NULL); // const is a lie

  inet_ntop(is_v4mapped(a) ? AF_INET : AF_INET6,
            address + 12 * is_v4mapped(a), buf, INET6_ADDRSTRLEN);
  return buf;
}

const char* format_prefix_str(const char *a)
{
  char* buf = format_address_str(a);
  int plen = strlen(a);
  /* might be pushing my luck here, using the buf twice */
  snprintf(buf, strlen(buf) + 4, "%s/%d", buf,
           (plen >= 96) * is_v4mapped_str(a) ? plen - 96 : plen);
  return buf;
}

const char* format_prefix_ref(const ubase_t a)
{
  char* buf = format_address_ref(a);
  int plen = addrdata[a].len;
  /* might be pushing my luck here, using the buf twice */
  snprintf(buf, strlen(buf) + 4, "%s/%d", buf,
           (plen >= 96) * is_v4mapped(a) ? plen - 96 : plen);
  return buf;
}

const char* format_eui64(const unsigned char* eui)
{
  char* buf = getnetbuf(NULL);
  snprintf(buf, 28, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", eui[0], eui[1],
           eui[2], eui[3], eui[4], eui[5], eui[6], eui[7]);
  return buf;
}

const char* format_thousands(unsigned int value)
{
  char* buf = getnetbuf();
  snprintf(buf, 15, "%d.%.3d", value / 1000, value % 1000);
  return buf;
}

#ifdef DEBUG_MODULE
addrflags_t* addrdata;
ip_addr *addresses;
int main() {
	addresses = calloc(255,sizeof(ip_addr));
	printf("do some io!\n");
	return -1;
}
#endif
