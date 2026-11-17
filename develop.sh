#!/bin/sh

# Log file on SD card
LOGFILE=/mnt/extsd/develop.log

# Start logging
echo "=== develop.sh started at $(date) ===" >> $LOGFILE
echo "Checking for SD card executable..." >> $LOGFILE

if [ -e /mnt/extsd/HDZGOGGLE ]; then
	echo "Found /mnt/extsd/HDZGOGGLE - launching from SD card" >> $LOGFILE
	ls -lh /mnt/extsd/HDZGOGGLE >> $LOGFILE 2>&1
	chmod +x /mnt/extsd/HDZGOGGLE >> $LOGFILE 2>&1
	/mnt/extsd/HDZGOGGLE >> $LOGFILE 2>&1 &
	echo "Launched HDZGOGGLE from SD card (PID: $!)" >> $LOGFILE
else
	echo "SD card HDZGOGGLE not found, using built-in /mnt/app/app/HDZGOGGLE" >> $LOGFILE
	/mnt/app/app/HDZGOGGLE &
	echo "Launched built-in HDZGOGGLE (PID: $!)" >> $LOGFILE
fi

echo "=== develop.sh completed ===" >> $LOGFILE