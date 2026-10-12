# PACSCORDER

PACSCORDER is an embedded Linux HDMI capture product. The video path is:

```text
HDMI source → Toshiba TC358743 (HDMI → MIPI CSI-2) → Raspberry Pi CSI-2 receiver → Media Controller → V4L2 → DMABUF → Encoder → Recorder / RTMP / WebRTC
```

It also integrates with Blackmagic ATEM switchers.

> **Status (2026-10-06): PHASE 0 — Bootstrap.**
> - No hardware exists.
> - No code exists.
> - Nothing has been tested.
>
> The repository holds the engineering rules, a verified source register and the documentation baseline. See [docs/PROJECT_STATUS.md](docs/PROJECT_STATUS.md).
>
> *(Status 2026-10-12: product platform CM4 (ADR-004); framework GStreamer, language C, build CMake (ADR-007, ADR-013, ADR-014). The first application code (slice C1: capture, recording and live encodes, mirrored fragmented-MP4 recording) builds and passes its development tests on a Mac with a test source. **Still no hardware: nothing has run on a CM4 or a TC358743.**)*

## Repository layout

| Path | Contents |
|---|---|
| `docs/` | All project documentation — start with [docs/README.md](docs/README.md) |
| `app/` | The PACSCORDER application (C, GStreamer) |
| `tests/` | Development tests (CTest) |
| `scripts/bringup/` | CM4 / TC358743 bring-up helpers |
| `tools/` | Repository tools (documentation check, encoder capacity test) |

## Build and test (development machine)

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/app/pacscorder --help
```

Requirements and the machine used so far: [docs/BUILD_SYSTEM.md](docs/BUILD_SYSTEM.md) §2A. Before committing documentation, run `python3 tools/doccheck.py docs`.

## Where to start

- [docs/README.md](docs/README.md): documentation index, and where each of the Rule 25 "new engineer" questions is answered.
- [docs/ENGINEERING_RULES.md](docs/ENGINEERING_RULES.md): the mandatory engineering and documentation rules for this project.
- [docs/PROJECT_STATUS.md](docs/PROJECT_STATUS.md): current phase, blockers and next step.
- [docs/OPEN_QUESTIONS.md](docs/OPEN_QUESTIONS.md): decisions and unknowns that must be resolved before product work starts.

The directory name "RASTER OS" is only a folder name; the product is PACSCORDER.
