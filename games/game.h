#ifndef GAME_H
#define GAME_H

#ifdef XV6
#include "xv6-wrapper.h"
#else
#include <stdint.h>
#endif

#define SCR_W   320
#define SCR_H   200

#define BTN_L  (1u << 0)
#define BTN_R  (1u << 1)
#define BTN_U  (1u << 2)
#define BTN_D  (1u << 3)
#define BTN_C  (1u << 4)
#define BTN_X  (1u << 5)
#define BTN_Z  (1u << 6)
#define BTN_SP (1u << 7)

void game_init();
// 30 fps
void game_update(unsigned buttons);
// 返回 320*200 的 RGBA 数组首指针
void *game_draw();
void game_audio(unsigned samples, int16_t *pcm);

#endif
