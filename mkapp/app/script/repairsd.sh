#!/bin/sh

rm -rf /tmp/repairsd.done
/bin/fsck.fat -a /dev/mmcblk0 > /tmp/fsck.log 2>&1
touch /tmp/repairsd.done
