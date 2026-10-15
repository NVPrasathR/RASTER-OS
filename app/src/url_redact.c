/*
 * PACSCORDER — make a stream URL safe to print in logs. See url_redact.h.
 */
#include "url_redact.h"

#include <stdlib.h>
#include <string.h>

/* strdup() is POSIX, not ISO C; the build uses strict C (found by the
 * Linux check, 2026-10-13). */
static char *
copy (const char *s)
{
  size_t n = strlen (s) + 1;
  char *c = malloc (n);

  return c ? memcpy (c, s, n) : NULL;
}

char *
pacs_url_redact (const char *url)
{
  const char *sep, *auth, *auth_end, *at, *query, *path_end, *key;
  size_t scheme_len;
  int rtmp;
  char *out, *o;

  if (url == NULL)
    return NULL;
  sep = strstr (url, "://");
  if (sep == NULL)
    return copy (url);

  scheme_len = (size_t) (sep - url);
  rtmp = (scheme_len == 4 && strncmp (url, "rtmp", 4) == 0) ||
         (scheme_len == 5 && strncmp (url, "rtmps", 5) == 0);

  auth = sep + 3;
  auth_end = auth + strcspn (auth, "/?");     /* host[:port], maybe with user:pass@ */
  at = NULL;
  for (const char *c = auth; c < auth_end; c++)
    if (*c == '@')
      at = c;                                  /* the last '@' ends the user info */

  query = strchr (auth_end, '?');
  path_end = query ? query : auth_end + strlen (auth_end);
  key = NULL;
  if (rtmp) {
    const char *slash = NULL;

    for (const char *c = auth_end; c < path_end; c++)
      if (*c == '/')
        slash = c;
    if (slash != NULL && slash + 1 < path_end)
      key = slash + 1;                         /* the stream key */
  }

  out = malloc (strlen (url) + 16);
  if (out == NULL)
    return NULL;
  o = out;
  memcpy (o, url, scheme_len + 3);             /* "scheme://" */
  o += scheme_len + 3;
  if (at != NULL) {
    memcpy (o, "***@", 4);
    o += 4;
    auth = at + 1;
  }
  memcpy (o, auth, (size_t) ((key ? key : path_end) - auth));
  o += (key ? key : path_end) - auth;
  if (key != NULL) {
    memcpy (o, "***", 3);
    o += 3;
  }
  if (query != NULL) {
    memcpy (o, "?***", 4);
    o += 4;
  }
  *o = '\0';
  return out;
}
