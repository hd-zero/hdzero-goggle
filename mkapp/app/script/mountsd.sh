#!/bin/sh

mount /dev/mmcblk0 /mnt/extsd
sleep 1

if [ -e /tmp/resource ]; then
    cp -r /tmp/resource /mnt/extsd/
    rm -rf /tmp/resource
fi

/mnt/app/app/record/record &
/mnt/app/app/record/sdstat &
