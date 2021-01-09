#ifndef __MIKAN__API_H__
#define __MIKAN__API_H__

#include <stdint.h>

#define BTN_L  (1u << 0)
#define BTN_R  (1u << 1)
#define BTN_U  (1u << 2)
#define BTN_D  (1u << 3)
#define BTN_C  (1u << 4)
#define BTN_X  (1u << 5)
#define BTN_Z  (1u << 6)
#define BTN_SP (1u << 7)

typedef void (*update_func_t)();
typedef void *(*draw_func_t)();
void register_loop(update_func_t update, draw_func_t draw);

uint32_t buttons();

// Provided by application
void game_init();
void game_update(unsigned buttons);
void *game_draw();

#endif
