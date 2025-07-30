FROM --platform=linux/x86_64 mcr.microsoft.com/devcontainers/base:ubuntu

RUN apt-get update

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get -y install --no-install-recommends \
    cmake \
    build-essential \
    ninja-build \
    libsdl2-dev