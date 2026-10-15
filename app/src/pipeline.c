/*
 * PACSCORDER — the GStreamer pipeline. See pipeline.h.
 *
 * Element settings and their sources are documented in
 * docs/SOFTWARE_ARCHITECTURE.md §10 (slices C1, C2, C5) and ADR-007,
 * ADR-015, ADR-016.
 */
#include "pipeline.h"

#include <errno.h>
#include <glib/gstdio.h>

#include <gst/app/gstappsink.h>
#include <gst/video/video.h>

#include "h264_level.h"
#include "publisher.h"
#include "writer.h"

#define STOP_TIMEOUT_SECONDS 15   /* give up waiting for end of stream */

struct _PacsPipeline {
  const PacsConfig *cfg;
  GstElement *pipeline;
  GMainLoop *loop;
  guint bus_watch;
  gboolean ok;
  gboolean stopping;
  gint n_capture;   /* buffers counted in streaming threads: g_atomic_int_* */
  gint n_rec;
  gint n_live;
  GPtrArray *publishers;      /* PacsPublisher *, slice C2 */
  GstElement *live_out;       /* appsink feeding the publishers */
  gboolean publishers_started;
  GPtrArray *writers;         /* PacsWriter *, one per drive, slice C5 */
  GstElement *rec_out;        /* appsink feeding the writers */
  gchar *session;             /* file-name prefix: recording start time */
  gint file_index;            /* current file number; g_atomic_int_* */
  gint split_pending;         /* g_atomic_int_*: next keyframe starts a new file */
  gint join_pending;          /* g_atomic_int_*: next keyframe starts the first file */
  guint split_timer;
  gint writers_pending;       /* writers still closing their last file */
  gboolean main_eos;
};

/* Create an element and add it to the bin, which then owns it. */
static GstElement *
add (GstBin *bin, const gchar *factory, const gchar *name, GError **error)
{
  GstElement *e = gst_element_factory_make (factory, name);

  if (e == NULL) {
    g_set_error (error, GST_CORE_ERROR, GST_CORE_ERROR_MISSING_PLUGIN,
                 "GStreamer element '%s' is not available", factory);
    return NULL;
  }
  gst_bin_add (bin, e);
  return e;
}

static GstElement *
add_capsfilter (GstBin *bin, const gchar *name, const gchar *caps_str,
                GError **error)
{
  GstElement *cf = add (bin, "capsfilter", name, error);
  GstCaps *caps;

  if (cf == NULL)
    return NULL;
  caps = gst_caps_from_string (caps_str);
  if (caps == NULL) {
    g_set_error (error, GST_CORE_ERROR, GST_CORE_ERROR_NEGOTIATION,
                 "bad caps string '%s'", caps_str);
    return NULL;
  }
  g_object_set (cf, "caps", caps, NULL);
  gst_caps_unref (caps);
  return cf;
}

static gboolean
link (GstElement *a, GstElement *b, GError **error)
{
  if (!gst_element_link (a, b)) {
    g_set_error (error, GST_CORE_ERROR, GST_CORE_ERROR_NEGOTIATION,
                 "cannot link %s to %s", GST_ELEMENT_NAME (a),
                 GST_ELEMENT_NAME (b));
    return FALSE;
  }
  return TRUE;
}

/*
 * The capture source.
 * dev: videotestsrc (is-live, pattern) [L-38] with I420 caps [L-39] — a
 *      development stand-in only.
 * cm4: v4l2src on the Unicam video node (legacy mode, ADR-010). The mode must
 *      already match the source's DV timings. UYVY follows ADR-005 (PROPOSED).
 */
