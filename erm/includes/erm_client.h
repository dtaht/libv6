 /**
 * erm_client.h
 *
 * Dave Taht
 * 2017-03-17
 */

#ifndef ERM_CLIENT_H
#define ERM_CLIENT_H

typedef uint32_t* erm_t;

erm_t erm_attach_client(char* instance) COLD;
uint32_t erm_status(erm_t erm) HOT;
erm_t* erm_query(erm_t erm, uint32_t* buf, int size) HOT;
int erm_close(erm_t erm) COLD;

#endif
