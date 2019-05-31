#! /bin/sh
###############################################################################
#
# This script is used for SYNC time between APQ8009 and MCU
#
# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
###############################################################################
set -e

case "$1" in
  start)
        echo -n "Starting transfer time to mcu" > /dev/kmsg
        echo 5f $(cat /sys/devices/soc/soc\:qcom,mcu_time_sync/mcu_time) > /dev/ttyHS0
        echo "done"
        ;;
  stop)
        echo -n "Stopping mcu time sync: "
        ;;
  restart)
        $0 stop
        $0 start
        ;;
  *)
        echo "Usage time_sync.sh { start | stop | restart}" >&2
        exit 1
        ;;
esac

exit 0

