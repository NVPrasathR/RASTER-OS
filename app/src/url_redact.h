/*
 * PACSCORDER — make a stream URL safe to print in logs.
 *
 * RTMP URLs carry the destination's stream key as their last path component
 * (rtmp[s]://host[:port]/application/stream), and any URL may carry a user
 * name and password or a token in its query. Those are secrets: logs may be
 * shared (owner decision, 2026-10-13). No GStreamer or GLib dependency, so it
 * can be unit-tested on any machine.
 */
#ifndef PACS_URL_REDACT_H
#define PACS_URL_REDACT_H

/*
 * Return a copy of url for logging, with
 *   - user:password@ in the authority replaced by "***@",
 *   - a query ("?...") replaced by "?***",
 *   - for rtmp:// and rtmps://, the last path component (the stream key)
 *     replaced by "***".
 * Anything without "://" is copied unchanged. The caller frees the result
 * with free(). Returns NULL if url is NULL or memory runs out.
 */
char *pacs_url_redact (const char *url);

#endif