static GstElement *
add_source (GstBin *bin, const PacsConfig *cfg, GError **error)
{
  GstElement *src, *caps;
  gchar *caps_str;

  if (cfg->backend == PACS_BACKEND_DEV) {
    src = add (bin, "videotestsrc", "src", error);
    if (src == NULL)
      return NULL;
    g_object_set (src, "is-live", TRUE, NULL);
    gst_util_set_object_arg (G_OBJECT (src), "pattern", cfg->dev_pattern);
    caps_str = g_strdup_printf ("video/x-raw,format=I420,width=%d,height=%d,"
                                "framerate=%d/%d", cfg->width, cfg->height,
                                cfg->fps_n, cfg->fps_d);
  } else {
    src = add (bin, "v4l2src", "src", error);
    if (src == NULL)
      return NULL;
    g_object_set (src, "device", cfg->device, NULL);
    caps_str = g_strdup_printf ("video/x-raw,format=UYVY,width=%d,height=%d,"
                                "framerate=%d/%d", cfg->width, cfg->height,
                                cfg->fps_n, cfg->fps_d);
  }
  caps = add_capsfilter (bin, "src_caps", caps_str, error);
  g_free (caps_str);
  if (caps == NULL || !link (src, caps, error))
    return NULL;
  return caps;
}

/*
 * One H.264 encoder plus the caps that fix its profile and level.
 * Returns the capsfilter after the encoder; *enc_out is the encoder.
 *
 * cm4 (bcm2835-codec through v4l2h264enc):
 *   recording: High, Level 4.2, VBR (video_bitrate_mode=0) at rec_bitrate (OQ-005)
 *   live:      Constrained Baseline, lowest level that fits (L-01..L-07),
 *              CBR (video_bitrate_mode=1) at live_bitrate, SPS/PPS repeated [D-15]
 *   Profile and level go in caps, which override extra-controls [L-61], [L-62];
 *   the other controls go in extra-controls, by their normalised names [L-63].
 *   A misspelt control is silently skipped [L-55]; tools/encoder_capacity_test.sh
 *   checks the names on the device.
 * dev: x264enc with zerolatency (no B-frames, nothing held back) [K-27], [K-28];
 *   a stand-in that exercises the pipeline, not the product's encoder settings.
 *   The recording role uses the 'superfast' preset because 'ultrafast' emits
 *   Constrained Baseline even when the caps ask for High (observed 2026-10-12
 *   with x264enc in GStreamer 1.28.7 on the development machine).
 */
static GstElement *
add_encoder (GstBin *bin, const PacsConfig *cfg, gboolean recording,
             gint fps_n, gint fps_d, GstElement **enc_out, GError **error)
{
  const gchar *role = recording ? "rec" : "live";
  gchar *name = g_strdup_printf ("%s_enc", role);
  gchar *caps_name = g_strdup_printf ("%s_h264_caps", role);
  guint bitrate = recording ? cfg->rec_bitrate : cfg->live_bitrate;
  const gchar *profile = recording ? "high" : "constrained-baseline";
  GstElement *enc = NULL, *caps = NULL;
  gchar *caps_str = NULL;

  if (cfg->backend == PACS_BACKEND_CM4) {
    const gchar *level = recording ? "4.2"
        : pacs_h264_min_level (cfg->width, cfg->height, fps_n, fps_d, bitrate,
                               FALSE);
    gchar *controls;
    GstStructure *s;

    if (level == NULL) {
      g_set_error (error, GST_CORE_ERROR, GST_CORE_ERROR_NEGOTIATION,
                   "no H.264 level up to 4.2 fits the %s encode", role);
      goto out;
    }
    enc = add (bin, "v4l2h264enc", name, error);
    if (enc == NULL)
      goto out;
    controls = g_strdup_printf ("controls,video_bitrate_mode=%d,video_bitrate=%u,"
                                "h264_i_frame_period=60,repeat_sequence_header=%d",
                                recording ? 0 : 1, bitrate, recording ? 0 : 1);
    s = gst_structure_from_string (controls, NULL);
    g_free (controls);
    g_object_set (enc, "extra-controls", s, NULL);
    gst_structure_free (s);
    caps_str = g_strdup_printf ("video/x-h264,profile=(string)%s,"
                                "level=(string)%s", profile, level);
  } else {
    enc = add (bin, "x264enc", name, error);
    if (enc == NULL)
      goto out;
    gst_util_set_object_arg (G_OBJECT (enc), "tune", "zerolatency");
    gst_util_set_object_arg (G_OBJECT (enc), "speed-preset",
                             recording ? "superfast" : "ultrafast");
    g_object_set (enc, "bitrate", bitrate / 1000,           /* kbit/s */
                  "key-int-max", (guint) (2 * fps_n / fps_d), NULL);
    caps_str = g_strdup_printf ("video/x-h264,profile=(string)%s", profile);
  }

  caps = add_capsfilter (bin, caps_name, caps_str, error);
  if (caps != NULL && !link (enc, caps, error))
    caps = NULL;

out:
  *enc_out = enc;
  g_free (caps_str);
  g_free (caps_name);
  g_free (name);
  return caps;
}

