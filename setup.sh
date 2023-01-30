#!/bin/bash
set -e

TOOLCHAIN_URL="https://toolchains.bootlin.com/downloads/releases/toolchains/armv7-eabihf/tarballs/armv7-eabihf--musl--stable-2018.02-2.tar.bz2"

if [ ! -d toolchain ]; then
	echo "Extracting toolchain..."
	mkdir toolchain
	wget -qO- "$TOOLCHAIN_URL" | tar xj --strip-components=1 -C toolchain
fi

rm -rf build && mkdir build
cmake . -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=toolchain/share/buildroot/toolchainfile.cmake -Bbuild
