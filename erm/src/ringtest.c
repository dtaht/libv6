
#include "preprocessor.h"
#include "shared.h"
#include "erm_bits.h"

typedef u8 msg8 VECTOR(16);

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

static int counter = 0;

const msg8 ones = {1};
const msg8 twos = {2};
const msg8 threes = {3};
const msg8 fours = {4};

void pushme();

void popme()
{
  int c = 0;
  while (!msg8_is_lowater()) {
   msg8 toss =  msg8_dequeue();
   c++;
  }
  printf("popped: %d\n", c);
  pushme(); 
}

// With profuse apologies to tom duff

void duffme()
{
  msg8 o[4];
  o[0] = ones;
  o[1] = twos;
  o[2] = threes;
  o[3] = fours;
  int i = 0;
  switch(i = msg8_size() & 3) {
  case 3: 
    msg8_enqueue(o[i++]);
  case 2:
    msg8_enqueue(o[i++]);
  case 1:
    msg8_enqueue(o[i++]);
  case 0:
    msg8_enqueue(o[i++]);
  default: break;
  }
  
  popme();
}


void pushme()
{
  msg8 o1 = ones;
  msg8 o2 = twos;
  msg8 o3 = threes;
  msg8 o4 = fours;
  if(!msg8_is_hiwater()) {
    msg8_enqueue(o1);
    //    msg8_enqueue(++counter);
  }
  if(!msg8_is_hiwater()) {
    msg8_enqueue(o2);
    //    msg8_enqueue(++counter);
  }
  if(!msg8_is_hiwater()) {
    msg8_enqueue(o3);
    //    msg8_enqueue(++counter);
  }
  if(!msg8_is_hiwater()) {
    msg8_enqueue(o4);
    //    msg8_enqueue(++counter);
  }
  popme();
}


int main() {
  //  msg8_enqueue(1);
  // msg8_enqueue(2);
  //msg8_enqueue(3);
  // msg8_enqueue(4);
  /*
  for(int i = 0; i < 4; i++) {
    printf("1,2,3,4 on ring: %d\n", msg8_dequeue());
  }
  for(int i = 0; i < 4; i++) {
    printf("overrun on ring: %d\n", msg8_dequeue());
  }
  for(int i = 0; i < 4; i++) {
    printf("run again on ring: %d\n", msg8_dequeue());
  }
  */
  // Hi and low water test (runs forever)

  pushme();

}
  
