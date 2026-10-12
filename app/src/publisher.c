/*
 * PACSCORDER — live publishers. See publisher.h.
 */
#include "publisher.h"

#include <gst/app/gstappsrc.h>

#define RESTART_DELAY_SECONDS 5
#define APPSRC_MAX_BUFFERS 60   /* about 2 s at 30 fps; older frames are dropped */

struct _PacsPublisher {
  PacsPublishKind kind;
  gchar *url;
  GstElement *pipeline;
  GstElement *appsrc;
  guint bus_watch;
  guint restart_id;
  gint running;                 /* g_atomic_int_*: read in the streaming thread */
  guint restarts;
  GstClock *clock;
  GstClockTime base_time;
  PacsKeyframeRequest request_keyframe;
  gpointer user_data;
};

static const gchar *
kind_name (PacsPublishKind kind)
{
  return kind == PACS_PUBLISH_RTSP ? "RTSP" : "RTMP";
}

static GstElement *
add (GstBin *bin, const gchar *factory, GError **error)
{
  GstElement *e = gst_element_factory_make (factory, NULL);

  if (e == NULL) {
    g_set_error (error, GST_CORE_ERROR, GST_CORE_ERROR_MISSING_PLUGIN,
                 "GStreamer element '%s' is not available", factory);
    return NULL;
  }
  gst_bin_add (bin, e);
  return e;
}

static gboolean restart (gpointer data);

static gboolean
on_bus (GstBus *bus, GstMessage *msg, gpointer data)
{
  PacsPublisher *pub = data;
  GError *err = NULL;
  gchar *dbg = NULL;

  (void) bus;
  if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR) {
    gst_message_parse_error (msg, &err, &dbg);
    g_printerr ("pacscorder: %s publisher %s failed: %s; retrying in %d s "
                "(recording continues)\n", kind_name (pub->kind), pub->url,
                err->message, RESTART_DELAY_SECONDS);
    g_atomic_int_set (&pub->running, 0);
    gst_element_set_state (pub->pipeline, GST_STATE_NULL);
    if (pub->restart_id == 0)
      pub->restart_id = g_timeout_add_seconds (RESTART_DELAY_SECONDS, restart, pub);
  } else if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_WARNING) {
    gst_message_parse_warning (msg, &err, &dbg);
    g_printerr ("pacscorder: %s publisher %s warning: %s\n",
                kind_name (pub->kind), pub->url, err->message);
  }
  g_clear_error (&err);
  g_free (dbg);
  return G_SOURCE_CONTINUE;
}

PacsPublisher *
pacs_publisher_new (PacsPublishKind kind, const gchar *url,
                    PacsKeyframeRequest request_keyframe, gpointer user_data,
                    GError **error)
{
  PacsPublisher *pub = g_new0 (PacsPublisher, 1);
  GstBin *bin;
  GstElement *parse, *q, *mux = NULL, *sink;
  GstBus *bus;

  pub->kind = kind;
  pub->url = g_strdup (url);
  pub->request_keyframe = request_keyframe;
  pub->user_data = user_data;
  pub->pipeline = gst_pipeline_new (NULL);
  bin = GST_BIN (pub->pipeline);

  pub->appsrc = add (bin, "appsrc", error);
  parse = pub->appsrc ? add (bin, "h264parse", error) : NULL;
  q = parse ? add (bin, "queue", error) : NULL;   /* GStreamer asks for one before the sink */
  if (q == NULL)
    goto fail;
  /* Live source in time format, timestamps kept from the main pipeline; when
   * the publisher falls behind, the oldest frames are dropped rather than
   * blocking the live encoder (and through it the capture tee). */
  g_object_set (pub->appsrc, "is-live", TRUE, "format", GST_FORMAT_TIME,
                "do-timestamp", FALSE, "max-buffers", (guint64) APPSRC_MAX_BUFFERS,
                "leaky-type", GST_APP_LEAKY_TYPE_DOWNSTREAM, NULL);
  /* SPS/PPS before every IDR, so a viewer or server that joins late can start
   * decoding ([D-15] for why in-band parameter sets matter). */
  g_object_set (parse, "config-interval", -1, NULL);

  if (kind == PACS_PUBLISH_RTSP) {
    sink = add (bin, "rtspclientsink", error);
    if (sink == NULL)
      goto fail;
    g_object_set (sink, "location", url, NULL);
    if (!gst_element_link_many (pub->appsrc, parse, q, sink, NULL))
      goto link_fail;
  } else {
    mux = add (bin, "flvmux", error);
    sink = mux ? add (bin, "rtmp2sink", error) : NULL;
    if (sink == NULL)
      goto fail;
    g_object_set (mux, "streamable", TRUE, NULL);
    g_object_set (sink, "location", url, NULL);
    if (!gst_element_link_many (pub->appsrc, parse, q, mux, sink, NULL))
      goto link_fail;
  }

  bus = gst_pipeline_get_bus (GST_PIPELINE (pub->pipeline));
  pub->bus_watch = gst_bus_add_watch (bus, on_bus, pub);
  gst_object_unref (bus);
  return pub;

link_fail:
  g_set_error (error, GST_CORE_ERROR, GST_CORE_ERROR_NEGOTIATION,
               "cannot link the %s publisher for %s", kind_name (kind), url);
fail:
  pacs_publisher_free (pub);
  return NULL;
}

