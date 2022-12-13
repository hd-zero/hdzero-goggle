#!/bin/sh

APP_DIR=/mnt/app
BIN_DIR=/usr/sbin
LIB_DIR=/usr/lib
DATA_DIR=/data

install_libffmpeg()
{
	src_dir=$1
	dst_dir=$2

	echo "[OTA]install libffmpeg"
	$src_dir/libs/libffmpeg/install.sh $src_dir $dst_dir
}

install_confs()
{
	src_dir=$1
	dst_dir=$2

	$src_dir/confs/install.sh $src_dir $dst_dir
}

install_apps()
{
	src_dir=$1
	dst_dir=$2

	app=gogglecmd
	echo "[OTA]link $app to $dst_dir"
	[ -f $dst_dir/$app ] && rm $dst_dir/$app
	ln -s $src_dir/$app $dst_dir/$app

	app=record
	echo "[OTA]link $app to $dst_dir"
	[ -f $dst_dir/$app ] && rm $dst_dir/$app
	ln -s $src_dir/$app $dst_dir/$app

	app=play
	echo "[OTA]link $app to $dst_dir"
	[ -f $dst_dir/$app ] && rm $dst_dir/$app
	ln -s $src_dir/$app $dst_dir/$app
}

echo "[OTA]install app package"

install_libffmpeg ${APP_DIR} ${LIB_DIR}
install_apps ${APP_DIR} ${BIN_DIR}
install_confs ${APP_DIR} ${DATA_DIR}

echo "[OTA]app package installed"

