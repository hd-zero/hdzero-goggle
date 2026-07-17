# Pinned to 22.04 (glibc 2.35) so the Linux emulator binary built here runs on any
# reasonably current distro. An unpinned :ubuntu tag tracks the newest LTS and glibc
# keeps climbing (26.04 => 2.43), which would make the binary fail with
# "version GLIBC_x.y not found" on older hosts -- breaking the build-in-container,
# run-natively promise. Windows/macOS outputs are PE/Mach-O and don't depend on this.
FROM --platform=linux/x86_64 mcr.microsoft.com/devcontainers/base:ubuntu-22.04

# Firmware builds need cmake/build-essential/ninja + the Bootlin toolchain that
# setup.sh downloads. Everything else turns this same container into a full HDZero
# EMULATOR workbench: SDL2 to build it, ffmpeg/libav to decode a fake FPV video
# feed, and Xvfb/x11vnc/xdotool/imagemagick to run it headless, drive it, screenshot.
RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get -y install --no-install-recommends \
        cmake build-essential ninja-build libsdl2-dev \
        wget ca-certificates bzip2 pkg-config \
        libavformat-dev libavcodec-dev libavutil-dev libswscale-dev ffmpeg \
        xvfb xdotool imagemagick && \
    rm -rf /var/lib/apt/lists/*