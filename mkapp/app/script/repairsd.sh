#!/bin/sh

# Define variables for log and done file paths
LOGFILE="/tmp/fsck.log"
DONEFILE="/tmp/repairsd.done"

# Define the device to check
DEVICE="/dev/mmcblk0"

# Start by removing the done file if it exists
rm -rf ${DONEFILE}

# Run fsck on the device, capturing output and exit status
/bin/fsck.fat -a ${DEVICE} > ${LOGFILE} 2>&1
EXITSTATUS=$?

# Add fsck exit status to the log file
echo "fsck exit status: ${EXITSTATUS}" >> ${LOGFILE}

# Depending on the exit status, handle different cases
# For now we will just echo the exit status into the done file and handle the issues in disk.c
# TODO: Talk about different fixes for these issues here in this script
case ${EXITSTATUS} in
    0)
        # No errors were detected, or the errors were corrected without intervention
        echo ${EXITSTATUS} > ${DONEFILE}
        ;;
    1)
        # Errors were found and corrected, but user intervention may be needed
        # You can decide how to handle this case
        echo ${EXITSTATUS} > ${DONEFILE}
        ;;
    2)
        # Errors were found but not corrected. The filesystem may be corrupt.
        # You can decide how to handle this case
        echo ${EXITSTATUS} > ${DONEFILE}
        ;;
    *)
        # Some other exit status was returned. This could indicate a different kind of error.
        # You can decide how to handle this case
        echo ${EXITSTATUS} > ${DONEFILE}
        ;;
esac

# The script will end here. If necessary, you can add commands to perform additional actions,
# like notifying the user of the error, or attempting to mount the filesystem read-only.
