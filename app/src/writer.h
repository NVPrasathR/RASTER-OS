/*
 * PACSCORDER — one recording drive (slice C5; ADR-016).
 *
 * Each drive is written by its own pipeline, fed with the recording encode:
 *
 *   appsrc ─ splitmuxsink (fragmented mp4mux)  → <dir>/pacs-<session>-<index>.mp4
 *
 * so that a failed, full, missing or stalled drive never stops the other
 * drive, the encoder or the live path (OQ-129, OQ-117, RISK-028). File splits
 * are driven by the application for all drives together, so the copies on the
 * two drives have the same names and boundaries; a drive that was missing or
 * failed rejoins at the next split (OQ-129).
 */
#ifndef PACS_WRITER_H
#define PACS_WRITER_H

#include <gst/gst.h>

typedef enum {
  PACS_WRITER_ABSENT,    /* not writing: missing, failed or stopped; retried at the next split */
  PACS_WRITER_JOINING,   /* pipeline running; waiting for a keyframe to open its first file */
  PACS_WRITER_ACTIVE,    /* writing */
  PACS_WRITER_FINISHING  /* end of stream sent; waiting for the last file to close */
} PacsWriterState;

typedef struct _PacsWriter PacsWriter;

/* Called on the main loop when a writer finished its last file or failed
 * while finishing (so the caller can tell when all writers are done). */
typedef void (*PacsWriterDone) (PacsWriter *w, gpointer user_data);

/* dir: recording directory. session: file-name prefix for this recording.
 * file_index: pointer to the application's current file index (read when a
 * file is opened). buffer_seconds: how much a stalled drive may fall behind
 * before it is treated as failed (OQ-117). fragment_ms: mp4mux fragment
 * duration (OQ-118). */
PacsWriter *pacs_writer_new (const gchar *dir, const gchar *session,
                             const gint *file_index, guint buffer_seconds,
                             guint fragment_ms, PacsWriterDone done,
                             gpointer user_data);

/* Try to start writing (at the session start or at a split): checks that the
 * directory can be written; on success the writer joins at the next keyframe.
 * Returns TRUE if the writer is (now) running. */
gboolean pacs_writer_try_start (PacsWriter *w);

/* Hand one encoded frame (running-time timestamps) to the writer; dropped
 * unless the writer is active. Called from the main pipeline's streaming thread. */
void pacs_writer_push (PacsWriter *w, GstSample *sample);

/* A joining writer starts writing with the frame about to be pushed (the
 * designated keyframe of a split or of the session start). Streaming thread. */
void pacs_writer_activate (PacsWriter *w);

/* An active writer starts a new file at the keyframe with this running time,
 * which is about to be pushed. Streaming thread. */
void pacs_writer_split_at (PacsWriter *w, GstClockTime running_time);

/* Finish: close the current file cleanly; done() is called when closed. */
void pacs_writer_finish (PacsWriter *w);

PacsWriterState pacs_writer_state (const PacsWriter *w);
const gchar *pacs_writer_dir (const PacsWriter *w);
guint pacs_writer_files (const PacsWriter *w);
guint pacs_writer_failures (const PacsWriter *w);

void pacs_writer_free (PacsWriter *w);

#endif
