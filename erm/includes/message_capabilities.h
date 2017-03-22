/**
 * message_capabilities.h
 *
 * Dave Taht
 * 2017-03-18
 */

#ifndef MESSAGE_CAPABILITIES_H
#define MESSAGE_CAPABILITIES_H

typedef struct {
  u16 gps : 1;
  u16 routes : 1;
  u16 routers : 1;
  u16 interfaces : 1;
  u16 uptime : 1;
  u16 load : 1;
  // ?
}
#endif
