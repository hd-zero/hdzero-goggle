#!/bin/sh

source /mnt/app/app/record/record-env.sh
/mnt/app/app/record/gogglecmd -rec quit
/mnt/app/app/record/gogglecmd -sds quit

if [ -e /mnt/extsd/resource ]; then
    cp -r /mnt/extsd/resource /tmp/
fi
sleep 2

echo "Umounting SD Card"
umount /mnt/extsd
if [ $? -eq 0 ]; then
    echo "Umounting SD Card: SUCCESS"
else
    echo "Umounting SD Card: FAILURE"
fi

BLKDEV=/dev/mmcblk0p1
if [ ! -b "$BLKDEV" ]; then
    BLKDEV=/dev/mmcblk0
fi

rm -f /tmp/mkfs.result
mkfs.vfat -F 32 "$BLKDEV" -n "HDZERO" > /tmp/mkfs.log 2>&1
RESULT=$?
echo "mkfs result: $RESULT" >> /tmp/mkfs.log
echo $RESULT > /tmp/mkfs.result

echo "Mounting SD Card"
mount "$BLKDEV" /mnt/extsd
if [ $? -eq 0 ]; then
    echo "Mounting SD Card: SUCCESS"
else
    echo "Mounting SD Card: FAILURE"
fi
sleep 1

if [ -e /tmp/resource ]; then
    cp -r /tmp/resource /mnt/extsd/
    rm -rf /tmp/resource
fi

/mnt/app/app/record/record &
/mnt/app/app/record/sdstat &

