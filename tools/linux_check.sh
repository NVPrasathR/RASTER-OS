#!/usr/bin/env bash
# PACSCORDER — Linux check: build the application and run its development tests
# in Debian trixie arm64 with Debian's GStreamer (tools/linux_check/Dockerfile).
#
# Development check only. It runs in a container on the development machine,
# not on a CM4: it shows whether the code builds and the development tests pass
# with the GStreamer and GLib versions of the CM4's distribution, nothing about
# the TC358743, V4L2 or the hardware encoder. Procedure: docs/BUILD_SYSTEM.md §2A.
#
# Needs: docker (on macOS, for example Colima: `colima start --vm-type vz`).
# The source tree is mounted read-only; the build stays inside the container.
# An 8 MB tmpfs stands in for a full drive (smoke_drives_dev case B).
# Usage: tools/linux_check.sh
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
IMAGE=pacscorder-linux-check

docker build -t "$IMAGE" "$ROOT/tools/linux_check"
docker run --rm --tmpfs /mnt/full:size=8m -e PACS_TEST_FULL_DIR=/mnt/full \
  -v "$ROOT":/src:ro "$IMAGE" bash -c '
    set -euo pipefail
    echo "== versions"
    . /etc/os-release; echo "$PRETTY_NAME ($(uname -m))"
    gst-launch-1.0 --version | head -2
    echo "glib $(pkg-config --modversion glib-2.0), cmake $(cmake --version | head -1 | cut -d" " -f3), $(cc --version | head -1)"
    echo "mediamtx $(mediamtx --version 2>&1 | head -1)"
    echo "== build"
    cmake -S /src -B /tmp/build >/dev/null
    if ! cmake --build /tmp/build -j "$(nproc)" >/tmp/build.log 2>&1; then
      cat /tmp/build.log; echo "BUILD FAILED"; exit 1
    fi
    if grep -E "warning" /tmp/build.log; then echo "(compiler warnings above)"; else echo "no compiler warnings"; fi
    echo "== tests"
    ctest --test-dir /tmp/build --output-on-failure
  '
