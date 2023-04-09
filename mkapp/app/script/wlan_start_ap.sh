#!/bin/sh
insmod /mnt/app/ko/xradio_mac.ko
insmod /mnt/app/ko/xradio_core.ko
insmod /mnt/app/ko/xradio_wlan.ko

ifconfig wlan0 192.168.2.122 netmask 255.255.255.0 up
hostapd /tmp/hostapd.conf&
udhcpd /mnt/app/wifi/udhcpd.conf&
route add default gw 192.168.2.1
/mnt/app/app/record/rtspLive&
