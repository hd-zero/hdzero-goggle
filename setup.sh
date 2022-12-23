#!/bin/bash
set -e

if [ ! -d linux-4.9 ]; then
	echo "Extracting linux headers..."
	tar xzf linux-4.9.tar.gz
fi

if [ ! -d media ]; then
	echo "Extracting media libraries..."
	cat media.tar.gzaa media.tar.gzab | tar xz 
fi

if [ ! -d toolchain ]; then
	echo "Extracting toolchain..."
	tar xzvf toolchain.tar.gz
fi

rm -rf build && mkdir build
cmake . -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake -Bbuild
