#!/bin/bash

ROOT_DIR=$PWD
SYS_PARTITION=${ROOT_DIR}/image/sys_partition_nor.fex

function envsetup()
{
    if [ "x$SHELL" != "x/bin/bash" ]; then
        case `ps -o command -p $$` in
            *bash*)
                ;;
            *)
                echo -n "WARNING: Only bash is supported, "
                echo "use of other shell would lead to erroneous results"
                ;;
        esac
    fi
}

function print_red()
{
    echo -e '\033[0;31;1m'
    echo $1
    echo -e '\033[0m'
}

function print_green()
{
    echo -e '\033[0;32;1m'
    echo $1
    echo -e '\033[0m'
}

function make_img_md5() 
{
	local BIN_DIR=$ROOT_DIR/bin

    #$1: target image
    ${BIN_DIR}/md5sum $1 | awk '{print $1}' > $1.md5
}

function make_app_res()
{
	local APP_PART_NAME=app
	local BIN_DIR=$ROOT_DIR/bin
	local IMG_DIR=$ROOT_DIR/image

	cd ${ROOT_DIR}/image
	cp $1 sys_partition_tmp_app.fex

	sed -i '/^[ \t]*downloadfile/d' sys_partition_tmp_app.fex
	${BIN_DIR}/busybox unix2dos sys_partition_tmp_app.fex
	${BIN_DIR}/script  sys_partition_tmp_app.fex > /dev/null
	${BIN_DIR}/update_mbr sys_partition_tmp_app.bin 1 sunxi_mbr_tmp_app.fex

	local APP_PART_DOWNLOAD_FILE_VER=app.ver
	local APP_PART_DOWNLOAD_FILE=app.fex
	local APP_PART_FILE_PATH=$ROOT_DIR/app
	local APP_PART_SIZE_IN_SECTOR=`${BIN_DIR}/parser_mbr sunxi_mbr_tmp_app.fex get_size_by_name ${APP_PART_NAME}`

	if [ x${APP_PART_DOWNLOAD_FILE} != x'' -a  x${APP_PART_SIZE_IN_SECTOR} != x'' ]; then
		let APP_PART_SIZE_IN_K=$APP_PART_SIZE_IN_SECTOR/2
		echo "APP_PART_DOWNLOAD_FILE = ${IMG_DIR}/${APP_PART_DOWNLOAD_FILE}"
		echo "---liran: make_ext4fs -l ${APP_PART_SIZE_IN_K}k -b 1024 -m 0 -j 1024 ${ROOT_DIR}/${APP_PART_DOWNLOAD_FILE} ${APP_PART_FILE_PATH}"
		rm -f ${IMG_DIR}/${APP_PART_DOWNLOAD_FILE}
		#${BIN_DIR}/make_ext4fs -l ${APP_PART_SIZE_IN_K}k -b 1024 -m 0 -j 1024 ${IMG_DIR}/${APP_PART_DOWNLOAD_FILE}  ${APP_PART_FILE_PATH}
		
		let APP_PART_SIZE_IN_BYTES=${APP_PART_SIZE_IN_K}*1024
		echo "---liran: mkfs.jffs2 l -e 0x10000 -p ${APP_PART_SIZE_IN_BYTES} -d ${APP_PART_FILE_PATH} -o ${IMG_DIR}/${APP_PART_DOWNLOAD_FILE}"
		${BIN_DIR}/mkfs.jffs2 -l -e 0x10000 -p ${APP_PART_SIZE_IN_K}k -d ${APP_PART_FILE_PATH} -o ${IMG_DIR}/${APP_PART_DOWNLOAD_FILE}
		[ -f ${APP_PART_FILE_PATH}/version ] && cp ${APP_PART_FILE_PATH}/version ${IMG_DIR}/${APP_PART_DOWNLOAD_FILE_VER}
	else
		printf "no app resource partitions\n"
	fi
	cd -
}

function get_app_version() 
{
	# key和value的分隔符，即等号两边有没有空格
	#delimeter='='
	delimeter=' = '
    file=$1
	section="app"
    key="ver"
    val=$(awk -F "$delimeter" '/\['${section}'\]/{a=1}a==1&&$1~/'${key}'/{print $2;exit}' $file)
    echo ${val}
}

function make_ota_package_for_app()
{
	local T=$ROOT_DIR
	local OTA_DIR=$T/ota_app
	local make_fail=0
	local ota_version=$T/image/app.ver
	local VER=$(get_app_version "$ota_version")
	local ota_package=hdzgoggle_app_ota-${VER}.tar

	mkdir -p "$OTA_DIR"
	\cd "$OTA_DIR"
	rm -f $ota_package $ota_package.md5

	target_list="$T/image/app.fex $ota_version"
	for i in $target_list; do
		img=${i##*/}
		if [ ! -f "$i" ]; then
			print_red "$i not exsit!"
			make_fail=1
			break
		fi
		dd if="$i" of="$img" bs=512 conv=sync
		make_img_md5 "$img"
		tar -rvf "$ota_package" "$img"
		tar -rvf "$ota_package" "$img.md5"
	done
	if [ x$make_fail = x"1" ]; then
		print_red "make fail"
		rm -f $ota_package
	else
		make_img_md5 "$ota_package"
		print_red "$OTA_DIR/$ota_package"
	fi
	\cd - > /dev/null
}

envsetup
make_app_res $SYS_PARTITION
make_ota_package_for_app