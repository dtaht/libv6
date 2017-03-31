
#include "preprocessor.h"
#include "shared.h"
#include "erm_bits.h"

typedef int msg8;

msg8 msg8_r[8]; //  SECTION("ringdata");

typedef struct
{
  bit3 in;
  bit3 out;
} msg8_ring_t;

msg8_ring_t msg8_i;  // SECTION("ringbufs");

#define atomic_inc(a) ((a)++)
#define atomic_update(a,b) (a = b)

#include "../gen/ring_msg8.h"

int main() {
  msg8_enqueue(1);
  msg8_enqueue(2);
  msg8_enqueue(3);
  msg8_enqueue(4);
  for(int i = 0; i < 4; i++) {
    printf("1,2,3,4 on ring: %d\n", msg8_dequeue());
  }
  for(int i = 0; i < 4; i++) {
    printf("overrun on ring: %d\n", msg8_dequeue());
  }
  for(int i = 0; i < 4; i++) {
    printf("run again on ring: %d\n", msg8_dequeue());
  }

}
  
