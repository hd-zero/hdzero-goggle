#!/bin/sh

set -e

PROGRAM_FLASH=esp_flash
DIR_TARGET=/mnt/extsd/ELRS

FILE_BIN0=${DIR_TARGET}/bootloader_dio_40m.bin
FILE_BIN1=${DIR_TARGET}/partitions.bin
FILE_BIN2=${DIR_TARGET}/boot_app0.bin
FILE_BIN3=${DIR_TARGET}/firmware.bin
ADDR_BIN0="1000"
ADDR_BIN1="8000"
ADDR_BIN2="e000"
ADDR_BIN3="10000"

#export gpio resource
if [ ! -d /sys/class/gpio/gpio128 ]
then
	echo "128" > /sys/class/gpio/export
	echo "out" > /sys/class/gpio/gpio128/direction 
fi

if [ ! -d /sys/class/gpio/gpio129 ]
then
	echo "129" > /sys/class/gpio/export
	echo "out" > /sys/class/gpio/gpio129/direction 
fi

#flash target
if [ -d ${DIR_TARGET} ]
then
	if [ -f ${FILE_BIN0} ]
	then
		${PROGRAM_FLASH}  ${FILE_BIN0} ${ADDR_BIN0}
		echo "flash ${FILE_BIN0} done"
	else
		echo "${FILE_BIN0} not exist,skip"
	fi

	if [ -f ${FILE_BIN1} ]
	then
		${PROGRAM_FLASH}  ${FILE_BIN1} ${ADDR_BIN1}
		echo "flash ${FILE_BIN1} done"
	else
		echo "${FILE_BIN1} not exist,skip"
	fi

	if [ -f ${FILE_BIN2} ]
	then
		${PROGRAM_FLASH}  ${FILE_BIN2} ${ADDR_BIN2}
		echo "flash ${FILE_BIN2} done"
	else
		echo "${FILE_BIN2} not exist,skip"
	fi

	if [ -f ${FILE_BIN3} ]
	then
		${PROGRAM_FLASH}  ${FILE_BIN3} ${ADDR_BIN3}
		echo "flash ${FILE_BIN3} done"
		echo "all done"
	else
		echo "${FILE_BIN3} not exist,skip"
	fi

else
	echo "${DIR_TARGET} not exist,skip"
fi

#reset target
echo "1" > /sys/class/gpio/gpio128/value
echo "0" > /sys/class/gpio/gpio129/value
sleep 1
echo "1" > /sys/class/gpio/gpio129/value
