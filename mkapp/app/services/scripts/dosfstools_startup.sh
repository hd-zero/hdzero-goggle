#!/bin/sh

ln -sfn /mnt/app/services/dosfstools/fatlabel /bin/fatlabel
ln -sfn /mnt/app/services/dosfstools/fsck.fat /bin/fsck.fat
ln -sfn /mnt/app/services/dosfstools/mkfs.fat /bin/mkfs.fat

# Repair SD Card if needed
/bin/fsck.fat -a /dev/mmcblk0 > /tmp/fsck.log 2>&1
mv /tmp/fsck.log /mnt/extsd/
