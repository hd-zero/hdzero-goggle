#!/bin/sh

APP_DIR=$1
LIB_DIR=$2
LIB_SRC=$APP_DIR/libs/libffmpeg

echo "[OTA]remove ffmpeg library link"

libfile=${LIB_DIR}/libavcodec.so.59
[ -f $libfile ] && rm $libfile

libfile=${LIB_DIR}/libavformat.so.59
[ -f $libfile ] && rm $libfile

libfile=${LIB_DIR}/libavutil.so.57
[ -f $libfile ] && rm $libfile

echo "[OTA]link ffmpeg library to ${LIB_DIR}"

ln -s ${LIB_SRC}/libavcodec.so.59.18.100 ${LIB_DIR}/libavcodec.so.59
ln -s ${LIB_SRC}/libavformat.so.59.16.100 ${LIB_DIR}/libavformat.so.59
ln -s ${LIB_SRC}/libavutil.so.57.17.100 ${LIB_DIR}/libavutil.so.57

echo "[OTA]ffmpeg library installed"