static void
start_pipeline (PacsPublisher *pub)
{
  if (pub->clock != NULL) {
    gst_pipeline_use_clock (GST_PIPELINE (pub->pipeline), pub->clock);
    gst_element_set_base_time (pub->pipeline, pub->base_time);
    gst_element_set_start_time (pub->pipeline, GST_CLOCK_TIME_NONE);
  }
  if (gst_element_set_state (pub->pipeline, GST_STATE_PLAYING) ==
      GST_STATE_CHANGE_FAILURE) {
    g_printerr ("pacscorder: %s publisher %s did not start; retrying in %d s\n",
                kind_name (pub->kind), pub->url, RESTART_DELAY_SECONDS);
    gst_element_set_state (pub->pipeline, GST_STATE_NULL);
    if (pub->restart_id == 0)
      pub->restart_id = g_timeout_add_seconds (RESTART_DELAY_SECONDS, restart, pub);
    return;
  }
  g_atomic_int_set (&pub->running, 1);
  g_print ("pacscorder: %s publisher started: %s\n", kind_name (pub->kind),
           pub->url);
  if (pub->request_keyframe)
    pub->request_keyframe (pub->user_data);
}

static gboolean
restart (gpointer data)
{
  PacsPublisher *pub = data;

  pub->restart_id = 0;
  pub->restarts++;
  start_pipeline (pub);
  return G_SOURCE_REMOVE;
}

void
pacs_publisher_start (PacsPublisher *pub, GstClock *clock,
                      GstClockTime base_time)
{
  gst_clear_object (&pub->clock);
  pub->clock = clock ? gst_object_ref (clock) : NULL;
  pub->base_time = base_time;
  start_pipeline (pub);
}

void
pacs_publisher_push (PacsPublisher *pub, GstSample *sample)
{
  if (g_atomic_int_get (&pub->running))
    gst_app_src_push_sample (GST_APP_SRC (pub->appsrc), sample);
}

void
pacs_publisher_stop (PacsPublisher *pub)
{
  if (pub->restart_id) {
    g_source_remove (pub->restart_id);
    pub->restart_id = 0;
  }
  if (g_atomic_int_get (&pub->running))
    gst_app_src_end_of_stream (GST_APP_SRC (pub->appsrc));
  g_atomic_int_set (&pub->running, 0);
  gst_element_set_state (pub->pipeline, GST_STATE_NULL);
}

void
pacs_publisher_free (PacsPublisher *pub)
{
  if (pub == NULL)
    return;
  if (pub->restart_id)
    g_source_remove (pub->restart_id);
  if (pub->bus_watch)
    g_source_remove (pub->bus_watch);
  if (pub->pipeline) {
    gst_element_set_state (pub->pipeline, GST_STATE_NULL);
    gst_object_unref (pub->pipeline);
  }
  gst_clear_object (&pub->clock);
  g_free (pub->url);
  g_free (pub);
}

const gchar *
pacs_publisher_url (const PacsPublisher *pub)
{
  return pub->url;
}

guint
pacs_publisher_restarts (const PacsPublisher *pub)
{
  return pub->restarts;
}
