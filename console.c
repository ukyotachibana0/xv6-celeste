// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

static void consputc(int);

static int panicked = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons;

static void
printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
  int i, c, locking;
  uint *argp;
  char *s;

  locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  if (fmt == 0)
    panic("null fmt");

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if((s = (char*)*argp++) == 0)
        s = "(null)";
      for(; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if(locking)
    release(&cons.lock);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  cprintf("lapicid %d: panic: ", lapicid());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
/*
#define CRTPORT 0x3d4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory
*/
#define pix(x, y) (*(uchar *)P2V(0xa0000 + (y) * 320 + (x)))

// Character size: 7*14
#define CHAR_W    7
#define CHAR_H    14
#include "font.h"
// Character count: 45*14
#define CHAR_ROWS 14
#define CHAR_COLS 45
static char cbuf[CHAR_ROWS][CHAR_COLS] = {{ 0 }};
static unsigned cursrow = 0, curscol = 0;
// Total drawing area: 315*196
// Border: left 2, right 3, top 2, bottom 2
#define PADDING_L 2
#define PADDING_R 3
#define PADDING_U 2
#define PADDING_D 2

extern unsigned char isconsole;

static void
cgaputc(int c)
{
  if (c == '\n') {
    cursrow++;
    curscol = 0;
    c = 0;
  } else if (c == BACKSPACE) {
    if (curscol != 0) curscol--;
    else if (cursrow != 0) {
      cursrow--;
      curscol = CHAR_COLS - 1;
    }
    cbuf[cursrow][curscol] = 0;
    c = 0;
  } else {
    cbuf[cursrow][curscol] = c;
    if (++curscol == CHAR_COLS) {
      cursrow++;
      curscol = 0;
    }
  }
  if (cursrow == CHAR_ROWS) {
    memmove(&cbuf[0][0], &cbuf[1][0], (CHAR_ROWS - 1) * CHAR_COLS);
    memset(&cbuf[CHAR_ROWS - 1][0], 0, CHAR_COLS);
    cursrow--;
  }

  if (!isconsole) return;

/*
  for (int r = 0; r < CHAR_ROWS; r++)
    for (int c = 0; c < CHAR_COLS; c++) {
      if (c == 0) uartputc('|');
      uartputc(cbuf[r][c] ? cbuf[r][c] : ' ');
      if (c == CHAR_COLS - 1) uartputc('|'), uartputc('\n');
    }
  for (int i = 0; i < 10; i++) uartputc('-');
  uartputc('\n');
*/

  // Set palette
  outb(0x3c8, 0);
  outb(0x3c9, 4); outb(0x3c9, 4); outb(0x3c9, 4);
  outb(0x3c8, 1);
  outb(0x3c9, 61); outb(0x3c9, 61); outb(0x3c9, 61);
  outb(0x3c8, 2);
  outb(0x3c9, 32); outb(0x3c9, 32); outb(0x3c9, 32);
  // Output image
  for (int r = 0; r < CHAR_ROWS; r++)
    for (int c = 0; c < CHAR_COLS; c++) {
      ushort x = PADDING_L + c * CHAR_W;
      ushort y = PADDING_U + r * CHAR_H;
      char ch = cbuf[r][c];
      unsigned iscontrol = 0;
      if (ch == 0) {
        ch = ' ';
      } else if (ch < 32) {
        iscontrol = 1;
        ch = ch + 'A' - 1;
      }
      uchar row = (ch - 32) / 16;
      uchar col = (ch - 32) % 16;
      ushort ptr = row * CHAR_W * 16 * CHAR_H + col * CHAR_W;
      for (int j = 0; j < CHAR_H; j++)
      for (int i = 0; i < CHAR_W; i++)
        if (font_data[ptr + (j * CHAR_W * 16 + i)])
          pix(x + i, y + j) = (iscontrol ? 2 : 1);
        else if (r == cursrow && c == curscol)
          pix(x + i, y + j) = 2;
        else
          pix(x + i, y + j) = 0;
    }
  for (int x = 0; x < 320; x++) {
    for (int y = 0; y < PADDING_U; y++) pix(x, y) = 0;
    for (int y = 199 - PADDING_D; y < 200; y++) pix(x, y) = 0;
  }
  for (int y = 0; y < 200; y++) {
    for (int x = 0; x < PADDING_L; x++) pix(x, y) = 0;
    for (int x = 319 - PADDING_R; x < 320; x++) pix(x, y) = 0;
  }

/*
  int pos;

  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  if(c == '\n')
    pos += 80 - pos%80;
  else if(c == BACKSPACE){
    if(pos > 0) --pos;
  } else
    crt[pos++] = (c&0xff) | 0x0700;  // black on white

  if(pos < 0 || pos > 25*80)
    panic("pos under/overflow");

  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }

  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
*/
}

void
consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    uartputc(c);
  cgaputc(c);
}

#define INPUT_BUF 128
struct {
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x

void
consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;

  acquire(&cons.lock);
  while((c = getc()) >= 0){
    switch(c){
    case C('P'):  // Process listing.
      // procdump() locks cons.lock indirectly; invoke later
      doprocdump = 1;
      break;
    case C('U'):  // Kill line.
      while(input.e != input.w &&
            input.buf[(input.e-1) % INPUT_BUF] != '\n'){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    case C('H'): case '\x7f':  // Backspace
      if(input.e != input.w){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    default:
      if(c != 0 && input.e-input.r < INPUT_BUF){
        c = (c == '\r') ? '\n' : c;
        input.buf[input.e++ % INPUT_BUF] = c;
        consputc(c);
        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          input.w = input.e;
          wakeup(&input.r);
        }
      }
      break;
    }
  }
  release(&cons.lock);
  if(doprocdump) {
    procdump();  // now call procdump() wo. cons.lock held
  }
}

int
consoleread(struct inode *ip, char *dst, int n)
{
  uint target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }
  release(&cons.lock);
  ilock(ip);

  return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip);

  return n;
}

void
consoleinit(void)
{
  initlock(&cons.lock, "console");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}

