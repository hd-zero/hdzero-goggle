#!/bin/bash
set -e

if [[ ! -d linux-4.9 ]]; then
	echo "installing linux headers..."
	tar xzf sdk/linux-4.9.tar.gz
fi

if [[ ! -d media ]]; then
	echo "installing media libraries..."
	cat sdk/media.tar.gzaa sdk/media.tar.gzab | tar xz 
fi

if [[ ! -d toolchain ]]; then
	echo "installing toolchain..."
	mkdir toolchain
	wget -qO- http://musl.cc/arm-linux-musleabihf-cross.tgz | tar xz -C toolchain
fi

rm -rf build && mkdir build
cmake . --toolchain=toolchain.cmake -Bbuild
