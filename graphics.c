#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"

#define TCCR    (0x0390/4)   // Timer Current Count
#define TICR    (0x0380/4)   // Timer Initial Count

extern uint ticks;

int sys_imgdraw(void){
  unsigned char* img;
  if(argptr(0, (void*)&img, 320*200*3) < 0)
    return -1;
  int palette[256] = {0};
  unsigned char palette_code = 0x00;
  int i, j;
  for(j = 0; j < 320; j++){
    for(i = 0; i < 200; i++){
      unsigned char r = img[i*320*3 + j*3] >> 2;
      unsigned char g = img[i*320*3 + j*3 + 1] >> 2;
      unsigned char b = img[i*320*3 + j*3 + 2] >> 2;

      int k = 0;
      for(k = 0; k < palette_code; k++){
        if(palette[k] == (int)r * 256 * 256 + (int)g * 256 + (int)b){
          break; 
        }
      }
      if(k >= palette_code){
        outb(0x3c8, palette_code);
        outb(0x3c9, r);
        outb(0x3c9, g);
        outb(0x3c9, b);
        *(unsigned char *)P2V(0xa0000 + i * 320 + j) = palette_code;
        palette[palette_code] = (int)r * 256 * 256 + (int)g * 256 + (int)b;
        palette_code++;
      }
      else{
        *(unsigned char *)P2V(0xa0000 + i * 320 + j) = k;
      }
    }
  }
  
  // for(int i = 0; i < palette_code; i++){
  //   cprintf("p: %d\n", palette[i]);
  // }
  return 1;
}

int sys_pitimer(void){
  outb(0x43, 0x0b);
  unsigned char low = inb(0x40);
  unsigned char high = inb(0x40);
  // if(is_apictimer & 0x08){
  //   return 1;
  // }
  // else return 0;
  return ( (unsigned int)high << 8 ) + low;
}

int sys_apictimer(void){
  int ticr = lapic[TICR];
  int tccr = lapic[TCCR];
  // cprintf("t: %d %d %d\n", ticks, ticr, tccr);
  int r = (ticr * (ticks + 1) - tccr) / 10; 
  return r;
}
