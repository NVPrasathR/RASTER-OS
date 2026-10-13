/*
 * PACSCORDER — the GStreamer pipeline (ADR-007).
 *
 *   source ─ tee ─┬─ queue ─ recording encoder ─ h264parse ─ appsink ──▶ one writer pipeline per drive (writer.c, ADR-016)
 *                 └─ queue ─ videorate drop-only ─ live encoder ─ appsink ──▶ publisher pipelines (publisher.c, ADR-015)
 *                                                                (fakesink when no publisher is configured)
 *
 * The main pipeline only captures and encodes; writing and publishing run in
 * their own pipelines so that a drive or network failure cannot stop it.
 * File splits are decided here, on the keyframe itself, for all drives.
 * Not yet: audio, EDID / DV timings and source changes (later slices).
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
