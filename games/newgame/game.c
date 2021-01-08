#include "game.h"
#include <string.h>

unsigned char img[128][128][4];

void game_init()
{
}

void game_update(unsigned buttons)
{
}

void *game_draw()
{
  return &img[0][0][0];
}

void game_audio(unsigned samples, int16_t *pcm)
{
  memset(pcm, 0, samples * 2);
}
