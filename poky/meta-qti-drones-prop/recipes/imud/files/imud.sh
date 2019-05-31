#! /bin/sh
###############################################################################
#
# This script is used for starting imud daemon
#
# Copyright (c) 2018 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#
###############################################################################
set -e

case "$1" in
  start)
        echo "Starting imud daemon: " > /dev/kmsg
        start-stop-daemon -S -b -a /sbin/imud
        echo "done"
        ;;
  stop)
        echo "Stopping imud daemon: " > /dev/kmsg
        start-stop-daemon -K -n imud
        ;;
  restart)
        $0 stop
        $0 start
        ;;
  *)
        echo "Usage imud.sh { start | stop | restart}"
        exit 1
        ;;
esac

exit 0

