#!/bin/sh

if [ ! -d build ]
then
	mkdir build
else
	rm build/* -rf
fi
cd build && cmake .. --toolchain=../toolchain.cmake
