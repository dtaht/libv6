#include <stdio.h>
#include <stdlib.h>
#include <smmintrin.h>

int c; // prevent optimizer from removing c from loops

__declspec(cpu_specific(core_i7_sse4_2))
int popcnt(unsigned long i) {
 return _mm_popcnt_u64(i);
}

__declspec(cpu_specific(generic))
int popcnt(unsigned long i) {
 int c = 0;
 for(; i; c++) {
   i &= i - 1;
 }
 return c;
}

__declspec(cpu_dispatch(generic, core_i7_sse4_2))
int popcnt(unsigned long i) {
}

int main() {
 size_t n = 1<<30;

 for(size_t i=0; i<n; i++) {
   c = popcnt(i);
 }

 return 0;
}
