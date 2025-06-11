#!/bin/sh

PLATFORM="$(cat /mnt/app/platform)"
VAbin=/mnt/extsd/${PLATFORM}_VA.bin
RXbin=/mnt/extsd/${PLATFORM}_RX.bin
VAcount=1
VAwrites=0
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

function disconnect_fpga_flash()
{
	echo "1">/sys/class/gpio/gpio258/value
}

function connect_fpga_flash()
{
	echo "0">/sys/class/gpio/gpio258/value
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
			beep_success
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

echo "<<<<-------------------------------------------------------------------->>>>"

if [ -e $VAbin ] | [ -e $RXbin ]; then 
	gpio_export
 	beep 1
fi

if [ -e $VAbin ]
then
    echo "find VA update file, start update"
    gpio_set_reset
    disconnect_fpga_flash
	sleep 1
    insmod /mnt/app/ko/w25q128.ko

	touch /tmp/update.ing
    # normally the VA is mounted at /dev/mtd10 
    #	check_mtd_write /dev/mtd8 16M $VAbin
    #	check_mtd_write /dev/mtd9 16M $VAbin
	check_mtd_write /dev/mtd10 16M $VAbin
	rm /tmp/update.ing -rf

	if [ "$VAwrites" == "$VAcount" ]; then
		if [ ! -f /mnt/extsd/DONOTREMOVE.txt ]; then
			rm $VAbin -rf
		fi
	fi

    echo "update finish $VAwrites VA, running"
    gpio_clear_reset
	sleep 1
    rmmod /mnt/app/ko/w25q128.ko
else
    echo "no VA update file,skip" 
fi

if [ -e $RXbin ]; then
    echo "find RX update file, start update"
    gpio_set_reset
	sleep 1
    insmod /mnt/app/ko/w25q128.ko
    # normally rx1 is /dev/mtd8 and rx0 is /dev/mtd9
	check_mtd_write /dev/mtd8 1M $RXbin
	check_mtd_write /dev/mtd9 1M $RXbin
    # check_mtd_write /dev/mtd10 1M $RXbin

	if [ "$RXwrites" == "$RXcount" ]; then
		if [ ! -f /mnt/extsd/DONOTREMOVE.txt ]; then
			rm $RXbin -rf
		fi
	fi

    echo "update finish $RXwrites RX, running"
    gpio_clear_reset
	sleep 1
    rmmod /mnt/app/ko/w25q128.ko
	beep 2
else
    echo "no RX update file,skip"
fi

echo "<<<<-------------------------------------------------------------------->>>>"
