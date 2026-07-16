#!/usr/bin/env bash
# HDZero SDL emulator runner. Works both inside the VS Code devcontainer and under
# plain `docker run` — the repo root is derived from this script's own location, so
# it does not matter where the workspace is mounted (/workspaces/... or /src).
#
#   emu.sh build                         configure + compile the emulator (build_emu_linux)
#   emu.sh shot [name] [warmup] [keys]   headless run -> screenshot emu-out/<name>.png
#   emu.sh all  [name] [warmup] [keys]   build, then shot
#
# The container's job is BUILD + HEADLESS SCREENSHOTS (great for CI / docs / PR
# previews). Interactive use belongs to the native app: ../run-native.sh (macOS/Linux).
#
# keys: space-separated xdotool keys ("Up Up space"); prefix a token with "hold:"
# for a long press (e.g. "hold:space" = wheel long-press, which enters the FPV view).
set -euo pipefail

REPO_ROOT="${REPO_ROOT:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}"
BUILD_DIR="$REPO_ROOT/build_emu_linux"
OUT_DIR="${OUT_DIR:-$REPO_ROOT/emu-out}"
# All run-time logs go under the gitignored tmp/ tree (repo .gitignore: /tmp),
# so they never clutter the repo root or emu-out.
LOG_DIR="${LOG_DIR:-$REPO_ROOT/tmp/logs}"
DISPLAY_NUM=:99
SCREEN=1920x1080x24
mkdir -p "$OUT_DIR" "$LOG_DIR"

do_build() {
    cmake -S "$REPO_ROOT" -B "$BUILD_DIR" \
        -DEMULATOR_BUILD=ON -DCMAKE_BUILD_TYPE=Debug \
        -DHDZ_GOGGLE=ON -DHDZ_BOXPRO=OFF -DHDZ_GOGGLE2=OFF
    make -C "$BUILD_DIR" -j"$(nproc)"
}

# Stage the device runtime paths so the app finds its data where it expects on real
# hardware:
#   /mnt/app   -> built-in resources (OSD fonts, language) symlinked from the repo.
#                 Without this, load_fc_osd_font() can't find BTFL_*.bmp and the OSD
#                 (FC glyphs + every bmp element) renders blank.
#   /mnt/extsd -> the SD card: a SAFE, gitignored copy at $HDZ_SDCARD (default
#                 repo/sdcard). The physical card is never exposed to the emulator.
# HDZ_MEDIA_DIR drives the playback page; HDZ_VIDEO (optional) is the fake FPV feed.
stage_runtime() {
    : "${HDZ_SDCARD:=$REPO_ROOT/sdcard}"
    # Point the app's configurable roots (paths_init) at the repo's built-in
    # resources and the safe sandbox copy. No /mnt symlinks; the goggle build keeps
    # its compiled-in /mnt/app + /mnt/extsd defaults.
    export HDZ_APP_ROOT="${HDZ_APP_ROOT:-$REPO_ROOT/mkapp/app}"
    export HDZ_EXTSD_ROOT="${HDZ_EXTSD_ROOT:-$HDZ_SDCARD}"
    export HDZ_MEDIA_DIR="${HDZ_MEDIA_DIR:-$HDZ_SDCARD/DCIM/100HDZRO}"
    export HDZ_VIDEO="${HDZ_VIDEO:-$HDZ_SDCARD/loop.mpg}"
}

do_shot() {
    local name="${1:-frame}" warmup="${2:-4}" keys="${3:-}"
    export DISPLAY="$DISPLAY_NUM"
    stage_runtime
    Xvfb "$DISPLAY_NUM" -screen 0 "$SCREEN" >"$LOG_DIR/xvfb.log" 2>&1 &
    local xvfb_pid=$!
    sleep 1
    ( cd "$BUILD_DIR" && ./HDZGOGGLE ) >"$LOG_DIR/app.log" 2>&1 &
    local app_pid=$!
    sleep "$warmup"
    if [ -n "$keys" ]; then
        for k in $keys; do
            case "$k" in
                hold:*) kk="${k#hold:}"; xdotool keydown "$kk"; sleep 0.8; xdotool keyup "$kk" ;;
                *)      xdotool key "$k" ;;
            esac
            sleep 0.4
        done
        sleep 1
    fi
    import -window root -display "$DISPLAY_NUM" "$OUT_DIR/${name}.png" 2>"$LOG_DIR/import.log" \
        || { xwd -root -display "$DISPLAY_NUM" | convert xwd:- "$OUT_DIR/${name}.png"; }
    kill "$app_pid" 2>/dev/null || true
    kill "$xvfb_pid" 2>/dev/null || true
    echo "wrote $OUT_DIR/${name}.png"
    echo "--- app.log (tail) ---"; tail -n 15 "$LOG_DIR/app.log" || true
}

cmd="${1:-shot}"
case "$cmd" in
    build) do_build ;;
    shot)  shift || true; do_shot "$@" ;;
    all)   do_build; shift || true; do_shot "$@" ;;
    *) echo "usage: emu.sh {build|shot|all} [name] [warmup] [keys]"; exit 2 ;;
esac
