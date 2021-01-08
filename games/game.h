#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#define SCR_SIZE 128

#define BTN_L  (1u << 0)
#define BTN_R  (1u << 1)
#define BTN_U  (1u << 2)
#define BTN_D  (1u << 3)
#define BTN_O  (1u << 4)
#define BTN_X  (1u << 5)

void game_init();
void game_update(unsigned buttons);
void *game_draw();
void game_audio(unsigned samples, int16_t *pcm);

#endif
