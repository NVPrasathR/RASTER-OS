/*
 * Unit test for pacs_h264_min_level (app/src/h264_level.c).
 * Expected values are reasoning from ITU-T H.264 Table A-1 and the bit-rate
 * factors in docs/REFERENCES.md [L-01]..[L-14].
 */
#include <stdio.h>
#include <string.h>

#include "h264_level.h"

static int failures;

static void
expect (const char *what, int w, int h, int fn, int fd, unsigned long br,
        int high, const char *want)
{
  const char *got = pacs_h264_min_level (w, h, fn, fd, br, high != 0);
  int ok = (got == NULL && want == NULL) ||
           (got != NULL && want != NULL && strcmp (got, want) == 0);

  printf ("%-4s %-52s got %-4s want %s\n", ok ? "ok" : "FAIL", what,
          got ? got : "none", want ? want : "none");
  if (!ok)
    failures++;
}

int
main (void)
{
  /* 1080p60 = 489,600 MB/s: above Levels 4/4.1 (245,760), fits 4.2 [L-14] */
  expect ("recording 1080p60 High 25 Mbit/s", 1920, 1080, 60, 1, 25000000, 1, "4.2");
  /* 1080p50 = 408,000 MB/s: needs 4.2, Level 4.1 does not suffice [L-04], [L-14] */
  expect ("recording 1080p50 High 25 Mbit/s", 1920, 1080, 50, 1, 25000000, 1, "4.2");
  /* 1080p30 = 244,800 MB/s fits Level 4; 17 Mbit/s is 85 % of CB L4 [L-13] */
  expect ("live 1080p30 CB 17 Mbit/s", 1920, 1080, 30, 1, 17000000, 0, "4");
  /* 1080p25: MaxFS 8160 > 5120 rules out 3.2 [L-02] */
  expect ("live 1080p25 CB 17 Mbit/s", 1920, 1080, 25, 1, 17000000, 0, "4");
  /* 29.97 fps */
  expect ("live 1080p29.97 CB 17 Mbit/s", 1920, 1080, 30000, 1001, 17000000, 0, "4");
  /* 720p30 fits 3.1 by MB rate, but 17 Mbit/s > 14 Mbit/s CB L3.1 [L-01], [L-06] */
  expect ("live 720p30 CB 17 Mbit/s", 1280, 720, 30, 1, 17000000, 0, "3.2");
  expect ("live 720p30 CB 10 Mbit/s", 1280, 720, 30, 1, 10000000, 0, "3.1");
  /* 25 Mbit/s equals the High L4 ceiling exactly; CB L4 is 20 Mbit/s [L-12] */
  expect ("1080p30 High 25 Mbit/s", 1920, 1080, 30, 1, 25000000, 1, "4");
  expect ("1080p30 CB 25 Mbit/s", 1920, 1080, 30, 1, 25000000, 0, "4.1");
  /* above Level 4.2 */
  expect ("1080p120 High", 1920, 1080, 120, 1, 25000000, 1, NULL);
  expect ("invalid size", 0, 1080, 30, 1, 1000, 0, NULL);

  printf ("%d failure(s)\n", failures);
  return failures == 0 ? 0 : 1;
}
