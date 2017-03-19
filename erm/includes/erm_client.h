/**
 * erm_client.h
 *
 * Dave Taht
 * 2017-03-17
 */

#ifndef ERM_CLIENT_H
#define ERM_CLIENT_H

typedef uint32_t* erm_t;

erm_t erm_attach_client(char* instance) __attribute__((cold));
uint32_t erm_status(erm_t erm) __attribute__((hot));
erm_t* erm_query(erm_t erm, uint32_t* buf, int size) __attribute__((cold));

int erm_close(erm_t erm) __attribute__((cold));

#endif
