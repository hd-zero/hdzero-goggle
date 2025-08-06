#!/bin/bash
set -e

function make_img_md5() {
    md5sum $1 | awk '{print $1}' > $1.md5
}

function get_app_version() {
	local base_version=$(cat "$ROOT_DIR/../VERSION")

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
PLATFORM="$(cat ${MKAPP_DIR}/app/platform)"

BIN_DIR=$MKAPP_DIR/bin
IMG_DIR=$MKAPP_DIR/ota_app
APP_DIR=$MKAPP_DIR/app
KO_DIR=$APP_DIR/ko

APP_SIZE=8388608
APP_IMAGE=${IMG_DIR}/app.fex
APP_VERSION=$(get_app_version)

# append BIN_DIR to PATH, preferring tooling which is already in PATH
export PATH="${PATH:+$PATH:}${BIN_DIR}"

if [ $PLATFORM == "HDZGOGGLE" ]; then
	PLATFORM_APP="HDZERO_GOGGLE"
    cp -a $KO_DIR/vdpo-goggle.ko $KO_DIR/vdpo.ko
elif [ $PLATFORM == "HDZBOXPRO" ]; then
    PLATFORM_APP="HDZERO_BOXPRO"
    cp -a $KO_DIR/vdpo-boxpro.ko $KO_DIR/vdpo.ko
elif [ $PLATFORM == "HDZGOGGLE2" ]; then
	PLATFORM_APP="HDZERO_GOGGLE2"
    cp -a $KO_DIR/vdpo-goggle2.ko $KO_DIR/vdpo.ko
fi

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

if [ $PLATFORM == "HDZGOGGLE" ]; then
    HAL_VERSION="$(cat "$MKAPP_DIR/hal/goggle/version")"
elif [ $PLATFORM == "HDZBOXPRO" ]; then
    HAL_VERSION="$(cat "$MKAPP_DIR/hal/boxpro/version")"
elif [ $PLATFORM == "HDZGOGGLE2" ]; then
    HAL_VERSION="$(cat "$MKAPP_DIR/hal/goggle2/version")"
else
    echo "Unknown platform: $PLATFORM"
    exit 1
fi
HAL_RX_VER=${HAL_VERSION%-*}
HAL_VA_VER=${HAL_VERSION#*-}
OTA_VERSION="${HAL_VERSION}-${APP_VERSION}"

if [ $PLATFORM == "HDZGOGGLE" ]; then
    cp $MKAPP_DIR/hal/goggle/HDZGOGGLE_RX.bin HDZGOGGLE_RX-${HAL_RX_VER}.bin
    cp $MKAPP_DIR/hal/goggle/HDZGOGGLE_VA.bin HDZGOGGLE_VA-${HAL_VA_VER}.bin
elif [ $PLATFORM == "HDZBOXPRO" ]; then
    cp $MKAPP_DIR/hal/boxpro/HDZBOXPRO_RX.bin HDZBOXPRO_RX-${HAL_RX_VER}.bin
    cp $MKAPP_DIR/hal/boxpro/HDZBOXPRO_VA.bin HDZBOXPRO_VA-${HAL_VA_VER}.bin
elif [ $PLATFORM == "HDZGOGGLE2" ]; then
    cp $MKAPP_DIR/hal/goggle2/HDZGOGGLE_RX.bin HDZGOGGLE_RX-${HAL_RX_VER}.bin
    cp $MKAPP_DIR/hal/goggle2/HDZGOGGLE_VA.bin HDZGOGGLE_VA-${HAL_VA_VER}.bin
fi

echo -e "\npacking ota:"
rm $ROOT_DIR/out/${PLATFORM_APP}-* || true

if [ $PLATFORM == "HDZGOGGLE" ]; then
    tar cvf $ROOT_DIR/out/${PLATFORM_APP}-${OTA_VERSION}.bin \
        hdzgoggle_app_ota-${APP_VERSION}.tar \
        HDZGOGGLE_RX-${HAL_RX_VER}.bin \
        HDZGOGGLE_VA-${HAL_VA_VER}.bin
elif [ $PLATFORM == "HDZBOXPRO" ]; then
    tar cvf $ROOT_DIR/out/${PLATFORM_APP}-${OTA_VERSION}.bin \
        hdzgoggle_app_ota-${APP_VERSION}.tar \
        HDZBOXPRO_RX-${HAL_RX_VER}.bin \
        HDZBOXPRO_VA-${HAL_VA_VER}.bin
elif [ $PLATFORM == "HDZGOGGLE2" ]; then
    tar cvf $ROOT_DIR/out/${PLATFORM_APP}-${OTA_VERSION}.bin \
        hdzgoggle_app_ota-${APP_VERSION}.tar \
        HDZGOGGLE_RX-${HAL_RX_VER}.bin \
        HDZGOGGLE_VA-${HAL_VA_VER}.bin
fi

echo -e "\ngenerated out/${PLATFORM_APP}-${OTA_VERSION}.bin"
