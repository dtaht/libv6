#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#if defined(HAVE_NEON)
#include <arm_neon.h>
// Ideally this would recognize a literal and CSE fix it
#define my_memcpy(dest,src,cnt) ((cnt) == 16) ? my_memcpy16(dest,src) : \
                                ((cnt) == 12 ) ? my_memcpy12(dest,src) : \
                                ((cnt) == 8 ) ? my_memcpy8(dest,src)  : \
                                memcpy((dest),(src),(cnt))
#include "my_memcpy.h"
#else
#define my_memcpy(dest,src,cnt) memcpy(dest,src,cnt)
#endif

// http://llvm.org/docs/BigEndianNEON.html
// https://gcc.gnu.org/ml/gcc-patches/2010-06/msg00409.html

