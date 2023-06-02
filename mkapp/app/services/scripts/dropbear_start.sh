#!/bin/sh

ln -sfn /mnt/app/services/dropbear/dropbearmulti /bin/dropbear
ln -sfn /mnt/app/services/dropbear/dropbearmulti /bin/scp
ln -sfn /mnt/app/services/dropbear/dropbearmulti /bin/ssh
ln -sfn /mnt/app/services/dropbear/dropbearmulti /bin/dropbearkey

# Generate host key once if it does not exist.
if [ ! -e /etc/dropbear/dropbear_rsa_host_key ]; then
    mkdir -p /etc/dropbear
    /bin/dropbearkey -t rsa -f /etc/dropbear/dropbear_rsa_host_key
fi

dropbear
