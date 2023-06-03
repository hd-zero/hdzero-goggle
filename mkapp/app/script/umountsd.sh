#!/bin/sh

source /mnt/app/app/record/record-env.sh
/mnt/app/app/record/gogglecmd -rec quit
/mnt/app/app/record/gogglecmd -sds quit

if [ -e /mnt/extsd/resource ]; then
    cp -r /mnt/extsd/resource /tmp/
fi

sleep 2
umount /mnt/extsd
sleep 1

