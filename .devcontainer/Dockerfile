FROM --platform=linux/x86_64 mcr.microsoft.com/devcontainers/base:ubuntu

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get -y install --no-install-recommends cmake build-essential ninja-build libsdl2-dev nodejs npm && \
    npm i lv_i18n -g