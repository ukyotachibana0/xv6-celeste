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

static short *buf;
static short user_buf[1024];
static int is_tr = 0;

extern unsigned isconsole;

static void refill()
{
  static unsigned buffer = 0;
  if (isconsole) {
    memset(buf + buffer, 0, 2048);
  } else {
    for (int i = 0; i < 1024; i++)
      buf[buffer + i] = user_buf[i];
    is_tr = 1;
  }
  // for (int i = 0; i < 2048; i++) buf[i] = (++phase) % 12999;
  // buf[0] = 29999;
  buffer ^= 1024;
}

//sound initialize
void sndinit()
{
  ioapicenable(IRQ_SB16, 0); //启用声卡中断

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
  buf = (short *)kalloc(); // 4k
  unsigned paddr = (unsigned)V2P(buf);
  cprintf("buffer: vaddr=%x paddr=%x\n", (unsigned)buf, paddr);
  memset(buf, 0, 4096);

  // Program DMA channel 5
  outb(0xd4, 0x04 + 1);  //关闭DMA 5
  outb(0xd8, 1);  //flip flop 随便写一个数
  outb(0xd6, 0x58 + 1);  // 16bit 自动初始化
  outb(0x8b, (paddr >> 16) & 0xff);  //写入段号
  outb(0xc4, (paddr >>  1) & 0xff);  // 写入段内偏移
  outb(0xc4, (paddr >>  9) & 0xff);  // 写入段内偏移
  outb(0xc6, 0xff);   
  outb(0xc6, 0x07); // 2048 words  2 byte - word
  outb(0xd4, 1);  //启用DMA 5
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

int sys_setsound(void){
  is_tr = 0;
  short* tmp_buf;
  argptr(0, (void*)&tmp_buf, 2048);
  for(int i = 0; i < 1024; i++){
    user_buf[i] = tmp_buf[i];
  }
  return 1;
}

int sys_trsound(void){
  return is_tr;
}
