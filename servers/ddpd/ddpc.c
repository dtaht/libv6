#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <locale.h>
#include <assert.h>
#include <getopt.h>
#include <iconv.h>
#include <fcntl.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/timerfd.h>

#include "v6lib.h"

#define PORT 5100
#define MAXBUF 65536
#define CONFFILE "/etc/config/ddpc"

// for each prefix provider
// use fdtimers instead of a timer wheel
// - adds simplicity to callbacks

// Commands to find equivalents for in netlink
// ip route show proto %s proto > %s
// ip addr show scope global
// ip -6 addr show scope global
// ip -6 addr replace %s proto %s lft %s
// ip -6 route replace %s proto %s expires %s
// time
// List the addresses on an interface

// timer check
// long long fired?


//		if(write(tool,cmd,csize)==-1) perror("writing cmd");
//		if(read(timer,&fired,sizeof(fired))!=8) perror("reading timer");
//		ctr+=fired;

static int timer_callback_create(int interval, int when) {
	struct itimerspec new_value = {0};
	int timer = timerfd_create(CLOCK_REALTIME, 0);
	new_value.it_interval = a->interval;
	new_value.it_value = a->interval;
        return timer;
}

backoff() {

}

establish_self() {

// Do I have routes?
// Do I have an IP address?
// Do I have an IPv6 address?
// Can I create a temporary ULA and announce it?

}

// After thinking about this for a while, even though
// this is protocol that needs not more than udp,
// the crypto requirement makes that harder.
// Using TCP instead simplifies some things
// but I need to think about blockage caused
// by hanging tcp sessions.
// We can also try tcp fast open on the crypto side

int client(int argc, char* argv[])
{
   int sock;
   int status;
   struct addrinfo sainfo, *psinfo;
   struct sockaddr_in6 sin6;
   int sin6len;
   char buffer[MAXBUF];
   u128 prefix = v6_gen_random_prefix(96);

   sin6len = sizeof(struct sockaddr_in6);

   sock = socket(PF_INET6, SOCK_DGRAM,0);

   memset(&sin6, 0, sizeof(struct sockaddr_in6));
   sin6.sin6_port = htons(0);
   sin6.sin6_family = AF_INET6;
   sin6.sin6_addr = in6addr_any;

   status = bind(sock, (struct sockaddr *)&sin6, sin6len);

   if(-1 == status)
     perror("bind"), exit(1);

   memset(&sainfo, 0, sizeof(struct addrinfo));
   memset(&sin6, 0, sin6len);

   sainfo.ai_flags = 0;
   sainfo.ai_family = PF_INET6;
   sainfo.ai_socktype = SOCK_DGRAM;
   sainfo.ai_protocol = IPPROTO_TCP;
   status = getaddrinfo("ip6-localhost", "5100", &sainfo, &psinfo);

   switch (status)
     {
      case EAI_FAMILY: printf("family\n");
	break;
      case EAI_SOCKTYPE: printf("stype\n");
	break;
      case EAI_BADFLAGS: printf("flag\n");
	break;
      case EAI_NONAME: printf("noname\n");
	break;
      case EAI_SERVICE: printf("service\n");
	break;
     }
   if(s > 0) {
   status = sendto(sock, buffer, strlen(buffer), 0,
		     (struct sockaddr *)psinfo->ai_addr, sin6len);
   printf("buffer : %s \t%d\n", buffer, status);
}
   // free memory
   freeaddrinfo(psinfo);
   psinfo = NULL;

   shutdown(sock, 2);
   close(sock);
   return 0;
}

int server(int argc, char *argv[])
{
   int sock;
   int status;
   struct sockaddr_in6 sin6;
   int sin6len;
   char buffer[MAXBUF];

   sock = socket(PF_INET6, SOCK_DGRAM,0);

   sin6len = sizeof(struct sockaddr_in6);

   memset(&sin6, 0, sin6len);

   /* FIXME: DON'T just use the first address returned in the structure */

   sin6.sin6_port = htons(PORT);
   sin6.sin6_family = AF_INET6;
   sin6.sin6_addr = in6addr_any;

   status = bind(sock, (struct sockaddr *)&sin6, sin6len);
   if(-1 == status)
     perror("bind"), exit(1);

   status = getsockname(sock, (struct sockaddr *)&sin6, &sin6len);

   printf("%d\n", ntohs(sin6.sin6_port));

   while(1) {
   status = recvfrom(sock, buffer, MAXBUF, 0,
		     (struct sockaddr *)&sin6, &sin6len);

   printf("buffer : %s\n", buffer);
   }
   shutdown(sock, 2);
   close(sock);
   return 0;
}

void usage(int argc, char *argv[]) {
	printf("Usage: %s must be invoked as either ddpc or ddpd\n", argv[0]);
	printf("%s: -p port -d debug -c conf -a address_or_dns\n"
               "    -P proto -s script -K keydir \n",
		argv[0]);
	exit(-1);
}

struct arg {
	u16 port;
	char *conf;
	char *address;
	char *keydir;
	char *proto;
	char *script;
	int debug;
	char *timeout;
};

typedef struct arg args;

// Capabilities? IF we can do this with netlink sockets
// The only capabilities we need to keep are the ability
// to read and write netlink sockets for routing and
// address changes

// minimal? - force advertising the derived leases
// and routes at some subset of the actual lease - 5m?

static const struct option long_options[] = {
	{ "port", required_argument	, NULL , 'p' } ,
	{ "conf"   , required_argument	, NULL , 'c' } ,
	{ "address", required_argument	, NULL , 'a' } ,
	{ "keydir" , required_argument	, NULL , 'K' } ,
	{ "proto"  , required_argument	, NULL , 'P' } ,
	{ "script" , required_argument	, NULL , 's' } ,
	{ "timeout", required_argument, NULL , 't' } ,
	{ "debug"  , required_argument	, NULL , 'd' } ,
	{ "retry"  , required_argument	, NULL , 'r' } ,
	{ "help"     , no_argument		, NULL , 'h' },
};

#define QSTRING "p:c:a:K:P:s:t:d:r:h"

static int process_options(int argc, char **argv, args *o)
{
	int          option_index = 0;
	int          opt = 0;
	optind       = 1;

	while(1)
	{
		opt = getopt_long(argc, argv,
				  QSTRING,
				  long_options, &option_index);
		if(opt == -1) break;

		switch (opt)
		{
		case 'p': o->port = strtoul(optarg,NULL,10);  break;
		case 'c': o->finterval = strtod(optarg,NULL); break;
		case 'a': o->command = optarg; break;
		case 'K': o->interface = optarg; break;
		case 'P': o->buffer = 1; break;
		case 't': o->buffer = 1; break;
		case 'd': o->buffer = 1; break;
		case 'r': o->buffer = 1; break;
		case '?':
		case 'h':
		default:  usage(NULL);
		}
	}
	return 0;
}

void main(int argc, char* argv[]) {
	char *prog = basename(argv[0]);
	args a;
	int status = 0;
	defaults(&a);
	process_options(&a);
        assigned = calloc(size,sizeof(v6_prefix_table));
	v6_store_prefix(assigned[4].prefix,v6_ones);
	v6_sanity(assigned[4].prefix);
	if(strcmp(prog,"ddpd") == 0) server(argc,argv);
      	else if(strcmp(prog,"ddpc") == 0) client(argc,argv);
	else usage(argc, argv);
	exit(0);
}
