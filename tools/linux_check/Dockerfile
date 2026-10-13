# PACSCORDER — Linux check image (development only; not the product image).
#
# Debian trixie arm64, the base of Raspberry Pi OS trixie, with GStreamer from
# Debian: builds the application and runs its development tests on the same
# distribution and GStreamer series as the CM4, on the development machine.
# It is not Raspberry Pi OS: no Raspberry Pi kernel, firmware, V4L2 devices or
# hardware encoder. See docs/BUILD_SYSTEM.md §2A; run with tools/linux_check.sh.
FROM debian:trixie

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential cmake pkg-config ca-certificates curl python3 ffmpeg \
        libglib2.0-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
        gstreamer1.0-tools gstreamer1.0-plugins-base gstreamer1.0-plugins-good \
        gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-rtsp \
    && rm -rf /var/lib/apt/lists/*

# MediaMTX for the publishing test; Debian does not package it. Pinned to the
# version used on the development machine and checked against the release's
# checksums.sha256.
ARG MTX_VERSION=1.21.0
ARG MTX_SHA256=a8113b5928ba1a934b81557b61b8a07954b76921a4b567d54c7f086f8b39d9a2
RUN curl -fsSL -o /tmp/mtx.tar.gz \
        "https://github.com/bluenviron/mediamtx/releases/download/v${MTX_VERSION}/mediamtx_v${MTX_VERSION}_linux_arm64.tar.gz" \
    && echo "${MTX_SHA256}  /tmp/mtx.tar.gz" | sha256sum -c - \
    && tar -xzf /tmp/mtx.tar.gz -C /usr/local/bin mediamtx \
    && rm /tmp/mtx.tar.gz
