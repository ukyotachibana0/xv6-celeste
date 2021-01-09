#ifndef XV6_WRAPPER_H
#define XV6_WRAPPER_H

#include "types.h"
#include "user.h"

typedef unsigned char bool;
#define true  ((unsigned char)1)
#define false ((unsigned char)0)

typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

typedef unsigned int size_t;

/*
float cosf(float);
float sinf(float);
float expf(float);
*/
#define cosf(x) (x)
#define sinf(x) (x)
#define expf(x) (x)

#endif