static GstPadProbeReturn
count_buffer (GstPad *pad, GstPadProbeInfo *info, gpointer counter)
{
  (void) pad;
  (void) info;
  g_atomic_int_inc ((gint *) counter);
  return GST_PAD_PROBE_OK;
}

/* Count the buffers that pass a static pad of an element. */
static void
add_counter (GstElement *e, const gchar *pad_name, gint *counter)
{
  GstPad *pad = gst_element_get_static_pad (e, pad_name);

  gst_pad_add_probe (pad, GST_PAD_PROBE_TYPE_BUFFER, count_buffer, counter,
                     NULL);
  gst_object_unref (pad);
}

/* Express a buffer's timestamps as running time in the main pipeline.
 * Encoders may shift their output timestamps (by 1000 hours with the encoder
 * used on the development machine, observed 2026-10-12) and describe the shift
 * in the segment; a publisher's appsrc starts its own segment at 0, so it must
 * receive running time or its sink waits for the shifted time. The publishers
 * share the main pipeline's clock and base time (publisher.c). */
static GstSample *
to_running_time (GstSample *sample)
{
  GstBuffer *in = gst_sample_get_buffer (sample);
  const GstSegment *seg = gst_sample_get_segment (sample);
  GstBuffer *out;
  GstSample *res;

  if (in == NULL || seg == NULL || seg->format != GST_FORMAT_TIME)
    return gst_sample_ref (sample);
  out = gst_buffer_copy (in);          /* shares the memory; new metadata */
  if (GST_BUFFER_PTS_IS_VALID (in))
    GST_BUFFER_PTS (out) = gst_segment_to_running_time (seg, GST_FORMAT_TIME,
                                                        GST_BUFFER_PTS (in));
  if (GST_BUFFER_DTS_IS_VALID (in))
    GST_BUFFER_DTS (out) = gst_segment_to_running_time (seg, GST_FORMAT_TIME,
                                                        GST_BUFFER_DTS (in));
  res = gst_sample_new (out, gst_sample_get_caps (sample), NULL, NULL);
  gst_buffer_unref (out);
  return res;
}

/* appsink → every publisher (runs in the live branch's streaming thread). */
static GstFlowReturn
on_live_sample (GstAppSink *sink, gpointer data)
{
  PacsPipeline *p = data;
  GstSample *sample = gst_app_sink_pull_sample (sink);
  GstSample *rt;

  if (sample == NULL)
    return GST_FLOW_EOS;
  rt = to_running_time (sample);
  for (guint i = 0; i < p->publishers->len; i++)
    pacs_publisher_push (g_ptr_array_index (p->publishers, i), rt);
  gst_sample_unref (rt);
  gst_sample_unref (sample);
  return GST_FLOW_OK;
}

static GstAppSinkCallbacks live_callbacks = { .new_sample = on_live_sample };

/* Ask the live encoder for a keyframe, e.g. when a publisher (re)starts:
 * an upstream force-key-unit event from the appsink [K-31]. */
static void
request_live_keyframe (gpointer data)
{
  PacsPipeline *p = data;

  if (p->live_out != NULL)
    gst_element_send_event (p->live_out,
        gst_video_event_new_upstream_force_key_unit (GST_CLOCK_TIME_NONE, TRUE, 0));
}

