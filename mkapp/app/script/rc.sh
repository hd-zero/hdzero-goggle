#!/bin/sh

function insmod_all()
{
	insmod /mnt/app/ko/gpio_keys_hdzero.ko
	insmod /mnt/app/ko/rotary_encoder.ko
}

function setup_uart()
{
	echo 0x0300B098 0x00775577 > /sys/class/sunxi_dump/write
	echo 0x0300B0D8 0x22777777 > /sys/class/sunxi_dump/write
	echo 0x0300B0fc 0x35517751 > /sys/class/sunxi_dump/write
}

function config_uart_vdpo()
{
	setup_uart
	insmod_all

	dispw -x
	dispw -s vdpo 1080p50
}

function write_flashes()
{
	/mnt/app/script/write_flashes.sh
}

echo "0 4 1 7">/proc/sys/kernel/printk

#auto write HDZERO_TX.bin / HDZERO_RX.bin /HDZERO_VA.bin when bootup if they are exist on SD card
write_flashes

#config UART and VDPO
config_uart_vdpo

#load driver 
insmod /mnt/app//ko/mcp3021.ko
insmod /mnt/app//ko/nct75.ko
#set Microphone Bias Control Register
aww 0x050967c0 0x110e6100

#record process
source /mnt/app//app/record/record-env.sh
if [ -e /mnt/extsd/RECORD.log ]; then
	/mnt/app/app/record/record > /mnt/extsd/RECORD.log 2>&1 &
else
	/mnt/app/app/record/record &
fi
#/mnt/app/app/record/gogglecmd -rec startao
/mnt/app/script/sdstat_log_backup.sh
#/mnt/app/app/record/sdstat &

#system led
/mnt/app/script/system_daemon.sh &

#start applicaion
if [ -e /mnt/extsd/develop.sh ]; then
	/mnt/extsd/develop.sh
else
	if [ -e /mnt/extsd/HDZGOGGLE.log ]; then
		/mnt/app/app/HDZGOGGLE > /mnt/extsd/HDZGOGGLE.log 2>&1 &
	else
		#GLOG_minloglevel=3 /mnt/app/app/HDZGOGGLE &  > /dev/null 2>&1
		/mnt/app/app/HDZGOGGLE &
	fi
fi