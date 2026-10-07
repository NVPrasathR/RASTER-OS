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

## Where to start

- [docs/README.md](docs/README.md): documentation index, and where each of the Rule 25 "new engineer" questions is answered.
- [docs/ENGINEERING_RULES.md](docs/ENGINEERING_RULES.md): the mandatory engineering and documentation rules for this project.
- [docs/PROJECT_STATUS.md](docs/PROJECT_STATUS.md): current phase, blockers and next step.
- [docs/OPEN_QUESTIONS.md](docs/OPEN_QUESTIONS.md): decisions and unknowns that must be resolved before product work starts.

The directory name "RASTER OS" is only a folder name; the product is PACSCORDER.