/* appsink → every drive writer (recording branch streaming thread). */
static GstFlowReturn
on_rec_sample (GstAppSink *sink, gpointer data)
{
  PacsPipeline *p = data;
  GstSample *sample = gst_app_sink_pull_sample (sink);
  GstSample *rt;

  if (sample == NULL)
    return GST_FLOW_EOS;
  rt = to_running_time (sample);

  /* The split point is decided here, on the frame itself, so that every drive
   * — including one that rejoins — starts its file on the same keyframe, even
   * if a drive's own queue lags behind. */
  GstBuffer *buf = gst_sample_get_buffer (rt);
  if (buf != NULL && !GST_BUFFER_FLAG_IS_SET (buf, GST_BUFFER_FLAG_DELTA_UNIT)) {
    gboolean split = g_atomic_int_compare_and_exchange (&p->split_pending, 1, 0);
    gboolean join = g_atomic_int_compare_and_exchange (&p->join_pending, 1, 0);

    if (split) {
      g_atomic_int_inc (&p->file_index);
      g_print ("pacscorder: split: file %05d starts at %" GST_TIME_FORMAT "\n",
               g_atomic_int_get (&p->file_index), GST_TIME_ARGS (GST_BUFFER_PTS (buf)));
    }
    if (split || join) {
      for (guint i = 0; i < p->writers->len; i++) {
        PacsWriter *w = g_ptr_array_index (p->writers, i);

        if (split)
          pacs_writer_split_at (w, GST_BUFFER_PTS (buf));
        pacs_writer_activate (w);
      }
    }
  }

  for (guint i = 0; i < p->writers->len; i++)
    pacs_writer_push (g_ptr_array_index (p->writers, i), rt);
  gst_sample_unref (rt);
  gst_sample_unref (sample);
  return GST_FLOW_OK;
}

static GstAppSinkCallbacks rec_callbacks = { .new_sample = on_rec_sample };

static void
request_rec_keyframe (PacsPipeline *p)
{
  if (p->rec_out != NULL)
    gst_element_send_event (p->rec_out,
        gst_video_event_new_upstream_force_key_unit (GST_CLOCK_TIME_NONE, TRUE, 0));
}

static void
on_writer_done (PacsWriter *w, gpointer data)
{
  PacsPipeline *p = data;

  (void) w;
  if (p->main_eos && --p->writers_pending <= 0) {
    g_print ("pacscorder: all recording files closed\n");
    p->ok = TRUE;
    g_main_loop_quit (p->loop);
  }
}

/* Every split_seconds (OQ-129: 30 minutes): let any missing or failed drive
 * rejoin (it waits, joining), mark a split as pending, and force a keyframe.
 * The split itself happens on that keyframe in on_rec_sample, for all drives. */
static gboolean
on_split (gpointer data)
{
  PacsPipeline *p = data;
  guint active = 0;

  for (guint i = 0; i < p->writers->len; i++) {
    PacsWriter *w = g_ptr_array_index (p->writers, i);

    if (pacs_writer_state (w) == PACS_WRITER_ABSENT && pacs_writer_try_start (w))
      g_print ("pacscorder: recording drive %s rejoins from file %05d\n",
               pacs_writer_dir (w), g_atomic_int_get (&p->file_index) + 1);
    if (pacs_writer_state (w) != PACS_WRITER_ABSENT)
      active++;
  }
  g_atomic_int_set (&p->split_pending, 1);
  request_rec_keyframe (p);
  if (active == 0)
    g_printerr ("pacscorder: ALERT: no recording drive is available\n");
  return G_SOURCE_CONTINUE;
}

static gboolean
create_writers (PacsPipeline *p)
{
  const PacsConfig *cfg = p->cfg;
  GDateTime *now = g_date_time_new_now_local ();

  p->session = g_date_time_format (now, "%Y%m%d-%H%M%S");
  g_date_time_unref (now);
  for (gint i = 0; i < cfg->n_rec_dirs; i++)
    g_ptr_array_add (p->writers,
        pacs_writer_new (cfg->rec_dirs[i], p->session, &p->file_index,
                         cfg->writer_buffer_seconds, cfg->fragment_ms,
                         on_writer_done, p));
  return TRUE;
}

