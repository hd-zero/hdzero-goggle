#!/bin/sh

/mnt/app/script/umountsd.sh
mkfs.vfat -F 32 /dev/mmcblk0
/mnt/app/script/mountsd.sh

