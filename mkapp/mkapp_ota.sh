#!/bin/bash
set -e

function make_img_md5() {
    md5sum $1 | awk '{print $1}' > $1.md5
}

function get_app_version() {
	local base_version=$(cat "$ROOT_DIR/VERSION")

    # check if we are on a tag
    git describe --exact-match --tags HEAD > /dev/null 2>&1
    if [ $? -ne 0 ]; then
        # no? attach commit hash
        echo "${base_version}-$(git rev-parse --short HEAD)"
    else
        echo "${base_version}"
    fi
}

ROOT_DIR=$PWD

MKAPP_DIR=$ROOT_DIR/mkapp
BIN_DIR=$MKAPP_DIR/bin
IMG_DIR=$MKAPP_DIR/ota_app
APP_DIR=$MKAPP_DIR/app

APP_SIZE=8388608
APP_IMAGE=${IMG_DIR}/app.fex
APP_VERSION=$(get_app_version)

# append BIN_DIR to PATH, preferring tooling which is already in PATH
export PATH="${PATH:+$PATH:}${BIN_DIR}"

echo "${APP_VERSION}" > ${APP_DIR}/version

rm -rf $IMG_DIR
mkdir -p $IMG_DIR

mkfs.jffs2 \
    --little-endian \
    --eraseblock=0x10000 \
    --root=${APP_DIR} \
    --output=${APP_IMAGE}
make_img_md5 ${APP_IMAGE}

FILESIZE=$(stat -c%s "${APP_IMAGE}")
if (( ${FILESIZE} > ${APP_SIZE})); then
    echo -e "\e[31;1mERROR: Firmware size is too large for partition!\e[0m"
    exit 1
fi

echo -e "\npacking app:"
cd $IMG_DIR
tar cvf $IMG_DIR/hdzgoggle_app_ota-${APP_VERSION}.tar *

HAL_VERSION="$(cat "$MKAPP_DIR/hal/ver.txt")"
HAL_RX_VER=${HAL_VERSION%-*}
HAL_VA_VER=${HAL_VERSION#*-}

OTA_VERSION="${HAL_VERSION}-${APP_VERSION}"

cp $MKAPP_DIR/hal/HDZGOGGLE_RX.bin HDZGOGGLE_RX-${HAL_RX_VER}.bin
cp $MKAPP_DIR/hal/HDZGOGGLE_VA.bin HDZGOGGLE_VA-${HAL_VA_VER}.bin

echo -e "\npacking ota:"
rm $ROOT_DIR/out/HDZERO_GOGGLE-* || true
tar cvf $ROOT_DIR/out/HDZERO_GOGGLE-${OTA_VERSION}.bin \
    hdzgoggle_app_ota-${APP_VERSION}.tar \
    HDZGOGGLE_RX-${HAL_RX_VER}.bin \
    HDZGOGGLE_VA-${HAL_VA_VER}.bin

echo -e "\ngenerated out/HDZERO_GOGGLE-${OTA_VERSION}.bin"
