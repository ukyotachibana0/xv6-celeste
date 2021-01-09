#include "game.h"
#include "raylib/include/raylib.h"
#include <stdlib.h>

#define WIN_SCALE   3

#define AUDIO_BLOCK_SIZE  1024

int main()
{
  InitWindow(SCR_W * WIN_SCALE, SCR_H * WIN_SCALE, "Window");
  SetTargetFPS(30);

  // Create empty texture by loading empty texture data,
  // as raylib does not provide a more convenient API
  void *data = malloc(SCR_W * SCR_H * 4);
  Texture2D tex = LoadTextureFromImage((Image) {
    .data = data,
    .width = SCR_W,
    .height = SCR_H,
    .mipmaps = 1,
    .format = UNCOMPRESSED_R8G8B8A8
  });
  free(data);

  game_init();

  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(AUDIO_BLOCK_SIZE);
  AudioStream astr = InitAudioStream(22050, 16, 1);
  PlayAudioStream(astr);
  int16_t *pcm = (int16_t *)malloc(AUDIO_BLOCK_SIZE * 2);

  while (!WindowShouldClose()) {
    bool step = (!IsKeyDown(KEY_SPACE) || IsKeyPressed(KEY_ENTER));
    BeginDrawing();
    ClearBackground(BLACK);

    if (step) {
      unsigned buttons = 0;
      if (IsKeyDown(KEY_LEFT))  buttons |= BTN_L;
      if (IsKeyDown(KEY_RIGHT)) buttons |= BTN_R;
      if (IsKeyDown(KEY_UP))    buttons |= BTN_U;
      if (IsKeyDown(KEY_DOWN))  buttons |= BTN_D;
      if (IsKeyDown(KEY_C))     buttons |= BTN_C;
      if (IsKeyDown(KEY_X))     buttons |= BTN_X;
      if (IsKeyDown(KEY_Z))     buttons |= BTN_Z;
      if (IsKeyDown(KEY_SPACE)) buttons |= BTN_SP;
      game_update(buttons);

      UpdateTexture(tex, game_draw());

      if (IsAudioStreamProcessed(astr)) {
        game_audio(AUDIO_BLOCK_SIZE, pcm);
        UpdateAudioStream(astr, pcm, AUDIO_BLOCK_SIZE);
      }
    }

    DrawTextureEx(tex, (Vector2) {0, 0}, 0, WIN_SCALE, WHITE);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
