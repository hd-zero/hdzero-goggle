#!/bin/bash
set -e

if [ ! -d toolchain ]; then
	echo "Extracting toolchain..."
	tar xzvf toolchain.tar.gz
fi

rm -rf build && mkdir build
cmake . -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake -Bbuild
