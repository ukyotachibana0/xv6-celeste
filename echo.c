#include "types.h"
#include "stat.h"
#include "user.h"

// int
// main(int argc, char *argv[])
// {
//   int i;

//   for(i = 1; i < argc; i++)
//     printf(1, "%s%s", argv[i], i+1 < argc ? " " : "\n");
//   exit();
// }

int
main(int argc, char *argv[])
{
  int keycode = 0x01;  // escape key pressed

  const int SCREEN_WIDTH = 320;
  const int SCREEN_HEIGHT = 200;
  const int SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT * 3;
  char* img = (char* )malloc(sizeof(char) * SCREEN_SIZE);
  memset(img, 0, SCREEN_SIZE);
  for (int i = 0; i < SCREEN_HEIGHT; i++) {
    for (int j = 0; j < SCREEN_WIDTH; j++) {
      img[i * SCREEN_WIDTH * 3 + j * 3] = i;
      img[i * SCREEN_WIDTH * 3 + j * 3 + 1] = i;
      img[i * SCREEN_WIDTH * 3 + j * 3 + 2] = i;
    }
  }
  
  //kbd(keycode);

  imgdraw(img);
  free(img);

  exit();
}
