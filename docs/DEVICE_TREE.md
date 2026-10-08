# PACSCORDER Device Tree

| | |
|---|---|
| Document status | DRAFT — no PACSCORDER Device Tree source or overlay exists. The baseline is the stock Raspberry Pi TC358743 overlay, unmodified, NOT YET LOADED ON PACSCORDER HARDWARE. |
| Last updated | 2026-10-08 |
| Applies to | Device Tree description of the TC358743 and its `config.txt` overlay configuration on Raspberry Pi 4 Model B, CM4, Pi 5 and CM5, for both the 2-lane and the 4-lane configuration the product needs (REQ-CAP-007), including the `tc358743-audio` overlay (HDMI audio required, REQ-CAP-006). Bring-up evaluates CM4 and CM5 side by side (owner, 2026-10-07; ADR-004 OPEN); Pi 4 Model B and Pi 5 content is kept. Overlay and driver sources: `raspberrypi/linux` branch `rpi-6.18.y` as of 2026-10-06; audio overlay and audio Device Tree sources as read for research topic I on 2026-10-08. |
| Verification | Source research of 2026-10-06, plus research topic I (HDMI audio path) of 2026-10-08 ([REFERENCES.md](REFERENCES.md)). No overlay has been loaded on PACSCORDER hardware: no hardware exists as of 2026-10-08. |
| Rules | [ENGINEERING_RULES.md](ENGINEERING_RULES.md) Rule 6 (Device Tree documentation), Rule 8, Rule 22, Rule 23 |

This document records how the TC358743 is, or will be, described to the Linux kernel on each candidate platform (Rule 6). It covers:

- what the kernel binding and the driver require;
- what the stock Raspberry Pi overlays contain and what they leave out;
- what a PACSCORDER overlay would need (PROPOSED);
- the `config.txt` lines per platform;
- the Device Tree change log in the Rule 6 format.

**State on 2026-10-06.** Device Tree work is `NOT STARTED`. No PACSCORDER overlay has been written, and no overlay has been loaded on PACSCORDER hardware. The overlay test, TEST-PLT-001, is `BLOCKED — HARDWARE REQUIRED`. Every board-dependent value comes from [HARDWARE.md](HARDWARE.md), where all of them are `UNKNOWN — VERIFICATION REQUIRED`.

**Owner decisions of 2026-10-07.** The product needs both a 2-lane and a 4-lane CSI-2 configuration, each capturing every frame rate its link can carry (REQ-CAP-007, DRAFT; OQ-001 ANSWERED). PACSCORDER will therefore need at least two Device Tree lane settings: `data-lanes = <1 2>` for the 2-lane configuration and `<1 2 3 4>` (the `4lane` parameter) for the 4-lane configuration (§5.8). Which platform and connector carry each configuration is still OPEN (ADR-004); no platform is chosen in this document.

**Owner decisions of 2026-10-07, second set (propagated 2026-10-08).** Bring-up evaluates **CM4 and CM5 side by side**, and the product platform is decided from measurements (ADR-004 stays OPEN), so the CM4 (§6.2) and CM5 (§6.4) configurations are the bring-up focus; the Pi 4 Model B and Pi 5 sections are kept. HDMI audio is required (REQ-CAP-006, DRAFT; OQ-004 ANSWERED), so the stock `tc358743-audio` overlay becomes part of the proposed configuration on every platform (§3.5, §6.0, §9). On CM5 that overlay's operation is unconfirmed (OQ-054). The other decisions of that set (H.264 and H.265 encoding; any HDMI camera plus ATEM outputs as sources) do not change the Device Tree.

**Source baseline.** The overlay and driver facts below come from `raspberrypi/linux` `rpi-6.18.y`, the default branch on 2026-10-06 [B-01], whose tip reported version 6.18.55 [E-37]. Raspberry Pi OS Lite 2026-10-06 ships kernel 6.18.50 [G-04], [G-06]. That the overlay sources of 6.18.50 match the branch tip is NEEDS VERIFICATION — KERNEL SOURCE INSPECTION REQUIRED (the same question for the driver file `tc358743.c` is OQ-097). *(2026-10-08: research topic I also read the `tc358743-audio` overlay, `overlay_map.dts` and the audio-related base Device Trees at the `rpi-6.18.y` branch head; the packaged 6.18.50 kernels' `.config` options were checked, source lines were not (research gap, topic I). OQ-097's scope note covers these files.)*

Fact IDs such as `[B-41]` point to [REFERENCES.md](REFERENCES.md). Facts from the `community` tier are worded as reports. Facts from the `reasoning` tier, and Claude's own reasoning, are labelled as such. `CORRECTED` entries are used in their corrected wording only. Everything marked **PROPOSED** is Claude's recommendation (Rule 23 priority 8) and has not been accepted by the owner.

---

## 1. Rule 6 summary

