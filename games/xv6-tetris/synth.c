#include "../game.h"

#ifdef XV6
#include "xv6-wrapper.h"
#else
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define pow2f(x) powf(2.0f, (x))
#ifndef M_PI
#define M_PI  3.1415926535897932384626433832795
#endif
#endif

// #define round(_x) ((int)floorf((_x) + 0.5f))
// #define frac(_x)  ((_x) - floorf(_x))
#define round(_x) ((int)((_x) + 0.5f))
#define frac(_x)  ((_x) < 0 ? (_x) - ((int)(_x) - 1) : (_x) - (int)(_x))

#define ENV_A (2e-3f * 22050) // 2 ms = 88.2 samples
#define ENV_K (0.32f * 22050) // Decays to 1/e every 0.32 s

// A crude sound emulator for retro consoles

static short buf[4096][1];

// Square *2, Triangle, Noise

static float freq[4];   // Frequency (in Hz)
static int len[4];      // Note length (in samples)
static float vol[4];    // Volume (linear)
static bool env[4];     // Envelope
static int phase[4];    // Length played (in samples); negative denotes delay

void synth_note(int channel, float frequency,
  float length, float delay, float volume, bool use_envelope)
{
  freq[channel] = frequency;
  len[channel] = round(length * 22050);
  vol[channel] = volume;
  env[channel] = use_envelope;
  phase[channel] = -round(delay * 22050);
}

static inline float my_randf()
{
  static unsigned seed = 0x20210110;
  uint32_t i = 0;
  seed = seed * 1103515245 + 12345;
  i = (seed & 0x7fff0000);
  seed = seed * 1103515245 + 12345;
  i = (i >> 1) | ((seed >> 16) & 0x7fff);
  return ((float)i / 0x3fffffff * 2) - 1;
}

#define X

static void dododo(short buf[][1], int block)
{
  if (block == 0) return;
  memset(buf, 0, block * sizeof buf[0]);
  for (int ch = 0; ch < 4; ch++) if (phase[ch] < len[ch]) {
    int start = (phase[ch] < 0 ? -phase[ch] : 0);
    int end = len[ch] - phase[ch];
    if (start > block) start = block;
    if (end > block) end = block;

    float step = 1.0f / 22050 * freq[ch];
    float rate = frac(phase[ch] * step);

  #define _envelope(_i) (env[ch] ? \
    ((_i) < ENV_A ? ((float)(_i) / ENV_A) : expf(-(_i) / ENV_K)) : 1)
  #define envelope _envelope(phase[ch] + i)

    if (ch <= 1) {
      // Square
      if (env[ch]) {
        for (int i = start; i < end; i++, rate = frac(rate + step)) {
          float v = vol[ch] * envelope * 32767 X;
          short ival = (rate < 0.5f ? +1 : -1) * round(v);
          buf[i][0] += ival;
        }
        /*
        for (int i = start; i < end; i++, rate = frac(rate + step)) {
          short x = buf[i][0];
          unsigned retry = 0;
        qwq:
          x = buf[i][0];
          short value = (rate < 0.5f ? +1 : -1) *
            round(vol[ch] * envelope * 32767 X);
          buf[i][0] += value;
          if (buf[i][0] >= 25000 || buf[i][0] < -25000 || retry) {
            printf(1, "channel %d: %d - %d (%d ->) (%d %d ~ %d ~ %d)\n",
              ch, i,
              (int)buf[i][0], (int)x,
              (int)(vol[ch] * 1000), (int)freq[ch],
              (int)value,
              (int)(x + round(vol[ch] * envelope * 32767 X))
              );
            if (buf[i][0] >= 25000 || buf[i][0] < -25000) {
              printf(1, "retry\n");
              buf[i][0] = x;
              retry = 1;
              goto qwq;
            }
          }
        }
        */
      } else {
        for (int i = start; i < end; i++, rate = frac(rate + step)) {
          float v = vol[ch] * envelope * 32767 X;
          short ival = round(rate < 0.5f ? +v : -v);
          buf[i][0] += ival;
          /*
          short x = buf[i][0];
          buf[i][0] += (rate < 0.5f ? +amp : -amp);
          if (buf[i][0] >= 25000 || buf[i][0] < -25000) {
            printf(1, "channel %d: %d - %d (%d ->) (%d*%d %d)\n",
              ch, i,
              (int)buf[i][0], (int)x,
              (int)(vol[ch] * 1000), (int)(rate * 1000), (int)freq[ch]);
          }
          */
        }
      }
    } else if (ch == 2) {
      // Triangle
      for (int i = start; i < end; i++, rate = frac(rate + step)) {
        float v = vol[ch] * envelope * 32767 X;
        float val = (rate < 0.5f ? rate * 4 - 1 : 3 - rate * 4);
        short ival = round(val * v);
        buf[i][0] += ival;
      }
    } else if (ch == 3) {
      for (int i = start; i < end; i++) {
        float v = vol[ch] * envelope * 32767 X;
        short ival = round(my_randf() * v);
        buf[i][0] += ival;
      }
    }

/*
    for (int i = start; i < end; i++)
      if (buf[i][0] >= 32000 || buf[i][0] < -32000) {
        printf(1, "channel %d: %d - %d (%d %d)\n", ch, i, buf[i][0],
          (int)(vol[ch] * 1000), (int)freq[ch]);
      }
*/

    phase[ch] += block;
  }
  // for (int i = 0; i < block; i++) buf[i][1] = buf[i][0];
}

