#!/bin/bash
set -e

function make_img_md5() {
    ${BIN_DIR}/md5sum $1 | awk '{print $1}' > $1.md5
}

function get_app_version() {
	local delimeter=' = '
    local file=$1
	local section="app"
    local key="ver"
    local val=$(awk -F "$delimeter" '/\['${section}'\]/{a=1}a==1&&$1~/'${key}'/{print $2;exit}' $file)
    echo ${val}
}

ROOT_DIR=$PWD

BIN_DIR=$ROOT_DIR/bin
IMG_DIR=$ROOT_DIR/ota_app
APP_DIR=$ROOT_DIR/app

APP_SIZE=8388608
APP_IMAGE=${IMG_DIR}/app.fex
APP_VERSION=$(get_app_version "$ROOT_DIR/app/version")

rm -rf $IMG_DIR
mkdir -p $IMG_DIR

${BIN_DIR}/mkfs.jffs2 \
    --little-endian \
    --eraseblock=0x10000 \
    --pad=${APP_SIZE} \
    --root=${APP_DIR} \
    --output=${APP_IMAGE}
make_img_md5 ${APP_IMAGE}

cd $IMG_DIR
tar cvf $IMG_DIR/hdzgoggle_app_ota-${APP_VERSION}.tar *