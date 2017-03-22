/**
 * sockets.c
 *
 * Dave Taht
 * 2017-03-14
 */

#include "debug.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <unistd.h>

//#include "sockets.h"

// unlike babeld we open one socket per interface
// We also open a udplite socket for grins

typedef struct sockaddr sockaddr_t;
typedef struct sockaddr_in6 sockaddr_in6_t;

// SO_MARK ?
// SO_PRIORITY
/*
    SO_RCVLOWAT and SO_SNDLOWAT Specify the minimum number of bytes in the
   buffer
    until the socket layer will pass the data to the protocol (SO_SNDLOWAT) or
    the user on receiving (SO_RCVLOWAT). These two values are initialized to
   1.
    SO_SNDLOWAT is not changeable on Linux (setsockopt(2) fails with the error
    ENOPROTOOPT). SO_RCVLOWAT is changeable only since Linux 2.4. The
   select(2)
    and poll(2) system calls currently do not respect the SO_RCVLOWAT setting
   on
    Linux, and mark a socket readable when even a single byte of data is
    available. A subsequent read from the socket will block until SO_RCVLOWAT
    bytes are available.

    SO_RCVTIMEO and SO_SNDTIMEO Specify the receiving or sending timeouts
   until
    reporting an error. The argument is a struct timeval. If an input or
   output
    function blocks for this period of time, and data has been sent or
   received,
    the return value of that function will be the amount of data transferred;
   if
    no data has been transferred and the timeout has been reached then -1 is
    returned with errno set to EAGAIN or EWOULDBLOCK, or EINPROGRESS (for
    connect(2)) just as if the socket was specified to be nonblocking. If the
    timeout is set to zero (the default) then the operation will never
   timeout.
    Timeouts only have effect for system calls that perform socket I/O (e.g.,
    read(2), recvmsg(2), send(2), sendmsg(2)); timeouts have no effect for
    select(2), poll(2), epoll_wait(2), and so on.
        */
/*

SO_SNDBUF

 */

// This is actually a trap or will be when I'm done

static int fail(int s)
{
  int saved_errno = errno;
  close(s);
  perror("mesg");
  BACKTRACE("err");
  errno = saved_errno;
  return -1;
}

typedef struct {
  u32 val;
  u32 ign;
  u32 proto;
  u32 opt;
} socket_defaults_t;

socket_defaults_t babel_defaults[] = {
  { 1, 1, IPPROTO_IPV6, IPV6_V6ONLY },
  { 0, 1, SOL_SOCKET, SO_REUSEADDR },
  { 1, 1, IPPROTO_IPV6, IPV6_MULTICAST_LOOP }, // ?
  { 0, 1, IPPROTO_IPV6, IPV6_UNICAST_HOPS },
  { 1, 1, IPPROTO_IPV6, IPV6_MULTICAST_HOPS },
#ifdef IPV6_TCLASS
  { 2, 0, IPPROTO_IPV6, IPV6_TCLASS }, // ECN
#endif
  { 0, 0, 0, 0 },
};

int erm_socket(sockaddr_in6_t* sin6, int size, int family, int conn, int port, int protocol, socket_defaults_t* o)
{
  int s, rc;
  if((s = socket(family, conn, protocol)) < 0) return -1;

  while(o->opt != 0) {
    int v = o->val;
    if((rc = setsockopt(s, o->proto, o->opt, &v, sizeof(int))) < 0 && o->ign)
      return fail(s);
    o++;
  }

  // NOTE: I am probably going to switch to blocking sockets
  // and selectively override them

  if((rc = fcntl(s, F_GETFL, 0)) < 0) return fail(s);
  if((rc = fcntl(s, F_SETFL, (rc | O_NONBLOCK))) < 0) return fail(s);
  if((rc = fcntl(s, F_GETFD, 0)) < 0) return fail(s);
  if((rc = fcntl(s, F_SETFD, rc | FD_CLOEXEC)) < 0) return fail(s);

  if((rc = bind(s, sin6, size)) < 0) return fail(s);

  return s;
}

socket_defaults_t tcp_defaults[] = { { 1, 1, SOL_SOCKET, SO_REUSEADDR }, { 0, 0, 0, 0 } };

int tcp_server_socket(int local, int port)
{
  int s = -1, rc;
  sockaddr_in6_t sin6 = { 0 };
  sin6.sin6_family = AF_INET6;
  sin6.sin6_port = htons(port);

  if(local)
    if((rc = inet_pton(AF_INET6, "::1", &sin6.sin6_addr)) < 0) return fail(s);

  if((s = erm_socket(&sin6, sizeof(sockaddr_in6_t), AF_INET6, SOCK_STREAM,
                        port, IPPROTO_TCP, tcp_defaults)) < 0)
    return fail(s);

  if((rc = listen(s, 2)) < 0) return fail(s);

  return s;
}

socket_defaults_t unix_defaults[] = { { 1, 0, SOL_SOCKET, SO_REUSEADDR },
                                      { 0, 0, 0, 0 } };

int unix_server_socket(const char* path)
{
  struct sockaddr_un sun = { 0 };
  int s, rc, saved_errno;

  if(strlen(path) >= sizeof(sun.sun_path)) return -1;

  strncpy(sun.sun_path, path, sizeof(sun.sun_path));

  if((s = erm_socket((sockaddr_in6_t*)&sun, sizeof(sun.sun_path), AF_UNIX,
                        SOCK_STREAM, 0, IPPROTO_TCP, unix_defaults)) < 0)
    return fail(s);

  if((rc = listen(s, 2)) < 0) goto fail_unlink;
  return s;

fail_unlink:
  saved_errno = errno;
  unlink(path);
  errno = saved_errno;
  return -1;
}

#ifdef DEBUG_MODULE
#define TEST_PORT 6669
int main()
{
  sockaddr_in6_t sin60 = { 0 };
  sockaddr_in6_t sin61 = { 0 };
  sockaddr_in6_t un = { 0 };
  sockaddr_in6_t tcp = { 0 };
  int fds[8];

  tcp.sin6_family = sin61.sin6_family = sin60.sin6_family = AF_INET6;
  tcp.sin6_port = sin61.sin6_port = sin60.sin6_port = htons(TEST_PORT);

  fds[0] = erm_socket(&sin60, sizeof(sockaddr_in6_t), AF_INET6, SOCK_DGRAM,
                         TEST_PORT, IPPROTO_UDP, babel_defaults);
  fds[1] = erm_socket(&sin61, sizeof(sockaddr_in6_t), AF_INET6, SOCK_DGRAM,
                         TEST_PORT, IPPROTO_UDPLITE, babel_defaults);
  fds[2] = unix_server_socket("/tmp/testtabeld.soc");
  fds[3] = tcp_server_socket(1, TEST_PORT);

  if(fds[0] < 0) printf("couldn't open babel socket\n");
  if(fds[1] < 0) printf("couldn't open babel-lite socket\n");
  if(fds[2] < 0) printf("couldn't open unix control socket\n");
  if(fds[3] < 0) printf("couldn't open tcp control socket\n");
  sleep(60);
  for(int i = 0; i < 8; i++) close(fds[i]);
  return 0;
}
#endif