| Rule 6 item | Stock overlay (source) | PACSCORDER value | Open questions | Section |
|---|---|---|---|---|
| I2C bus | `&i2c_csi_dsi`; `&i2c_csi_dsi0` with `cam0` [B-41], [B-42] | UNKNOWN — VERIFICATION REQUIRED | OQ-011, OQ-021, OQ-043, OQ-052 | [§5.1](#51-i2c-bus) |
| TC358743 address | `reg = <0x0f>` [B-41], [A-15] | UNKNOWN — VERIFICATION REQUIRED (0x0f expected, not confirmed) | OQ-026 | [§5.2](#52-tc358743-address) |
| Reset GPIO | None [B-41], [C-23] | UNKNOWN — VERIFICATION REQUIRED | OQ-020 | [§5.3](#53-reset-gpio) |
| Interrupt GPIO | None; driver polls every 1000 ms [A-30], [C-20] | UNKNOWN — VERIFICATION REQUIRED | OQ-020 | [§5.4](#54-interrupt-gpio) |
| Power supplies | None in binding, overlay or driver [B-04], [B-05], [C-23] | UNKNOWN — VERIFICATION REQUIRED | OQ-022, OQ-023 | [§5.5](#55-power-supplies) |
| CSI endpoint | Port 0 endpoint linked to `csi1` (`csi0` with `cam0`) [B-42] | UNKNOWN — VERIFICATION REQUIRED | OQ-021 | [§5.6](#56-csi-endpoint) |
| Clock configuration | REFCLK: `fixed-clock`, 27 MHz declared. Link: `link-frequencies` 486000000. `clock-noncontinuous` [A-45], [B-41] | REFCLK UNKNOWN (OQ-019). Link frequency: PROPOSED keep 486000000 (ADR-008, PROPOSED). | OQ-019, OQ-037, OQ-050, OQ-099 | [§5.7](#57-clock-configuration) |
| CSI lanes | `data-lanes = <1 2>`; `<1 2 3 4>` with `4lane` [B-42], [C-12] | Both needed: `<1 2>` for the 2-lane and `<1 2 3 4>` for the 4-lane configuration (REQ-CAP-007). Routed lanes per board: UNKNOWN — VERIFICATION REQUIRED | OQ-021, OQ-038 | [§5.8](#58-csi-lanes) |
| Lane mapping | `clock-lanes = <0>`, data lanes in order [B-41], [C-12] | UNKNOWN — VERIFICATION REQUIRED | OQ-021 | [§5.9](#59-lane-mapping) |
| Compatible string | `"toshiba,tc358743"` [A-20], [B-41] | PROPOSED: same, if the in-tree driver is used (ADR-002 PROPOSED, not decided) | OQ-013 | [§5.10](#510-compatible-strings) |
| Pi 4 configuration | `tc358743` overlay, 2 lanes | UNKNOWN — platform undecided (ADR-004, per configuration). Candidate for the 2-lane configuration only (REQ-CAP-007) | OQ-011, OQ-014, OQ-100 | [§6.1](#61-raspberry-pi-4-model-b) |
| CM4 configuration | `tc358743` overlay, CAM1 4 lanes / CAM0 2 lanes | UNKNOWN — platform undecided. CAM1: 4-lane candidate; CAM0: 2-lane candidate (REQ-CAP-007). Evaluated side by side with CM5 in bring-up (owner, 2026-10-07) | OQ-011, OQ-014, OQ-018, OQ-100 | [§6.2](#62-compute-module-4) |
| Pi 5 configuration | `tc358743` redirected to `tc358743-pi5` [C-11] | UNKNOWN — platform undecided. 4-lane candidate; 2-lane only with a 2-lane bridge board (REQ-CAP-007; OQ-021) | OQ-011, OQ-049, OQ-050, OQ-100 | [§6.3](#63-raspberry-pi-5) |
| CM5 configuration | As Pi 5; carrier-dependent I2C pairing | UNKNOWN — platform undecided. As Pi 5 (REQ-CAP-007); carrier-dependent. Evaluated side by side with CM4 in bring-up (owner, 2026-10-07) | OQ-011, OQ-052, OQ-100 | [§6.4](#64-compute-module-5) |
| Audio overlay (I2S) — added 2026-10-08 | `tc358743-audio`: enables `i2s_clk_consumer`, adds a `linux,spdif-dir` stub codec as clock master, creates the ALSA card `tc358743` [I-01], [I-02], [I-03] | PROPOSED: stock `tc358743-audio`, unmodified, on every platform (audio required, REQ-CAP-006). CM5 operation unconfirmed. A custom pin group only if GPIO 21 must be freed | OQ-025, OQ-054, OQ-114 | [§3.5](#35-tc358743-audio) |

---

## 2. The binding and what the driver requires

The TC358743 binding is the plain-text file `Documentation/devicetree/bindings/media/i2c/toshiba,tc358743.txt`. No YAML schema exists, and the file is the same in mainline and `rpi-6.18.y` [A-44], [B-03]. The driver `drivers/media/i2c/tc358743.c` is the same in both trees apart from one formatting difference [A-48], [B-02].

| Property | Binding | What the driver does with it | Facts |
|---|---|---|---|
| `compatible` | Required: `"toshiba,tc358743"` | Matched by the driver; I2C id `"tc358743"` | [B-04], [A-20] |
| `reg` | Not in the property list; the example uses `<0x0f>` | I2C address used for the CHIPID read | [B-04], [A-15] |
| `clocks`, `clock-names` | Required; input named `"refclk"` | Gets `"refclk"`; rate must be 26, 27 or 42 MHz. Any other rate leads to a kernel BUG rather than a clean probe failure (CORRECTED). | [B-04], [B-07], [A-22] |
| `reset-gpios` | Optional; example `GPIO_ACTIVE_LOW` | If present: wait 5–10 ms, assert 1–2 ms, deassert, wait 20 ms before the CHIPID read | [B-05], [B-13] |
| `interrupts` | Optional; example `IRQ_TYPE_LEVEL_HIGH` | If present: threaded IRQ, `IRQF_TRIGGER_HIGH \| IRQF_ONESHOT`. Otherwise polling every 1000 ms (10 ms with CEC). | [B-05], [B-19] |
| Endpoint on port 0 | Endpoint properties are marked optional | **Required in practice.** Probe fails with `-EINVAL` unless port 0 has an endpoint that parses as CSI-2 D-PHY with 1–4 data lanes and at least one `link-frequencies` entry. | [B-06] |
| `data-lanes` | Optional: `<1 2 3 4>` or `<1 2>` | Must be 1–4 at probe; never compared with the lane count computed at runtime | [B-05], [B-06], [C-15] |
| `clock-lanes` | Optional: `<0>` | — | [B-05] |
| `clock-noncontinuous` | Optional boolean | Affects only one register write in `tc358743_set_csi()`. Every stream-on writes continuous-clock mode, and the driver reports flags 0 to the receiver. | [B-05], [B-37] |
| `link-frequencies` | Optional; 64-bit; half the bit rate per lane | Only entry 0 is used. Bit rate per lane = 2 × entry 0, which must be 62.5 Mbps–1 Gbps. Timing tables exist only for 594 and 972 Mbps; any other rate logs `untested bps per lane` and uses the 594 Mbps values. | [A-44], [B-08], [B-09], [A-24] |
| Supply properties | None defined | The driver requests no regulator | [B-04], [B-05], [C-23] |

**Runtime lane selection.** The driver chooses the number of active lanes at runtime from the timings and the format. It reports the result to the receiver without clamping it to the DT `data-lanes` value (CORRECTED) [A-25], [B-31]. Both Raspberry Pi receivers fail stream start with `-EINVAL` when more lanes are requested than their DT endpoint provides; fewer, such as 3 of 4, are accepted [B-32], [C-16]. Reasoning: `data-lanes` must therefore state the lanes that are physically wired, no more and no fewer.

---

## 3. What the stock Raspberry Pi overlays contain

### 3.1 `tc358743.dtsi` (shared by both TC358743 overlays)

| Item | Content | Facts |
|---|---|---|
| I2C parent | `&i2c_csi_dsi`; `cam0` retargets to `&i2c_csi_dsi0` | [B-41], [B-42] |
| Node and address | `tc358743@f`, `reg = <0x0f>` | [B-41], [A-15] |
| Compatible | `"toshiba,tc358743"` | [B-41] |
| Reference clock | `clocks = <&cam1_clk>`, `clock-names = "refclk"`; `cam1_clk` gets `clock-frequency = <27000000>`. `cam0` switches to `cam0_clk`. Both are `fixed-clock` nodes, disabled by default in the base DT. | [B-41], [B-42], [A-45], [B-47] |
| Bridge endpoint | `clock-lanes = <0>`, `clock-noncontinuous`, `link-frequencies = /bits/ 64 <486000000>`, `data-lanes = <1 2>` | [B-41], [C-12] |
| Receiver | CSI fragment targets `csi1`; `cam0` retargets to `csi0`. `4lane` also sets `data-lanes` on `csi1_ep`. | [B-42] |
| Not present | No `reset-gpios`, no `interrupts`, no regulator and no reference to `cam0_reg` or `cam1_reg` | [B-41], [C-23], [A-30] |

Lines quoted in the source register (excerpts only; node nesting and the rest of the file are not reproduced):

```dts
/* Excerpts quoted in [B-41], [C-12], [A-45] — NOT the complete file */
tc358743: tc358743@f {
        compatible = "toshiba,tc358743";
        reg = <0x0f>;
        clocks = <&cam1_clk>;
        clock-names = "refclk";
        /* ... */
                clock-lanes = <0>;
                clock-noncontinuous;
                link-frequencies = /bits/ 64 <486000000>;
                /* ... */
                data-lanes = <1 2>;

/* clock fragment (clk_frag), target cam1_clk: */
        clock-frequency = <27000000>;

/* overrides, quoted in [B-42], [C-12]: */
        4lane = <0>, "-2+3-7+8";
        link-frequency = <&tc358743_0>,"link-frequencies#0";
        cam0 = <&i2c_frag>, "target:0=",<&i2c_csi_dsi0>,
               <&csi_frag>, "target:0=",<&csi0>,
               <&clk_frag>, "target:0=",<&cam0_clk>,
               <&tc358743>, "clocks:0=",<&cam0_clk>;
```

### 3.2 `tc358743-overlay.dts` (Pi 4 family)

- Compatible `brcm,bcm2835`; it includes `tc358743.dtsi` [B-43], [E-42].
- By default, `fragment@100` (`legacy_frag`) changes the `csi1` compatible to `"brcm,bcm2835-unicam-legacy"`, so Unicam runs in video-node-centric (legacy) mode [B-43], [C-10].
- `media-controller = <0>,"!100"` removes that fragment when set, which leaves the Media Controller compatible in place [B-43], [E-42]. `cam0` also retargets the legacy fragment to `csi0` [C-10].
- **Effect on control.** In legacy mode, EDID and DV-timings ioctls go through the video node, and sub-device nodes are registered read-only. In Media Controller mode they must be issued on a read-write `/dev/v4l-subdevN` (both CORRECTED) [B-25], [C-36]. This choice is ADR-006 (PROPOSED: Media Controller mode everywhere; OQ-014).

### 3.3 `tc358743-pi5-overlay.dts` (BCM2712)

- It includes the same `tc358743.dtsi` with compatible `brcm,bcm2712`. It has no legacy fragment and no `media-controller` parameter [B-44], [C-11], [E-43]. Its parameters are `4lane`, `link-frequency` and `cam0` [G-13].
- On Pi 5, `i2c_csi_dsi` is an alias of `i2c_csi_dsi1` = `&i2c4` (MIPI1 connector, 100 kHz), and `csi1` = `&rp1_csi1` (compatible `"raspberrypi,rp1-cfe"`). Dummy `i2c0if`/`i2c0mux` nodes exist for overlay compatibility (CORRECTED) [B-44].
- On CM5 the I2C mapping depends on the IO-board DT (CORRECTED) [B-44]; see §6.4.

### 3.4 The Pi 5 `overlay_map` redirect

On BCM2712 (Pi 5 and CM5) the overlay map silently redirects `dtoverlay=tc358743` to `tc358743-pi5` [C-11], [E-43]. The map entry quoted in the register is:

```dts
tc358743 { bcm2835; bcm2711; bcm2712 = "tc358743-pi5"; };
```

Consequences:

- The same `config.txt` line applies a different overlay on Pi 5/CM5 than on Pi 4/CM4.
- The Pi 5/CM5 path is always Media Controller [C-11]. Parameters must be valid for `tc358743-pi5`, which has no `media-controller` parameter [G-13].
- *(Added 2026-10-08, research topic I.)* `overlay_map.dts` has no `tc358743-audio` node; its only TC358743 node is the one quoted above [I-05]. Official documentation says an overlay not mentioned in the map is assumed to be compatible with all platforms, where `bcm2712` covers Pi 5 and CM5 [I-06]. So on Pi 5/CM5 `dtoverlay=tc358743-audio` loads under its own name and the firmware does not block it; whether it then operates depends on its labels resolving in the BCM2712 base Device Tree [I-06] (§3.5).
- The redirect needs `overlay_map.dtb` on the boot partition. Buildroot copies it, together with the `.dtbo` files, from the `raspberrypi/firmware` tarball [E-14]. Buildroot's `raspberrypi5_defconfig` disables overlay installation [E-15], [G-61], and the LTS-era firmware commit has no `tc358743-pi5.dtbo` [E-53]. See OQ-065 and [BUILD_SYSTEM.md](BUILD_SYSTEM.md).

### 3.5 `tc358743-audio`

- A `simple-audio-card` named `tc358743`. A dummy codec (`"linux,spdif-dir"`) is bit-clock and frame master; the CPU DAI is `i2s_clk_consumer` with two 32-bit TDM slots [B-46]. Its only parameter is `card-name` [G-14].
- Documented wiring: LRCK/WFS to GPIO 19, BCK/SCK to GPIO 18, DATA/SD to GPIO 20 [A-47], [G-14]. Official documentation says audio needs this overlay in addition to `tc358743` [C-37].
- Use only if audio is required (OQ-004, REQ-CAP-006). Behaviour on Pi 5/CM5 is unverified (OQ-054). *(Superseded 2026-10-07: audio is required — REQ-CAP-006, OQ-004 ANSWERED — so the overlay is part of the proposed configuration on every platform (§6.0). Pi 5/CM5 behaviour is still unverified; see the CM5 notes below.)*

**Overlay contents per fragment** *(added 2026-10-08, research topic I; `rpi-6.18.y`)*:

| Part | Content | Facts |
|---|---|---|
| Root | `compatible = "brcm,bcm2835"`; the overlay never references the plain `&i2s` label | [I-01] |
| `fragment@0` | Target `<&i2s_clk_consumer>`; sets only `status = "okay"` | [I-01] |
| `fragment@1` (target-path `/`) | Adds node `tc358743_codec: tc358743-codec` with `#sound-dai-cells = <0>`, `compatible = "linux,spdif-dir"`, `status = "okay"`. The TC358743 has no ASoC codec driver of its own. | [I-02] |
| `fragment@2` | Makes `<&sound>` a `simple-audio-card`, format `"i2s"`, name `"tc358743"`. `bitclock-master` and `frame-master` both point at the codec subnode, so the TC358743 drives BCK and LRCK. CPU DAI `<&i2s_clk_consumer>` with `dai-tdm-slot-num = <2>` and `dai-tdm-slot-width = <32>`. | [I-03] |
| Overrides | Only `card-name` (default `"tc358743"`) | [I-03], [I-04] |

Lines quoted in the source register (excerpts only; NOT the complete file):

```dts
/* Excerpts quoted in [I-01], [I-02], [I-03] — NOT the complete file */
compatible = "brcm,bcm2835";
fragment@0 { target = <&i2s_clk_consumer>; __overlay__ { status = "okay"; }; };
tc358743_codec: tc358743-codec {
        #sound-dai-cells = <0>;
        compatible = "linux,spdif-dir";
        status = "okay";
};
simple-audio-card,format = "i2s";
simple-audio-card,name = "tc358743";
simple-audio-card,bitclock-master = <&dailink0_master>;
simple-audio-card,frame-master = <&dailink0_master>;
simple-audio-card,cpu { sound-dai = <&i2s_clk_consumer>; dai-tdm-slot-num = <2>; dai-tdm-slot-width = <32>; };
dailink0_master: simple-audio-card,codec { sound-dai = <&tc358743_codec>; };
__overrides__ { card-name = <&sound_overlay>,"simple-audio-card,name"; }
```

- **Load with `tc358743`.** Official documentation says audio needs this overlay in addition to `tc358743` [C-37]. A Raspberry Pi engineer (6by9) stated on the forum that `tc358743-audio` "*requires* dtoverlay=tc358743 to be loaded too", because the TC358743 has to be configured by its driver (community source) [I-16].
- **Clock roles (reasoning).** The datasheet makes the TC358743 the I2S clock master only [I-25]; the overlay makes the codec link clock master and the Pi the clock consumer. The overlay's 2 × 32-bit slots match the datasheet's 32-bit time slots: 64 bit clocks per frame [I-28].
- **The stub codec knows nothing about the TC358743.** `linux,spdif-dir` (`spdif_receiver.c`, module `snd-soc-spdif-rx`) has one capture-only DAI, `dir-hifi`, accepting 1–384 channels at 8–768 kHz in S16_LE, S20_3LE, S24_LE, S32_LE or IEC958_SUBFRAME_LE. It has no DAI operations, no ALSA controls and no reference to the TC358743 driver [I-11]. Reasoning: nothing in this Device Tree path tells ALSA the HDMI sample rate [I-18]; the application has to read the driver's control (RISK-023, OQ-111; [TC358743_DRIVER.md](TC358743_DRIVER.md) §22.1).
- **ALSA names.** The card id is `tc358743` (from `simple-audio-card,name`), so the device can be opened as `hw:CARD=tc358743,DEV=0` whatever card index is assigned. On CM4 the PCM is named `bcm2835-i2s-dir-hifi dir-hifi-0` [I-15]. Reasoning from source: on CM5 the PCM name will differ (expected `1f000a4000.i2s-dir-hifi dir-hifi-0`, not verified on hardware), so software should select the card by id, not by PCM name [I-17].

**How `i2s_clk_consumer` resolves per platform** *(added 2026-10-08)*:

| Platform | Label resolves to | Pins | Notes | Facts |
|---|---|---|---|---|
| Pi 4 Model B | Not covered by the topic I register entries | GPIO 18/19/20 per README [A-47], [G-14] | KERNEL SOURCE INSPECTION REQUIRED for the Pi 4 Model B base DT | — |
| CM4 (BCM2711) | `bcm270x-rpi.dtsi` points `i2s_clk_producer` and `i2s_clk_consumer` at the single `&i2s` node (`i2s@7e203000`, `"brcm,bcm2835-i2s"`, disabled by default in `bcm283x.dtsi`). `fragment@0` therefore flips the same node as `dtparam=i2s=on` (default off). | `bcm2711-rpi-cm4.dts` sets `&i2s` `pinctrl-0 = <&i2s_pins>`; `i2s_pins` = GPIO 18, 19, 20, 21 in `BCM2835_FSEL_ALT0` | Capture: exactly 2 channels, 8–384 kHz [I-13] | [I-10], [I-13], [I-30] |
| Pi 5 (BCM2712) | `bcm2712-rpi.dtsi` defines the labels (see CM5 row) | As CM5 if the Pi 5 board DT includes `bcm2712-rpi.dtsi`, which is not in the register | KERNEL SOURCE INSPECTION REQUIRED (OQ-054) | [I-07] |
| CM5 (BCM2712) | `bcm2712-rpi.dtsi`: `i2s: &rp1_i2s0`, `i2s_clk_producer: &rp1_i2s0`, `i2s_clk_consumer: &rp1_i2s1`; `sound: sound { status = "disabled"; }`. `bcm2712-rpi-cm5.dtsi` includes `bcm2712-rpi.dtsi`, so both labels the overlay needs exist on CM5. | `&i2s_clk_consumer` `pinctrl-0 = <&rp1_i2s1_18_21>`: function `i2s1` on GPIO 18, 19, 20, 21, `bias-disable` | `rp1_i2s1` is `i2s@a4000`, `"snps,designware-i2s"`, RP1 DMA I2S1 TX/RX, `status = "disabled"`, interrupt commented out ("Providing an interrupt disables DMA") [I-08]. `dwc-i2s` accepts the codec-master format (BC_FC) only on a clock-consumer instance; RP1 I2S1 is that instance [I-09]. The overlay's 2 × 32-bit TDM setting passes `dwc-i2s`'s checks; capture channel count and formats are read from hardware registers [I-14]. | [I-07], [I-08], [I-09], [I-14] |

- **CM5 status.** Nothing in the sources rules the CM5 path out, but no official statement or test result shows audio captured through it (research gap, topic I). UNKNOWN — VERIFICATION REQUIRED; HARDWARE TEST REQUIRED (OQ-054, TEST-AUD-001). Reasoning in ADR-004: a bring-up gate before CM5 can be chosen.
- **Pins claimed.** On both CM4 and CM5 the pin group claims GPIO 18–21, including GPIO 21, which the TC358743 path does not use [I-30]. Freeing GPIO 21 would need a PACSCORDER overlay with its own pin group for GPIO 18–20 (research gap, topic I; OQ-114). Overlays that default to GPIO 18 or 18/19 conflict (`pwm`, `pwm-2chan`, `gpio-ir`, `audremap` `pins_18_19` on BCM2711) [I-31]; see [HARDWARE.md](HARDWARE.md) §4.6.1.

### 3.6 Overlay parameters

| Parameter | `tc358743` | `tc358743-pi5` | Effect | Facts |
|---|---|---|---|---|
| `4lane` | Yes | Yes | `data-lanes = <1 2 3 4>` on the bridge endpoint and on `csi1_ep` | [B-42], [G-12], [G-13] |
| `link-frequency` | Yes | Yes | Writes `link-frequencies` entry 0. The README lists only `297000000` and `486000000` (default) as supported; the driver has timing tables only for these two and warns `untested bps per lane` for any other value. | [B-42], [G-12], [G-13], [B-45], [B-09] |
| `media-controller` | Yes, default off | **No** | Removes `fragment@100` (legacy compatible) | [B-43], [G-12], [G-13] |
| `cam0` | Yes | Yes | Retargets I2C to `i2c_csi_dsi0`, CSI to `csi0`, clock to `cam0_clk` | [B-42], [G-12], [G-13] |

**Errors and stale text in the overlays README** (do not rely on them):

- `297000000` is labelled "574Mbit/s"; the driver rate is 2 × 297 MHz = 594 Mbit/s [A-46], [B-45], [C-13].
- The `tc358743-pi5` entry still says "Uses Unicam 1" and calls `4lane` Compute-Module-CAM1-only [C-13].
- The `cam0` description mentions "CSI0, i2c_vc, and cam0_reg" [G-12], but `tc358743.dtsi` references no `cam0_reg` or `cam1_reg` [C-23].
- `tc358743-audio` refers to a `tc358743-fast` overlay that has no README entry [B-46]. *(2026-10-08: `tc358743-fast` is a stale name. The overlays Makefile builds only `tc358743.dtbo`, `tc358743-audio.dtbo` and `tc358743-pi5.dtbo`, and the directory has only those overlays plus `tc358743.dtsi` [I-04]. Reasoning from [I-04] and [C-37]: read "tc358743-fast" as `tc358743`.)*

### 3.7 Where the compiled overlays come from

- In Buildroot, the prebuilt `.dtbo` files and `overlay_map.dtb` are copied from the `raspberrypi/firmware` tree, not built from the kernel [E-14], [G-61].
- Firmware commit `063bcab6` (Buildroot 2026.08) contains `tc358743.dtbo`, `tc358743-pi5.dtbo` and `overlay_map.dtb`. LTS firmware commit `5476720d` has no `tc358743-pi5.dtbo` [E-53].
- That the Raspberry Pi OS Lite 2026-10-06 image contains these three files is not a separate register fact. NEEDS VERIFICATION on the image.

---

## 4. What the stock overlay does not provide, and what a PACSCORDER overlay would need

### 4.1 Gaps

| Not provided by the stock overlay | Fact | Consequence | What PACSCORDER would need (PROPOSED) | Depends on |
|---|---|---|---|---|
| `reset-gpios` | [B-41], [C-23] | The driver never pulses RESETN. Reasoning from [B-13]: board circuitry must release RESETN before the probe reads CHIPID. | Add `reset-gpios` if RESETN is wired to a Pi GPIO | OQ-020 |
| `interrupts` | [B-41], [A-30], [C-20] | Status is polled every 1000 ms (RISK-013) | Add an interrupt if INT is wired to a Pi GPIO | OQ-020 |
| Any regulator; CAM_GPIO handling | [C-23], [C-51] | The camera power-enable line is expected to stay low (reasoning) [C-51] | Handle CAM_GPIO in the overlay if the board uses it (§4.3) | OQ-022 |
| REFCLK other than 27 MHz | [A-45]; no parameter for it [G-12], [G-13] | A DT value other than 26, 27 or 42 MHz leads to a kernel BUG [A-22] (RISK-007). Reasoning from [B-07], [B-08]: if the DT says 27 MHz but the oscillator is 26 or 42 MHz, the driver computes its PLL settings from the wrong reference; the effect on the lane rate is HARDWARE TEST REQUIRED. | Set `clock-frequency` to the measured oscillator | OQ-019 |
| I2C address other than 0x0f | [A-15]; no parameter for it [G-12], [G-13] | Probe fails with `-ENODEV` [A-19] | Change `reg` | OQ-026 |
| I2C/CSI pairing that matches the connector (CM5 on the CM4 IO Board) | [C-05], [C-27]; research open question, topic C | The research found that neither stock pairing appears to match the CAM1 connector, so the bridge may be probed on an I2C bus or CSI receiver that does not belong to the connector in use (unconfirmed; HARDWARE TEST REQUIRED) | Retarget the I2C and CSI fragments | OQ-052 |
| *(Added 2026-10-08.)* An audio pin group without GPIO 21 (`tc358743-audio`) | [I-30] | The I2S pin group claims GPIO 18–21, although the TC358743 path uses only 18, 19 and 20 [I-30] | Only if GPIO 21 must be freed: a PACSCORDER audio overlay with its own pin group for GPIO 18–20 (research gap, topic I). BUILD TEST REQUIRED. | OQ-114 |
| *(Added 2026-10-08.)* Any path that tells ALSA the HDMI audio sample rate | [I-11], [I-18] | The stub codec has no controls and no link to the TC358743 driver [I-11]; reasoning: the kernel does not carry HDMI rate changes into ALSA [I-18] (RISK-023). Without `interrupts` a rate change also takes up to about 1 s to reach the driver's control [I-22]. | Not a Device Tree fix: the application reads the driver's sampling-rate control or its change event ([TC358743_DRIVER.md](TC358743_DRIVER.md) §22.1). Wiring INT (row above) shortens the delay (research gap, topic I: possible but untested). | OQ-111, OQ-020 |

### 4.2 Decision rule (PROPOSED)

- **Stock overlay, unmodified.** If HW REV A matches every stock assumption, bring-up uses the stock overlay through `config.txt` only. The assumptions are: 27 MHz REFCLK; I2C address 0x0f; wired lanes equal to the `data-lanes` setting; no INT or RESETN connection required; no dependency on CAM_GPIO; and an I2C/CSI pairing that matches the connector. This follows the bring-up approach of ADR-002 (PROPOSED) and ADR-003 (ACCEPTED 2026-10-07).
- **PACSCORDER overlay.** Otherwise, a PACSCORDER overlay derived from `tc358743.dtsi` is needed. Status: `NOT STARTED`. It will be kept in this repository and recorded in the change log (§9) before it is loaded. Its build procedure belongs in [BUILD_SYSTEM.md](BUILD_SYSTEM.md); compiler and install steps are NEEDS VERIFICATION. Because the product runs its own project-built OS image (REQ-BLD-002, owner 2026-10-07; build tool ADR-003, ACCEPTED 2026-10-07: `rpi-image-gen`), such an overlay would be installed by that image build.

### 4.3 Properties a PACSCORDER overlay would add (PROPOSED sketch)

> **PROPOSED SKETCH — NOT COMPILED, NOT LOADED, NOT TESTED.** Placeholders in `<ANGLE_BRACKETS>` are `UNKNOWN — VERIFICATION REQUIRED`. Only the property names and flags come from sources: `reset-gpios` with `GPIO_ACTIVE_LOW` [B-05], [A-27]; `interrupts` with `IRQ_TYPE_LEVEL_HIGH` [B-05], [A-29]. `interrupt-parent` is generic Device Tree usage that is not in the source register: NEEDS VERIFICATION.

```text
Additional properties on the tc358743@f node:
    reset-gpios      = <&<GPIO_CONTROLLER> <RESET_GPIO> GPIO_ACTIVE_LOW>;   /* only if OQ-020: RESETN wired */
    interrupt-parent = <&<GPIO_CONTROLLER>>;                                /* NEEDS VERIFICATION */
    interrupts       = <<INT_GPIO> IRQ_TYPE_LEVEL_HIGH>;                    /* only if OQ-020: INT wired */
Values the stock overlay fixes that may need changing:
    reg              = <<I2C_ADDRESS>>;           /* stock 0x0f; OQ-026 */
    clock-frequency  = <<REFCLK_HZ>>;             /* on cam1_clk / cam0_clk; stock 27000000; OQ-019 */
```

Notes on the placeholders:

- **GPIO controller.** On Pi 5/CM5 the base DT references RP1 GPIOs as `&rp1_gpio` [C-22]. The label of the main BCM2711 GPIO controller on Pi 4/CM4 is not in the source register: KERNEL SOURCE INSPECTION REQUIRED. It is needed only if INT or RESETN is wired to a Pi GPIO (OQ-020).
- **Firmware expander GPIO.** The Pi 4/CM4 camera power-enable line is a firmware-controlled expander GPIO [C-21]. The driver toggles reset with `gpiod_set_value()` [A-28]. Whether that call is valid on the expander GPIO is KERNEL SOURCE INSPECTION REQUIRED. It matters only if the board drives RESETN from CAM_GPIO (OQ-022).
- **INT as a strap.** If INT turns out to be an address strap, as on TC358749XBG [A-50], a pull on the INT line during reset could change the address (OQ-026).

**CAM_GPIO options (PROPOSED; depends on OQ-022):**

1. The board does not use CAM_GPIO: nothing to do.
2. The board uses CAM_GPIO as a power enable: the camera regulator must end up enabled. Reasoning from the regulator code: `regulator-boot-on` makes the enable GPIO start high at probe [C-51]. Whether it stays high with no consumer is KERNEL SOURCE INSPECTION REQUIRED.
3. The board uses CAM_GPIO as RESETN: `reset-gpios` would point at the same line that the base DT already gives to `cam1_reg`/`cam0_reg` [C-21], [C-22]. How to resolve two users of one GPIO is KERNEL SOURCE INSPECTION REQUIRED.

---

## 5. Rule 6 items in detail

### 5.1 I2C bus

**Stock overlay:** the TC358743 node is placed on `&i2c_csi_dsi`, or on `&i2c_csi_dsi0` with `cam0` [B-41], [B-42].
**PACSCORDER value:** UNKNOWN — VERIFICATION REQUIRED (platform OQ-011, connector OQ-021, runtime bus number OQ-043).

How the labels resolve on each platform:

| Platform and connector | Label used | Controller | Linux bus | Pins | Facts |
|---|---|---|---|---|---|
| Pi 4B camera connector | `i2c_csi_dsi` | i2c0 through an `i2c-mux-pinctrl` channel (`i2c@1`) | `/dev/i2c-10` | GPIO 44/45 | [C-24] |
| CM4 CAM1 (default) | `i2c_csi_dsi` | as Pi 4B | `i2c-10` | GPIO 44/45 on the CM4 IO Board | [C-24], [C-25] |
| CM4 CAM0 (`cam0`) | `i2c_csi_dsi0` | i2c0, mux channel `i2c@0` | `i2c-0` | GPIO 0/1 on the CM4 IO Board; J6 jumpers needed there | [C-24], [C-25], [C-03] |
| Pi 5 CAM/DISP1 (default) | `i2c_csi_dsi` → `i2c_csi_dsi1` | RP1 i2c4, 100 kHz | `/dev/i2c-11` (symlink `i2c-4`) | GPIO 40/41 | [C-26], [B-44] |
| Pi 5 CAM/DISP0 (`cam0`) | `i2c_csi_dsi0` | RP1 i2c6 | `/dev/i2c-10` (symlink `i2c-6`) | GPIO 38/39 | [C-26] |
| CM5 on CM5 IO Board, CAM/DISP 1 | `i2c_csi_dsi1` | RP1 i2c0 | symlink `i2c-11` | GPIO 0/1; J6 jumpers needed [C-06] | [C-27], [B-44] |
| CM5 on CM5 IO Board, CAM/DISP 0 | `i2c_csi_dsi0` | RP1 i2c6 | UNKNOWN | GPIO 38/39 | [C-27] |
| CM5 on CM4 IO Board, CAM1 / DISP1 / RTC / fan | `i2c_csi_dsi1` | i2c6 | Alias `i2c10` points at `i2c_csi_dsi`; alias `i2c11` is deleted | UNKNOWN | [C-27], [B-44] |
| CM5 on CM4 IO Board, CAM0 / DISP0 | `i2c_csi_dsi0` | i2c0 | UNKNOWN | UNKNOWN. The CM4 CAM0 pins (128–142) carry USB 3.0 on CM5 [C-05]. | [C-27] |

- **Which label `i2c_csi_dsi` resolves to on the CM5 IO Board** is not stated in a register fact. The research notes say the default overlay targets CAM/DISP 1 there, and that a DT comment conflicts with the official J6-jumper statement (research gap, topic C). HARDWARE TEST REQUIRED — OQ-052.
- **Kernel modules.** `I2C_BCM2835` and `I2C_MUX_PINCTRL` are built as modules in the Raspberry Pi defconfigs [E-39]. Reasoning: on Pi 4/CM4 the mux driver must be loaded before the TC358743 can probe. On Buildroot, module autoloading needs mdev or udev [E-50] (OQ-065).
- **Bus speed.** Pi 5 `i2c4` runs at 100 kHz [B-44]. The bus speed of the other camera buses is not in the source register. The TC358743 supports 100 kHz and 400 kHz; a 2 MHz claim in an older brief conflicts with the datasheet [A-14]. PACSCORDER value: UNKNOWN — VERIFICATION REQUIRED (OQ-031).
- **Bus sharing.** With CM5 on the CM4 IO Board, `i2c_csi_dsi1` also serves DISP1, the on-board RTC and the fan controller [C-27]. Buildroot's CM4IO and CM5IO sample `config.txt` files enable `dtparam=i2c_vc=on` and `dtoverlay=i2c-rtc,pcf85063a,i2c_csi_dsi`, putting an RTC on the same bus as the TC358743 (research gap, topic E; not a register fact). If PACSCORDER uses a carrier with another device on the camera bus, record that device's address and check it against the TC358743 address (HARDWARE TEST REQUIRED, OQ-026).
- **Names change.** On Pi 5 the 6.18 device tree numbers the camera buses through the aliases `i2c10` (CAM/DISP0) and `i2c11` (CAM/DISP1) [C-26]. An issue reporter and a Raspberry Pi engineer reported the bridge entity as `tc358743 11-000f` or `tc358743 10-000f` on 6.18 kernels. The same engineer reported buses `i2c-4` and `i2c-6` on earlier kernels (CORRECTED) [C-28]. The entity name therefore contains a bus number that has changed between kernels. Software must not hard-code bus numbers or entity names (OQ-043).

### 5.2 TC358743 address

- **Stock overlay:** `reg = <0x0f>`, node `tc358743@f` [B-41], [A-15].
- **PACSCORDER value:** UNKNOWN — VERIFICATION REQUIRED. DATASHEET REQUIRED; HARDWARE TEST REQUIRED — OQ-026, TEST-HW-001.
- The public datasheet states no address and no address-select strap [A-15]. On the sister part TC358749XBG, the INT pin selects 0x0F or 0x1F at reset [A-50].
- The driver prints addresses shifted left by one, so 0x0f appears as `0x1e` [A-16]. If the CHIPID read fails or the chip-ID byte is not 0x00, it logs `not a TC358743 on address 0x%x` and returns `-ENODEV` [A-19].
- The stock overlay has no parameter to change the address [G-12], [G-13].

### 5.3 Reset GPIO

- **Stock overlay:** none [B-41], [C-23].
- **PACSCORDER value:** UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED — OQ-020.
- RESETN is active low [A-27]. The binding example declares `reset-gpios` with `GPIO_ACTIVE_LOW` [A-27], [B-05].
- The driver requests the line as `GPIOD_OUT_LOW` (deasserted). It waits 5–10 ms, asserts reset for 1–2 ms, deasserts it, then waits 20 ms. These timings are the driver's, not Toshiba's [A-28], [B-13]. Toshiba's minimum pulse width is not public (OQ-031).
- Driver removal does not assert reset [B-40].
- Without `reset-gpios`, reasoning from [B-13]: RESETN must already be deasserted by board circuitry when the driver probes.

### 5.4 Interrupt GPIO

- **Stock overlay:** none. The driver therefore polls every 1000 ms, or every 10 ms when a CEC adapter is registered [A-30], [C-20]. CEC is not enabled in the Raspberry Pi defconfigs [B-20].
- **PACSCORDER value:** UNKNOWN — VERIFICATION REQUIRED. VENDOR CONFIRMATION REQUIRED — OQ-020.
- INT is active high and level-triggered [A-29]. The binding example uses `IRQ_TYPE_LEVEL_HIGH` [A-29], [B-05]. The driver requests a threaded IRQ with `IRQF_TRIGGER_HIGH | IRQF_ONESHOT` [B-19].
- Related, not DT: on Pi 5 a Raspberry Pi engineer reported that source-change events must be subscribed on the TC358743 sub-device node, not the video node [C-42] (OQ-051).
- *(Added 2026-10-08, research topic I.)* The poll interval also governs HDMI audio. The shared `tc358743.dtsi` has no `interrupts` property, so the driver polls every 1000 ms; `CONFIG_VIDEO_TC358743_CEC` is not set in either defconfig or in the packaged 6.18.50 `rpi-v8` and `rpi-2712` kernels, so the 10 ms CEC interval does not apply. A source sample-rate change can therefore take up to about 1 s, plus I2C time, to reach the driver's audio sampling-rate control [I-22] (OQ-111, OQ-020; RISK-023).

### 5.5 Power supplies

- **Binding and driver:** no supply properties are defined [B-04], [B-05], and the driver requests no regulator [C-23]. The TC358743 rails cannot be controlled from the Device Tree with the stock driver.
- **Camera power-enable regulators in the base DT:** Pi 4B `cam1_reg` on expander GPIO 5; CM4 both ports on expander GPIO 5 [C-21]. Pi 5 RP1 GPIO 34 (MIPI0) and 46 (MIPI1); CM5 RP1 GPIO 34 shared [C-22]. They are not consumed by the TC358743 overlay, so the line is expected to stay low (reasoning) [C-51].
- On the CM5 IO Board, a camera on CAM/DISP 1 cannot be powered down [C-06].
- **PACSCORDER value:** UNKNOWN — VERIFICATION REQUIRED (OQ-022, OQ-023). Options are in §4.3.

### 5.6 CSI endpoint

- **Stock overlay:** the bridge endpoint on port 0 links to the receiver endpoint of `csi1`, or of `csi0` with `cam0` [B-42]. The driver requires this endpoint [B-06].
- **Receiver nodes:**
  - Pi 4/CM4: `csi0` is `csi@7e800000` (2 lanes) and `csi1` is `csi@7e801000` (4 lanes). The Pi 4B DT limits `csi1` to 2 lanes [C-08], [B-48].
  - Pi 5/CM5: `rp1_csi0` is `csi@110000` and `rp1_csi1` is `csi@128000`, labelled `csi0`/`csi1` [C-29].
- **Resulting media graph:**
  - On Pi 5/CM5 the link from the bridge to the CFE `csi2` entity is created immutable and enabled. Userspace must enable the `csi2` source pad to video node link [C-32].
  - On Pi 4/CM4 in Media Controller mode, Unicam registers only `unicam-image` for this single-pad bridge, linked directly from the bridge pad (CORRECTED) [C-36].
- Which Unicam driver and mode actually bind on a running Pi 4/CM4 is HARDWARE TEST REQUIRED (OQ-044).
- **PACSCORDER value:** UNKNOWN — VERIFICATION REQUIRED (OQ-021).

### 5.7 Clock configuration

**Reference clock (REFCLK)**

- The stock overlay declares 27 MHz on the `fixed-clock` node `cam1_clk` (`cam0_clk` with `cam0`) [A-45], [B-41]. The Pi does not generate this clock; the bridge board must carry its own oscillator [A-45], [B-47].
- The value must equal the real oscillator frequency. The driver accepts 26, 27 or 42 MHz [B-07]. Any other value leads to a kernel BUG rather than a clean failure (CORRECTED) [A-22], [B-11].
- Reasoning: only 27 MHz gives the exact 594/972 Mbps rates of the driver's timing tables [A-23], [B-10].
- **PACSCORDER value:** UNKNOWN — VERIFICATION REQUIRED (OQ-019).

**CSI-2 link frequency**

- `link-frequencies` is half the bit rate per lane [A-44]. The stock default is `486000000` (972 Mbps per lane) [A-45]. The README lists `297000000` (594 Mbps) as the only other supported value [B-45], and the driver has timing tables only for these two rates [B-09].
- Unicam lanes run at up to 1 Gbit/s, a maximum link frequency of 500 MHz [C-07].
- RP1 CFE (Pi 5/CM5) cannot read a link rate from this bridge, so it always programs its D-PHY for 999 Mbps (reasoning) [C-31], [B-49]. That matches the 972 Mbps default but not 594 Mbps (reasoning) [C-52] (RISK-011, OQ-050).
- At 594 Mbps, 1080p60 RGB888 needs 6 lanes and cannot be carried even on 4 lanes (reasoning) [C-47], [C-49].
- **PROPOSED (ADR-008; owner decision OQ-099):** keep the default `486000000` on all platforms unless a hardware test shows a reason to change it. Inputs: [C-47], [C-49], [C-52]. ADR-008 also proposes evaluating `297000000` only on a CM4 CAM1 4-lane link for 1080p60 UYVY in TEST-CAP-002: at 594 Mbit/s that mode uses all 4 lanes instead of 3 of 4 (reasoning) [C-47], which avoids the unproven 3-of-4-lane case (OQ-038). A driver comment says 594 Mbit/s is meant for 4-lane 1080p60 [C-44].

**Clock-lane mode**

- The stock overlay sets `clock-noncontinuous` [C-12]. The driver writes continuous-clock mode at every stream-on and reports flags 0 to the receiver ("Support for non-continuous CSI-2 clock is missing in the driver") [B-37].
- The effective clock mode on the wire is UNKNOWN — KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED (OQ-037).

### 5.8 CSI lanes

- **Stock overlay:** `data-lanes = <1 2>` by default. `4lane` switches both the bridge endpoint and `csi1_ep` to `<1 2 3 4>` [B-42], [C-12].
- **Receiver limits in the base DT:** Pi 4B `csi1` 2 lanes; CM4 `csi0` 2 lanes and `csi1` 4 lanes [B-48], [C-08]. Unicam accepts only 1, 2 or 4 data lanes, in order, on its endpoint [C-17].
- **PACSCORDER value:** both lane settings are required, one per configuration: `data-lanes = <1 2>` (2-lane configuration) and `<1 2 3 4>` (4-lane configuration) (REQ-CAP-007, owner 2026-10-07). The lanes actually routed by each bridge board, and therefore which setting a given unit uses, are UNKNOWN — VERIFICATION REQUIRED (OQ-021).

**Lane configurations required by REQ-CAP-007 — DT setting per candidate connector.** The connector for each configuration is not chosen (ADR-004, OPEN). The table only maps candidates to configurations.

| Candidate connector | Lanes at the connector | Configuration it can serve | DT lane setting for that configuration |
|---|---|---|---|
| Pi 4 Model B camera connector | 2 [C-01] | 2-lane only | Overlay default `<1 2>` [C-12]; never `4lane` (warning below) |
| CM4 CAM0 | 2 [C-02] | 2-lane only | `cam0`, default `<1 2>` [B-42], [C-12]; never `4lane` |
| CM4 CAM1 | 4 [C-02] | 4-lane; 2-lane with a 2-lane bridge board (reasoning; OQ-021) | `4lane` → `<1 2 3 4>` [B-42], [G-12]; default `<1 2>` for a 2-lane board |
| Pi 5 CAM/DISP1 | 4 [C-04] | 4-lane; 2-lane with a 2-lane bridge board (reasoning; OQ-021) | `4lane` [G-13] (end-to-end capture: OQ-049); default `<1 2>` for a 2-lane board |
| Pi 5 CAM/DISP0 | 4 [C-04] | 2-lane with a 2-lane bridge board (reasoning; OQ-021); 4-lane NEEDS VERIFICATION (OQ-049) | `cam0`, default `<1 2>` [C-12]; whether `cam0` + `4lane` sets 4 lanes on `csi0`: NEEDS VERIFICATION (OQ-049) |
| CM5 MIPI0 / MIPI1 | 4 [C-05] | 4-lane; 2-lane with a 2-lane bridge board (reasoning; OQ-021) | As Pi 5; carrier-dependent (OQ-052) |

Reasoning from [B-32], [C-16] and §2: in every row the DT setting must equal the lanes the bridge board and cable actually route. Whether one bridge-board design can serve both configurations, for example a 4-lane board run with `<1 2>`, is UNKNOWN — VERIFICATION REQUIRED (VENDOR CONFIRMATION REQUIRED, HARDWARE TEST REQUIRED; OQ-021).

> **WARNING — never set `4lane` on a 2-lane connector (Pi 4 Model B, CM4 CAM0).** Declaring more lanes than the connector carries does **not** fail the probe. Unicam logs `subdevice requires %u data lanes when %u are supported` and then adopts the endpoint's lane count [C-17].
>
> Reasoning from [C-16], [C-17] and [B-31]: after that, the stream-start check compares the bridge's request against 4 instead of 2. Modes that need 3 or 4 lanes are then no longer rejected with a clear `-EINVAL`, although only 2 lanes are wired. The resulting behaviour is unknown (HARDWARE TEST REQUIRED). The research also recommends that documentation forbid `4lane` on Pi 4B and CM4 CAM0 (research gap, topic C).

Lanes the driver requests at the default 972 Mbps per lane (reasoning) [C-47], [B-33]:

| Mode | UYVY (16 bpp) | RGB888 (24 bpp) |
|---|---|---|
| 1080p60 | 3 | 4 |
| 1080p50 | 2 | 3 |
| 1080p30 | 2 | 2 |
| 720p60 | 1 [B-33] | 2 [B-33] |

- A request above the DT `data-lanes` is rejected at stream start [B-32], [C-16]. A request below it, such as 3 of 4, is accepted [C-16]. Whether frames are captured correctly with 3 active lanes on a 4-lane endpoint is HARDWARE TEST REQUIRED (OQ-038). This is why a 4-lane port is necessary but not shown sufficient for 1080p60 UYVY at the default link frequency; see §5.7 and ADR-008 (PROPOSED).

**Supported-mode limits per lane configuration** (reasoning-tier entries and official documentation; not test results):

- **2-lane configuration (`<1 2>`).** At 972 Mbps per lane, 1080p30 UYVY, 1080p30 RGB888 and 1080p50 UYVY fit; 1080p50 RGB888 and 1080p60 in either format do not [C-48]. 720p60 fits in both formats (1 and 2 lanes) [B-33]. Official documentation gives the same limit: at most 1080p30 RGB888 or 1080p50 YUV422 on 2 lanes [C-37]. At 594 Mbps only 1080p30 UYVY fits [C-48]. "All frame rates" on this configuration therefore means all rates up to that limit (REQ-CAP-007).
- **4-lane configuration (`<1 2 3 4>`).** At 972 Mbps all six 1080p30/50/60 × UYVY/RGB888 combinations fit, but the driver activates only 2–4 lanes, so not every mode uses all four [C-49]. Official documentation: 4 lanes on a Compute Module carry 1080p60 in either format [C-37]. 1080p60 UYVY on 3 of 4 lanes is unproven (OQ-038).
- **EDID per configuration (reasoning; OQ-002).** The EDID is not a Device Tree property: userspace must supply it with `VIDIOC_S_EDID` [C-37], and the driver's written-EDID block count is 0 after probe (CORRECTED) [B-21]. Because the two lane settings carry different mode sets [C-48], [C-49], the EDID that userspace loads may need to differ per lane configuration, matching the `data-lanes` value in use. The built-in `hdmi` EDID type of `v4l2-ctl` advertises up to 1080p60 [B-24], more than the 2-lane configuration carries. EDID content: OQ-002; procedure: [V4L2.md](V4L2.md).

### 5.9 Lane mapping

- **Stock overlay:** `clock-lanes = <0>`; data lanes in order, `<1 2>` or `<1 2 3 4>` [B-41], [C-12].
- **Binding:** it lists `data-lanes` and `clock-lanes` as its only lane properties [A-44], [B-05].
- **Unicam:** it accepts data lanes only in order [C-17]. Reasoning: a PACSCORDER board used with Pi 4/CM4 must route lanes in order, connector to connector; lane reordering cannot be expressed in the DT for Unicam.
- **RP1 CFE:** whether it supports lane reordering or polarity inversion is not in the source register. KERNEL SOURCE INSPECTION REQUIRED.
- **PACSCORDER lane mapping and polarity:** UNKNOWN — VERIFICATION REQUIRED (OQ-021).

### 5.10 Compatible strings

| Node | Compatible | Bound by | Facts |
|---|---|---|---|
| TC358743 | `"toshiba,tc358743"` | `tc358743` module (`CONFIG_VIDEO_TC358743=m` in both Raspberry Pi defconfigs) | [A-20], [A-35], [B-20] |
| Pi 4/CM4 `csi0`/`csi1`, base DT | `"brcm,bcm2835-unicam"` | Downstream `bcm2835-unicam-legacy` module, Media Controller mode (CORRECTED) | [C-08], [C-09], [E-40] |
| Pi 4/CM4 `csi1` (or `csi0` with `cam0`) after the stock overlay | `"brcm,bcm2835-unicam-legacy"` | Same module; legacy mode unless its `media_controller` module parameter or the `brcm,media-controller` DT property is set (CORRECTED) | [B-43], [C-10], [E-40], [G-21] |
| Mainline Unicam | `"brcm,bcm2835-unicam-upstream"` (not used by the stock DT) | `bcm2835-unicam` module | [C-09], [G-21], [E-40] |
| Pi 5/CM5 `rp1_csi0`/`rp1_csi1` | `"raspberrypi,rp1-cfe"` | Downstream `rp1-cfe-downstream` module | [C-29], [E-44] |
| Mainline RP1 CFE | `"raspberrypi,rp1-cfe-upstream"` (not used by the stock DT) | `rp1-cfe` module | [C-29], [E-44] |
| Overlay roots | `tc358743-overlay`: `brcm,bcm2835`; `tc358743-pi5-overlay`: `brcm,bcm2712` | Firmware overlay loader | [B-43], [B-44] |

Kconfig symbol meanings differ in the 6.12.61 kernel pinned by Buildroot 2026.08 [E-45]; see [BUILD_SYSTEM.md](BUILD_SYSTEM.md).

---

## 6. Platform configurations

### 6.0 Rules common to all platforms

- **File location.** Raspberry Pi OS reads `config.txt` from the boot partition, mounted at `/boot/firmware/` (CORRECTED) [G-11]. Buildroot installs the file named by `BR2_PACKAGE_RPI_FIRMWARE_CONFIG_FILE` [E-16]; its Pi 4 and Pi 5 sample files contain no camera or TC358743 overlay [E-17]. (The CM4IO/CM5IO samples add an RTC overlay on the camera bus; see §5.1.)
- **Attested syntax.** `dtoverlay=tc358743,<param>=<val>` [G-12] and `dtoverlay=tc358743-pi5,<param>=<val>` [G-13]. Official documentation says appending `,cam0` to the `dtoverlay` line selects connector 0 [C-39].
- **Syntax that NEEDS VERIFICATION (OQ-100).** The source register does not attest two things:
  - that a boolean parameter can be given by name alone (`,4lane`, `,media-controller`);
  - how several parameters are combined on one line.

  The lines below use the bare-name form as written in ADR-006. Check both points against the official `config.txt` documentation before use (OQ-100).
- **`camera_auto_detect`.** Official documentation requires `camera_auto_detect=0` for the listed camera-sensor overlays, which do not include the TC358743. Disabling it for the TC358743 is prudent but not a documented requirement (CORRECTED) [C-39], [G-15]. **PROPOSED:** set `camera_auto_detect=0` until OQ-072 is answered by test.
- **One file for several models.** `config.txt` supports `[pi4]`, `[pi5]`, `[cm4]` and `[cm5]` filters [G-71] (reasoning-tier entry, CORRECTED). Whether one filter matches more than one candidate, for example whether `[pi4]` also applies to a CM4, is not in the source register: NEEDS VERIFICATION (OQ-100). If it does, a CM4 could receive two TC358743 lines. **PROPOSED:** until this is checked, keep one `config.txt` per platform.
- **CMA.** The `vc4-kms-v3d` and `cma` overlays take `cma-64` … `cma-512` and `cma-size` parameters. `cma-192` and above need 1 GB of RAM [C-40], [E-47]. Defaults [C-40]:
  - `vc4-kms-v3d-pi4`: (512 − 4) MB;
  - `vc4-kms-v3d-pi5`: 64 MB;
  - base-DT pool: 64 MB, limited to the lower 768 MB on Pi 4/CM4 and to the lower 1 GB by `bcm2712.dtsi` (Pi 5/CM5) [E-47] (CORRECTED).

  Reasoning (reasoning-tier entry, CORRECTED) [C-53]: one 1920 × 1080 UYVY frame is 1920 × 1080 × 2 bytes = 4,147,200 bytes, so four capture buffers take about 16.6 MB (RGB888: 6,220,800 bytes per frame, about 24.9 MB for four). On Pi 4/CM4 Unicam allocates them from CMA; whether Pi 5/CM5 CFE buffers come from CMA at all is not established [C-53]. The CMA size and the exact `config.txt` line are UNKNOWN — VERIFICATION REQUIRED (OQ-061, OQ-053).
- **Audio.** Add `dtoverlay=tc358743-audio` [C-37], [G-14] only if audio is required (OQ-004). It is unverified on Pi 5/CM5 (OQ-054). *(Superseded 2026-10-07: audio is required — REQ-CAP-006, OQ-004 ANSWERED. **PROPOSED (2026-10-08):** add `dtoverlay=tc358743-audio`, together with the `tc358743` / `tc358743-pi5` line, on every platform [C-37], [G-14]. It is written out in the CM4 (§6.2) and CM5 (§6.4) snippets, the bring-up platforms. For Pi 4 Model B and Pi 5 add the same line; on Pi 5 how the labels resolve is KERNEL SOURCE INSPECTION REQUIRED (§3.5). The overlay is not in `overlay_map`, so it is loaded under its own name on every platform [I-05], [I-06]. It is still unverified on Pi 5/CM5 (OQ-054). It claims GPIO 18–21 (OQ-114).)*
- **Comments.** In the snippets below every comment is on its own line, starting with `#`. Whether `config.txt` accepts a comment after a value on the same line is not in the source register: NEEDS VERIFICATION (OQ-100; listed in that entry's scope note). Do not add trailing comments.
- Every line below is **PROPOSED — NOT YET RUN ON PACSCORDER HARDWARE**. The board-dependent choices (`4lane`, `cam0`) stay UNKNOWN until OQ-018 and OQ-021 are answered and the platform for each lane configuration (REQ-CAP-007) is chosen (ADR-004, OPEN).

### 6.1 Raspberry Pi 4 Model B

- One 2-lane connector [C-01]; `csi1` is limited to 2 lanes in the DT [B-48]. Receiver: Unicam [C-09]. I2C: `/dev/i2c-10` on GPIO 44/45 [C-24].
- Capability: at most 1080p30 RGB888 or 1080p50 YUV422; no 1080p60 [C-37], [C-48] (RISK-001). Under REQ-CAP-007 (owner, 2026-10-07) Pi 4 Model B is therefore a candidate for the 2-lane configuration only; it cannot serve the 4-lane configuration, where 1080p60 is required. The platform choice remains OPEN (ADR-004).
- **Never use `4lane`** (§5.8 warning).
- **Never use `cam0`.** Reasoning from [C-07] and [C-21]: Pi 4B routes out only the second Unicam instance, and its `cam0_reg` is a dummy.

```ini
# /boot/firmware/config.txt — Pi 4 Model B
# PROPOSED — NOT YET RUN ON PACSCORDER HARDWARE
# Precaution, not a documented requirement [C-39]; OQ-072:
camera_auto_detect=0

# Option A — overlay default: legacy Unicam video-node mode [B-43], [C-10]
dtoverlay=tc358743

# Option B — ADR-006 (PROPOSED): Media Controller mode [B-43], [G-12]
# Bare boolean parameter form: NEEDS VERIFICATION (§6.0; OQ-100)
#dtoverlay=tc358743,media-controller
```

### 6.2 Compute Module 4

- CAM1: 4 lanes, `csi1`, `i2c-10`. CAM0: 2 lanes, `csi0`, `i2c-0` [C-02], [C-08], [C-25].
- On the CM4 IO Board, CAM0 needs both J6 jumpers [C-03]. On a custom carrier, Raspberry Pi's camera drivers assume CAM1 uses `i2c-10` and CAM0 uses `i2c-0` [C-25]; the carrier's actual wiring is UNKNOWN (OQ-018).
- Capability: 1080p60 on CAM1 with 4 lanes [C-37], [C-49]. The 4-lane port is necessary but not shown sufficient for 1080p60 UYVY: at the default 972 Mbit/s the driver activates 3 of the 4 lanes (reasoning) [C-47], which is unproven (OQ-038). ADR-008 (PROPOSED) proposes evaluating `link-frequency=297000000` for this case in TEST-CAP-002 (§5.7; OQ-099). CAM0 has the same 2-lane limits as Pi 4B [C-37], [C-48].
- Under REQ-CAP-007, CAM1 is a candidate for the 4-lane configuration and CAM0 for the 2-lane configuration (§5.8 table). This is a mapping, not a platform choice (ADR-004, OPEN).
- `4lane` is documented for the Compute Module CAM1 connector [G-12]. **Never use `4lane` with `cam0`** (CAM0 is 2-lane [C-02]; §5.8 warning).
- **Bring-up platform** (owner, 2026-10-07: CM4 and CM5 side by side; ADR-004 OPEN).
- **HDMI audio** *(added 2026-10-08; required, REQ-CAP-006)*. `tc358743-audio` enables the single `bcm2835-i2s` node through `i2s_clk_consumer`, which is the same node `dtparam=i2s=on` controls, with pins GPIO 18–21 in ALT0 [I-10]. Reasoning from [I-10]: a separate `dtparam=i2s=on` is therefore not needed for this path. Capture is exactly 2 channels at 8–384 kHz, S16_LE, S24_LE or S32_LE [I-13]; the PCM is `bcm2835-i2s-dir-hifi dir-hifi-0` on card `tc358743` [I-15]. The CAM0 and CAM1 snippets both carry the line, because the I2S path does not depend on the camera connector (reasoning from [I-10]). On the CM4 IO Board the GPIO voltage is selectable, 1.8 V or 3.3 V, and VDDIO2 should match it [I-29] (OQ-024).

```ini
# /boot/firmware/config.txt — CM4, bridge on CAM1 (4 lanes)
# PROPOSED — NOT YET RUN ON PACSCORDER HARDWARE
# OQ-072 [C-39]:
camera_auto_detect=0
# 4 lanes, CM CAM1 only [G-12], and only if the bridge board routes 4 lanes (OQ-021).
# Bare boolean form: NEEDS VERIFICATION (§6.0; OQ-100)
dtoverlay=tc358743,4lane
# For 1080p60 UYVY see §5.7: 3 of 4 lanes at the default link frequency (OQ-038; ADR-008).
# ADR-006 (PROPOSED) adds Media Controller mode. Combined-parameter syntax NEEDS VERIFICATION (OQ-100):
#dtoverlay=tc358743,4lane,media-controller
# Added 2026-10-08. HDMI audio, required (REQ-CAP-006): in addition to tc358743 [C-37], [G-14].
# CM4: bcm2835-i2s on GPIO 18-21 [I-10]. Claims GPIO 18-21 (OQ-114). Test: TEST-AUD-001.
dtoverlay=tc358743-audio
```

```ini
# /boot/firmware/config.txt — CM4, bridge on CAM0 (2 lanes)
# PROPOSED — NOT YET RUN ON PACSCORDER HARDWARE
# OQ-072 [C-39]:
camera_auto_detect=0
# Connector 0 [C-39], [B-42]. Never add 4lane here.
dtoverlay=tc358743,cam0
# ADR-006 (PROPOSED). Combined-parameter syntax NEEDS VERIFICATION (OQ-100):
#dtoverlay=tc358743,cam0,media-controller
# Added 2026-10-08. HDMI audio, required (REQ-CAP-006): in addition to tc358743 [C-37], [G-14].
# CM4: bcm2835-i2s on GPIO 18-21 [I-10]. Claims GPIO 18-21 (OQ-114). Test: TEST-AUD-001.
dtoverlay=tc358743-audio
```

### 6.3 Raspberry Pi 5

- Two 4-lane ports, 1.5 Gbps per lane [C-04]. Receiver: RP1 CFE, Media Controller only [C-29], [C-11].
- Under REQ-CAP-007, Pi 5 is a candidate for the 4-lane configuration; it serves the 2-lane configuration only with a bridge board that routes 2 lanes, using the default `<1 2>` (reasoning; OQ-021). This is a mapping, not a platform choice (ADR-004, OPEN).
- The default overlay target is connector 1 [C-39]: CAM/DISP1, I2C `i2c_csi_dsi1` (`/dev/i2c-11`), CSI `rp1_csi1` [B-44], [C-26]. `cam0` selects CAM/DISP0: `i2c_csi_dsi0` (`/dev/i2c-10`) and `csi0` [B-42], [C-26], [C-29].
- `dtoverlay=tc358743` is redirected to `tc358743-pi5` [C-11]. Naming `tc358743-pi5` directly is also attested [G-13]. **PROPOSED:** name `tc358743-pi5` explicitly, so that the file shows which overlay is applied.
- **Do not add `media-controller`.** `tc358743-pi5` has no such parameter [G-13]. What the loader does with an unknown parameter is NEEDS VERIFICATION (OQ-100).
- **`4lane` on Pi 5.** The README text that calls it CM-CAM1-only is stale [C-13]. Whether capture through it succeeds end to end on the Pi 5 connectors is HARDWARE TEST REQUIRED (OQ-049).
- **`4lane` with `cam0`.** `4lane` is documented to set `data-lanes` on `csi1_ep` [B-42]. Whether the `cam0` + `4lane` combination also updates the `csi0` endpoint is NEEDS VERIFICATION — KERNEL SOURCE INSPECTION REQUIRED (OQ-049). A 4-lane configuration on CAM/DISP0 is therefore not settled; the CAM/DISP0 snippet below keeps it as a commented alternative.
- Keep `link-frequency` at its default (§5.7; ADR-008, PROPOSED; OQ-099; RISK-011, OQ-050).
- **HDMI audio** *(added 2026-10-08; required, REQ-CAP-006)*. `tc358743-audio` is not in `overlay_map` and loads under its own name; `bcm2712` covers Pi 5 [I-05], [I-06]. The labels it needs are defined in `bcm2712-rpi.dtsi` [I-07]; whether the Pi 5 board Device Tree includes that file is not in the register: KERNEL SOURCE INSPECTION REQUIRED (OQ-054). Pi 5 is not a bring-up platform (owner, 2026-10-07: CM4 and CM5), so no audio line is written into the Pi 5 snippets; §6.0 applies if Pi 5 is used.
- There is no official Pi 5 TC358743 documentation [C-38]. A Raspberry Pi engineer reported a capture sequence for Pi 5 on kernel 6.18.39 [C-33] (community report; 6.18.39 is the reporter's kernel, not the 6.18.50 that Raspberry Pi OS 2026-10-06 ships [G-04]). It is a userspace procedure; see [V4L2.md](V4L2.md) and [TESTING.md](TESTING.md).

```ini
# /boot/firmware/config.txt — Pi 5, bridge on CAM/DISP1 (default connector)
# PROPOSED — NOT YET RUN ON PACSCORDER HARDWARE
# OQ-072 [C-39]:
camera_auto_detect=0
# [G-13]. Bare boolean form NEEDS VERIFICATION (OQ-100). 4lane on Pi 5: OQ-049.
# Use 4lane only if the bridge board routes 4 lanes (OQ-021); otherwise omit it.
dtoverlay=tc358743-pi5,4lane
# Equivalent through the overlay map [C-11]:
#dtoverlay=tc358743,4lane
```

```ini
# /boot/firmware/config.txt — Pi 5, bridge on CAM/DISP0
# PROPOSED — NOT YET RUN ON PACSCORDER HARDWARE
# OQ-072 [C-39]:
camera_auto_detect=0
# Connector 0 [C-39], [G-13]; 2 data lanes, the tc358743.dtsi default [C-12].
dtoverlay=tc358743-pi5,cam0
# 4 lanes on CAM/DISP0 are NOT settled: whether 4lane with cam0 sets 4 lanes on the
# csi0 endpoint is NEEDS VERIFICATION, KERNEL SOURCE INSPECTION REQUIRED (OQ-049).
# Combined syntax NEEDS VERIFICATION (OQ-100). Only if the bridge board routes 4 lanes (OQ-021):
#dtoverlay=tc358743-pi5,cam0,4lane
```

### 6.4 Compute Module 5

CM5 is a BCM2712 device, so it uses the same `tc358743-pi5` overlay as Pi 5 [C-11], [E-43]. Its two MIPI interfaces are 4-lane [C-05]. Under REQ-CAP-007 it maps to the configurations as Pi 5 does (§5.8 table). The camera power-enable is RP1 GPIO 34, shared by both interfaces [C-22]. **The correct configuration depends on the carrier board**, which is UNKNOWN — VERIFICATION REQUIRED (OQ-018, OQ-052). CM5 is a bring-up platform, evaluated side by side with CM4 (owner, 2026-10-07; ADR-004 OPEN).

**HDMI audio on CM5** *(added 2026-10-08, research topic I; audio required, REQ-CAP-006)*

- `dtoverlay=tc358743` loads `tc358743-pi5` on CM5, while `tc358743-audio` has no `overlay_map` entry and loads under its own name; the firmware does not block it [I-05], [I-06].
- Its labels resolve: `bcm2712-rpi-cm5.dtsi` includes `bcm2712-rpi.dtsi`, where `i2s_clk_consumer` is `rp1_i2s1` (pin group `rp1_i2s1_18_21`, GPIO 18–21, function `i2s1`) and the `sound` node exists [I-07], [I-08]. RP1 I2S1 is the clock-consumer instance that a codec-master link needs [I-09]; the overlay's 2 × 32-bit slots pass the `dwc-i2s` TDM check [I-14].
- The kernel options the path needs are enabled for CM5: `bcm2712_defconfig` sets `CONFIG_SND_SIMPLE_CARD=m` and `CONFIG_SND_DESIGNWARE_I2S=m`, and `CONFIG_SND_SOC_SPDIF`, selected by `CONFIG_SND_RP1_AUDIO_OUT=m`, is `=m` in the packaged `rpi-2712` kernel [I-12] (§7).
- The ALSA PCM name will differ from CM4 (reasoning from source; expected `1f000a4000.i2s-dir-hifi dir-hifi-0`, not verified); select the card by id `tc358743` [I-15], [I-17].
- On CM5 the base Device Tree's power-button and fan entries do not use header GPIO 18–21 [I-32]. The CM5 IO Board's GPIO voltage is selectable, 1.8 V or 3.3 V, and VDDIO2 should match it [I-29] (OQ-024).
- **Unconfirmed.** No official statement or test result shows audio captured through this path (research gap, topic I). The capture channel count and formats of RP1 I2S1 come from hardware registers not visible in source [I-14]. HARDWARE TEST REQUIRED — OQ-054, TEST-AUD-001; reasoning in ADR-004: a bring-up gate for CM5.

**CM5 on the CM5 IO Board**

- CAM/DISP 1 needs two J6 jumpers to route I2C, and a camera on it cannot be powered down. CAM/DISP 0 has a camera power-down signal [C-06].
- I2C: CAM/DISP 1 is RP1 i2c0 on GPIO 0/1 (symlink `i2c-11`); CAM/DISP 0 is RP1 i2c6 on GPIO 38/39 [C-27].
- Which connector the default overlay targets, and whether the jumpers are needed, is open: the research notes report a DT comment that contradicts the official jumper statement (research gap, topic C). HARDWARE TEST REQUIRED — OQ-052.

```ini
# /boot/firmware/config.txt — CM5 on CM5 IO Board
# PROPOSED — NOT YET RUN ON PACSCORDER HARDWARE
# OQ-072 [C-39]:
camera_auto_detect=0
# Target connector on the CM5 IO Board NEEDS VERIFICATION (OQ-052). J6 jumpers [C-06].
# Use 4lane only if the bridge board routes 4 lanes (OQ-021); otherwise omit it.
dtoverlay=tc358743-pi5,4lane
# CAM/DISP 0 instead. Combined syntax NEEDS VERIFICATION (OQ-100).
# csi0 endpoint lanes: KERNEL SOURCE INSPECTION REQUIRED (OQ-049).
#dtoverlay=tc358743-pi5,cam0,4lane
# Added 2026-10-08. HDMI audio, required (REQ-CAP-006) [G-14]; loads under its own name [I-05], [I-06].
# CM5: RP1 I2S1 on GPIO 18-21 [I-07], [I-08]. Capture UNCONFIRMED (OQ-054): bring-up gate, TEST-AUD-001.
# Claims GPIO 18-21 (OQ-114).
dtoverlay=tc358743-audio
```

**CM5 on the CM4 IO Board**

- CM5 MIPI0 uses the CM4 CAM1 pins [C-05]; reasoning from that fact: the CM4 IO Board CAM1 connector reaches CM5 MIPI0 (`rp1_csi0`). Its I2C bus is `i2c_csi_dsi1` = i2c6, shared with DISP1, the RTC and the fan [C-27], [B-44].
- The stock overlay pairs `i2c_csi_dsi` with `csi1` by default, and `i2c_csi_dsi0` with `csi0` under `cam0` [B-41], [B-42]. The research found that neither pairing matches the CM4 IO Board CAM1 connector on CM5 (research open question, topic C; not a register fact).
- **No `config.txt` line is proposed.** A custom overlay, or test evidence that a stock combination probes on the right bus and captures, is required. UNKNOWN — VERIFICATION REQUIRED; KERNEL SOURCE INSPECTION REQUIRED; HARDWARE TEST REQUIRED (OQ-052).
- The CM4 IO Board CAM0 connector cannot be used with CM5: those pins carry USB 3.0 [C-05].

**CM5 on a custom carrier:** UNKNOWN — VERIFICATION REQUIRED (OQ-018).

### 6.5 Per-platform summary

| | Pi 4 Model B | CM4 | Pi 5 | CM5 |
|---|---|---|---|---|
| Overlay applied | `tc358743` [G-12] | `tc358743` [G-12] | `tc358743-pi5` [C-11], [G-13] | `tc358743-pi5` [C-11], [E-43] |
| Receiver compatible | `brcm,bcm2835-unicam-legacy` by default; Media Controller compatible with `media-controller` [B-43] | as Pi 4B [B-43] | `raspberrypi,rp1-cfe` [B-44] | `raspberrypi,rp1-cfe` [C-29] |
| Control model | Legacy by default; Media Controller with `media-controller` (ADR-006 PROPOSED) [C-10] | as Pi 4B | Media Controller only [C-11] | Media Controller only [C-11] |
| Lane configuration it can serve (REQ-CAP-007; §5.8) | 2-lane only [C-01] | CAM0: 2-lane only; CAM1: 4-lane [C-02] | 4-lane; 2-lane with a 2-lane bridge board (reasoning; OQ-021) | as Pi 5; carrier-dependent (OQ-052) |
| `4lane` allowed | **No** (2-lane) [C-01], [C-17] | CAM1 only [C-02], [G-12] | By lanes, yes [C-04]; HARDWARE TEST REQUIRED (OQ-049). With `cam0` (CAM/DISP0), whether it reaches `csi0`: NEEDS VERIFICATION (OQ-049) | By lanes, yes [C-05]; carrier-dependent (OQ-052) |
| `cam0` | Not applicable (reasoning) [C-07] | CAM0, 2 lanes [C-02] | CAM/DISP0 [C-26] | Carrier-dependent (OQ-052) |
| `media-controller` | Optional (ADR-006 PROPOSED) | Optional (ADR-006 PROPOSED) | Not a parameter [G-13] | Not a parameter [G-13] |
| `link-frequency` | Default 486000000 (PROPOSED keep: ADR-008, OQ-099) | as Pi 4B; ADR-008 proposes evaluating 297000000 on CAM1 4-lane only, for 1080p60 UYVY in TEST-CAP-002 | Keep default (ADR-008); CFE uses 999 Mbps [C-31], [C-52] | as Pi 5 |
| Default I2C bus | `i2c-10` [C-24] | CAM1 `i2c-10`, CAM0 `i2c-0` [C-25] | CAM/DISP1 `i2c-11` [C-26] | Carrier-dependent [C-27] |
| Audio overlay `tc358743-audio` (added 2026-10-08; audio required, REQ-CAP-006) | PROPOSED line (§6.0) [G-14]; label resolution not covered by topic I: KERNEL SOURCE INSPECTION REQUIRED | PROPOSED line (§6.2); `bcm2835-i2s`, GPIO 18–21 ALT0 [I-10] | PROPOSED line (§6.0); labels in `bcm2712-rpi.dtsi` [I-07], Pi 5 inclusion KERNEL SOURCE INSPECTION REQUIRED (OQ-054) | PROPOSED line (§6.4); RP1 I2S1, GPIO 18–21 [I-07], [I-08]; capture unconfirmed (OQ-054) |
| Bring-up role (owner, 2026-10-07; ADR-004 OPEN) | Documented candidate | Evaluated side by side with CM5 | Documented candidate | Evaluated side by side with CM4 |

The `4lane` row states what the Pi-side connector allows. On every platform, `4lane` also requires that the PACSCORDER bridge board routes 4 data lanes, which is UNKNOWN — VERIFICATION REQUIRED (OQ-021).

---

## 7. Kernel-side prerequisites (summary)

- `CONFIG_VIDEO_TC358743=m` in both Raspberry Pi arm64 defconfigs; the 2026-10-06 image ships `tc358743.ko.xz` for both kernels [B-20], [G-16]. Both Unicam drivers and both RP1 CFE drivers are modules [G-17]. The I2C controller and pinctrl-mux drivers are modules [E-39].
- Details, including Buildroot module autoloading [E-50] and overlay installation [E-14], [E-15], are in [BUILD_SYSTEM.md](BUILD_SYSTEM.md) (OQ-065).
- *(Added 2026-10-08, research topic I.)* HDMI audio: both arm64 defconfigs (`bcm2711_defconfig` for CM4, `bcm2712_defconfig` for CM5) set `CONFIG_SND_SIMPLE_CARD=m`, `CONFIG_SND_BCM2835_SOC_I2S=m`, `CONFIG_SND_DESIGNWARE_I2S=m` and `CONFIG_SND_DESIGNWARE_PCM=y`. `CONFIG_SND_SOC_SPDIF`, which provides the `linux,spdif-dir` stub codec, is not set directly; it is selected by `CONFIG_SND_RP1_AUDIO_OUT=m`, and the packaged `rpi-v8` and `rpi-2712` 6.18.50 kernels both contain `CONFIG_SND_SOC_SPDIF=m` [I-12], [I-11]. `CONFIG_VIDEO_TC358743_CEC` is not set in either defconfig or packaged kernel [I-22]. Whether a project-built image (REQ-BLD-002; ADR-003 ACCEPTED, `rpi-image-gen`) keeps these options: BUILD TEST REQUIRED.

## 8. Diagnostic messages tied to Device Tree mistakes

These messages come from the cited driver source. **None has been observed on PACSCORDER hardware.** The procedures that would show them are TEST-HW-001, TEST-DRV-001 and TEST-PLT-001 in [TESTING.md](TESTING.md), all NOT YET RUN ON PACSCORDER HARDWARE.

| Message (from source) | Emitted by | Likely Device Tree cause | Facts |
|---|---|---|---|
| `failed to get refclk` | tc358743 | `clocks` / `clock-names` missing or wrong | [A-22] |
| `unsupported refclk rate: %u Hz`, followed by a kernel BUG if the chip answers | tc358743 | `clock-frequency` not 26, 27 or 42 MHz | [A-22], [B-11] |
| `missing endpoint node`, `missing CSI-2 properties in endpoint`, `invalid number of lanes` | tc358743 | Port 0 endpoint absent or incomplete | [B-06] |
| `unsupported bps per lane` | tc358743 | `link-frequencies` outside 31.25–500 MHz (reasoning: half of 62.5 Mbps–1 Gbps) | [B-08] |
| `untested bps per lane: %u bps` | tc358743 | `link-frequencies` other than 297 or 486 MHz | [B-09], [A-24] |
| `not a TC358743 on address 0x%x` (8-bit form: `0x1e` for 0x0f) | tc358743 | Wrong `reg` or bus; chip unpowered or held in reset | [A-16], [A-19] |
| `subdevice requires %u data lanes when %u are supported` | Unicam | `data-lanes` (for example `4lane`) exceeds the connector | [C-17] |
| `Device has requested %u data lanes, which is >%u configured in DT` | Unicam or RP1 CFE, at stream start | The input mode needs more lanes than `data-lanes` | [B-32], [C-16] |
| `Unable to determine sensor link rate, using 999 Mbps` | RP1 CFE | Logged at error level (`cfe_err`), but expected for this bridge: not a Device Tree mistake | [C-31], [B-49] |

---

## 9. Device Tree change log

Every Device Tree or overlay change is recorded here in the Rule 6 format, newest last. Each entry also names its files, platforms, dependencies and test (Rule 1). Entries are never rewritten (Rule 21).

### 2026-10-06 — Baseline declared: stock overlay, no change

| | |
|---|---|
| Files | None in this repository. The baseline is `raspberrypi/linux` `rpi-6.18.y`: `arch/arm/boot/dts/overlays/tc358743.dtsi`, `tc358743-overlay.dts`, `tc358743-pi5-overlay.dts`, `overlay_map.dts`. (Path note added by the 2026-10-06 cross-document review: the `arch/arm/boot/dts/overlays/` directory of each file is attested by the source URLs of [B-41] and [A-45] (`tc358743.dtsi`), [B-43], [C-10] and [E-42] (`tc358743-overlay.dts`), [B-44] (`tc358743-pi5-overlay.dts`) and [E-43] (`overlay_map.dts`).) |
| Platforms | Pi 4 Model B, CM4, Pi 5, CM5 |
| Dependencies | OQ-011, OQ-018, OQ-019, OQ-020, OQ-021, OQ-022, OQ-026 |
| Test | TEST-PLT-001 — `BLOCKED — HARDWARE REQUIRED` |

```text
OLD
No PACSCORDER Device Tree source or overlay exists.
↓
CHANGE
None. No Device Tree change has been made. The baseline is declared as the
stock Raspberry Pi overlays from raspberrypi/linux rpi-6.18.y, unmodified:
tc358743 on Pi 4 Model B and CM4; tc358743 redirected to tc358743-pi5 on
Pi 5 and CM5.
↓
REASON
No PACSCORDER hardware exists (owner, 2026-10-06). No hardware value that
would justify a change is known: REFCLK frequency, I2C address, RESETN and
INT wiring, routed lanes and CAM_GPIO use are all UNKNOWN. ADR-002 and
ADR-003 (both PROPOSED) propose bring-up with stock kernel components.
↓
EXPECTED RESULT
From sources only, not from hardware. On a board that matches the stock
assumptions (27 MHz REFCLK [A-45], I2C address 0x0f [A-15], data lanes
matching the overlay setting [B-41], [B-42], no INT or RESETN connection
required), the tc358743 driver probes and registers its sub-device [B-06],
[B-15]. TEST-PLT-001 records the result.
↓
ACTUAL RESULT
NOT YET LOADED ON PACSCORDER HARDWARE. BLOCKED — HARDWARE REQUIRED.
```

### 2026-10-08 — Baseline extended: stock `tc358743-audio` overlay added to the proposed configuration

| | |
|---|---|
| Files | None in this repository. Stock overlay `arch/arm/boot/dts/overlays/tc358743-audio-overlay.dts` in `raspberrypi/linux` `rpi-6.18.y` (path as in the source URL of [I-01]). PROPOSED `config.txt` line `dtoverlay=tc358743-audio` (§6.0, §6.2, §6.4). |
| Platforms | CM4 and CM5 (bring-up platforms, written out in §6.2 and §6.4); Pi 4 Model B and Pi 5 by §6.0 |
| Dependencies | REQ-CAP-006 (DRAFT; OQ-004 ANSWERED 2026-10-07), OQ-025, OQ-024, OQ-054, OQ-114 |
| Test | TEST-AUD-001 — `BLOCKED — HARDWARE REQUIRED` |

```text
OLD
Proposed configuration: tc358743 (Pi 4 Model B, CM4) or tc358743-pi5
(Pi 5, CM5) only. tc358743-audio "only if audio is required" (OQ-004).
↓
CHANGE
No Device Tree source change. The stock tc358743-audio overlay, unmodified,
is added to the proposed config.txt of every platform, after the
tc358743 / tc358743-pi5 line.
↓
REASON
The owner made HDMI audio required on 2026-10-07 (REQ-CAP-006, OQ-004
ANSWERED). Official documentation says audio needs tc358743-audio in
addition to tc358743 [C-37]. No PACSCORDER hardware value is known that
would justify a modified overlay; a custom pin group is needed only if
GPIO 21 must be freed (OQ-114).
↓
EXPECTED RESULT
From sources only, not from hardware. An ALSA card with id "tc358743"
appears [I-03], [I-15]. On CM4 the CPU side is bcm2835-i2s on GPIO 18-21,
2-channel capture [I-10], [I-13]. On CM5 the labels resolve to RP1 I2S1 on
GPIO 18-21 [I-07], [I-08], but no source shows audio captured that way (OQ-054).
TEST-AUD-001 records the result.
↓
ACTUAL RESULT
NOT YET LOADED ON PACSCORDER HARDWARE. BLOCKED — HARDWARE REQUIRED.
```

---

## Verification status

### Verified from sources (fact IDs)

Every source statement in this document cites an entry of [REFERENCES.md](REFERENCES.md) whose verdict is `CONFIRMED` or `CORRECTED`. This document cites these entries:

| Topic | Fact IDs |
|---|---|
| A — TC358743 hardware | A-14, A-15, A-16, A-19, A-20, A-22, A-23, A-24, A-25, A-27, A-28, A-29, A-30, A-35, A-44, A-45, A-46, A-47, A-48, A-50 |
| B — tc358743 Linux driver and binding | B-01, B-02, B-03, B-04, B-05, B-06, B-07, B-08, B-09, B-10, B-11, B-13, B-15, B-19, B-20, B-21, B-24, B-25, B-31, B-32, B-33, B-37, B-40, B-41, B-42, B-43, B-44, B-45, B-46, B-47, B-48, B-49 |
| C — Raspberry Pi CSI-2 receive path | C-01, C-02, C-03, C-04, C-05, C-06, C-07, C-08, C-09, C-10, C-11, C-12, C-13, C-15, C-16, C-17, C-20, C-21, C-22, C-23, C-24, C-25, C-26, C-27, C-28, C-29, C-31, C-32, C-33, C-36, C-37, C-38, C-39, C-40, C-42, C-44, C-47, C-48, C-49, C-51, C-52, C-53 |
| E — Buildroot and kernel configuration | E-14, E-15, E-16, E-17, E-37, E-39, E-40, E-42, E-43, E-44, E-45, E-47, E-50, E-53 |
| G — Raspberry Pi OS and image tooling | G-04, G-06, G-11, G-12, G-13, G-14, G-15, G-16, G-17, G-21, G-61, G-71 |
| I — HDMI audio path (added 2026-10-08) | I-01, I-02, I-03, I-04, I-05, I-06, I-07, I-08, I-09, I-10, I-11, I-12, I-13, I-14, I-15, I-16, I-17, I-18, I-22, I-25, I-28, I-29, I-30, I-31, I-32 |

- `CORRECTED` entries, used in their corrected wording only: A-22, A-25, B-11, B-21, B-25, B-44, C-28, C-36, C-39, C-53, E-40, E-47, G-11, G-71.
- `community` entries, worded as reports: C-28, C-33, C-42; added 2026-10-08: I-16.
- `reasoning` entries, labelled as reasoning: A-23, B-10, B-11, B-33, B-47, B-49, C-47, C-48, C-49, C-51, C-52, C-53, G-71; added 2026-10-08: I-17, I-18, I-28.
- Statements marked *research gap* or *research open question* come from [research/2026-10-06-source-research.json](research/2026-10-06-source-research.json). They are not register facts and are recorded only to state what is unknown. Those for topic I (added 2026-10-08) come from [research/2026-10-08-hevc-audio-research.json](research/2026-10-08-hevc-audio-research.json), with the same status.
- The `config.txt` lines in §6 combine attested syntax ([G-12], [G-13], [C-39]) with forms marked NEEDS VERIFICATION (OQ-100). None has been run. The `dtoverlay=tc358743-audio` line added on 2026-10-08 uses the attested overlay name and no parameter [G-14].

### Verified on PACSCORDER hardware

Nothing (no hardware exists as of 2026-10-08). No overlay has been loaded and no `config.txt` line has been tested. TEST-PLT-001, TEST-HW-001, TEST-DRV-001 and TEST-AUD-001 are `BLOCKED — HARDWARE REQUIRED`.

## Change history

| Date | Change | By |
|---|---|---|
| 2026-10-06 | Created from source research of 2026-10-06 | Claude (session 2026-10-06) |
| 2026-10-06 | Review against REFERENCES.md: corrected the REFCLK-mismatch consequence (kernel BUG only for DT values other than 26/27/42 MHz); attributed "only 297000000/486000000 supported" to the README and added the driver timing-table fact [B-09]; limited the E-17 sample-file statement to Pi 4/Pi 5; showed the CMA buffer calculation with inputs [C-53]; marked the 4lane snippets as conditional on 4 routed lanes (OQ-021); labelled reasoning and PROPOSED statements; corrected the change-log section reference (§9); removed a success word. No Device Tree change made; the change log is unchanged. | Claude (session 2026-10-06) |
| 2026-10-06 | Cross-document consistency fixes: link-frequency recommendation linked to ADR-008 (PROPOSED) and OQ-099 in §1, §5.7, §6.3 and §6.5, including ADR-008's proposal to evaluate 297000000 only on CM4 CAM1 4-lane for 1080p60 UYVY [C-44]; 4-lane port marked necessary but not shown sufficient for 1080p60 UYVY (3 of 4 lanes at 972 Mbit/s, OQ-038) in §5.8 and §6.2; `config.txt` syntax caveats (bare boolean, combined parameters, `[pi4]` matching CM4, unknown parameters) linked to OQ-100 in §1, §6.0, §6.3 and the snippets; Pi 5 CAM/DISP0 snippet now keeps `,cam0` as the active line and the `cam0` + `4lane` form as a commented alternative marked NEEDS VERIFICATION (OQ-049), so no CAM/DISP0 4-lane configuration is presented as settled; shipped kernel 6.18.50 also cited to [G-04] and 6.18.39 identified as the kernel of the [C-33] report; overlay-source-versus-tip question cross-referenced to OQ-097; §9 Files row: path note added citing the register source URLs that attest `arch/arm/boot/dts/overlays/` for all four files (the hardware-dt review's "only the filename is attested" concern does not hold, so no NEEDS VERIFICATION marker was needed; the original entry text is unchanged). Added citations C-44, G-04. No Device Tree change made; no status changed. Final verification pass (same date): §6.0 trailing-comment syntax now linked to OQ-100, whose scope note names it; §4 placeholder notes link the Pi 4/CM4 GPIO controller label to OQ-020 and the expander-GPIO reset question to OQ-022. | Claude (session 2026-10-06) |
| 2026-10-07 | Owner decisions of 2026-10-07 propagated: both 2-lane and 4-lane configurations required (REQ-CAP-007; OQ-001 ANSWERED) in the header, a new "Owner decisions of 2026-10-07" paragraph, §1 (CSI lanes and per-platform rows), §5.8, §6.0–§6.4 and §6.5; §5.8 now maps each candidate connector to its lane configuration and DT lane setting, adds the 720p60 lane counts [B-33], the per-configuration supported-mode limits [C-37], [C-48], [C-49], and the per-configuration EDID note (OQ-002, reasoning [B-21], [B-24]); Pi 4 Model B recorded as a 2-lane candidate only (§6.1); one-board-for-both question linked to OQ-021; §4.2 notes that a PACSCORDER overlay would be installed by the project-built OS image (REQ-BLD-002; ADR-003 still PROPOSED). Added citations B-21, B-24, B-33. No platform chosen; no ADR status changed; no Device Tree change made, so the §9 change log is unchanged. | Claude (session 2026-10-07) |
| 2026-10-07 | ADR-003 ACCEPTED by the owner propagated (status wording); §4.2 decision rule: bring-up approach of ADR-002 (PROPOSED) and ADR-003 (ACCEPTED 2026-10-07), and the PACSCORDER-overlay bullet's build tool ADR-003 ACCEPTED (`rpi-image-gen`). Not changed: the dated §9 change-log entry of 2026-10-06 (it keeps "ADR-002 and ADR-003 (both PROPOSED)", Rule 21), evidence and citations, ADR-002 and ADR-008 (PROPOSED), ADR-004 (OPEN), overlay status (NOT STARTED), Change-history rows. | Claude (session 2026-10-07) |
| 2026-10-08 | Owner decisions of 2026-10-07 (second set) and research topic I (HDMI audio path) propagated. Header and a new "second set" paragraph: CM4 and CM5 evaluated side by side (ADR-004 OPEN), HDMI audio required (REQ-CAP-006, OQ-004 ANSWERED); source-baseline note on the topic I sources read at the branch head (OQ-097 scope note). §1: CM4/CM5 rows note the bring-up role; new "Audio overlay (I2S)" row. §3.4: `tc358743-audio` is not in `overlay_map` and loads under its own name [I-05], [I-06]. §3.5: "use only if audio is required" marked superseded; added the overlay's fragments with quoted excerpts [I-01]–[I-04], the load-with-`tc358743` statements [C-37] and (community) [I-16], clock roles (reasoning [I-25], [I-28]), the stub codec [I-11] and the missing rate path (reasoning [I-18]; RISK-023, OQ-111), ALSA names [I-15] and (reasoning) [I-17], a per-platform label-resolution table (CM4 [I-10], [I-13]; CM5 [I-07], [I-08], [I-09], [I-14]; Pi 4 Model B and Pi 5 marked KERNEL SOURCE INSPECTION REQUIRED), CM5 status (unconfirmed, OQ-054) and GPIO 18–21 claims and conflicts [I-30], [I-31] (OQ-114). §3.6: stale "tc358743-fast" name explained [I-04]. §4.1: gap rows for an audio pin group without GPIO 21 (OQ-114) and for the missing ALSA rate path [I-11], [I-18], [I-22] (OQ-111, OQ-020). §5.4: audio-rate polling latency [I-22]. §6.0: "only if audio is required" marked superseded; PROPOSED `dtoverlay=tc358743-audio` on every platform. §6.2 CM4: audio bullet [I-10], [I-13], [I-15], [I-29] and the audio line in both CM4 snippets. §6.3 Pi 5: audio bullet [I-05], [I-06], [I-07]. §6.4 CM5: new "HDMI audio on CM5" notes [I-05]–[I-09], [I-12], [I-14], [I-15], [I-17], [I-29], [I-32] and the audio line in the CM5 IO Board snippet. §6.5: audio-overlay and bring-up-role rows. §7: audio kernel options [I-11], [I-12], [I-22]. §9: new change-log entry "2026-10-08 — Baseline extended: stock `tc358743-audio` overlay added to the proposed configuration" (no source change; NOT YET LOADED). Verification status: topic I IDs, community I-16, reasoning I-17, I-18, I-28, 2026-10-08 research JSON, TEST-AUD-001. No REQ or ADR status changed; overlay status NOT STARTED unchanged. | Claude (session 2026-10-08) |
