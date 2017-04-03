/*
 *	ERM -- The Babel TLV format
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

#define BABEL_FLAG_DEF_PREFIX 0x80
#define BABEL_FLAG_ROUTER_ID 0x40

typedef struct PACKED {
  u8 magic;
  u8 version;
  u16 length;
} babel_pkt_header;

typedef struct PACKED {
  u8 type;
  u8 length;
  u8 value[VMAXSIZE(254)];
} babel_tlv;

typedef struct PACKED {
  u8 type;
  u8 length;
  u16 reserved;
  u16 nonce;
  u16 interval;
} babel_tlv_ack_req;

typedef struct PACKED {
  u8 type;
  u8 length;
  u16 nonce;
} babel_tlv_ack;

typedef struct PACKED {
  u8 type;
  u8 length;
  u16 reserved;
  u16 seqno;
  u16 interval;
} babel_tlv_hello;

typedef struct PACKED {
  u8 type;
  u8 length;
  u8 ae;
  u8 reserved;
  u16 rxcost;
  u16 interval;
  u8 addr[VMAXSIZE(16)];
} babel_tlv_ihu;

typedef struct PACKED {
  u8 type;
  u8 length;
  u16 reserved;
  v64 router_id;
} babel_tlv_router_id;

typedef struct PACKED {
  u8 type;
  u8 length;
  u8 ae;
  u8 reserved;
  u8 addr[VMAXSIZE(16)];
} babel_tlv_next_hop;

typedef struct PACKED {
  u8 type;
  u8 length;
  u8 ae;
  u8 flags;
  u8 plen;
  u8 omitted;
  u16 interval;
  u16 seqno;
  u16 metric;
  u8 addr[VMAXSIZE(16)];
} babel_tlv_update;

typedef struct PACKED {
  u8 type;
  u8 length;
  u8 ae;
  u8 plen;
  u8 addr[VMAXSIZE(16)];
} babel_tlv_route_request;

typedef struct PACKED {
  u8 type;
  u8 length;
  u8 ae;
  u8 plen;
  u16 seqno;
  u8 hop_count;
  u8 reserved;
  v64 router_id;
  u8 addr[VMAXSIZE(16)];
} babel_tlv_seqno_request;
