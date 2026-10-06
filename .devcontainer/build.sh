#!/usr/bin/env bash
# One reproducible entry point for every HDZero emulator build target.
# EVERY target is compiled INSIDE a devcontainer image; each RUNS natively on its OS.
#
#   build.sh prepare-mac  stage Apple's SDK from your Xcode into .devcontainer (one-time)
#   build.sh images       build all three images (hdzero-dev, hdzero-dev-win, hdzero-dev-mac)
#   build.sh linux        Linux emulator   -> build_emu_linux/HDZGOGGLE     (image: hdzero-dev)
#   build.sh windows      Windows emulator -> build_win_x64/HDZGOGGLE.exe   (image: hdzero-dev-win, mingw)
#   build.sh mac          macOS emulator   -> build_xmac/HDZGOGGLE          (image: hdzero-dev-mac, LLVM cross)
#   build.sh all          images + linux + windows + mac
#
# Nothing is compiled on the host. The ONLY host-side step is prepare-mac, which
# TARs Apple's (non-redistributable) SDK out of your local Xcode so the mac image
# can COPY it in — SDK staging, not app compilation.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DC="$REPO_ROOT/.devcontainer"
DRUN() { docker run --rm -v "$REPO_ROOT:/src" -w /src "$@"; }

# Make sure the target image is present AND current before running it. This goes
# through docker's layer cache, so it's near-instant when nothing changed, but it
# rebuilds when the Dockerfile or its base image changes. That matters: a plain
# "build only if missing" check would leave a *stale* image in place after e.g. the
# glibc-floor base pin changes, silently building a binary against the wrong glibc.
# (It also avoids the confusing "pull access denied" that a bare `docker run` gives
# when the image was never built locally, since we build rather than pull.)
ensure_image() {
    local img="$1" dockerfile="$2"
    echo ">> ensuring image '$img' is current (${dockerfile})"
    docker build -f "$DC/$dockerfile" -t "$img" "$DC"
}

prepare_mac() {
    local sdk="/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk"
    [ -d "$sdk" ] || sdk="$(xcrun --show-sdk-path 2>/dev/null || true)"
    [ -n "$sdk" ] && [ -d "$sdk" ] || { echo "!! no macOS SDK found — install Xcode / run 'xcode-select --install'"; exit 1; }
    echo ">> staging macOS SDK: $sdk"
    tar -cf "$DC/MacOSX.sdk.tar" -C "$(dirname "$sdk")" "$(basename "$sdk")"
    echo ">> wrote $DC/MacOSX.sdk.tar (gitignored)"
}

images() {
    docker build -f "$DC/Dockerfile"         -t hdzero-dev     "$DC"
    docker build -f "$DC/Dockerfile.windows" -t hdzero-dev-win "$DC"
    [ -f "$DC/MacOSX.sdk.tar" ] || prepare_mac
    docker build -f "$DC/Dockerfile.macos"   -t hdzero-dev-mac "$DC"
}

linux() {
    ensure_image hdzero-dev Dockerfile
    DRUN hdzero-dev bash -lc '
        cmake -S . -B build_emu_linux -DEMULATOR_BUILD=ON -DHDZ_GOGGLE=ON -DCMAKE_BUILD_TYPE=Debug &&
        make -C build_emu_linux -j"$(nproc)"'
}

windows() {
    ensure_image hdzero-dev     Dockerfile          # base for the mingw image
    ensure_image hdzero-dev-win Dockerfile.windows
    DRUN hdzero-dev-win bash -lc '
        cmake -S . -B build_win_x64 -DEMULATOR_BUILD=ON -DHDZ_GOGGLE=ON -DCMAKE_BUILD_TYPE=Debug \
            -DCMAKE_TOOLCHAIN_FILE=.devcontainer/windows-x64.cmake &&
        make -C build_win_x64 -j"$(nproc)"'
}

mac() {
    [ -f "$DC/MacOSX.sdk.tar" ] || prepare_mac
    ensure_image hdzero-dev-mac Dockerfile.macos
    DRUN hdzero-dev-mac bash -lc '
        cmake -S . -B build_xmac -DEMULATOR_BUILD=ON -DHDZ_GOGGLE=ON -DCMAKE_BUILD_TYPE=Debug \
            -DCMAKE_TOOLCHAIN_FILE=.devcontainer/darwin-arm64.cmake &&
        make -C build_xmac -j"$(nproc)"'
    echo ">> built build_xmac/HDZGOGGLE (arm64 Mach-O) — run natively with: BUILD_DIR=build_xmac ./run-native.sh"
}

case "${1:-all}" in
    prepare-mac) prepare_mac ;;
    images)      images ;;
    linux)       linux ;;
    windows)     windows ;;
    mac)         mac ;;
    all)         images; linux; windows; mac ;;
    *) echo "usage: build.sh {prepare-mac|images|linux|windows|mac|all}"; exit 2 ;;
esac
