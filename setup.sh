#!/bin/bash
set -e

TOOLCHAIN_URL="https://toolchains.bootlin.com/downloads/releases/toolchains/armv7-eabihf/tarballs/armv7-eabihf--musl--stable-2018.02-2.tar.bz2"

if [ ! -d toolchain ]; then
	echo "Extracting toolchain..."
	mkdir toolchain
	wget -qO- "$TOOLCHAIN_URL" | tar xj --strip-components=1 -C toolchain
fi

rm -rf build_*

echo
echo "**********************************************"
echo "Preparing HDZero Goggle Build Environment....."
echo "**********************************************"
cmake . -DHDZ_GOGGLE=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=toolchain/share/buildroot/toolchainfile.cmake -Bbuild_goggle

echo
echo "**********************************************"
echo "Preparing HDZero BoxPro Build Environment....."
echo "**********************************************"
cmake . -DHDZ_BOXPRO=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=toolchain/share/buildroot/toolchainfile.cmake -Bbuild_boxpro

echo
echo "****************************************"
echo "*** HDZero Goggle Build Instructions ***"
echo "****************************************"
echo "cd build_goggle; make -j"

echo
echo "****************************************"
echo "*** HDZero BoxPro Build Instructions ***"
echo "****************************************"
echo "cd build_boxpro; make -j"
