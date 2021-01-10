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
  consmode(1);
  printf(1, "hello from mmtest\n");

  const int SCREEN_WIDTH = 320;
  const int SCREEN_HEIGHT = 200;
  const int SCREEN_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT * 3;
  unsigned char* img = (unsigned char*)malloc(sizeof(char) * SCREEN_SIZE);
  memset(img, 0, SCREEN_SIZE);

  int T = 0;
  int tmp = 0;
  int tmp_t = 0;
  while (1) {
    if (kbd('q')) break;
    // if (T % 30 == 0) printf(1, "frame: %d\n", T);
    // TODO: Replace with vertical retrace
    // printf(1, "1: %d\n", apictimer());
    // printf(1, "2: %d\n", apictimer());
    // int i = apictimer();
    int cur = apictimer();
    if(cur < tmp){
      // printf(1, "0: %d %d\n", cur, tmp);
      tmp = 0;
    }
    // if(tmp == 0)
    //   printf(1, "what: %d %d\n", j, tmp);
    if(cur - tmp > 8000 && uptime() != tmp_t){
      // printf(1, "%d\n", T);
      // printf(1, "5s: %d\n", i);
      // printf(1, "j: %d %d\n", cur, tmp);
      T++;
      int x, y;
      for (y = 0; y < 200; y++)
        for (x = 0; x < 200; x++) {
          int y1 = y;
          int x1 = x + (SCREEN_WIDTH - 200) / 2;
          int index = image[y * 200 + x];
          if (index == 7) index = T % 14;
          img[(y1 * SCREEN_WIDTH + x1) * 3 + 0] = palette[index][0];
          img[(y1 * SCREEN_WIDTH + x1) * 3 + 1] = palette[index][1];
          img[(y1 * SCREEN_WIDTH + x1) * 3 + 2] = palette[index][2];
        }
      imgdraw(img);
      // sleep(20);
      tmp = cur;
    }
    // sleep(10);

    if(trsound()){
      short* s_buf = (short*)malloc(2048);
      memset(s_buf, 0, 2048);
      static int phase = 0;
      for(int i = 0; i < 1024; i++){
        s_buf[i] = (phase >= 25 ? 50 - phase : phase) * 120;
        phase = (phase + 1) % 50;
      }
      setsound(s_buf);
      free(s_buf);
    }
  }
  free(img);

  consmode(0);
  exit();
}
