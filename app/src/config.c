/*
 * PACSCORDER — run-time configuration (command line). See config.h.
 */
#include "config.h"

#include <stdlib.h>
#include <string.h>

static gboolean
parse_fraction (const gchar *s, gint *n, gint *d)
{
  gchar *end = NULL;
  long num = strtol (s, &end, 10);
  long den = 1;

  if (end == s || num <= 0)
    return FALSE;
  if (*end == '/') {
    const gchar *ds = end + 1;
    den = strtol (ds, &end, 10);
    if (end == ds || den <= 0)
      return FALSE;
  }
  if (*end != '\0')
    return FALSE;
  *n = (gint) num;
  *d = (gint) den;
  return TRUE;
}

gboolean
pacs_config_parse (PacsConfig *cfg, int *argc, char ***argv, GError **error)
{
  gchar *backend = NULL;
  gchar *fps = NULL;
  gchar **rec_dirs = NULL;
  gchar **rtmp_urls = NULL;
  gint width = 1920, height = 1080;
  gint split = 1800, fragment = 1000, duration = 0, wbuf = 10;
  gint rec_br = 25000000, live_br = 17000000;
  gboolean ok = FALSE;

  memset (cfg, 0, sizeof *cfg);

  GOptionEntry entries[] = {
    { "backend", 0, 0, G_OPTION_ARG_STRING, &backend,
      "dev (test source + software encoder) or cm4 (v4l2src + v4l2h264enc); default dev", "NAME" },
    { "device", 0, 0, G_OPTION_ARG_FILENAME, &cfg->device,
      "cm4: capture video node (default /dev/video0; number not yet known, OQ-043)", "PATH" },
    { "width", 0, 0, G_OPTION_ARG_INT, &width, "capture width (default 1920)", "PX" },
    { "height", 0, 0, G_OPTION_ARG_INT, &height, "capture height (default 1080)", "PX" },
    { "fps", 0, 0, G_OPTION_ARG_STRING, &fps,
      "capture frame rate, N or N/D (default 60)", "RATE" },
    { "pattern", 0, 0, G_OPTION_ARG_STRING, &cfg->dev_pattern,
      "dev: videotestsrc pattern (default smpte)", "NAME" },
    { "rec-dir", 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &rec_dirs,
      "recording directory; give it twice to mirror (ADR-009)", "DIR" },
    { "split-seconds", 0, 0, G_OPTION_ARG_INT, &split,
      "start a new file every N seconds (default 1800, OQ-129)", "N" },
    { "fragment-ms", 0, 0, G_OPTION_ARG_INT, &fragment,
      "MP4 fragment duration in ms (default 1000 — UNDECIDED, OQ-118)", "MS" },
    { "writer-buffer-seconds", 0, 0, G_OPTION_ARG_INT, &wbuf,
      "a drive more than N s behind is treated as failed (default 10 — UNDECIDED, OQ-117)", "N" },
    { "rec-bitrate", 0, 0, G_OPTION_ARG_INT, &rec_br,
      "recording bit rate in bit/s (default 25000000, OQ-005)", "BPS" },
    { "live-bitrate", 0, 0, G_OPTION_ARG_INT, &live_br,
      "live bit rate in bit/s (default 17000000, OQ-005)", "BPS" },
    { "rtsp-url", 0, 0, G_OPTION_ARG_STRING, &cfg->rtsp_url,
      "publish the live encode over RTSP to this URL (MediaMTX; serves WebRTC viewers)", "URL" },
    { "rtmp-url", 0, 0, G_OPTION_ARG_STRING_ARRAY, &rtmp_urls,
      "publish the live encode over RTMP to this URL; repeatable, up to 4 (OQ-007)", "URL" },
    { "duration", 0, 0, G_OPTION_ARG_INT, &duration,
      "stop after N seconds (default 0 = until stopped, OQ-006)", "N" },
    G_OPTION_ENTRY_NULL
  };

  GOptionContext *ctx = g_option_context_new ("- PACSCORDER recorder");
  g_option_context_add_main_entries (ctx, entries, NULL);
  if (!g_option_context_parse (ctx, argc, argv, error))
    goto out;

  if (backend == NULL || g_str_equal (backend, "dev"))
    cfg->backend = PACS_BACKEND_DEV;
  else if (g_str_equal (backend, "cm4"))
    cfg->backend = PACS_BACKEND_CM4;
  else {
    g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                 "unknown backend '%s' (use dev or cm4)", backend);
    goto out;
  }

  cfg->fps_n = 60;
  cfg->fps_d = 1;
  if (fps != NULL && !parse_fraction (fps, &cfg->fps_n, &cfg->fps_d)) {
    g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                 "bad --fps '%s' (use N or N/D)", fps);
    goto out;
  }

  if (width <= 0 || height <= 0 || split <= 0 || fragment <= 0 || wbuf <= 0 ||
      duration < 0 || rec_br <= 0 || live_br <= 0) {
    g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                 "sizes, rates, durations and bit rates must be positive");
    goto out;
  }

  guint n = rec_dirs ? g_strv_length (rec_dirs) : 0;
  if (n == 0 || n > PACS_MAX_REC_DIRS) {
    g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                 "give --rec-dir once or twice (got %u)", n);
    goto out;
  }
  for (guint i = 0; i < n; i++)
    cfg->rec_dirs[i] = g_strdup (rec_dirs[i]);
  cfg->n_rec_dirs = (gint) n;

  n = rtmp_urls ? g_strv_length (rtmp_urls) : 0;
  if (n > PACS_MAX_RTMP) {
    g_set_error (error, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
                 "at most %d --rtmp-url (got %u)", PACS_MAX_RTMP, n);
    goto out;
  }
  for (guint i = 0; i < n; i++)
    cfg->rtmp_urls[i] = g_strdup (rtmp_urls[i]);
  cfg->n_rtmp = (gint) n;

  if (cfg->device == NULL)
    cfg->device = g_strdup ("/dev/video0");
  if (cfg->dev_pattern == NULL)
    cfg->dev_pattern = g_strdup ("smpte");

  cfg->width = width;
  cfg->height = height;
  cfg->split_seconds = (guint) split;
  cfg->fragment_ms = (guint) fragment;
  cfg->writer_buffer_seconds = (guint) wbuf;
  cfg->rec_bitrate = (guint) rec_br;
  cfg->live_bitrate = (guint) live_br;
  cfg->duration_seconds = (guint) duration;
  ok = TRUE;