static gboolean
create_publishers (PacsPipeline *p, GError **error)
{
  const PacsConfig *cfg = p->cfg;
  PacsPublisher *pub;

  if (cfg->rtsp_url != NULL) {
    pub = pacs_publisher_new (PACS_PUBLISH_RTSP, cfg->rtsp_url,
                              request_live_keyframe, p, error);
    if (pub == NULL)
      return FALSE;
    g_ptr_array_add (p->publishers, pub);
  }
  for (gint i = 0; i < cfg->n_rtmp; i++) {
    pub = pacs_publisher_new (PACS_PUBLISH_RTMP, cfg->rtmp_urls[i],
                              request_live_keyframe, p, error);
    if (pub == NULL)
      return FALSE;
    g_ptr_array_add (p->publishers, pub);
  }
  return TRUE;
}

static void
start_publishers (PacsPipeline *p)
{
  GstClock *clock;

  if (p->publishers_started || p->publishers->len == 0)
    return;
  p->publishers_started = TRUE;
  clock = gst_pipeline_get_clock (GST_PIPELINE (p->pipeline));
  for (guint i = 0; i < p->publishers->len; i++)
    pacs_publisher_start (g_ptr_array_index (p->publishers, i), clock,
                          gst_element_get_base_time (p->pipeline));
  gst_clear_object (&clock);
}

static gboolean
build (PacsPipeline *p, GError **error)
{
  const PacsConfig *cfg = p->cfg;
  GstBin *bin = GST_BIN (p->pipeline);
  GstElement *src_out, *cap_tee, *rec_q, *rec_enc, *rec_caps, *rec_parse,
      *rec_out, *live_q, *rate, *rate_caps, *live_enc, *live_caps, *live_sink;
  gint live_n, live_d;
  gchar *rate_str;

  pacs_config_live_fps (cfg, &live_n, &live_d);

  /* capture */
  if ((src_out = add_source (bin, cfg, error)) == NULL)
    return FALSE;
  if ((cap_tee = add (bin, "tee", "capture_tee", error)) == NULL ||
      !link (src_out, cap_tee, error))
    return FALSE;

  /* recording branch: one encode, handed to one writer pipeline per drive
   * (ADR-016) through an appsink that never drops; the writers buffer and
   * isolate drive problems themselves (writer.c). */
  if ((rec_q = add (bin, "queue", "rec_queue", error)) == NULL)
    return FALSE;
  if ((rec_caps = add_encoder (bin, cfg, TRUE, cfg->fps_n, cfg->fps_d,
                               &rec_enc, error)) == NULL)
    return FALSE;
  if ((rec_parse = add (bin, "h264parse", "rec_parse", error)) == NULL ||
      (rec_out = add (bin, "appsink", "rec_out", error)) == NULL)
    return FALSE;
  g_object_set (rec_out, "sync", FALSE, "drop", FALSE, NULL);
  gst_app_sink_set_callbacks (GST_APP_SINK (rec_out), &rec_callbacks, p, NULL);
  p->rec_out = rec_out;
  if (!link (cap_tee, rec_q, error) || !link (rec_q, rec_enc, error) ||
      !link (rec_caps, rec_parse, error) || !link (rec_parse, rec_out, error))
    return FALSE;

  /* frame counters, reported at the end of the run */
  add_counter (src_out, "src", &p->n_capture);
  add_counter (rec_parse, "sink", &p->n_rec);

  /* live branch: drop to the live rate with no added latency [M-54], then encode */
  if ((live_q = add (bin, "queue", "live_queue", error)) == NULL ||
      (rate = add (bin, "videorate", "live_rate", error)) == NULL)
    return FALSE;
  g_object_set (rate, "drop-only", TRUE, NULL);
  rate_str = g_strdup_printf ("video/x-raw,framerate=%d/%d", live_n, live_d);
  rate_caps = add_capsfilter (bin, "live_rate_caps", rate_str, error);
  g_free (rate_str);
  if (rate_caps == NULL)
    return FALSE;
  if ((live_caps = add_encoder (bin, cfg, FALSE, live_n, live_d, &live_enc,
                                error)) == NULL)
    return FALSE;
  /* Live output: with publishers, an appsink hands each encoded frame to them
   * (each runs in its own pipeline, ADR-015); without, a fakesink. Neither may
   * block the encoder: the appsink drops old frames if it ever falls behind. */
  if (p->publishers->len > 0) {
    live_sink = add (bin, "appsink", "live_out", error);
    if (live_sink == NULL)
      return FALSE;
    g_object_set (live_sink, "sync", FALSE, "max-buffers", 8, "drop", TRUE, NULL);
    gst_app_sink_set_callbacks (GST_APP_SINK (live_sink), &live_callbacks, p, NULL);
    p->live_out = live_sink;
  } else {
    live_sink = add (bin, "fakesink", "live_sink", error);
    if (live_sink == NULL)
      return FALSE;
    g_object_set (live_sink, "sync", FALSE, NULL);
  }
  add_counter (live_sink, "sink", &p->n_live);
  return link (cap_tee, live_q, error) && link (live_q, rate, error) &&
      link (rate, rate_caps, error) && link (rate_caps, live_enc, error) &&
      link (live_caps, live_sink, error);
}