// Sequence data

/*
static inline unsigned read_int(char **_p)
{
  char *p = *_p;
  unsigned ret = 0;
  while (*p != '\0' && (*p < '0' || *p > '9')) *p++;
  while (*p >= '0' && *p <= '9') ret = ret * 10 + *(p++) - '0';
  *_p = p;
  return ret;
}

static inline void skip_nondigit(char **_p)
{
  char *p = *_p;
  while (*p != '\0' && (*p < '0' || *p > '9')) *p++;
  *_p = p;
}
*/

static int seq_total, seq_count;  // Length in samples, number of notes
static int seq_index, seq_sample; // Current index and sample

static struct note {
  unsigned t, ch, env;
  float freq, len, vol;
} notes[4096];

// Advance sequencer by a given number of samples

static inline void seq_advance(int samples)
{
  int i = seq_index;
  int start = seq_sample, end = seq_sample + samples;
  int done = 0;

  // Fails for very very short sequences (less than a block in total)
  // but who needs such ones anyway?
  while (1) {
    int delay;

    if (notes[i].t >= start && notes[i].t < end) {
      delay = notes[i].t - start;
      start = notes[i].t;
    } else if (notes[i].t + seq_total >= start && notes[i].t + seq_total < end) {
      delay = notes[i].t + seq_total - start;
      start = notes[i].t + seq_total;
    } else break;

    // Previous time
    dododo(buf + done, delay);
    done += delay;

    // Onset
    synth_note(notes[i].ch, notes[i].freq,
      notes[i].len, 0, notes[i].vol, notes[i].env);

    i = (i + 1 == seq_count ? 0 : i + 1);
  }

  dododo(buf + done, samples - done);
  seq_index = i;
  seq_sample = end % seq_total;
}

void game_audio(unsigned samples, int16_t *pcm)
{
  seq_advance(samples);
  for (unsigned i = 0; i < samples; i++)
    // pcm[i] = ((int)buf[i][0] + (int)buf[i][1]) >> 2;
    pcm[i] = buf[i][0] >> 1;
}

#include "korobeiniki.h"

void seq_read()
{
  // Parse data
  // <milliseconds for each tick>
  // <length in ticks>
  // Each note following:
  //   <time in ticks> <channel> <envelope on/off (1/0)>
  //   <MIDI note number> <length in ticks> <volume in percent>
  float secs_per_tick = krb_ticklen * 1e-6f;
  float samples_per_tick = secs_per_tick * 22050;
  seq_total = round(krb_seqtotal * samples_per_tick);

  int count = 0;
  do {
    notes[count].t = round(krb_seq[count][0] * samples_per_tick);
    notes[count].ch = krb_seq[count][1];
    notes[count].env = krb_seq[count][2];
    int pitch = krb_seq[count][3];
    notes[count].freq = 440.0f * pow2f((pitch - 69) / 12.0f);
    notes[count].len = krb_seq[count][4] * secs_per_tick;
    notes[count].vol = krb_seq[count][5] * 0.01f;
    count++;
  } while (count < (sizeof krb_seq / sizeof krb_seq[0]));

  seq_count = count;
  seq_index = seq_sample = 0;
}
