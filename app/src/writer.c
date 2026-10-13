/*
 * PACSCORDER — one recording drive. See writer.h.
 */
#include "writer.h"

#include <errno.h>
#include <glib/gstdio.h>
#include <gst/app/gstappsrc.h>
#include <string.h>
#include <unistd.h>

struct _PacsWriter {
  gchar *dir;
  gchar *session;
  const gint *file_index;
  guint buffer_seconds;
  guint fragment_ms;
  PacsWriterDone done;
  gpointer user_data;

  GstElement *pipeline;
  GstElement *appsrc;
  GstElement *splitmux;
  guint bus_watch;

  gint state;            /* PacsWriterState; g_atomic_int_*: read in streaming threads */
  gint stall_reported;   /* g_atomic_int_*: one stall report per failure */
  guint files;
  guint failures;
};

static void
destroy_pipeline (PacsWriter *w)
{
  if (w->bus_watch) {
    g_source_remove (w->bus_watch);
    w->bus_watch = 0;
  }
  if (w->pipeline) {
    gst_element_set_state (w->pipeline, GST_STATE_NULL);
    gst_object_unref (w->pipeline);
    w->pipeline = w->appsrc = w->splitmux = NULL;
  }
}

/* Stop writing after a failure; the drive is retried at the next split. */
static void
fail (PacsWriter *w, const gchar *why)
{
  gboolean finishing = g_atomic_int_get (&w->state) == PACS_WRITER_FINISHING;

  g_printerr ("pacscorder: ALERT: recording drive %s: %s — recording continues "
              "on the other drive; this drive is retried at the next file\n",
              w->dir, why);
  w->failures++;
  g_atomic_int_set (&w->state, PACS_WRITER_ABSENT);
  destroy_pipeline (w);
  g_atomic_int_set (&w->stall_reported, 0);
  if (finishing && w->done)
    w->done (w, w->user_data);
}

static gchar *
on_format_location (GstElement *splitmux, guint fragment_id, gpointer data)
{
  PacsWriter *w = data;
  gchar *name = g_strdup_printf ("pacs-%s-%05d.mp4", w->session,
                                 g_atomic_int_get ((gint *) w->file_index));

  (void) splitmux;
  (void) fragment_id;
  gchar *path = g_build_filename (w->dir, name, NULL);
  g_free (name);
  return path;
}

static gboolean
on_bus (GstBus *bus, GstMessage *msg, gpointer data)
{
  PacsWriter *w = data;
  GError *err = NULL;
  gchar *dbg = NULL;
  const GstStructure *s;

  (void) bus;
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_ERROR:
      gst_message_parse_error (msg, &err, &dbg);
      w->bus_watch = 0;                 /* removed by returning G_SOURCE_REMOVE */
      fail (w, err->message);           /* destroys the pipeline */
      g_clear_error (&err);
      g_free (dbg);
      return G_SOURCE_REMOVE;
    case GST_MESSAGE_EOS:               /* only after pacs_writer_finish() */
      g_atomic_int_set (&w->state, PACS_WRITER_ABSENT);
      w->bus_watch = 0;
      destroy_pipeline (w);
      if (w->done)
        w->done (w, w->user_data);
      return G_SOURCE_REMOVE;
    case GST_MESSAGE_ELEMENT:
      s = gst_message_get_structure (msg);
      if (s != NULL && gst_structure_has_name (s, "splitmuxsink-fragment-opened")) {
        guint64 rt = GST_CLOCK_TIME_NONE;

        gst_structure_get_uint64 (s, "running-time", &rt);
        g_print ("pacscorder: opened %s (starts at %" GST_TIME_FORMAT ")\n",
                 gst_structure_get_string (s, "location"), GST_TIME_ARGS (rt));
      } else if (s != NULL && gst_structure_has_name (s, "splitmuxsink-fragment-closed")) {
        w->files++;
        g_print ("pacscorder: closed %s\n", gst_structure_get_string (s, "location"));
      }
      break;
    default:
      break;
  }
  return G_SOURCE_CONTINUE;
}

PacsWriter *
pacs_writer_new (const gchar *dir, const gchar *session, const gint *file_index,
                 guint buffer_seconds, guint fragment_ms, PacsWriterDone done,
                 gpointer user_data)
{
  PacsWriter *w = g_new0 (PacsWriter, 1);

  w->dir = g_strdup (dir);
  w->session = g_strdup (session);
  w->file_index = file_index;
  w->buffer_seconds = buffer_seconds;
  w->fragment_ms = fragment_ms;
  w->done = done;
  w->user_data = user_data;
  w->state = PACS_WRITER_ABSENT;
  return w;
}

