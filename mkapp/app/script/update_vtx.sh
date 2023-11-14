#!/bin/sh

VTX_BIN="$1"

gpio_export()
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

gpio_set_reset()
{
        echo "0">/sys/class/gpio/gpio224/value
        echo "1">/sys/class/gpio/gpio228/value
}

gpio_clear_reset()
{
        echo "1">/sys/class/gpio/gpio224/value
        echo "0">/sys/class/gpio/gpio228/value
}

gpio_set_send()
{
        echo "1">/sys/class/gpio/gpio224/value
        echo "0">/sys/class/gpio/gpio228/value
}

# If firmware file was NOT supplied then default to primary location for emergency restore
if [ -z "$VTX_BIN" ]; then
    if [ `ls /mnt/extsd/HDZERO_TX.bin | grep bin | wc -l` -eq 1 ]
    then
        VTX_BIN="/mnt/extsd/HDZERO_TX.bin"
    fi
fi

if [ -e $VTX_BIN ]
then
	echo "Flashing $VTX_BIN"
        gpio_export
        gpio_set_send
        insmod /mnt/app/ko/w25q128.ko
        value=`mtd_debug info /dev/mtd8 | grep mtd.size | grep 1M`

        if [ ! -z "$value" ]
	then
                filesize=`ls -l $VTX_BIN | awk '{print $5}'`
                mtd_debug erase /dev/mtd8 0 65536 > /dev/null
		cp $VTX_BIN /tmp/
		mtd_debug write /dev/mtd8 0 $filesize $VTX_BIN > /dev/null
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

