/**
 * blanusa.c
 *
 * Dave Taht
 * 2017-03-21
 */

#include <stdint.h>
#include <math.h>

#define generic uint16_t
#define blanusa blanusauint16_t

#include "blanusa.h"
#undef generic
#undef blanusa
#define generic uint32_t
#define blanusa blanusauint32_t


#include "blanusa.h"

#undef generic
#undef blanusa
#define generic uint64_t
#define blanusa blanusauint64_t
#include "blanusa.h"
#undef generic
#undef blanusa
#define generic float
#define blanusa blanusafloat
#include "blanusa.h"
#undef generic
#undef blanusa
#define generic double
#define blanusa blanusadouble
#include "blanusa.h"
#undef generic
#undef blanusa