static gboolean
on_stop_timeout (gpointer data)
{
  PacsPipeline *p = data;

  g_printerr ("pacscorder: no end of stream within %d s; stopping anyway\n",
              STOP_TIMEOUT_SECONDS);
  p->ok = FALSE;
  g_main_loop_quit (p->loop);
  return G_SOURCE_REMOVE;
}

static gboolean
on_bus (GstBus *bus, GstMessage *msg, gpointer data)
{
  PacsPipeline *p = data;
  GError *err = NULL;
  gchar *dbg = NULL;

  (void) bus;
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
      /* The encoders are drained; now let every drive close its last file. */
      g_print ("pacscorder: end of stream\n");
      if (!p->main_eos) {
        p->main_eos = TRUE;
        p->writers_pending = (gint) p->writers->len;
        for (guint i = 0; i < p->writers->len; i++)
          pacs_writer_finish (g_ptr_array_index (p->writers, i));
        if (p->writers_pending <= 0) {
          p->ok = TRUE;
          g_main_loop_quit (p->loop);
        }
      }
      break;
    case GST_MESSAGE_ERROR:
      gst_message_parse_error (msg, &err, &dbg);
      g_printerr ("pacscorder: error from %s: %s\n%s\n",
                  GST_OBJECT_NAME (GST_MESSAGE_SRC (msg)), err->message,
                  dbg ? dbg : "");
      p->ok = FALSE;
      g_main_loop_quit (p->loop);
      break;
    case GST_MESSAGE_WARNING:
      gst_message_parse_warning (msg, &err, &dbg);
      g_printerr ("pacscorder: warning from %s: %s\n",
                  GST_OBJECT_NAME (GST_MESSAGE_SRC (msg)), err->message);
      break;
    case GST_MESSAGE_STATE_CHANGED:
      if (GST_MESSAGE_SRC (msg) == GST_OBJECT (p->pipeline)) {
        GstState new_state;

        gst_message_parse_state_changed (msg, NULL, &new_state, NULL);
        if (new_state == GST_STATE_PLAYING)
          start_publishers (p);
      }
      break;
    default:
      break;
  }
  g_clear_error (&err);
  g_free (dbg);
  return G_SOURCE_CONTINUE;
}

static gboolean
on_duration (gpointer data)
{
  pacs_pipeline_stop (data);
  return G_SOURCE_REMOVE;
}

