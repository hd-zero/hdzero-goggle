#!/bin/sh

function gpio_export()
{
        echo "224">/sys/class/gpio/export
        echo "228">/sys/class/gpio/export
        echo "131">/sys/class/gpio/export
        echo "258">/sys/class/gpio/export
        echo "out">/sys/class/gpio/gpio224/direction
        echo "out">/sys/class/gpio/gpio228/direction
        echo "out">/sys/class/gpio/gpio131/direction
        echo "out">/sys/class/gpio/gpio258/direction
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
        mtd_debug erase /dev/mtd10 0 16777216
        mtd_debug write /dev/mtd10 0 $filesize2 /mnt/extsd/HDZGOGGLE_VA.bin
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

    valude=`mtd_debug info /dev/mtd8 | grep mtd.size | grep 1M`                
    if [ "$valude" = "" ];then                                     
			beep_on
			usleep 500
			beep_off
			exit 0
		fi
    
    valude=`mtd_debug info /dev/mtd9 | grep mtd.size | grep 1M`                
    if [ "$valude" = "" ];then                                     
			beep_on
			usleep 500
			beep_off
			exit 0
		fi

    mtd_debug erase /dev/mtd8 0 65536
        mtd_debug write /dev/mtd8 0 $filesize /mnt/extsd/HDZGOGGLE_RX.bin
    mtd_debug erase /dev/mtd9 0 65536
        mtd_debug write /dev/mtd9 0 $filesize /mnt/extsd/HDZGOGGLE_RX.bin

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
