#!/usr/bin/env bash
# Launch the natively-built HDZero emulator on the HOST (macOS/Linux) in a real
# SDL window. This is the interactive counterpart to .devcontainer/emu.sh, which
# runs headless in the container for CI screenshots.
#
# Build first IN THE CONTAINER (nothing is compiled on the host):
#   .devcontainer/build.sh mac      # -> build_xmac/HDZGOGGLE      (macOS)
#   .devcontainer/build.sh linux    # -> build_emu_linux/HDZGOGGLE (Linux)
# Then:  ./run-native.sh
#
# In the window, long-press the wheel (D) to enter the FPV view; the mock
# video device plays $HDZ_VIDEO (default sdcard/loop.mpg) behind the OSD.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Default to the container-built binary for this OS (override with BUILD_DIR=...).
if [ -z "${BUILD_DIR:-}" ]; then
    case "$(uname -s)" in
        Darwin) BUILD_DIR="$REPO_ROOT/build_xmac" ;;
        *)      BUILD_DIR="$REPO_ROOT/build_emu_linux" ;;
    esac
fi
BIN="$BUILD_DIR/HDZGOGGLE"
LOG_DIR="${LOG_DIR:-$REPO_ROOT/tmp/logs}"
mkdir -p "$LOG_DIR"

[ -x "$BIN" ] || { echo "not built: $BIN — run cmake + make first (see header)"; exit 1; }

# Same configurable roots the goggle build compiles away: point them at the repo's
# built-in resources and the safe, gitignored sdcard copy.
: "${HDZ_SDCARD:=$REPO_ROOT/sdcard}"
export HDZ_APP_ROOT="${HDZ_APP_ROOT:-$REPO_ROOT/mkapp/app}"
export HDZ_EXTSD_ROOT="${HDZ_EXTSD_ROOT:-$HDZ_SDCARD}"
export HDZ_MEDIA_DIR="${HDZ_MEDIA_DIR:-$HDZ_SDCARD/DCIM/100HDZRO}"
export HDZ_VIDEO="${HDZ_VIDEO:-$HDZ_SDCARD/loop.mpg}"

echo ">> launching $BIN"
echo "   video feed : $HDZ_VIDEO"
echo "   FPV view   : long-press D (wheel) once the menu is up"
echo "   log        : $LOG_DIR/native-run.log"
( cd "$BUILD_DIR" && "./$(basename "$BIN")" ) 2>&1 | tee "$LOG_DIR/native-run.log"
