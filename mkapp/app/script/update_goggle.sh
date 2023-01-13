#!/bin/sh


function gpio_export()
{
	if [ ! -f /sys/class/gpio/gpio224/direction ]
	then
	      echo "224">/sys/class/gpio/export
	      echo "out">/sys/class/gpio/gpio224/direction
	fi


	if [ ! -f /sys/class/gpio/gpio228/direction ]
	then
				echo "228">/sys/class/gpio/export
        echo "out">/sys/class/gpio/gpio228/direction
	fi

	if [ ! -f /sys/class/gpio/gpio258/direction ]
	then
			  echo "258">/sys/class/gpio/export
        echo "out">/sys/class/gpio/gpio258/direction
	fi	
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


FILE_TARGET=/mnt/extsd/HDZERO_GOGGLE*.bin
TMP_DIR=/tmp/goggle_update

function untar_fils()
{
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


function update_rx()
{

	echo "find RX update file, start update"
	filesize=`ls -l ${TMP_DIR}/HDZGOGGLE_RX*.bin| awk '{print $5}'`
	gpio_export
	gpio_set_reset
	insmod /mnt/app/ko/w25q128.ko

	mtd_debug erase /dev/mtd8 0 65536
	mtd_debug write /dev/mtd8 0 $filesize ${TMP_DIR}/HDZGOGGLE_RX.bin
	mtd_debug erase /dev/mtd9 0 65536
	mtd_debug write /dev/mtd9 0 $filesize ${TMP_DIR}/HDZGOGGLE_RX.bin
	

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

	mtd_debug erase /dev/mtd10 0 16777216
	mtd_debug write /dev/mtd10 0 $filesize2 ${TMP_DIR}/HDZGOGGLE_VA.bin
	echo "update finish VA, running"
	gpio_clear_reset
	sleep 1
	rmmod /mnt/app/ko/w25q128.ko
}


if [ `ls /mnt/extsd/HDZERO_GOGGLE*.bin |grep bin |wc -l` == "1" ]
then
	echo "0" > /tmp/progress_goggle
	echo "0"
	untar_fils
	mv ${TMP_DIR}/hdzgoggle_app_ota*.tar ${TMP_DIR}/hdzgoggle_app_ota.tar
	cp -f /mnt/app/setting.ini /mnt/UDISK/
	#disable it66021
	i2cset -y 3 0x49 0x10 0xff
	update_rx
	echo "1"
	echo "1" > /tmp/progress_goggle
	update_fpga
	echo "45"
	echo "45" > /tmp/progress_goggle
	hdz_upgrade_app.sh
	echo "100"
	echo "100" > /tmp/progress_goggle
	echo "all done"

else
	if [ `ls /mnt/extsd/HDZERO_GOGGLE*.bin |grep bin |wc -l` == "0" ]
	then
        echo "skip"              
	else
		echo "repeat"
	fi
fi
