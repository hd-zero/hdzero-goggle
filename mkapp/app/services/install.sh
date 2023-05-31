#!/bin/sh

VERSION=""
ARCHIVE=""

install_package()
{
	cd /mnt/extsd
	tar --no-same-owner --warning=no-timestamp -xvf $ARCHIVE
	cd ${ARCHIVE%.*}
	./install.sh
	echo $VERSION > /mnt/app/services/version

	/mnt/app/services/startup.sh &
	
	cd /mnt/extsd
	rm -rf $ARCHIVE ${ARCHIVE%.*}
}

# Acquire the version with the highest revision
for PKG in $(ls -1vr /mnt/extsd/hdzgoggle-services-* 2> /dev/null)
do
	FILE=$(basename $PKG)
	ARCHIVE="/mnt/extsd/$FILE"
	VERSION=$(echo ${FILE%.*} | sed 's/.*-\([0-9\.][0-9\.]*\).*/\1/')

	if [ ! -f /mnt/app/services/version ]; then
		echo "Installing services: $ARCHIVE"
	else
		echo "Replacing services: $(cat /mnt/app/services/version) -> $VERSION"
	fi
	install_package
	break
done

