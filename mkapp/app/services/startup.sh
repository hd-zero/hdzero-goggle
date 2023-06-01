#!/bin/sh

for FILE in $(ls /mnt/app/services/scripts/)
do
	echo "Starting service: $FILE"
	/mnt/app/services/scripts/$FILE &
done
