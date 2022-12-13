#!/bin/sh

function  gpio_export()
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

if [ -e /mnt/extsd/HDZERO_TX.bin ]                                                                                                                                                                                                            
then                                                                                                                                                                                                                                           
        gpio_export                                                                                                                                                                                                                            
        gpio_set_send            
        insmod /mnt/app/ko/w25q128.ko
        valude=`mtd_debug info /dev/mtd8 | grep mtd.size | grep 1M`                                                                                                                                                                            
        if [ "$valude" != "" ];then                                                                                                                                                                                                            
                filesize=`ls -l /mnt/extsd/HDZERO_TX.bin| awk '{print $5}'`
                mtd_debug erase /dev/mtd8 0 65536 > /dev/null
		cp /mnt/extsd/HDZERO_TX.bin /tmp/.	                
		mtd_debug write /dev/mtd8 0 $filesize /mnt/extsd/HDZERO_TX.bin > /dev/null
		mtd_debug read /dev/mtd8 0 $filesize /tmp/HDZERO_TX_RB.bin > /dev/null                                 
		echo "all done"
	else
		echo "detect device failed"
        fi                                                                                                                                                                                                                                     
        gpio_clear_reset                                                                                                                                                                                                                       
	sleep 1
        rmmod w25q128.ko                                                                                                                                                                                                           
else
        echo "skip"              
fi 

