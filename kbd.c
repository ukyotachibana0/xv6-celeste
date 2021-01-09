#include "types.h"
#include "x86.h"
#include "defs.h"
#include "kbd.h"

unsigned char isconsole = 1;

int
kbdgetc(void)
{
  static uint shift;
  static uchar *charcode[4] = {
    normalmap, shiftmap, ctlmap, ctlmap
  };
  uint st, data, c;

  st = inb(KBSTATP);
  if((st & KBS_DIB) == 0)
    return -1;
  data = inb(KBDATAP);

  // uartputc('o');
  // uartputc('[');
  // uartputc("0123456789abcdef"[data / 16]);
  // uartputc("0123456789abcdef"[data % 16]);
  // uartputc(']');
  // uartputc('\n');

  if(data == 0xE0){
    shift |= E0ESC;
    return (isconsole ? 0 : -1);
  } else if(data & 0x80){
    // Key released

    if(shift & E0ESC){
      if(data == 0xcb){   // left arrow
        kb_mode[0] = 0;
      }
      if(data == 0xc8){   // up arrow
        kb_mode[1] = 0;
      }
      if(data == 0xcd){   // right arrow
        kb_mode[2] = 0;
      }
      if(data == 0xd0){   // down arrow
        kb_mode[3] = 0;
      }
    }

    data = (shift & E0ESC ? data : data & 0x7F);
    shift &= ~(shiftcode[data] | E0ESC);

    // uartputc('r');
    // uartputc('[');
    // uartputc("0123456789abcdef"[data / 16]);
    // uartputc("0123456789abcdef"[data % 16]);
    // uartputc(']');
    // uartputc('\n');

    if(mode_map[data] != 0){
      kb_mode[mode_map[data]] = 0;
    }
    return (isconsole ? 0 : -1);
  } else if(shift & E0ESC){
    // Last character was an E0 escape; or with 0x80
    data |= 0x80;
    shift &= ~E0ESC;

    // uartputc('p');
    // uartputc('[');
    // uartputc("0123456789abcdef"[data / 16]);
    // uartputc("0123456789abcdef"[data % 16]);
    // uartputc(']');
    // uartputc('\n');

    if(data == 0xcb){   // left arrow
      kb_mode[0] = 1;
    }
    if(data == 0xc8){   // up arrow
      kb_mode[1] = 1;
    }
    if(data == 0xcd){   // right arrow
      kb_mode[2] = 1;
    }
    if(data == 0xd0){   // down arrow
      kb_mode[3] = 1;
    }

  }

  if(mode_map[data] != 0){
    kb_mode[mode_map[data]] = 1;
  }

  // uartputc('w');
  // uartputc('[');
  // uartputc("0123456789abcdef"[data / 16]);
  // uartputc("0123456789abcdef"[data % 16]);
  // uartputc(']');
  // uartputc('\n');

  shift |= shiftcode[data];
  shift ^= togglecode[data];
  c = charcode[shift & (CTL | SHIFT)][data];
  if(shift & CAPSLOCK){
    if('a' <= c && c <= 'z')
      c += 'A' - 'a';
    else if('A' <= c && c <= 'Z')
      c += 'a' - 'A';
  }
  return (isconsole ? c : -1);
}

void
kbdintr(void)
{
  consoleintr(kbdgetc);
}

int kb_mode[256] = { 0 };

int sys_kbd(void)
{
  int index;
  if(argint(0, (void*)&index) < 0)
    return -1;
  if(index < 0 || index > 255)    //param out of range
    return -1;
  return kb_mode[index];
}

int sys_consmode(void)
{
  int mode;
  if (argint(0, &mode) < 0) return -1;
  isconsole = (mode == 0);
  return 0;
}
