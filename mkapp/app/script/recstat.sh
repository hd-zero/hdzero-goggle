#!/bin/sh

if [ $# -gt 0 ];
then
        i=1
        while [ $i -gt 0 ]
        do
                dataline=$(cat /tmp/record.dat)
                echo "$i: $dataline"
                sleep 3
                let i++
        done
else
        for i in $(seq 1 30)
        do
                dataline=$(cat /tmp/record.dat)
                echo "$i: $dataline"
                sleep 3
        done
fi

#reboot