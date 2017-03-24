#include "erm_common.h"
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  char old;
  int ip;
  int new;
} NC1;

typedef struct {
  char old;
  int ip;
  int new;
} PACKED NC2;

typedef struct PACKED {
  char old;
  int ip;
  int new;
} NC3;

// THESE ARE THE WRONG FORM OF THE DECLARATION

typedef PACKED struct {
  char old;
  int ip;
  int new;
} NC4;

typedef struct {
  char old;
  int ip;
  int new;
} NC5 PACKED;

int main(void)
{
  printf("size unpacked: %d\n", (int)sizeof(NC1));
  printf("size packed: %d\n", (int)sizeof(NC2));
  size_t r = ((sizeof(NC1) == sizeof(NC2)) && (sizeof(NC2) == sizeof(NC3)) &&
              (sizeof(NC3) == sizeof(NC4)) && (sizeof(NC1) != sizeof(NC4)));
  printf("compiler packing %s\n", r ? "ok" : "bad");
  return r;
}
