#!/bin/sh

source /mnt/app/app/record/record-env.sh
/mnt/app/app/record/gogglecmd -rec quit
mkfs.vfat -F 32 /dev/mmcblk0
umount /mnt/extsd
mount /dev/mmcblk0 /mnt/extsd
/mnt/app/app/record/record &