PacsPipeline *
pacs_pipeline_new (const PacsConfig *cfg, GError **error)
{
  PacsPipeline *p = g_new0 (PacsPipeline, 1);
  GstBus *bus;

  p->cfg = cfg;
  p->pipeline = gst_pipeline_new ("pacscorder");
  p->loop = g_main_loop_new (NULL, FALSE);
  p->publishers = g_ptr_array_new_with_free_func ((GDestroyNotify) pacs_publisher_free);
  p->writers = g_ptr_array_new_with_free_func ((GDestroyNotify) pacs_writer_free);
  if (!create_writers (p) || !create_publishers (p, error) || !build (p, error)) {
    pacs_pipeline_free (p);
    return NULL;
  }
  bus = gst_pipeline_get_bus (GST_PIPELINE (p->pipeline));
  p->bus_watch = gst_bus_add_watch (bus, on_bus, p);
  gst_object_unref (bus);
  return p;
}

gboolean
pacs_pipeline_run (PacsPipeline *p)
{
  guint started = 0;

  g_print ("pacscorder: recording session %s\n", p->session);
  for (guint i = 0; i < p->writers->len; i++)
    if (pacs_writer_try_start (g_ptr_array_index (p->writers, i)))
      started++;
  if (started == 0)
    g_printerr ("pacscorder: ALERT: no recording drive is available; drives are "
                "retried at every file split\n");
  g_atomic_int_set (&p->join_pending, 1);    /* first keyframe opens file 00000 */
  p->split_timer = g_timeout_add_seconds (p->cfg->split_seconds, on_split, p);

  if (gst_element_set_state (p->pipeline, GST_STATE_PLAYING) ==
      GST_STATE_CHANGE_FAILURE) {
    g_printerr ("pacscorder: pipeline failed to start\n");
    gst_element_set_state (p->pipeline, GST_STATE_NULL);
    return FALSE;
  }
  if (p->cfg->duration_seconds > 0)
    g_timeout_add_seconds (p->cfg->duration_seconds, on_duration, p);
  g_main_loop_run (p->loop);
  for (guint i = 0; i < p->publishers->len; i++) {
    PacsPublisher *pub = g_ptr_array_index (p->publishers, i);

    pacs_publisher_stop (pub);
    g_print ("pacscorder: publisher %s: %u restart(s)\n", pacs_publisher_log_url (pub),
             pacs_publisher_restarts (pub));
  }
  gst_element_set_state (p->pipeline, GST_STATE_NULL);
  if (p->split_timer) {
    g_source_remove (p->split_timer);
    p->split_timer = 0;
  }
  for (guint i = 0; i < p->writers->len; i++) {
    PacsWriter *w = g_ptr_array_index (p->writers, i);

    g_print ("pacscorder: drive %s: %u file(s) closed, %u failure(s)\n",
             pacs_writer_dir (w), pacs_writer_files (w), pacs_writer_failures (w));
  }
  g_print ("pacscorder: frames captured %d, recording encode %d, live encode %d\n",
           g_atomic_int_get (&p->n_capture), g_atomic_int_get (&p->n_rec),
           g_atomic_int_get (&p->n_live));
  return p->ok;
}

void
pacs_pipeline_stop (PacsPipeline *p)
{
  if (p->stopping)
    return;
  p->stopping = TRUE;
  g_print ("pacscorder: stopping (finalising files)\n");
  gst_element_send_event (p->pipeline, gst_event_new_eos ());
  g_timeout_add_seconds (STOP_TIMEOUT_SECONDS, on_stop_timeout, p);
}

void
pacs_pipeline_free (PacsPipeline *p)
{
  if (p == NULL)
    return;
  if (p->bus_watch)
    g_source_remove (p->bus_watch);
  if (p->pipeline) {
    gst_element_set_state (p->pipeline, GST_STATE_NULL);
    gst_object_unref (p->pipeline);
  }
  if (p->loop)
    g_main_loop_unref (p->loop);
  if (p->publishers)
    g_ptr_array_unref (p->publishers);
  if (p->split_timer)
    g_source_remove (p->split_timer);
  if (p->writers)
    g_ptr_array_unref (p->writers);
  g_free (p->session);
  g_free (p);
}