out:
  g_option_context_free (ctx);
  g_free (backend);
  g_free (fps);
  g_strfreev (rec_dirs);
  g_strfreev (rtmp_urls);
  return ok;
}

void
pacs_config_clear (PacsConfig *cfg)
{
  g_free (cfg->device);
  g_free (cfg->dev_pattern);
  for (gint i = 0; i < cfg->n_rec_dirs; i++)
    g_free (cfg->rec_dirs[i]);
  g_free (cfg->rtsp_url);
  for (gint i = 0; i < cfg->n_rtmp; i++)
    g_free (cfg->rtmp_urls[i]);
  memset (cfg, 0, sizeof *cfg);
}

void
pacs_config_live_fps (const PacsConfig *cfg, gint *fps_n, gint *fps_d)
{
  /* fps > 30  <=>  fps_n > 30 * fps_d */
  if (cfg->fps_n > 30 * cfg->fps_d) {
    if (cfg->fps_n % 2 == 0) {
      *fps_n = cfg->fps_n / 2;
      *fps_d = cfg->fps_d;
    } else {
      *fps_n = cfg->fps_n;
      *fps_d = cfg->fps_d * 2;
    }
  } else {
    *fps_n = cfg->fps_n;
    *fps_d = cfg->fps_d;
  }
}
