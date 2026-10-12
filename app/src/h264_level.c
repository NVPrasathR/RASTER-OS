/*
 * PACSCORDER — minimum H.264 level for an encode. See h264_level.h.
 */
#include "h264_level.h"

#include <stddef.h>

typedef struct {
  const char *name;            /* caps string [L-61] */
  unsigned long max_mbps;      /* MaxMBPS, macroblocks/s */
  unsigned long max_fs;        /* MaxFS, macroblocks */
  unsigned long max_br;        /* MaxBR, in units of the profile factor */
} Level;

/* ITU-T H.264 Table A-1 [L-01]..[L-05]. */
static const Level levels[] = {
  { "3.1", 108000, 3600, 14000 },
  { "3.2", 216000, 5120, 20000 },
  { "4",   245760, 8192, 20000 },
  { "4.1", 245760, 8192, 50000 },
  { "4.2", 522240, 8704, 50000 },
};

const char *
pacs_h264_min_level (int width, int height, int fps_n, int fps_d,
                     unsigned long bitrate, bool high_profile)
{
  if (width <= 0 || height <= 0 || fps_n <= 0 || fps_d <= 0)
    return NULL;

  /* Frame size in macroblocks: 1920x1080 is coded as 120 x 68 = 8160 [L-09]. */
  unsigned long mbs = (unsigned long) ((width + 15) / 16) *
                      (unsigned long) ((height + 15) / 16);
  /* VCL bit-rate factor: 1000 for Baseline/Constrained Baseline/Main [L-06],
   * 1250 for High [L-07]. */
  unsigned long factor = high_profile ? 1250UL : 1000UL;

  for (size_t i = 0; i < sizeof levels / sizeof levels[0]; i++) {
    const Level *l = &levels[i];
    /* mbs * fps_n / fps_d <= max_mbps, without floating point */
    if (mbs > l->max_fs)
      continue;
    if (mbs * (unsigned long) fps_n > l->max_mbps * (unsigned long) fps_d)
      continue;
    if (bitrate > l->max_br * factor)
      continue;
    return l->name;
  }
  return NULL;
}
