#!/bin/sh

if [ -z "$(readlink /bin/ash | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/ash
fi
if [ -z "$(readlink /bin/base64 | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/base64
fi
if [ -z "$(readlink /bin/cat | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/cat
fi
if [ -z "$(readlink /bin/chattr | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/chattr
fi
if [ -z "$(readlink /bin/chgrp | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/chgrp
fi
if [ -z "$(readlink /bin/chmod | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/chmod
fi
if [ -z "$(readlink /bin/chown | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/chown
fi
if [ -z "$(readlink /bin/conspy | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/conspy
fi
if [ -z "$(readlink /bin/cp | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/cp
fi
if [ -z "$(readlink /bin/cpio | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/cpio
fi
if [ -z "$(readlink /bin/cttyhack | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/cttyhack
fi
if [ -z "$(readlink /bin/date | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/date
fi
if [ -z "$(readlink /bin/dd | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/dd
fi
if [ -z "$(readlink /bin/df | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/df
fi
if [ -z "$(readlink /bin/dmesg | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/dmesg
fi
if [ -z "$(readlink /bin/dnsdomainname | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/dnsdomainname
fi
if [ -z "$(readlink /bin/dumpkmap | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/dumpkmap
fi
if [ -z "$(readlink /bin/echo | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/echo
fi
if [ -z "$(readlink /bin/ed | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/ed
fi
if [ -z "$(readlink /bin/egrep | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/egrep
fi
if [ -z "$(readlink /bin/false | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/false
fi
if [ -z "$(readlink /bin/fatattr | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/fatattr
fi
if [ -z "$(readlink /bin/fdflush | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/fdflush
fi
if [ -z "$(readlink /bin/fgrep | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/fgrep
fi
if [ -z "$(readlink /bin/fsync | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/fsync
fi
if [ -z "$(readlink /bin/getopt | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/getopt
fi
if [ -z "$(readlink /bin/grep | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/grep
fi
if [ -z "$(readlink /bin/gunzip | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/gunzip
fi
if [ -z "$(readlink /bin/gzip | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/gzip
fi
if [ -z "$(readlink /bin/hostname | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/hostname
fi
if [ -z "$(readlink /bin/hush | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/hush
fi
if [ -z "$(readlink /bin/ionice | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/ionice
fi
if [ -z "$(readlink /bin/iostat | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/iostat
fi
if [ -z "$(readlink /bin/ipcalc | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/ipcalc
fi
if [ -z "$(readlink /bin/kbd_mode | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/kbd_mode
fi
if [ -z "$(readlink /bin/kill | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/kill
fi
if [ -z "$(readlink /bin/link | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/link
fi
if [ -z "$(readlink /bin/linux32 | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/linux32
fi
if [ -z "$(readlink /bin/linux64 | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/linux64
fi
if [ -z "$(readlink /bin/ln | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/ln
fi
if [ -z "$(readlink /bin/login | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/login
fi
if [ -z "$(readlink /bin/ls | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/ls
fi
if [ -z "$(readlink /bin/lsattr | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/lsattr
fi
if [ -z "$(readlink /bin/lzop | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/lzop
fi
if [ -z "$(readlink /bin/makemime | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/makemime
fi
if [ -z "$(readlink /bin/mkdir | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/mkdir
fi
if [ -z "$(readlink /bin/mknod | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/mknod
fi
if [ -z "$(readlink /bin/mktemp | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/mktemp
fi
if [ -z "$(readlink /bin/more | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/more
fi
if [ -z "$(readlink /bin/mount | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/mount
fi
if [ -z "$(readlink /bin/mountpoint | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/mountpoint
fi
if [ -z "$(readlink /bin/mpstat | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/mpstat
fi
if [ -z "$(readlink /bin/mt | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/mt
fi
if [ -z "$(readlink /bin/mv | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/mv
fi
if [ -z "$(readlink /bin/netstat | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/netstat
fi
if [ -z "$(readlink /bin/nice | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/nice
fi
if [ -z "$(readlink /bin/pidof | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/pidof
fi
if [ -z "$(readlink /bin/ping | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/ping
fi
if [ -z "$(readlink /bin/ping6 | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/ping6
fi
if [ -z "$(readlink /bin/pipe_progress | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/pipe_progress
fi
if [ -z "$(readlink /bin/printenv | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/printenv
fi
if [ -z "$(readlink /bin/ps | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/ps
fi
if [ -z "$(readlink /bin/pwd | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/pwd
fi
if [ -z "$(readlink /bin/reformime | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/reformime
fi
if [ -z "$(readlink /bin/rev | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/rev
fi
if [ -z "$(readlink /bin/rm | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/rm
fi
if [ -z "$(readlink /bin/rmdir | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/rmdir
fi
if [ -z "$(readlink /bin/rpm | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/rpm
fi
if [ -z "$(readlink /bin/run-parts | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/run-parts
fi
if [ -z "$(readlink /bin/scriptreplay | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/scriptreplay
fi
if [ -z "$(readlink /bin/sed | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/sed
fi
if [ -z "$(readlink /bin/setarch | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/setarch
fi
if [ -z "$(readlink /bin/setpriv | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/setpriv
fi
if [ -z "$(readlink /bin/setserial | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/setserial
fi
if [ -z "$(readlink /bin/sh | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/sh
fi
if [ -z "$(readlink /bin/sleep | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/sleep
fi
if [ -z "$(readlink /bin/stat | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/stat
fi
if [ -z "$(readlink /bin/stty | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/stty
fi
if [ -z "$(readlink /bin/su | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/su
fi
if [ -z "$(readlink /bin/sync | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/sync
fi
if [ -z "$(readlink /bin/tar | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/tar
fi
if [ -z "$(readlink /bin/touch | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/touch
fi
if [ -z "$(readlink /bin/true | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/true
fi
if [ -z "$(readlink /bin/umount | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/umount
fi
if [ -z "$(readlink /bin/uname | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/uname
fi
if [ -z "$(readlink /bin/usleep | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/usleep
fi
if [ -z "$(readlink /bin/vi | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/vi
fi
if [ -z "$(readlink /bin/watch | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/watch
fi
if [ -z "$(readlink /bin/zcat | grep "busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /bin/zcat
fi
if [ -z "$(readlink /sbin/acpid | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/acpid
fi
if [ -z "$(readlink /sbin/adjtimex | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/adjtimex
fi
if [ -z "$(readlink /sbin/arp | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/arp
fi
if [ -z "$(readlink /sbin/blkid | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/blkid
fi
if [ -z "$(readlink /sbin/blockdev | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/blockdev
fi
if [ -z "$(readlink /sbin/bootchartd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/bootchartd
fi
if [ -z "$(readlink /sbin/depmod | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/depmod
fi
if [ -z "$(readlink /sbin/devmem | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/devmem
fi
if [ -z "$(readlink /sbin/fbsplash | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/fbsplash
fi
if [ -z "$(readlink /sbin/fdisk | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/fdisk
fi
if [ -z "$(readlink /sbin/findfs | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/findfs
fi
if [ -z "$(readlink /sbin/freeramdisk | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/freeramdisk
fi
if [ -z "$(readlink /sbin/fsck | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/fsck
fi
if [ -z "$(readlink /sbin/fsck.minix | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/fsck.minix
fi
if [ -z "$(readlink /sbin/fstrim | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/fstrim
fi
if [ -z "$(readlink /sbin/getty | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/getty
fi
if [ -z "$(readlink /sbin/halt | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/halt
fi
if [ -z "$(readlink /sbin/hdparm | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/hdparm
fi
if [ -z "$(readlink /sbin/hwclock | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/hwclock
fi
if [ -z "$(readlink /sbin/ifconfig | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/ifconfig
fi
if [ -z "$(readlink /sbin/ifdown | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/ifdown
fi
if [ -z "$(readlink /sbin/ifenslave | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/ifenslave
fi
if [ -z "$(readlink /sbin/ifup | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/ifup
fi
if [ -z "$(readlink /sbin/init | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/init
fi
if [ -z "$(readlink /sbin/insmod | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/insmod
fi
if [ -z "$(readlink /sbin/ip | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/ip
fi
if [ -z "$(readlink /sbin/ipaddr | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/ipaddr
fi
if [ -z "$(readlink /sbin/iplink | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/iplink
fi
if [ -z "$(readlink /sbin/ipneigh | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/ipneigh
fi
if [ -z "$(readlink /sbin/iproute | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/iproute
fi
if [ -z "$(readlink /sbin/iprule | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/iprule
fi
if [ -z "$(readlink /sbin/iptunnel | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/iptunnel
fi
if [ -z "$(readlink /sbin/klogd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/klogd
fi
if [ -z "$(readlink /sbin/loadkmap | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/loadkmap
fi
if [ -z "$(readlink /sbin/logread | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/logread
fi
if [ -z "$(readlink /sbin/losetup | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/losetup
fi
if [ -z "$(readlink /sbin/lsmod | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/lsmod
fi
if [ -z "$(readlink /sbin/makedevs | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/makedevs
fi
if [ -z "$(readlink /sbin/mdev | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/mdev
fi
if [ -z "$(readlink /sbin/mkdosfs | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/mkdosfs
fi
if [ -z "$(readlink /sbin/mke2fs | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/mke2fs
fi
if [ -z "$(readlink /sbin/mkfs.ext2 | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/mkfs.ext2
fi
if [ -z "$(readlink /sbin/mkfs.minix | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/mkfs.minix
fi
if [ -z "$(readlink /sbin/mkfs.vfat | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/mkfs.vfat
fi
if [ -z "$(readlink /sbin/mkswap | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/mkswap
fi
if [ -z "$(readlink /sbin/modinfo | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/modinfo
fi
if [ -z "$(readlink /sbin/modprobe | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/modprobe
fi
if [ -z "$(readlink /sbin/nameif | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/nameif
fi
if [ -z "$(readlink /sbin/pivot_root | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/pivot_root
fi
if [ -z "$(readlink /sbin/poweroff | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/poweroff
fi
if [ -z "$(readlink /sbin/raidautorun | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/raidautorun
fi
if [ -z "$(readlink /sbin/reboot | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/reboot
fi
if [ -z "$(readlink /sbin/rmmod | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/rmmod
fi
if [ -z "$(readlink /sbin/route | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/route
fi
if [ -z "$(readlink /sbin/runlevel | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/runlevel
fi
if [ -z "$(readlink /sbin/setconsole | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/setconsole
fi
if [ -z "$(readlink /sbin/slattach | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/slattach
fi
if [ -z "$(readlink /sbin/start-stop-daemon | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/start-stop-daemon
fi
if [ -z "$(readlink /sbin/sulogin | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/sulogin
fi
if [ -z "$(readlink /sbin/swapoff | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/swapoff
fi
if [ -z "$(readlink /sbin/swapon | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/swapon
fi
if [ -z "$(readlink /sbin/switch_root | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/switch_root
fi
if [ -z "$(readlink /sbin/sysctl | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/sysctl
fi
if [ -z "$(readlink /sbin/syslogd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/syslogd
fi
if [ -z "$(readlink /sbin/tunctl | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/tunctl
fi
if [ -z "$(readlink /sbin/udhcpc | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/udhcpc
fi
if [ -z "$(readlink /sbin/uevent | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/uevent
fi
if [ -z "$(readlink /sbin/vconfig | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/vconfig
fi
if [ -z "$(readlink /sbin/watchdog | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/watchdog
fi
if [ -z "$(readlink /sbin/zcip | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /sbin/zcip
fi
if [ -z "$(readlink /usr/bin/[ | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/[
fi
if [ -z "$(readlink /usr/bin/[[ | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/[[
fi
if [ -z "$(readlink /usr/bin/awk | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/awk
fi
if [ -z "$(readlink /usr/bin/basename | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/basename
fi
if [ -z "$(readlink /usr/bin/beep | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/beep
fi
if [ -z "$(readlink /usr/bin/blkdiscard | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/blkdiscard
fi
if [ -z "$(readlink /usr/bin/bunzip2 | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/bunzip2
fi
if [ -z "$(readlink /usr/bin/bzcat | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/bzcat
fi
if [ -z "$(readlink /usr/bin/bzip2 | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/bzip2
fi
if [ -z "$(readlink /usr/bin/cal | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/cal
fi
if [ -z "$(readlink /usr/bin/chpst | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/chpst
fi
if [ -z "$(readlink /usr/bin/chrt | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/chrt
fi
if [ -z "$(readlink /usr/bin/chvt | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/chvt
fi
if [ -z "$(readlink /usr/bin/cksum | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/cksum
fi
if [ -z "$(readlink /usr/bin/clear | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/clear
fi
if [ -z "$(readlink /usr/bin/cmp | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/cmp
fi
if [ -z "$(readlink /usr/bin/comm | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/comm
fi
if [ -z "$(readlink /usr/bin/crontab | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/crontab
fi
if [ -z "$(readlink /usr/bin/cryptpw | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/cryptpw
fi
if [ -z "$(readlink /usr/bin/cut | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/cut
fi
if [ -z "$(readlink /usr/bin/dc | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/dc
fi
if [ -z "$(readlink /usr/bin/deallocvt | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/deallocvt
fi
if [ -z "$(readlink /usr/bin/diff | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/diff
fi
if [ -z "$(readlink /usr/bin/dirname | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/dirname
fi
if [ -z "$(readlink /usr/bin/dos2unix | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/dos2unix
fi
if [ -z "$(readlink /usr/bin/dpkg | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/dpkg
fi
if [ -z "$(readlink /usr/bin/dpkg-deb | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/dpkg-deb
fi
if [ -z "$(readlink /usr/bin/du | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/du
fi
if [ -z "$(readlink /usr/bin/dumpleases | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/dumpleases
fi
if [ -z "$(readlink /usr/bin/eject | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/eject
fi
if [ -z "$(readlink /usr/bin/env | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/env
fi
if [ -z "$(readlink /usr/bin/envdir | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/envdir
fi
if [ -z "$(readlink /usr/bin/envuidgid | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/envuidgid
fi
if [ -z "$(readlink /usr/bin/expand | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/expand
fi
if [ -z "$(readlink /usr/bin/expr | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/expr
fi
if [ -z "$(readlink /usr/bin/factor | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/factor
fi
if [ -z "$(readlink /usr/bin/fallocate | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/fallocate
fi
if [ -z "$(readlink /usr/bin/fgconsole | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/fgconsole
fi
if [ -z "$(readlink /usr/bin/find | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/find
fi
if [ -z "$(readlink /usr/bin/flock | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/flock
fi
if [ -z "$(readlink /usr/bin/fold | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/fold
fi
if [ -z "$(readlink /usr/bin/free | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/free
fi
if [ -z "$(readlink /usr/bin/ftpget | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/ftpget
fi
if [ -z "$(readlink /usr/bin/ftpput | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/ftpput
fi
if [ -z "$(readlink /usr/bin/fuser | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/fuser
fi
if [ -z "$(readlink /usr/bin/groups | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/groups
fi
if [ -z "$(readlink /usr/bin/hd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/hd
fi
if [ -z "$(readlink /usr/bin/head | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/head
fi
if [ -z "$(readlink /usr/bin/hexdump | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/hexdump
fi
if [ -z "$(readlink /usr/bin/hostid | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/hostid
fi
if [ -z "$(readlink /usr/bin/id | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/id
fi
if [ -z "$(readlink /usr/bin/install | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/install
fi
if [ -z "$(readlink /usr/bin/ipcrm | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/ipcrm
fi
if [ -z "$(readlink /usr/bin/ipcs | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/ipcs
fi
if [ -z "$(readlink /usr/bin/killall | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/killall
fi
if [ -z "$(readlink /usr/bin/last | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/last
fi
if [ -z "$(readlink /usr/bin/less | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/less
fi
if [ -z "$(readlink /usr/bin/logger | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/logger
fi
if [ -z "$(readlink /usr/bin/logname | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/logname
fi
if [ -z "$(readlink /usr/bin/lpq | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/lpq
fi
if [ -z "$(readlink /usr/bin/lpr | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/lpr
fi
if [ -z "$(readlink /usr/bin/lsof | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/lsof
fi
if [ -z "$(readlink /usr/bin/lspci | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/lspci
fi
if [ -z "$(readlink /usr/bin/lsscsi | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/lsscsi
fi
if [ -z "$(readlink /usr/bin/lsusb | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/lsusb
fi
if [ -z "$(readlink /usr/bin/lzcat | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/lzcat
fi
if [ -z "$(readlink /usr/bin/lzma | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/lzma
fi
if [ -z "$(readlink /usr/bin/man | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/man
fi
if [ -z "$(readlink /usr/bin/md5sum | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/md5sum
fi
if [ -z "$(readlink /usr/bin/mesg | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/mesg
fi
if [ -z "$(readlink /usr/bin/microcom | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/microcom
fi
if [ -z "$(readlink /usr/bin/mkfifo | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/mkfifo
fi
if [ -z "$(readlink /usr/bin/mkpasswd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/mkpasswd
fi
if [ -z "$(readlink /usr/bin/nc | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/nc
fi
if [ -z "$(readlink /usr/bin/nl | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/nl
fi
if [ -z "$(readlink /usr/bin/nmeter | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/nmeter
fi
if [ -z "$(readlink /usr/bin/nohup | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/nohup
fi
if [ -z "$(readlink /usr/bin/nproc | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/nproc
fi
if [ -z "$(readlink /usr/bin/nsenter | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/nsenter
fi
if [ -z "$(readlink /usr/bin/nslookup | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/nslookup
fi
if [ -z "$(readlink /usr/bin/od | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/od
fi
if [ -z "$(readlink /usr/bin/openvt | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/openvt
fi
if [ -z "$(readlink /usr/bin/passwd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/passwd
fi
if [ -z "$(readlink /usr/bin/paste | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/paste
fi
if [ -z "$(readlink /usr/bin/patch | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/patch
fi
if [ -z "$(readlink /usr/bin/pgrep | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/pgrep
fi
if [ -z "$(readlink /usr/bin/pkill | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/pkill
fi
if [ -z "$(readlink /usr/bin/pmap | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/pmap
fi
if [ -z "$(readlink /usr/bin/printf | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/printf
fi
if [ -z "$(readlink /usr/bin/pscan | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/pscan
fi
if [ -z "$(readlink /usr/bin/pstree | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/pstree
fi
if [ -z "$(readlink /usr/bin/pwdx | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/pwdx
fi
if [ -z "$(readlink /usr/bin/readlink | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/readlink
fi
if [ -z "$(readlink /usr/bin/realpath | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/realpath
fi
if [ -z "$(readlink /usr/bin/renice | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/renice
fi
if [ -z "$(readlink /usr/bin/reset | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/reset
fi
if [ -z "$(readlink /usr/bin/resize | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/resize
fi
if [ -z "$(readlink /usr/bin/rpm2cpio | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/rpm2cpio
fi
if [ -z "$(readlink /usr/bin/runsv | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/runsv
fi
if [ -z "$(readlink /usr/bin/runsvdir | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/runsvdir
fi
if [ -z "$(readlink /usr/bin/rx | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/rx
fi
if [ -z "$(readlink /usr/bin/script | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/script
fi
if [ -z "$(readlink /usr/bin/seq | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/seq
fi
if [ -z "$(readlink /usr/bin/setkeycodes | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/setkeycodes
fi
if [ -z "$(readlink /usr/bin/setsid | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/setsid
fi
if [ -z "$(readlink /usr/bin/setuidgid | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/setuidgid
fi
if [ -z "$(readlink /usr/bin/sha1sum | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/sha1sum
fi
if [ -z "$(readlink /usr/bin/sha256sum | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/sha256sum
fi
if [ -z "$(readlink /usr/bin/sha3sum | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/sha3sum
fi
if [ -z "$(readlink /usr/bin/sha512sum | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/sha512sum
fi
if [ -z "$(readlink /usr/bin/showkey | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/showkey
fi
if [ -z "$(readlink /usr/bin/shred | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/shred
fi
if [ -z "$(readlink /usr/bin/shuf | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/shuf
fi
if [ -z "$(readlink /usr/bin/smemcap | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/smemcap
fi
if [ -z "$(readlink /usr/bin/softlimit | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/softlimit
fi
if [ -z "$(readlink /usr/bin/sort | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/sort
fi
if [ -z "$(readlink /usr/bin/split | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/split
fi
if [ -z "$(readlink /usr/bin/ssl_client | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/ssl_client
fi
if [ -z "$(readlink /usr/bin/strings | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/strings
fi
if [ -z "$(readlink /usr/bin/sum | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/sum
fi
if [ -z "$(readlink /usr/bin/sv | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/sv
fi
if [ -z "$(readlink /usr/bin/svc | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/svc
fi
if [ -z "$(readlink /usr/bin/tac | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/tac
fi
if [ -z "$(readlink /usr/bin/tail | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/tail
fi
if [ -z "$(readlink /usr/bin/taskset | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/taskset
fi
if [ -z "$(readlink /usr/bin/tcpsvd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/tcpsvd
fi
if [ -z "$(readlink /usr/bin/tee | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/tee
fi
if [ -z "$(readlink /usr/bin/telnet | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/telnet
fi
if [ -z "$(readlink /usr/bin/test | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/test
fi
if [ -z "$(readlink /usr/bin/tftp | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/tftp
fi
if [ -z "$(readlink /usr/bin/time | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/time
fi
if [ -z "$(readlink /usr/bin/timeout | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/timeout
fi
if [ -z "$(readlink /usr/bin/top | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/top
fi
if [ -z "$(readlink /usr/bin/tr | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/tr
fi
if [ -z "$(readlink /usr/bin/traceroute | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/traceroute
fi
if [ -z "$(readlink /usr/bin/traceroute6 | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/traceroute6
fi
if [ -z "$(readlink /usr/bin/truncate | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/truncate
fi
if [ -z "$(readlink /usr/bin/tty | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/tty
fi
if [ -z "$(readlink /usr/bin/ttysize | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/ttysize
fi
if [ -z "$(readlink /usr/bin/udpsvd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/udpsvd
fi
if [ -z "$(readlink /usr/bin/unexpand | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/unexpand
fi
if [ -z "$(readlink /usr/bin/uniq | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/uniq
fi
if [ -z "$(readlink /usr/bin/unix2dos | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/unix2dos
fi
if [ -z "$(readlink /usr/bin/unlink | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/unlink
fi
if [ -z "$(readlink /usr/bin/unlzma | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/unlzma
fi
if [ -z "$(readlink /usr/bin/unshare | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/unshare
fi
if [ -z "$(readlink /usr/bin/unxz | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/unxz
fi
if [ -z "$(readlink /usr/bin/unzip | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/unzip
fi
if [ -z "$(readlink /usr/bin/uptime | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/uptime
fi
if [ -z "$(readlink /usr/bin/users | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/users
fi
if [ -z "$(readlink /usr/bin/uudecode | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/uudecode
fi
if [ -z "$(readlink /usr/bin/uuencode | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/uuencode
fi
if [ -z "$(readlink /usr/bin/vlock | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/vlock
fi
if [ -z "$(readlink /usr/bin/volname | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/volname
fi
if [ -z "$(readlink /usr/bin/w | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/w
fi
if [ -z "$(readlink /usr/bin/wall | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/wall
fi
if [ -z "$(readlink /usr/bin/wc | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/wc
fi
if [ -z "$(readlink /usr/bin/wget | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/wget
fi
if [ -z "$(readlink /usr/bin/which | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/which
fi
if [ -z "$(readlink /usr/bin/who | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/who
fi
if [ -z "$(readlink /usr/bin/whoami | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/whoami
fi
if [ -z "$(readlink /usr/bin/whois | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/whois
fi
if [ -z "$(readlink /usr/bin/xargs | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/xargs
fi
if [ -z "$(readlink /usr/bin/xxd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/xxd
fi
if [ -z "$(readlink /usr/bin/xz | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/xz
fi
if [ -z "$(readlink /usr/bin/xzcat | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/xzcat
fi
if [ -z "$(readlink /usr/bin/yes | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/bin/yes
fi
if [ -z "$(readlink /usr/sbin/addgroup | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/addgroup
fi
if [ -z "$(readlink /usr/sbin/add-shell | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/add-shell
fi
if [ -z "$(readlink /usr/sbin/adduser | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/adduser
fi
if [ -z "$(readlink /usr/sbin/arping | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/arping
fi
if [ -z "$(readlink /usr/sbin/brctl | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/brctl
fi
if [ -z "$(readlink /usr/sbin/chat | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/chat
fi
if [ -z "$(readlink /usr/sbin/chpasswd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/chpasswd
fi
if [ -z "$(readlink /usr/sbin/chroot | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/chroot
fi
if [ -z "$(readlink /usr/sbin/crond | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/crond
fi
if [ -z "$(readlink /usr/sbin/delgroup | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/delgroup
fi
if [ -z "$(readlink /usr/sbin/deluser | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/deluser
fi
if [ -z "$(readlink /usr/sbin/dhcprelay | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/dhcprelay
fi
if [ -z "$(readlink /usr/sbin/dnsd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/dnsd
fi
if [ -z "$(readlink /usr/sbin/ether-wake | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/ether-wake
fi
if [ -z "$(readlink /usr/sbin/fakeidentd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/fakeidentd
fi
if [ -z "$(readlink /usr/sbin/fbset | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/fbset
fi
if [ -z "$(readlink /usr/sbin/fdformat | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/fdformat
fi
if [ -z "$(readlink /usr/sbin/fsfreeze | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/fsfreeze
fi
if [ -z "$(readlink /usr/sbin/ftpd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/ftpd
fi
if [ -z "$(readlink /usr/sbin/httpd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/httpd
fi
if [ -z "$(readlink /usr/sbin/i2cdetect | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/i2cdetect
fi
if [ -z "$(readlink /usr/sbin/i2cdump | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/i2cdump
fi
if [ -z "$(readlink /usr/sbin/i2cget | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/i2cget
fi
if [ -z "$(readlink /usr/sbin/i2cset | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/i2cset
fi
if [ -z "$(readlink /usr/sbin/ifplugd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/ifplugd
fi
if [ -z "$(readlink /usr/sbin/inetd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/inetd
fi
if [ -z "$(readlink /usr/sbin/killall5 | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/killall5
fi
if [ -z "$(readlink /usr/sbin/loadfont | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/loadfont
fi
if [ -z "$(readlink /usr/sbin/lpd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/lpd
fi
if [ -z "$(readlink /usr/sbin/nanddump | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/nanddump
fi
if [ -z "$(readlink /usr/sbin/nandwrite | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/nandwrite
fi
if [ -z "$(readlink /usr/sbin/nbd-client | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/nbd-client
fi
if [ -z "$(readlink /usr/sbin/ntpd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/ntpd
fi
if [ -z "$(readlink /usr/sbin/partprobe | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/partprobe
fi
if [ -z "$(readlink /usr/sbin/popmaildir | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/popmaildir
fi
if [ -z "$(readlink /usr/sbin/powertop | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/powertop
fi
if [ -z "$(readlink /usr/sbin/rdate | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/rdate
fi
if [ -z "$(readlink /usr/sbin/rdev | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/rdev
fi
if [ -z "$(readlink /usr/sbin/readahead | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/readahead
fi
if [ -z "$(readlink /usr/sbin/readprofile | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/readprofile
fi
if [ -z "$(readlink /usr/sbin/remove-shell | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/remove-shell
fi
if [ -z "$(readlink /usr/sbin/rtcwake | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/rtcwake
fi
if [ -z "$(readlink /usr/sbin/sendmail | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/sendmail
fi
if [ -z "$(readlink /usr/sbin/setfont | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/setfont
fi
if [ -z "$(readlink /usr/sbin/setlogcons | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/setlogcons
fi
if [ -z "$(readlink /usr/sbin/svlogd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/svlogd
fi
if [ -z "$(readlink /usr/sbin/telnetd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/telnetd
fi
if [ -z "$(readlink /usr/sbin/tftpd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/tftpd
fi
if [ -z "$(readlink /usr/sbin/ubiattach | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/ubiattach
fi
if [ -z "$(readlink /usr/sbin/ubidetach | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/ubidetach
fi
if [ -z "$(readlink /usr/sbin/ubimkvol | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/ubimkvol
fi
if [ -z "$(readlink /usr/sbin/ubirename | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/ubirename
fi
if [ -z "$(readlink /usr/sbin/ubirmvol | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/ubirmvol
fi
if [ -z "$(readlink /usr/sbin/ubirsvol | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/ubirsvol
fi
if [ -z "$(readlink /usr/sbin/ubiupdatevol | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/ubiupdatevol
fi
if [ -z "$(readlink /usr/sbin/udhcpd | grep "bin/busybox")" ]; then
    ln -sfn /mnt/app/services/busybox/busybox /usr/sbin/udhcpd
fi

