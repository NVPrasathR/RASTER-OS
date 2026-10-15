/*
 * PACSCORDER — live publishers (slice C2; ADR-015, PROPOSED).
 *
 * Each publisher is its own small pipeline fed with the live encode through
 * appsrc, so that a network failure stops only that publisher and never the
 * recording. A failed publisher is restarted after a delay.
 *
 *   RTSP:  appsrc ─ h264parse ─ queue ─ rtspclientsink   → MediaMTX (serves WebRTC readers) [K-05], [F-45]
 *   RTMP:  appsrc ─ h264parse ─ queue ─ flvmux ─ rtmp2sink → RTMP destination [F-33], [F-35]
 */
#ifndef PACS_PUBLISHER_H
#define PACS_PUBLISHER_H

#include <gst/gst.h>

typedef enum {
  PACS_PUBLISH_RTSP,
  PACS_PUBLISH_RTMP
} PacsPublishKind;

typedef struct _PacsPublisher PacsPublisher;

/* Called when a publisher (re)starts and needs a keyframe from the encoder. */
typedef void (*PacsKeyframeRequest) (gpointer user_data);

/* Create a publisher for url. clock/base_time: the main pipeline's, so that
 * buffer timestamps keep their meaning in the publisher pipeline. */
PacsPublisher *pacs_publisher_new (PacsPublishKind kind, const gchar *url,
                                   PacsKeyframeRequest request_keyframe,
                                   gpointer user_data, GError **error);

/* Start (or restart) the publisher, sharing the main pipeline's clock. */
void pacs_publisher_start (PacsPublisher *pub, GstClock *clock,
                           GstClockTime base_time);

/* Hand one encoded frame to the publisher; dropped while it is not running. */
void pacs_publisher_push (PacsPublisher *pub, GstSample *sample);

/* Send end-of-stream and stop. */
void pacs_publisher_stop (PacsPublisher *pub);

void pacs_publisher_free (PacsPublisher *pub);

/* The URL for logs: stream key, user name, password and query hidden (url_redact.h). */
const gchar *pacs_publisher_log_url (const PacsPublisher *pub);
guint pacs_publisher_restarts (const PacsPublisher *pub);

#endif
