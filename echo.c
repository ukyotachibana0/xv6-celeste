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

#include "img/CelesteClassic3_processed.h"

int
main(int argc, char *argv[])
{
  //int keycode = 0x01;  // escape key pressed

  const int SCREEN_WIDTH = 320;
  const int SCREEN_HEIGHT = 200;
  const int SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT * 3;
  unsigned char* img = (unsigned char*)malloc(sizeof(char) * SCREEN_SIZE);
  memset(img, 0, SCREEN_SIZE);
/*
  int i, j;
  for (i = 0; i < SCREEN_HEIGHT; i++) {
    for (j = 0; j < SCREEN_WIDTH; j++) {
      if(i == 0 || i == 199 || j == 0 || j == 319){
        img[i * SCREEN_WIDTH * 3 + j * 3] = 255;
        img[i * SCREEN_WIDTH * 3 + j * 3 + 1] = 100;
        img[i * SCREEN_WIDTH * 3 + j * 3 + 2] = 100;
        continue;
      } 
      img[i * SCREEN_WIDTH * 3 + j * 3] = 100;
      unsigned char _i = i/40;
      img[i * SCREEN_WIDTH * 3 + j * 3 + 1] = 100;
      img[i * SCREEN_WIDTH * 3 + j * 3 + 2] = _i * 40;
      
    }
  }
*/

  int x, y;
  for (y = 0; y < 200; y++)
    for (x = 0; x < 200; x++) {
      int y1 = y;
      int x1 = x + (SCREEN_WIDTH - 200) / 2;
      img[(y1 * SCREEN_WIDTH + x1) * 3 + 0] = palette[image[y * 200 + x]][0];
      img[(y1 * SCREEN_WIDTH + x1) * 3 + 1] = palette[image[y * 200 + x]][1];
      img[(y1 * SCREEN_WIDTH + x1) * 3 + 2] = palette[image[y * 200 + x]][2];
    }
  
  //kbd(keycode);

  imgdraw(img);
  free(img);

  exit();
}
