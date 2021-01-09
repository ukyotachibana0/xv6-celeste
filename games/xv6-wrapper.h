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

static inline float fminf(float a, float b) { return a < b ? a : b; }
static inline float fmaxf(float a, float b) { return a > b ? a : b; }
static inline float floorf(float x) {
  int i = (int)x;
  return (x >= 0 ? (float)i : (i == x ? i : (i - 1)));
}
static inline float roundf(float x) { return floorf(x + 0.5f); }
static inline float fabsf(float x) { return x >= 0 ? x : -x; }
static inline float fmodf(float x, float y) {
  // Assumes y > 0
  return x - floorf(x / y) * y;
}

#ifndef M_PI
#define M_PI    3.1415926535897932f
#define M_2_PI  6.2831853071795865f
#define M_PI_2  9.8696044010893586f
#define M_E     2.718281828459045f
#endif
static inline float sinf(float x) {
  x = fmodf(x + M_PI, M_2_PI) - M_PI;
  if (x < 0) return -sinf(-x);
  float x2 = x * x;
  // Bhaskara I's sine approximation formula
  return 4 * x * (M_PI - x) / (1.25f * M_PI_2 - x * (M_PI - x));
  // return ((1.f/120 * x2 - 1.f/6) * x2 + 1) * x;
}
static inline float cosf(float x) { return sinf(x + M_PI/2); }

static inline float expf(float x) {
  float base = M_E;
  if (x < 0) { base = 1.0f/M_E; x = -x; }
  unsigned y = (unsigned)x;
  x -= y;
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
static inline float lnf(float x) { return 0; }  // TODO
static inline float powf(float x, float y) { return expf(y * lnf(x)); }

#endif
