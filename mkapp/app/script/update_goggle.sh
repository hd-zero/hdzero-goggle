#!/bin/sh

PLATFORM="$(cat /mnt/app/platform)"
TMP_DIR=/tmp/goggle_update
HDZ_BIN="$1"

TMP_RX_BIN="${TMP_DIR}/${PLATFORM}_RX.bin"
TMP_VA_BIN="${TMP_DIR}/${PLATFORM}_VA.bin"
WILDCARD_RX_BIN="${TMP_DIR}/${PLATFORM}_RX*.bin"
WILDCARD_VA_BIN="${TMP_DIR}/${PLATFORM}_VA*.bin"

if [ $PLATFORM == "HDZGOGGLE"]; then
	WILDCARD_HDZ_BIN="/mnt/extsd/HDZERO_GOGGLE*.bin"
elif [ $PLATFORM == "HDZBOXPRO"]; then
	WILDCARD_HDZ_BIN="/mnt/extsd/HDZERO_BOXPRO*.bin"
fi

VAbin=${TMP_DIR}/HDZGOGGLE_VA.bin
VAcount=1
VAwrites=0
RXbin=${TMP_DIR}/HDZGOGGLE_RX.bin
RXcount=2
RXwrites=0

function gpio_export()
{
	echo "224">/sys/class/gpio/export
	echo "228">/sys/class/gpio/export
	echo "258">/sys/class/gpio/export
	echo "131">/sys/class/gpio/export
	echo "out">/sys/class/gpio/gpio224/direction
	echo "out">/sys/class/gpio/gpio228/direction
	echo "out">/sys/class/gpio/gpio258/direction
	echo "out">/sys/class/gpio/gpio131/direction
}

function beep()
{
	if [ ! -z "$1" ];then
	delay=$1
	else
	delay=1
	fi
	echo "1">/sys/class/gpio/gpio131/value
	sleep $delay
        echo "0">/sys/class/gpio/gpio131/value
}

function beep_success()
{
	beep 0.1
	sleep 0.5
	beep 0.05
	sleep 1
}


function beep_failure()
{
	beep 1
	sleep 0.5
	beep 1
	sleep 0.5
	beep 0.05
	sleep 1
}


function gpio_set_reset()
{
	echo "0">/sys/class/gpio/gpio224/value
	echo "1">/sys/class/gpio/gpio228/value
}

function gpio_clear_reset()
{
	echo "1">/sys/class/gpio/gpio224/value
	echo "0">/sys/class/gpio/gpio228/value
}

function disconnect_fpga_flash()
{
	echo "1">/sys/class/gpio/gpio258/value
}

function connect_fpga_flash()
{
	echo "0">/sys/class/gpio/gpio258/value
}

# eg: check_mtd_write /dev/mtdX required-size bin-file
function check_mtd_write()
{
	filesize=`ls -l $3 | awk '{print $5}'`
	mtd_info=`mtd_debug info $1`
	echo "$mtd_info"
	mtdsize=`echo "$mtd_info" | grep mtd.size | grep "($2)"`                
	if [ ! -z "$mtdsize" ];then
	        echo "$1 size is ($2)" 
		mtdsizeB=`echo "$mtdsize" |cut -d " " -f 3`
		echo mtd_debug erase $1 0 $mtdsizeB
		mtd_debug erase $1 0 $mtdsizeB
		echo mtd_debug write $1 0 $filesize $3
		mtd_debug write $1 0 $filesize $3
		if [ $? == 0 ]; then
#			beep_success
			if [ "$3" == "$VAbin" ]; then
				VAwrites=$((VAwrites + 1))
			fi
			if [ "$3" == "$RXbin" ]; then
				RXwrites=$((RXwrites + 1))
			fi
		fi 
	else
	        echo "$1 size is NOT ($2) !" 
		beep_failure
	fi
}
function untar_file()
{
	FILE_TARGET="$1"

	if [ ! -e ${TMP_DIR} ]
	then
		mkdir ${TMP_DIR}
	else
		rm ${TMP_DIR} -rf
		mkdir ${TMP_DIR}
	fi

	tar xf ${FILE_TARGET} -C ${TMP_DIR} 2>&1 > /dev/null
	mv ${WILDCARD_RX_BIN} ${TMP_RX_BIN}
	mv ${WILDCARD_VA_BIN} ${TMP_VA_BIN}
}
 

function update_rx()
{
	echo "find RX update file, start update"
	gpio_export
	gpio_set_reset
	insmod /mnt/app/ko/w25q128.ko
	check_mtd_write /dev/mtd8 1M ${TMP_RX_BIN}
	sleep 1
	check_mtd_write /dev/mtd9 1M ${TMP_RX_BIN}
	echo "update finish RX, running"
	gpio_clear_reset
	sleep 1
	rmmod /mnt/app/ko/w25q128.ko
}

function update_fpga()
{
	echo "find VA update file, start update"
	gpio_export
	gpio_set_reset
	disconnect_fpga_flash
	insmod /mnt/app/ko/w25q128.ko
	check_mtd_write /dev/mtd10 16M ${TMP_VA_BIN}
	echo "update finish VA, running"
	gpio_clear_reset
	sleep 1
	rmmod /mnt/app/ko/w25q128.ko
}

# If firmware file was NOT supplied then default to primary location for emergency restore
if [ -z "$HDZ_BIN" ]; then
	if [ `ls ${WILDCARD_HDZ_BIN} | grep bin | wc -l` -eq 1 ]
	then
		HDZ_BIN="${WILDCARD_HDZ_BIN}"
	fi
fi

if [ ! -z "$HDZ_BIN" ]; then
	echo "Flashing $HDZ_BIN"
	echo "0" > /tmp/progress_goggle
	echo "0"
	untar_file "$HDZ_BIN"
	mv ${TMP_DIR}/hdzgoggle_app_ota*.tar ${TMP_DIR}/hdzgoggle_app_ota.tar
	cp -f /mnt/app/setting.ini /mnt/UDISK/
	#disable it66021
	i2cset -y 3 0x49 0x10 0xff
	echo "1"
	echo "1" > /tmp/progress_goggle
	update_rx
	echo "6"
	echo "6" > /tmp/progress_goggle
	update_fpga
 	echo "45"
	echo "45" > /tmp/progress_goggle
	hdz_upgrade_app.sh
	echo "100"
	echo "100" > /tmp/progress_goggle
	echo "all done"
else
	echo "skip"
fi
