#!/bin/sh

function gpio_export()
{
	if [ ! -f /sys/class/gpio/gpio134/direction ]
	then
	        echo "134">/sys/class/gpio/export
	        echo "out">/sys/class/gpio/gpio134/direction
	fi
}
gpio_export

while true
do
	echo "0">/sys/class/gpio/gpio134/value
	sleep 1
	echo "1">/sys/class/gpio/gpio134/value
	sleep 1
done
