/**
 * resend.h
 *
 * Dave Taht
 * 2017-03-11
 */

#ifndef RESEND_H
#define RESEND_H

struct resend {
  uint8_t kind;
  uint8_t max;
  unsigned short delay;
  ttime_t time;
  address_t src_prefix;
  address_t prefix;
  seqno_t seqno;
  id_t id;
  iface_t ifp;
};

#endif
