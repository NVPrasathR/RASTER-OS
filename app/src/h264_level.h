/*
 * PACSCORDER — minimum H.264 level for an encode.
 *
 * Limits from ITU-T H.264 (V16, 06/2026) Table A-1 [L-01]..[L-05] and the
 * bit-rate factors of A.3.1 / Table A-2 [L-06], [L-07]. No GStreamer or GLib
 * dependency, so it can be unit-tested on any machine.
 */
#ifndef PACS_H264_LEVEL_H
#define PACS_H264_LEVEL_H

#include <stdbool.h>

/*
 * Return the lowest level, as the caps string GStreamer's v4l2h264enc accepts
 * ("3.1", "3.2", "4", "4.1", "4.2"; Level 4.0 is "4" [L-61]), whose frame size,
 * macroblock rate and VCL bit-rate ceiling all fit the encode. Return NULL if
 * none of these levels fits.
 *
 * fps_n/fps_d: frame rate; bitrate: bit/s; high_profile: true for High
 * (factor 1250), false for Constrained Baseline / Main (factor 1000).
 */
const char *pacs_h264_min_level (int width, int height, int fps_n, int fps_d,
                                 unsigned long bitrate, bool high_profile);

#endif
