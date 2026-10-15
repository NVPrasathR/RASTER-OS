/*
 * Unit test for pacs_url_redact (app/src/url_redact.c): stream keys, user
 * names, passwords and query tokens must never reach the log.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "url_redact.h"

static int failures;

static void
expect (const char *url, const char *want)
{
  char *got = pacs_url_redact (url);
  int ok = (got == NULL && want == NULL) ||
           (got != NULL && want != NULL && strcmp (got, want) == 0);

  printf ("%-4s %-50s -> %s\n", ok ? "ok" : "FAIL", url ? url : "(null)",
          got ? got : "(null)");
  if (!ok) {
    printf ("     want %s\n", want ? want : "(null)");
    failures++;
  }
  free (got);
}

int
main (void)
{
  expect ("rtmp://127.0.0.1:11935/live/rtmp", "rtmp://127.0.0.1:11935/live/***");
  expect ("rtmps://ingest.example.com/live2/abcd-efgh-ijkl", "rtmps://ingest.example.com/live2/***");
  expect ("rtmp://host/app/instance/key", "rtmp://host/app/instance/***");
  expect ("rtmp://user:pass@host:1935/app/key", "rtmp://***@host:1935/app/***");
  expect ("rtmp://host/app/key?secret=1", "rtmp://host/app/***?***");
  expect ("rtmp://host/app/", "rtmp://host/app/");
  expect ("rtsp://127.0.0.1:18554/live", "rtsp://127.0.0.1:18554/live");
  expect ("rtsp://admin:secret@cam.local:554/stream?token=abc", "rtsp://***@cam.local:554/stream?***");
  expect ("rtsp://u:p@q@host/path", "rtsp://***@host/path");
  expect ("not a url", "not a url");
  expect (NULL, NULL);

  printf ("%s: %d failure(s)\n", failures ? "FAIL" : "PASS", failures);
  return failures ? 1 : 0;
}
