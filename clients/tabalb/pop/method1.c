#include <stdio.h>
#include <stdlib.h>
#include <smmintrin.h>

int c; // prevent optimizer from removing c from loops

int main() {
 size_t n = 1<<30;

 for(size_t i=0; i<n; i++) {
   // Enabled desired method below

#if 0
   // Straightforward method
   c = 0;
   for(int j=0; j<sizeof(i)*8; j++) {
     if(i & (1ul << j)) c++;
   }
#endif

#if 1
   // bit twiddling method
   c = 0;
   unsigned long z = i;
   for(; z; c++) {
     z &= z - 1;
   }
#endif

#if 0
   // POPCNT method
   c = _mm_popcnt_u64(i);
#endif
 }

 return 0;
}
