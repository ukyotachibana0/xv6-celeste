#include <stdio.h>
#include <stdint.h>

static uint8_t image[] = {
  #include "CelesteClassic3.png.h"
};

static char image_out[sizeof(image) / 3];

int main()
{
  fprintf(stderr, "pixels: %d\n", (int)sizeof(image) / 3);
  // Should be 40000

  uint8_t palette[256][3];
  int palette_n = 0;

  for (int x = 0; x < 200; x++)
    for (int y = 0; y < 200; y++) {
      uint8_t r = image[(x * 200 + y) * 3 + 0];
      uint8_t g = image[(x * 200 + y) * 3 + 1];
      uint8_t b = image[(x * 200 + y) * 3 + 2];
      int k;
      for (k = 0; k < palette_n; k++) {
        if (r == palette[k][0] &&
            g == palette[k][1] &&
            b == palette[k][2])
          break;
      }
      if (k == palette_n) {
        palette[k][0] = r;
        palette[k][1] = g;
        palette[k][2] = b;
        palette_n++;
      }
      image_out[x * 200 + y] = k;
    }

  fprintf(stderr, "number of colours: %d\n", palette_n);

  printf("static unsigned char palette[%d][3] = {\n", palette_n);
  for (int i = 0; i < palette_n; i++)
    printf("  {%d, %d, %d},\n",
      palette[i][0], palette[i][1], palette[i][2]);
  printf("};\n\n");

  printf("static unsigned char image[] = {\n");
  for (int x = 0; x < 200; x++)
    for (int y = 0; y < 200; y++) {
      printf("%d,%c", image_out[x * 200 + y], y % 20 == 19 ? '\n' : ' ');
    }
  printf("};\n");

  return 0;
}
