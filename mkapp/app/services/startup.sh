#!/bin/sh

if [ ! -f /mnt/app/services/scripts/install/install.done ]; then
	for FILE in $(ls /mnt/app/services/scripts/install/)
	do
		echo "Installing service: $FILE"
		/mnt/app/services/scripts/install/$FILE &
	done
	touch /mnt/app/services/scripts/install/install.done
fi

for FILE in $(ls /mnt/app/services/scripts/runtime/)
do
	echo "Starting service: $FILE"
	/mnt/app/services/scripts/runtime/$FILE &
done
