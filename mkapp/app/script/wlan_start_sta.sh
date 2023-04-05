#!/bin/sh
insmod /mnt/app/ko/xradio_mac.ko
insmod /mnt/app/ko/xradio_core.ko
insmod /mnt/app/ko/xradio_wlan.ko

ifconfig wlan0 up
mkdir /var/log
wpa_supplicant -Dnl80211 -iwlan0 -c/mnt/app/wifi/wpa_supplicant.conf&
ifconfig wlan0 192.168.2.122
route add default gw 192.168.2.1
