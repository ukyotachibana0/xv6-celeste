#include "types.h"
#include "x86.h"
#include "defs.h"

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

void sndinit()
{
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
}
