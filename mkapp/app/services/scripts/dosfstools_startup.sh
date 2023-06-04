#!/bin/sh

ln -sfn /mnt/app/services/dosfstools/fatlabel /bin/fatlabel
ln -sfn /mnt/app/services/dosfstools/fsck.fat /bin/fsck.fat
ln -sfn /mnt/app/services/dosfstools/mkfs.fat /bin/mkfs.fat

# Generate new rule if it does not exist.
if [ ! -e /etc/udev/rules.d/11-extsd-v1.rules ]; then
    rm -rf /etc/udev/rules.d/11-extsd-*

    echo "ACTION==\"add\", KERNEL==\"mmcblk[0-9]\", SUBSYSTEM==\"block\", RUN+=\"/mnt/app/script/repairsd.sh\"" > /etc/udev/rules.d/11-extsd-v1.rules
    echo "ACTION==\"add\", KERNEL==\"mmcblk[0-9]p[0-9]\", SUBSYSTEM==\"block\", RUN+=\"/mnt/app/script/repairsd.sh\"" >> /etc/udev/rules.d/11-extsd-v1.rules

    udevadm control --reload-rules
fi

