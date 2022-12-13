#!/bin/sh

if [ -f /tmp/progress_vtx ]
then
	cat /tmp/progress_vtx
else
	echo "0"
fi

if [ -f /tmp/progress_goggle ]
then
	cat /tmp/progress_goggle
else
	echo "0"
fi
