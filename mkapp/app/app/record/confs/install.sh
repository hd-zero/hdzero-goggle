#!/bin/sh

#APP_DIR=/mnt/app
#DATA_DIR=/data
APP_DIR=$1
DATA_DIR=$2

install_confs()
{
	confs_src=$1/confs
	confs_dst=$2/confs

	if [ ! -L $confs_dst ]; then
		echo "[OTA]copy $confs_src to $confs_dst"
		cp -a $confs_src $confs_dst
		return
	fi

	conf=gogglecmd.conf
	[ ! -f $confs_dst/$conf ] && cp $confs_src/$conf $confs_dst

	conf=record.conf
	[ ! -f $confs_dst/$conf ] && cp $confs_src/$conf $confs_dst
}

echo "[OTA]install confs to ${DATA_DIR}"

install_confs ${APP_DIR} ${DATA_DIR}

echo "[OTA]install confs done"

