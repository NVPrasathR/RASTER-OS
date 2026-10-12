/*
 * PACSCORDER — the GStreamer pipeline (ADR-007).
 *
 *   source ─ tee ─┬─ queue ─ recording encoder ─ h264parse ─ tee ─┬─ queue ─ splitmuxsink (dir 1)
 *                 │                                               └─ queue ─ splitmuxsink (dir 2)
 *                 └─ queue ─ videorate drop-only ─ live encoder ─ h264parse ─ fakesink
 *
 * Slice C1: recording and live encode; publishing (RTSP/RTMP), audio, EDID and
 * DV-timings handling, and drive-failure handling come in later slices.
 */
#ifndef PACS_PIPELINE_H
#define PACS_PIPELINE_H

#include <gst/gst.h>

#include "config.h"

typedef struct _PacsPipeline PacsPipeline;

/* Build the pipeline from cfg (which must outlive it). NULL + error on failure. */
PacsPipeline *pacs_pipeline_new (const PacsConfig *cfg, GError **error);

/* Run until end of stream, an error, or pacs_pipeline_stop(). Returns TRUE
 * when the pipeline reached end of stream cleanly (all files finalised). */
gboolean pacs_pipeline_run (PacsPipeline *p);

/* Ask the pipeline to finish: sends end-of-stream so open files are
 * finalised. Safe to call more than once; call from the main loop. */
void pacs_pipeline_stop (PacsPipeline *p);

void pacs_pipeline_free (PacsPipeline *p);

#endif
