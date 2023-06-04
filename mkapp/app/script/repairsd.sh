#!/bin/sh

/bin/fsck.fat -a /dev/mmcblk0 > /tmp/fsck.log 2>&1
