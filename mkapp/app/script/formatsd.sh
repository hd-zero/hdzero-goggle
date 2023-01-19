#!/bin/sh

source /mnt/app/app/record/record-env.sh
/mnt/app/app/record/gogglecmd -rec quit
/mnt/app/app/record/gogglecmd -sds quit

if [ -e /mnt/extsd/resource ]; then
    cp -r /mnt/extsd/resource /tmp/
fi

sleep 2
umount /mnt/extsd
mkfs.vfat -F 32 /dev/mmcblk0
mount /dev/mmcblk0 /mnt/extsd
sleep 1

if [ -e /tmp/resource ]; then
    cp -r /tmp/resource /mnt/extsd/
    rm -rf /tmp/resource
fi

/mnt/app/app/record/record &
/mnt/app/app/record/sdstat &

