# HDZero Goggle Emulator

A cross-platform desktop build of the goggle UI, for developing and previewing the
LVGL interface without hardware. It runs the **real** application code — the same
`main()`, menu system, OSD, and page logic that ships on the goggle — rendered
through SDL2 instead of the framebuffer, with the hardware peripherals replaced by
compiled-in mocks.

Runs on **Linux, macOS, and Windows**.

<!-- screenshot: docs/img/emulator-menu.png -->

## Design

Two rules keep the emulator honest and keep the firmware untouched:

- **Build in a container, run natively.** Every target — including macOS — is
  compiled inside a devcontainer image (reproducible, no host toolchain sprawl);
  the resulting binary runs natively on its OS. The macOS image cross-compiles
  Mach-O with LLVM/clang; since Apple's SDK isn't redistributable you stage it once
  from your own Xcode (`build.sh prepare-mac`), but the compile still runs in the
  container. (CI is the one place macOS builds natively — on a `macos-latest`
  runner — because CI can't stage that SDK.)
- **Zero cost on the goggle.** All emulator code is behind `EMULATOR_BUILD` or in
  `src/emulator/*`, which CMake only compiles for the emulator. The goggle firmware
  binary contains none of it. Hardware drivers that differ (UART, I2C, RTC, the
  framebuffer) are split into per-target files selected by CMake, so the goggle
  driver stays free of emulator `#ifdef`s.

The peripherals are mocked at their hardware seam, so the app is unchanged:

| Hardware | Emulator stand-in |
|----------|-------------------|
| Display engine / framebuffer | SDL2 window + software compositor |
| DM5680 video receiver | `video_emu` — decodes a looping MPEG-1 clip (pl_mpeg) behind the OSD |
| DM5680 serial link | socketpair-backed UART + a mock that answers version/RSSI |
| I2C bus, RTC | no-op bus, in-memory clock |
| Device shell commands (`system_exec`) | logged and skipped (never touches the host) |

## Build

All builds go through one entry point, [`.devcontainer/build.sh`](../.devcontainer/build.sh):

```bash
.devcontainer/build.sh images     # build the devcontainer images (one-time)
.devcontainer/build.sh linux      # -> build_emu_linux/HDZGOGGLE
.devcontainer/build.sh windows    # -> build_win_x64/HDZGOGGLE.exe   (mingw cross)
.devcontainer/build.sh mac        # -> build_xmac/HDZGOGGLE          (macOS, LLVM cross)
```

macOS needs the SDK staged from your Xcode once: `.devcontainer/build.sh prepare-mac`.

## Run

On the host (macOS/Linux):

```bash
./run-native.sh                   # launches the built binary in an SDL window
```

In the window, **long-press the wheel (SPACE)** to enter the FPV view.

### Fake FPV video feed

Point `HDZ_VIDEO` at an MPEG-1 file and the mock receiver plays it behind the OSD
(defaults to `sdcard/loop.mpg`). Generate one from any clip:

```bash
ffmpeg -i input.mp4 -t 5 -an -c:v mpeg1video -b:v 6M -s 1928x1088 -r 30 sdcard/loop.mpg
```

`ffmpeg` is only used offline to make the clip — no ffmpeg is linked into any app
binary; the emulator decodes with the vendored single-header `pl_mpeg`.

### Resource paths

The app's filesystem roots are configurable so the emulator finds resources on the
host (the goggle keeps its compiled-in `/mnt/app`, `/mnt/extsd` defaults):

- `HDZ_APP_ROOT` — built-in resources (OSD fonts, language)
- `HDZ_EXTSD_ROOT` — the SD card
- `HDZ_CONFIG` — an ini file with `[paths] app_root`/`extsd_root`

`run-native.sh` sets sensible defaults.

## CI & releases

[`.github/workflows/build_emulator.yml`](../.github/workflows/build_emulator.yml)
builds all three targets on every push / PR (with a headless Linux screenshot as a
regression check) — Linux and Windows in the devcontainer images, macOS natively on
a `macos-latest` runner (CI can't stage the Apple SDK the cross image needs).
Pushing a **semver tag** (`v*`) additionally publishes a GitHub Release with the
per-platform binaries attached.
