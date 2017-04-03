/*
 *	ERM -- The Babel message format
 *
 *	© 2015--2017 Toke Høiland-Jørgensen
 *	© 2017 Michael David Täht
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ERM_BABEL_H_
#define _ERM_BABEL_H_

#define EA_BABEL_METRIC EA_CODE(EAP_BABEL, 0)
#define EA_BABEL_ROUTER_ID EA_CODE(EAP_BABEL, 1)

#define BABEL_MAGIC 42
#define BABEL_VERSION 2
#define BABEL_PORT 6696
#define BABEL_INFINITY 0xFFFF

#define BABEL_HELLO_INTERVAL_WIRED 4 /* Default hello intervals in seconds */
#define BABEL_HELLO_INTERVAL_WIRELESS 4
#define BABEL_UPDATE_INTERVAL_FACTOR 4
#define BABEL_IHU_INTERVAL_FACTOR 3
#define BABEL_IHU_EXPIRY_FACTOR(X) ((X)*3 / 2)   /* 1.5 */
#define BABEL_HELLO_EXPIRY_FACTOR(X) ((X)*3 / 2) /* 1.5 */
#define BABEL_ROUTE_EXPIRY_FACTOR(X) ((X)*7 / 2) /* 3.5 */
#define BABEL_ROUTE_REFRESH_INTERVAL                                           \
  2 /* Seconds before route expiry to send route request */
#define BABEL_HOLD_TIME 10 /* Expiry time for our own routes */
#define BABEL_RXCOST_WIRED 96
#define BABEL_RXCOST_WIRELESS 256
#define BABEL_INITIAL_HOP_COUNT 255
#define BABEL_MAX_SEND_INTERVAL 5
#define BABEL_TIME_UNITS 100 /* On-wire times are counted in centiseconds */
#define BABEL_SEQNO_REQUEST_EXPIRY 60
#define BABEL_GARBAGE_INTERVAL 300

/* Max interval that will not overflow when carried as 16-bit centiseconds */
#define BABEL_MAX_INTERVAL (0xFFFF / BABEL_TIME_UNITS)

#define BABEL_OVERHEAD (IP6_HEADER_LENGTH + UDP_HEADER_LENGTH)
#define BABEL_MIN_MTU (512 + BABEL_OVERHEAD)

enum babel_tlv_type {
  BABEL_TLV_PAD1 = 0,
  BABEL_TLV_PADN = 1,
  BABEL_TLV_ACK_REQ = 2,
  BABEL_TLV_ACK = 3,
  BABEL_TLV_HELLO = 4,
  BABEL_TLV_IHU = 5,
  BABEL_TLV_ROUTER_ID = 6,
  BABEL_TLV_NEXT_HOP = 7,
  BABEL_TLV_UPDATE = 8,
  BABEL_TLV_ROUTE_REQUEST = 9,
  BABEL_TLV_SEQNO_REQUEST = 10,
  BABEL_TLV_TS_PC = 11,
  BABEL_TLV_HMAC = 12,
  BABEL_TLV_SS_UPDATE = 13,
  BABEL_TLV_SS_REQUEST = 14,
  BABEL_TLV_SS_SEQNO_REQUEST = 15,
  BABEL_TLV_MAX
};

enum babel_iface_type {
  /* In practice, UNDEF and WIRED give equivalent behaviour */
  BABEL_IFACE_TYPE_UNDEF = 0,
  BABEL_IFACE_TYPE_WIRED = 1,
  BABEL_IFACE_TYPE_WIRELESS = 2,
  BABEL_IFACE_TYPE_MAX
};

enum babel_ae_type {
  BABEL_AE_WILDCARD = 0,
  BABEL_AE_IP4 = 1,
  BABEL_AE_IP6 = 2,
  BABEL_AE_IP6_LL = 3,
  BABEL_AE_MAX
};

typedef struct {
  u8 type;
  u16 nonce;
  u16 interval;
  ip_addr sender;
} babel_msg_ack_req;

typedef struct {
  u8 type;
  u16 nonce;
} babel_msg_ack;

typedef struct {
  u8 type;
  u16 seqno;
  u16 interval;
  ip_addr sender;
} babel_msg_hello;

typedef struct {
  u8 type;
  u8 ae;
  u16 rxcost;
  u16 interval;
  ip_addr addr;
  ip_addr sender;
} babel_msg_ihu;

typedef struct {
  u8 type;
  u8 wildcard;
  u16 interval;
  u16 seqno;
  u16 metric;
  v64 router_id;
  net_addr net;
  ip_addr next_hop;
  ip_addr sender;
} babel_msg_update;

typedef struct {
  u8 type;
  u8 full;
  net_addr net;
} babel_msg_route_request;

typedef struct {
  u8 type;
  u8 hop_count;
  u16 seqno;
  v64 router_id;
  net_addr net;
  ip_addr sender;
} babel_msg_seqno_request;

typedef union {
  u8 type;
  babel_msg_ack_req ack_req;
  babel_msg_ack ack;
  babel_msg_hello hello;
  babel_msg_ihu ihu;
  babel_msg_update update;
  babel_msg_route_request route_request;
  babel_msg_seqno_request seqno_request;
} babel_msg ;

//typedef struct {
//  union babel_msg msg;
//} babel_msg_node;

#endif
