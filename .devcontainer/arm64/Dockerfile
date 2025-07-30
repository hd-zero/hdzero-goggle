FROM mcr.microsoft.com/devcontainers/base:ubuntu-22.04

RUN dpkg --add-architecture armhf && apt-get update

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get -y install --no-install-recommends \
    cmake \
    build-essential \
    ninja-build \
    libsdl2-dev \
    wget \
    gcc-arm-linux-gnueabihf \
    g++-arm-linux-gnueabihf \
    libcrypt-dev:armhf \
    mtd-utils \
    && rm -rf /var/lib/apt/lists/*