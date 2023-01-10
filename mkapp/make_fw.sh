#!/bin/bash
FILE1=`ls ota_app/hdzgoggle_app_ota-*.tar`
FILE2=hal/HDZGOGGLE_RX.bin
FILE3=hal/HDZGOGGLE_VA.bin
FILE4=hal/ver.txt


assert_file_exist()
{
	if [ ! -f ${FILE1} ];then
		echo "${FILE1} not exist"
		exit 10
	fi
	if [ ! -f ${FILE2} ];then
		echo "${FILE2} not exist"
		exit 10
	fi
	if [ ! -f ${FILE3} ];then
		echo "${FILE3} not exist"
		exit 10
	fi
}

get_ver()
{
	OTA_VER=`ls ${FILE1}`
	OTA_VER=${OTA_VER#*-}
	OTA_VER=${OTA_VER%%.*}

	RXVA_VER=`cat ${FILE4}`
	RX_VER=${RXVA_VER%-*}
	VA_VER=${RXVA_VER#*-}
}

generate()
{
	cp ${FILE1} ${TAR_FILE1}
	cp ${FILE2} ${TAR_FILE2}
	cp ${FILE3} ${TAR_FILE3}
	tar cvf out.tar ${TAR_FILE1} ${TAR_FILE2} ${TAR_FILE3} 
	mv out.tar HDZERO_GOGGLE-${OTA_VER}.${RX_VER}.${VA_VER}.bin
	rm ${TAR_FILE1}
	rm ${TAR_FILE2}
	rm ${TAR_FILE3}
}


#main process
assert_file_exist
get_ver

TAR_FILE1=${FILE1#*/}
TAR_FILE2=${FILE2#*/}
TAR_FILE2=${TAR_FILE2%.*}-${RX_VER}.bin
TAR_FILE3=${FILE3#*/}
TAR_FILE3=${TAR_FILE3%.*}-${VA_VER}.bin
generate

echo "generate HDZERO_GOGGLE-${OTA_VER}.${RX_VER}.${VA_VER}.bin"
