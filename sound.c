#include "types.h"
#include "x86.h"
#include "defs.h"
#include "traps.h"
#include "memlayout.h"

static inline uchar sb16_in()
{
  while (!(inb(0x22e) & (1 << 7))) { }
  return inb(0x22a);
}
static inline void sb16_out(uchar data)
{
  while (inb(0x22c) & (1 << 7)) { }
  outb(0x22c, data);
}
static inline uchar sb16_mixer_in(uchar reg)
{
  outb(0x224, reg);
  return inb(0x225);
}
static inline void sb16_mixer_out(uchar reg, uchar data)
{
  outb(0x224, reg);
  outb(0x225, data);
}

static char *buf;

static void refill()
{
  static unsigned seed = 2021;
  for (int i = 0; i < 4096; i++) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    buf[i] = (seed >> 24) & 0xff;
  }
}

void sndinit()
{
  ioapicenable(IRQ_SB16, 0);

  // Initialize DSP
  outb(0x226, 1);
  microdelay(3);
  outb(0x226, 0);
  while (sb16_in(0x22a) != 0xaa) { }
  // Configure DMA & IRQ
  sb16_mixer_out(0x80, (1 << 1));             // IRQ 5
  sb16_mixer_out(0x81, (1 << 1) | (1 << 5));  // DMA 1 (8-bit), 5 (16-bit)
  cprintf("IRQ selection: 0x%x\n", sb16_mixer_in(0x80));
  cprintf("DMA selection: 0x%x\n", sb16_mixer_in(0x81));

  // Allocate buffer
  buf = kalloc();
  unsigned paddr = (unsigned)V2P(buf);
  cprintf("buffer: vaddr=%x paddr=%x\n", (unsigned)buf, paddr);
  memset(buf, 0, 4096);

  // Program DMA channel 5
  outb(0xd4, 0x04 + 1);
  outb(0xd8, 1);
  outb(0xd6, 0x58 + 1);
  outb(0x8b, (paddr >> 16) & 0xff);
  outb(0xc4, (paddr >>  1) & 0xff);
  outb(0xc4, (paddr >>  9) & 0xff);
  outb(0xc6, 0xff);
  outb(0xc6, 0x03); // 1024 words
  outb(0xd4, 1);
  cprintf("DMA status: %x\n", inb(0xd0));

  sb16_out(0x41);
  sb16_out(0x56);
  sb16_out(0x22); // 22050

  sb16_out(0xb6);
  sb16_out(0x10);
  sb16_out(0xff); // 1024 samples
  sb16_out(0x03);
  cprintf("DMA status: %x\n", inb(0xd0));
}

void sndintr()
{
  // Acknowledge
  inb(0x22f);
  // for (const char *s = "interrupt!\n"; *s != 0; s++)
  //   uartputc(*s);
  refill();
}
