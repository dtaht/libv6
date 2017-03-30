#include "preprocessor.h"
#include "shared.h"
#include "erm_bits.h"

/* C is still not up to deeply packing typedefs sanely darn it */

typedef struct PACKED {
  bit1 a;
  bit16 b;
  bitf c;
} bittest;

typedef struct PACKED {
  bit1 a;
  bit2 c;
} bittest2;

int main() {
  printf("in a perfect world, this would be 4 bytes: %u \n", sizeof(bittest));
  printf("in a perfect world, this would be 1 byte: %u \n", sizeof(bittest2));
  return 0;
}
