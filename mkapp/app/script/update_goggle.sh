#!/bin/sh

GOGGLE_BIN="$1"
TMP_DIR=/tmp/goggle_update

function gpio_export()
{
	if [ ! -f /sys/class/gpio/gpio224/direction ]
	then
	      echo "224">/sys/class/gpio/export
	fi
	echo "out">/sys/class/gpio/gpio224/direction
	if [ ! -f /sys/class/gpio/gpio228/direction ]
	then
		echo "228">/sys/class/gpio/export
	fi
        echo "out">/sys/class/gpio/gpio228/direction
	if [ ! -f /sys/class/gpio/gpio258/direction ]
	then
		echo "258">/sys/class/gpio/export
	fi
	echo "out">/sys/class/gpio/gpio258/direction
	if [ ! -f /sys/class/gpio/gpio131/direction ] 
        then                                                          
                echo "131">/sys/class/gpio/export
        fi
        echo "out">/sys/class/gpio/gpio131/direction
}

function beep_success()
{
    	echo "1">/sys/class/gpio/gpio131/value
		sleep 0.1
    	echo "0">/sys/class/gpio/gpio131/value
		sleep 0.5
    	echo "1">/sys/class/gpio/gpio131/value
		sleep 0.05
    	echo "0">/sys/class/gpio/gpio131/value
}


function beep_failure()
{
    	echo "1">/sys/class/gpio/gpio131/value
		sleep 1
    	echo "0">/sys/class/gpio/gpio131/value
		sleep 0.5
    	echo "1">/sys/class/gpio/gpio131/value
		sleep 1
    	echo "0">/sys/class/gpio/gpio131/value
		sleep 0.5
    	echo "1">/sys/class/gpio/gpio131/value
		sleep 0.05
    	echo "0">/sys/class/gpio/gpio131/value
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

function gpio_set_send()
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
	mv ${TMP_DIR}/HDZGOGGLE_RX*.bin ${TMP_DIR}/HDZGOGGLE_RX.bin
	mv ${TMP_DIR}/HDZGOGGLE_VA*.bin ${TMP_DIR}/HDZGOGGLE_VA.bin
}
 
# eg: check_mtd_write /dev/mtdX check-size erase-size file-size bin-file
function check_mtd_write()
{
	mtd_info=`mtd_debug info $1`
	echo "$mtd_info"
	value=`echo "$mtd_info" | grep mtd.size | grep "($2)"`                
	if [ ! -z "$value" ];then
	    echo "$1 size is ($2)" 
		mtd_debug erase $1 0 $3
		mtd_debug write $1 0 $4 $5
		beep_success
	else
	    echo "$1 size is NOT ($2) !" 
		beep_failure
	fi
}

function update_rx()
{
	echo "find RX update file, start update"
	filesize=`ls -l ${TMP_DIR}/HDZGOGGLE_RX*.bin| awk '{print $5}'`
	gpio_export
	gpio_set_reset
	insmod /mnt/app/ko/w25q128.ko
	check_mtd_write /dev/mtd8 1M 65536 $filesize ${TMP_DIR}/HDZGOGGLE_RX.bin
    echo "5"                                                                          
    echo "5" > /tmp/progress_goggle
	sleep 1
	check_mtd_write /dev/mtd9 1M 65536 $filesize ${TMP_DIR}/HDZGOGGLE_RX.bin
    echo "10"                                                                          
    echo "10" > /tmp/progress_goggle
	echo "update finish RX, running"
	gpio_clear_reset
	sleep 1
	rmmod /mnt/app/ko/w25q128.ko
}

function update_fpga()
{
	echo "find VA update file, start update"
	filesize2=`ls -l ${TMP_DIR}/HDZGOGGLE_VA*.bin| awk '{print $5}'`
	gpio_export
	gpio_set_reset
	disconnect_fpga_flash
	insmod /mnt/app/ko/w25q128.ko
	check_mtd_write /dev/mtd10 16M 16777216 $filesize2 ${TMP_DIR}/HDZGOGGLE_VA.bin
    echo "45"                                                                         
    echo "45" > /tmp/progress_goggle  
	echo "update finish VA, running"
	gpio_clear_reset
	sleep 1
	rmmod /mnt/app/ko/w25q128.ko
}

# If firmware file was NOT supplied then default to primary location for emergency restore
if [ -z "$GOGGLE_BIN" ]; then
    if [ `ls /mnt/extsd/HDZERO_GOGGLE*.bin | grep bin | wc -l` -eq 1 ]
    then
        GOGGLE_BIN="/mnt/extsd/HDZERO_GOGGLE*.bin"
    fi
fi

if [ ! -z "$GOGGLE_BIN" ]
then
	echo "Flashing $GOGGLE_BIN"
	echo "0" > /tmp/progress_goggle
	echo "0"
	untar_file "$GOGGLE_BIN"
	mv ${TMP_DIR}/hdzgoggle_app_ota*.tar ${TMP_DIR}/hdzgoggle_app_ota.tar
	cp -f /mnt/app/setting.ini /mnt/UDISK/
	#disable it66021
	i2cset -y 3 0x49 0x10 0xff
        update_rx
	update_fpga
	hdz_upgrade_app.sh
	echo "100"
	echo "100" > /tmp/progress_goggle
	echo "all done"
else
	if [ `ls /mnt/extsd/HDZERO_GOGGLE*.bin | grep bin | wc -l` -eq 0 ]
	then
		echo "skip"
	else
		echo "repeat"
	fi
fi
