
#include "preprocessor.h"
#include "shared.h"
#include "erm_bits.h"

/* Lets see if I can describe variants better to myself.
   I am now conflating scheduling with dataflow to some
   extent. This took a long time to work out last time too.

   I keep wondering if I can write the regs directly on
   on the parallella...

   */

/* Splay */

typedef struct {
  u64 cpu0_out:4; // actually you'd just call this cpu and use
  u64 cpu1_out:4; // it for in and out
  u64 cpu2_out:4;
  u64 cpu3_out:4;
  u64 cpu4_out:4;
  u64 cpu5_out:4;
  u64 cpu6_out:4;
  u64 cpu7_out:4;
  u64 cpu8_out:4;
  u64 cpu9_out:4;
  u64 cpua_out:4;
  u64 cpub_out:4;
  u64 cpuc_out:4;
  u64 cpud_out:4;
  u64 cpue_out:4;
  u64 cpuf_out:4;
} splay;

/* Multi */

typedef struct {
  u16 cpu0_out:4;
  u16 cpu1_out:4;
  u16 cpu0_in:4;
  u16 cpu1_in:4;
} multi;

/* In some dream arch where I could write 32 cpus in every direction
   based on a 128 bit address register "splaying" */

typedef multi VECTOR(16);

/* In a world where I could go looking for the first free cpu with
   ffs on the bitfield - out of 1024 vcpus */

u8 ubercpus[128];

/* Version 1 */

typedef u8 msg8 VECTOR(16);

msg8 msg8_r[2*2*2*2*2]; //  SECTION("ringdata");

typedef struct
{
  bit5 in;
  bit5 out;
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

// Pop me until we hit the low water mark

void popme()
{
  int c = 0;
  while (msg8_is_lowater()) {
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

// Push me until we hit the hi water mark

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
  
