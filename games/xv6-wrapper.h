#ifndef XV6_WRAPPER_H
#define XV6_WRAPPER_H

#include "types.h"
#include "user.h"
#include <stdarg.h>

#ifndef NO_BOOL
typedef unsigned char bool;
#define true  ((unsigned char)1)
#define false ((unsigned char)0)
#endif

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

typedef unsigned int size_t;

#define fprintf(...)
#define assert(...)

#ifdef NOOP_PRINTF
#define printf(...)
#endif

#define NULL  ((void *)0)

static float fminf(float a, float b) { return a < b ? a : b; }
static float fmaxf(float a, float b) { return a > b ? a : b; }
static float floorf(float x) {
  int i = (int)x;
  return (float)(x >= 0 ? i : (i == x ? i : (i - 1)));
}
static float roundf(float x) { return floorf(x + 0.5f); }
static float fabsf(float x) { return x >= 0 ? x : -x; }
static float fmodf(float x, float y) {
  // Assumes y > 0
  return x - floorf(x / y) * y;
}

#ifndef M_PI
#define M_PI    3.1415926535897932f
#define M_2_PI  6.2831853071795865f
#define M_PI_2  9.8696044010893586f
#define M_E     2.7182818284590452f
#define M_LN2   0.6931471805599453f
#endif
static float sinf(float x) {
  x = fmodf(x + M_PI, M_2_PI) - M_PI;
  unsigned neg = 0;
  if (x < 0) { neg = 1; x = -x; }
  float x2 = x * x;
  // Bhaskara I's sine approximation formula
  return (neg ? -4 : 4) * x * (M_PI - x) / (1.25f * M_PI_2 - x * (M_PI - x));
  // return ((1.f/120 * x2 - 1.f/6) * x2 + 1) * x;
}
static float cosf(float x) { return sinf(x + M_PI/2); }

static float expf(float x) {
  float base = M_E;
  unsigned y = (unsigned)x;
  if (x < 0) { base = 1.0f/M_E; y = -y; x += y; }
  else x -= y;
  float ret = 1;
  while (y > 0) {
    if (y & 1) ret *= base;
    base *= base;
    y >>= 1;
  }
  // 0 < x < 1
  // (2, 2) Padé approximant
  ret *= ((x+3)*(x+3) + 3) / ((x-3)*(x-3) + 3);
  return ret;
}
static float pow2f(float y) { return expf(y * M_LN2); }

#endif
