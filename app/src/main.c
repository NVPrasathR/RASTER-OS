/*
 * PACSCORDER — recorder entry point.
 *
 * Build: see docs/BUILD_SYSTEM.md (CMake, ADR-014).
 * Status: IMPLEMENTED — NOT TESTED on PACSCORDER hardware (slice C1).
 */
#include <glib-unix.h>
#include <gst/gst.h>
#include <signal.h>

#include "config.h"
#include "pipeline.h"

static gboolean
on_signal (gpointer data)
{
  pacs_pipeline_stop (data);
  return G_SOURCE_CONTINUE;
}

int
main (int argc, char **argv)
{
  PacsConfig cfg;
  PacsPipeline *p;
  GError *error = NULL;
  gboolean ok;

  gst_init (&argc, &argv);

  if (!pacs_config_parse (&cfg, &argc, &argv, &error)) {
    g_printerr ("pacscorder: %s\n", error->message);
    g_error_free (error);
    return 2;
  }

  p = pacs_pipeline_new (&cfg, &error);
  if (p == NULL) {
    g_printerr ("pacscorder: %s\n", error->message);
    g_error_free (error);
    pacs_config_clear (&cfg);
    return 1;
  }

  g_unix_signal_add (SIGINT, on_signal, p);
  g_unix_signal_add (SIGTERM, on_signal, p);

  ok = pacs_pipeline_run (p);

  pacs_pipeline_free (p);
  pacs_config_clear (&cfg);
  return ok ? 0 : 1;
}
