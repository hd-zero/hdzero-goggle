#!/bin/sh

if [ ! -d build ]
then
	mkdir build
else
	rm build/* -rf
fi
cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake
