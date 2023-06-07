#!/bin/sh

source /mnt/app/app/record/record-env.sh
/mnt/app/app/record/gogglecmd -rec quit
/mnt/app/app/record/gogglecmd -sds quit
sleep 2

echo "Umounting SD Card"
umount /mnt/extsd
if [ $? -eq 0 ]; then
    echo "Umounting SD Card: SUCCESS"
else
    echo "Umounting SD Card: FAILURE"
fi

rm -f /tmp/fsck.result
/bin/fsck.fat -y /dev/mmcblk0 > /tmp/fsck.log 2>&1
RESULT=$?
echo "fsck result: $RESULT" >> /tmp/fsck.log
echo $RESULT > /tmp/fsck.result

echo "Mounting SD Card"
mount /dev/mmcblk0 /mnt/extsd
if [ $? -eq 0 ]; then
    echo "Mounting SD Card: SUCCESS"
else
    echo "Mounting SD Card: FAILURE"
fi
sleep 1

/mnt/app/app/record/record &
/mnt/app/app/record/sdstat &
