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
#include "v6lib.h"

#define PORT 5100
#define MAXBUF 65536
#define CONFFILE "/etc/config/ddpc"

size_t pos = 0;
size_t size = 127;
size_t begin = 0;
size_t end = 0;

static v6_prefix_table *assigned;

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
   sainfo.ai_protocol = IPPROTO_UDP;
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
   int s = v6_find_prefix(assigned, size, prefix, 48);
   s = v6_find_prefix(assigned, size, prefix, 64);
   s = v6_find_prefix(assigned, size, prefix, 96);
   s = v6_find_prefix(assigned, size, prefix, 127);
   s = v6_find_prefix(assigned, size, prefix, 128);

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

   /* just use the first address returned in the structure */

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

void main(int argc, char* argv[]) {
	char *prog = basename(argv[0]);
        assigned = calloc(size,sizeof(v6_prefix_table));
	v6_store_prefix(assigned[4].prefix,v6_ones);
	v6_sanity(assigned[4].prefix);
	if(strcmp(prog,"ddpd") == 0) server(argc,argv);
      	else if(strcmp(prog,"ddpc") == 0) client(argc,argv);
	else usage(argc, argv);
	exit(0);
}