gboolean
pacs_writer_try_start (PacsWriter *w)
{
  GstElement *mux;
  GstBus *bus;

  if (g_atomic_int_get (&w->state) != PACS_WRITER_ABSENT)
    return TRUE;

  /* Is the drive there and writable? */
  if (g_mkdir_with_parents (w->dir, 0755) != 0 || g_access (w->dir, W_OK) != 0) {
    gchar *why = g_strdup_printf ("not available (%s)", g_strerror (errno));

    g_printerr ("pacscorder: ALERT: recording drive %s: %s — recording continues "
                "on the other drive; this drive is retried at the next file\n",
                w->dir, why);
    g_free (why);
    return FALSE;
  }

  w->pipeline = gst_pipeline_new (NULL);
  w->appsrc = gst_element_factory_make ("appsrc", NULL);
  w->splitmux = gst_element_factory_make ("splitmuxsink", NULL);
  mux = gst_element_factory_make ("mp4mux", NULL);
  if (w->appsrc == NULL || w->splitmux == NULL || mux == NULL) {
    g_printerr ("pacscorder: appsrc, splitmuxsink or mp4mux is not available\n");
    if (mux)
      gst_object_unref (gst_object_ref_sink (mux));
    if (w->appsrc)
      gst_object_unref (gst_object_ref_sink (w->appsrc));
    if (w->splitmux)
      gst_object_unref (gst_object_ref_sink (w->splitmux));
    gst_clear_object (&w->pipeline);
    w->appsrc = w->splitmux = NULL;
    return FALSE;
  }
  gst_bin_add_many (GST_BIN (w->pipeline), w->appsrc, w->splitmux, NULL);

  /* appsrc holds the backlog when the drive is slow; its level is checked in
   * pacs_writer_push, and a drive more than buffer_seconds behind is treated
   * as failed instead of blocking anything upstream (OQ-117). */
  g_object_set (w->appsrc, "format", GST_FORMAT_TIME, "do-timestamp", FALSE,
                "block", FALSE, "max-bytes", (guint64) 0, "max-buffers", (guint64) 0,
                "max-time", (guint64) 0, NULL);
  /* fragmented MP4 [J-39]; robust muxing is not set (fragmented mode would
   * silently disable it anyway [J-39]); splits come from the application */
  g_object_set (mux, "fragment-duration", w->fragment_ms, NULL);
  g_object_set (w->splitmux, "muxer", mux, "max-size-time", (guint64) 0,
                "max-size-bytes", (guint64) 0, "send-keyframe-requests", FALSE, NULL);
  g_signal_connect (w->splitmux, "format-location",
                    G_CALLBACK (on_format_location), w);

  if (!gst_element_link (w->appsrc, w->splitmux)) {
    g_printerr ("pacscorder: cannot link the writer for %s\n", w->dir);
    destroy_pipeline (w);
    return FALSE;
  }

  bus = gst_pipeline_get_bus (GST_PIPELINE (w->pipeline));
  w->bus_watch = gst_bus_add_watch (bus, on_bus, w);
  gst_object_unref (bus);

  g_atomic_int_set (&w->stall_reported, 0);
  g_atomic_int_set (&w->state, PACS_WRITER_JOINING);
  if (gst_element_set_state (w->pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
    fail (w, "writer pipeline did not start");
    return FALSE;
  }
  return TRUE;
}

static gboolean
on_stall (gpointer data)
{
  PacsWriter *w = data;
  gchar *why;

  if (g_atomic_int_get (&w->state) != PACS_WRITER_ACTIVE)
    return G_SOURCE_REMOVE;
  why = g_strdup_printf ("fell more than %u s behind (stalled drive)",
                         w->buffer_seconds);
  fail (w, why);
  g_free (why);
  return G_SOURCE_REMOVE;
}

void
pacs_writer_activate (PacsWriter *w)
{
  g_atomic_int_compare_and_exchange (&w->state, PACS_WRITER_JOINING,
                                     PACS_WRITER_ACTIVE);
}

void
pacs_writer_split_at (PacsWriter *w, GstClockTime running_time)
{
  if (g_atomic_int_get (&w->state) == PACS_WRITER_ACTIVE)
    g_signal_emit_by_name (w->splitmux, "split-at-running-time", running_time);
}

void
pacs_writer_push (PacsWriter *w, GstSample *sample)
{
  if (g_atomic_int_get (&w->state) != PACS_WRITER_ACTIVE)
    return;

  if (gst_app_src_get_current_level_time (GST_APP_SRC (w->appsrc)) >
      (guint64) w->buffer_seconds * GST_SECOND) {
    if (g_atomic_int_compare_and_exchange (&w->stall_reported, 0, 1))
      g_idle_add (on_stall, w);
    return;
  }
  GstFlowReturn ret = gst_app_src_push_sample (GST_APP_SRC (w->appsrc), sample);
  if (ret != GST_FLOW_OK)
    g_printerr ("pacscorder: drive %s refused a frame: %s\n", w->dir,
                gst_flow_get_name (ret));
}

void
pacs_writer_finish (PacsWriter *w)
{
  gint state = g_atomic_int_get (&w->state);

  if (state == PACS_WRITER_ACTIVE) {
    g_atomic_int_set (&w->state, PACS_WRITER_FINISHING);
    gst_app_src_end_of_stream (GST_APP_SRC (w->appsrc));
  } else {
    /* A drive that never started writing (absent, or joining when the
     * recording stopped) has no file to close; stopping it without
     * end-of-stream keeps splitmuxsink from opening an empty file. */
    if (state == PACS_WRITER_JOINING) {
      g_atomic_int_set (&w->state, PACS_WRITER_ABSENT);
      destroy_pipeline (w);
    }
    if (w->done)
      w->done (w, w->user_data);
  }
}

PacsWriterState
pacs_writer_state (const PacsWriter *w)
{
  return (PacsWriterState) g_atomic_int_get ((gint *) &w->state);
}

const gchar *
pacs_writer_dir (const PacsWriter *w)
{
  return w->dir;
}

guint
pacs_writer_files (const PacsWriter *w)
{
  return w->files;
}

guint
pacs_writer_failures (const PacsWriter *w)
{
  return w->failures;
}

void
pacs_writer_free (PacsWriter *w)
{
  if (w == NULL)
    return;
  destroy_pipeline (w);
  g_free (w->dir);
  g_free (w->session);
  g_free (w);
}
