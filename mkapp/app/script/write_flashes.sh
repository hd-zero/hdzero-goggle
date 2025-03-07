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
        if [ ! -f /sys/class/gpio/gpio131/direction ] 
        then                                                          
                echo "131">/sys/class/gpio/export
                echo "out">/sys/class/gpio/gpio131/direction                     
        fi
}

function beep_on()
{
        echo "1">/sys/class/gpio/gpio131/value
}

function beep_off()
{
        echo "0">/sys/class/gpio/gpio131/value
}

function beep()
{
				echo "1">/sys/class/gpio/gpio131/value
				sleep 1
        echo "0">/sys/class/gpio/gpio131/value
}

beep_success()
{
    echo "1">/sys/class/gpio/gpio131/value
		sleep 0.1
    echo "0">/sys/class/gpio/gpio131/value
		sleep 0.5
    echo "1">/sys/class/gpio/gpio131/value
		sleep 0.05
    echo "0">/sys/class/gpio/gpio131/value
}


beep_failure()
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

function gpio_set_send()
{
        echo "1">/sys/class/gpio/gpio224/value
        echo "0">/sys/class/gpio/gpio228/value
}

# eg: check_mtd_write /dev/mtdX check-size erase-size file-size bin-file
check_mtd_write()
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




echo "<<<<-------------------------------------------------------------------->>>>"

if [ -e /mnt/extsd/HDZGOGGLE_VA.bin ]
then
    echo "find VA update file, start update"
        filesize2=`ls -l /mnt/extsd/HDZGOGGLE_VA.bin| awk '{print $5}'`
    gpio_export
    gpio_set_reset
    disconnect_fpga_flash
		sleep 1
    insmod /mnt/app/ko/w25q128.ko


		touch /tmp/update.ing
    #	check_mtd_write /dev/mtd8 16M 16777216 $filesize2 ${TMP_DIR}/HDZGOGGLE_VA.bin
    #	check_mtd_write /dev/mtd9 16M 16777216 $filesize2 ${TMP_DIR}/HDZGOGGLE_VA.bin
	  check_mtd_write /dev/mtd10 16M 16777216 $filesize2 ${TMP_DIR}/HDZGOGGLE_VA.bin
		rm /tmp/update.ing -rf

		if [ ! -f /mnt/extsd/DONOTREMOVE.txt ]
		then
			rm /mnt/extsd/HDZGOGGLE_VA.bin -rf
		fi

    echo "update finish VA, running"
    gpio_clear_reset
	  sleep 1
    rmmod /mnt/app/ko/w25q128.ko
else
    echo "no update file,skip"
fi


if [ -e /mnt/extsd/HDZGOGGLE_RX.bin ]
then
    echo "find RX update file, start update"
        filesize=`ls -l /mnt/extsd/HDZGOGGLE_RX.bin| awk '{print $5}'`
    gpio_export
    gpio_set_reset
		sleep 1
    insmod /mnt/app/ko/w25q128.ko

	  check_mtd_write /dev/mtd8 1M 65536 $filesize ${TMP_DIR}/HDZGOGGLE_RX.bin
	  check_mtd_write /dev/mtd9 1M 65536 $filesize ${TMP_DIR}/HDZGOGGLE_RX.bin
    #	check_mtd_write /dev/mtd10 1M 65536 $filesize ${TMP_DIR}/HDZGOGGLE_RX.bin

		if [ ! -f /mnt/extsd/DONOTREMOVE.txt ]
		then
			rm /mnt/extsd/HDZGOGGLE_RX.bin -rf
		fi

    echo "update finish RX, running"
    gpio_clear_reset
		sleep 1
    rmmod /mnt/app/ko/w25q128.ko
		
		beep
else
    echo "no update file,skip"
fi

echo "<<<<-------------------------------------------------------------------->>>>"