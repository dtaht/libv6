
enum ksockets { v6rnotify, v6anotify, v4rnotify, v4anotify, linknotify, rulenotify, MAXNOTIFY };

typedef struct {
    unsigned short seqno;
    int sock;
    struct sockaddr_nl sockaddr;
    socklen_t socklen;
} netlink_socket;

typedef struct {
  short status:MAXNOTIFY;
  netlink_socket sockets[MAXNOTIFY];
} kernel_sockets;
