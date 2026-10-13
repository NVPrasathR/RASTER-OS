#!/usr/bin/env python3
"""PACSCORDER development test helper (not canonical): are two MP4 files the
same recording?

Compares two files byte for byte, except the creation and modification times
of the mvhd, tkhd and mdhd boxes. mp4mux writes the wall-clock time, in whole
seconds, into those fields when it is created or reset for a file and when it
rewrites the header at the end (GStreamer 1.28.7 source, read 2026-10-13:
isomp4/atoms.c atoms_get_current_qt_time, gstqtmux.c gst_qt_mux_reset and
gst_qt_mux_send_moov). Each drive's writer runs its own mp4mux (ADR-016), so
two copies of the same frames differ in these fields when the two muxers stamp
a file either side of a second boundary — e.g. a rejoining drive's first file,
stamped when its pipeline was created (docs/SOFTWARE_ARCHITECTURE.md §10.2).
Every other byte must match.

Usage: mp4_same.py A.mp4 B.mp4
Exit:  0 the same apart from those fields; 1 different; 2 unreadable.
"""
import struct
import sys

CONTAINERS = {b"moov", b"trak", b"mdia"}
TIMED = {b"mvhd", b"tkhd", b"mdhd"}


def time_fields(data):
    """(offset, length) of the creation + modification times of every timed box."""
    fields = []

    def walk(start, end):
        off = start
        while off + 8 <= end:
            size, kind = struct.unpack_from(">I4s", data, off)
            header = 8
            if size == 1:
                size = struct.unpack_from(">Q", data, off + 8)[0]
                header = 16
            elif size == 0:
                size = end - off
            if size < header or off + size > end:
                raise ValueError("bad box '%s' at offset %d" % (kind.decode("latin-1"), off))
            if kind in CONTAINERS:
                walk(off + header, off + size)
            elif kind in TIMED:
                width = 8 if data[off + header] == 1 else 4   # full-box version 1: 64-bit times
                fields.append((off + header + 4, 2 * width))
            off += size

    walk(0, len(data))
    return fields


def main(argv):
    if len(argv) != 3:
        print("usage: mp4_same.py A.mp4 B.mp4", file=sys.stderr)
        return 2
    try:
        files = [bytearray(open(p, "rb").read()) for p in argv[1:]]
        fields = [time_fields(d) for d in files]
    except (OSError, ValueError, struct.error) as e:
        print("mp4_same: %s" % e, file=sys.stderr)
        return 2

    a, b = files
    if fields[0] != fields[1]:
        print("mp4_same: the header boxes are at different places", file=sys.stderr)
        return 1
    changed = sum(1 for off, n in fields[0] if a[off:off + n] != b[off:off + n])
    for off, n in fields[0]:
        a[off:off + n] = b[off:off + n] = bytes(n)
    if a != b:
        first = next((i for i, (x, y) in enumerate(zip(a, b)) if x != y), min(len(a), len(b)))
        print("mp4_same: files differ at byte %d (sizes %d and %d)" % (first, len(a), len(b)),
              file=sys.stderr)
        return 1
    print("same: every byte matches except the time stamps of %d of %d header boxes"
          % (changed, len(fields[0])))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
