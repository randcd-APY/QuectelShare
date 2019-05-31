#!/bin/sh
#==============================================================================
# FILE: device_configuration.sh
#
# DESCRIPTION:
# Setup device apropriately during boot for various services to work as expected.
#
# Copyright 2013 Qualcomm Technologies, Inc. All rights reserved.
# Qualcomm Technologies Proprietary and Confidential.
#==============================================================================

USERID=`id -u`
if [ ${USERID} -ne 0 ];then
   echo "Setup must be run as root"
   exit 1
fi

echo "Setting up device"
# /dev/socket
if [ ! -d /dev/socket ]; then
   echo "Creating /dev/socket"
   mkdir /dev/socket
fi
chgrp users /dev/socket
chmod g+w /dev/socket

# /data
if [ ! -d /data ]; then
   echo "Creating /data"
   mkdir /data
fi
chgrp users /data
chmod g+w /data

# /data/time
if [ ! -d /data/time ]; then
    echo "Creating /data/time"
    mkdir /data/time
fi
chgrp users /data/time
chmod g+w /data/time

ln -sf /usr/lib/modules /lib

echo "Mounting debugfs"
mkdir -p /d
mount -t debugfs nodev /sys/kernel/debug
ln -sf /sys/kernel/debug /d

echo "Device configuration completed."
exit 0

