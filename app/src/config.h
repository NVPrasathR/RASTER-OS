/*
 * PACSCORDER — run-time configuration (command line).
 *
 * Defaults are the owner decisions recorded in docs/OPEN_QUESTIONS.md
 * (OQ-005, OQ-129, OQ-131); values marked UNDECIDED are placeholders that
 * must not be read as decisions.
 */
#ifndef PACS_CONFIG_H
#define PACS_CONFIG_H

#include <glib.h>

typedef enum {
  PACS_BACKEND_DEV,   /* development machine: videotestsrc + x264enc stand-ins */
  PACS_BACKEND_CM4    /* product: v4l2src (Unicam video node, ADR-010) + v4l2h264enc */
} PacsBackend;

#define PACS_MAX_REC_DIRS 2   /* ADR-009: NVMe SSD and HDD mirror */

typedef struct {
  PacsBackend backend;

  /* Capture. On the CM4 the mode must already be set from the source's
   * DV timings (scripts/bringup/tc358743.sh until the application does it). */
  gchar *device;          /* cm4 video node; the number is UNKNOWN (OQ-043) */
  gint   width;
  gint   height;
  gint   fps_n;
  gint   fps_d;
  gchar *dev_pattern;     /* dev backend: videotestsrc pattern */

  /* Recording encode and files (OQ-005, ADR-009, OQ-129). */
  gchar *rec_dirs[PACS_MAX_REC_DIRS];
  gint   n_rec_dirs;
  guint  split_seconds;   /* new file every 30 minutes = 1800 (OQ-129) */
  guint  fragment_ms;     /* mp4mux fragment-duration — UNDECIDED (OQ-118) */
  guint  rec_bitrate;     /* bit/s: 25 Mbit/s VBR (OQ-005) */

  /* Live encode (OQ-005, OQ-131). */
  guint  live_bitrate;    /* bit/s: 17 Mbit/s CBR (OQ-005) */

  /* Run control. */
  guint  duration_seconds;  /* 0 = until stopped (OQ-006) */
} PacsConfig;

/* Fill cfg from argv. Returns FALSE and sets error on bad input. */
gboolean pacs_config_parse (PacsConfig *cfg, int *argc, char ***argv,
                            GError **error);
void pacs_config_clear (PacsConfig *cfg);

/* Live frame rate (OQ-131): the capture rate halved when it is above 30 fps
 * (60 -> 30, 59.94 -> 29.97, 50 -> 25), otherwise the capture rate. */
void pacs_config_live_fps (const PacsConfig *cfg, gint *fps_n, gint *fps_d);

#endif
