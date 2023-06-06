#!/bin/sh

source /mnt/app/app/record/record-env.sh
/mnt/app/app/record/gogglecmd -rec quit
/mnt/app/app/record/gogglecmd -sds quit
sleep 2

umount /mnt/extsd
rm -f /tmp/fsck.result
/bin/fsck.fat -y /dev/mmcblk0 > /tmp/fsck.log 2>&1
RESULT=$?
echo "fsck result: $RESULT" >> /tmp/fsck.log
echo $RESULT > /tmp/fsck.result

mount /dev/mmcblk0 /mnt/extsd
sleep 1

/mnt/app/app/record/record &
/mnt/app/app/record/sdstat &
