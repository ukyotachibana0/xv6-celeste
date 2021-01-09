#ifndef CELESTE_RAYLIB_P8_SFX_DEFS_H
#define CELESTE_RAYLIB_P8_SFX_DEFS_H

#ifdef XV6
#include "xv6-wrapper.h"
#else
#include <stdint.h>
#endif

typedef struct {
  uint16_t pitch:6;
  uint16_t wform:4;
  uint16_t vol:3;
  uint16_t eff:3;
} p8_note;

typedef struct {
  uint8_t spd;
  uint8_t lpstart;
  uint8_t lpend;
  p8_note notes[32];
} p8_snd;

#endif
