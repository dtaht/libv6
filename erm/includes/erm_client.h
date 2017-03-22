 /**
 * erm_client.h
 *
 * Dave Taht
 * 2017-03-17
 */

#ifndef ERM_CLIENT_H
#define ERM_CLIENT_H

typedef u32* erm_t;

erm_t erm_attach_client(char* instance) COLD;
u32 erm_status(erm_t erm) HOT;
erm_t* erm_query(erm_t erm, u32* buf, int size) HOT;
int erm_close(erm_t erm) COLD;

#endif
