/**
 * message_capabilities.h
 *
 * Dave Taht
 * 2017-03-18
 */

#ifndef MESSAGE_CAPABILITIES_H
#define MESSAGE_CAPABILITIES_H

typedef struct {
  uint16_t gps : 1;
  uint16_t routes : 1;
  uint16_t routers : 1;
  uint16_t interfaces : 1;
  uint16_t uptime : 1;
  uint16_t load : 1;
  // ?
}
#endif
