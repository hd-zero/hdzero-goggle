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

DISKDEV=/dev/mmcblk0
BLKDEV=${DISKDEV}p1
if [ ! -b "$BLKDEV" ]; then
    BLKDEV=$DISKDEV
fi

rm -f /tmp/mkfs.result
mkfs.vfat -F 32 "$BLKDEV" -n "HDZERO" > /tmp/mkfs.log 2>&1
RESULT=$?
echo "mkfs result: $RESULT" >> /tmp/mkfs.log
echo $RESULT > /tmp/mkfs.result

# mkfs.vfat only rewrites the filesystem inside the partition; the MBR
# partition type byte is left as it was. SDXC cards (>32GB) ship as exFAT
# with type 0x07 (NTFS/exFAT), so after formatting they hold FAT32 in a
# partition still labelled 0x07. Linux ignores the type byte and mounts
# fine, but macOS picks the filesystem driver from it, tries exFAT, and
# reports "The disk you attached was not readable by this computer".
# Set the type to 0x0C (FAT32 LBA) so the card mounts on every OS.
# Only the 0x07 case is touched; 0xEE (GPT protective MBR) and anything
# already FAT-typed are left alone.
if [ "$RESULT" -eq 0 ] && [ "$BLKDEV" != "$DISKDEV" ]; then
    PTYPE=$(dd if="$DISKDEV" bs=1 skip=450 count=1 2>/dev/null | hexdump -v -e '1/1 "%02x"')
    echo "partition type: 0x$PTYPE" >> /tmp/mkfs.log
    if [ "$PTYPE" = "07" ]; then
        printf '\014' | dd of="$DISKDEV" bs=1 seek=450 count=1 conv=notrunc,fsync >> /tmp/mkfs.log 2>&1
        blockdev --rereadpt "$DISKDEV" >> /tmp/mkfs.log 2>&1
        echo "partition type changed to 0x0c (FAT32 LBA)" >> /tmp/mkfs.log
    fi
fi

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

