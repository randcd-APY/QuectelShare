# !/bin/sh
###############################################################################
# Begin Bluetooth
#
# Description : Qualcomm BT bootstrap script
#
# Version : 1.0
#
# Notes: This script starts bt_bootstrap for QCA chip
#
# Copyright (c) 2016 Qualcomm Technologies, Inc.  All Rights Reserved.
# Qualcomm Technologies Proprietary and Confidential.
#
##############################################################################

set -e

operation=$1

enable_power_ctrl=1

BIN_PATH=/data
BOOTSTRAP_APP=bt_bootstrap
BOOTSTRAP_APP_FULL_NAME=$BIN_PATH/$BOOTSTRAP_APP

BT_CONSOLE_LOG=/data/bt_console.log


show_usage()
{
    echo "Usage: ./bt_bootstrap { start | stop | restart }"
}

init_platform_info()
{
    PLATFORM=`cat /proc/cpuinfo | grep Hardware | cut -d ' ' -f5`
    echo "Platform: $PLATFORM"
}

power_on()
{
    if [ "$enable_power_ctrl" = "1" ]; then
        echo "power qca chip on"
        echo 1 > /sys/class/rfkill/rfkill0/state
    fi
}

power_off()
{
    if [ "$enable_power_ctrl" = "1" ]; then
        echo "power qca chip off"
        echo 0 > /sys/class/rfkill/rfkill0/state
    fi
}

start_bootstrap()
{
    init_platform_info

    power_on

    echo "start $BOOTSTRAP_APP_FULL_NAME"

    $BOOTSTRAP_APP_FULL_NAME --bt-chip qca --bt-port /dev/ttyHS0 --bt-bs-baud 3000000 --flow-control 1 >$BT_CONSOLE_LOG 2&>1

    echo "bt bootstrap done"
}

stop_bootstrap()
{
    power_off

    echo "stop $BOOTSTRAP_APP"

#   pidof $BOOTSTRAP_APP | xargs kill -9
}

restart_bootstrap()
{
    stop_bootstrap

    start_bootstrap
}

if [ -n "$operation" ]; then
    case "$operation" in
        start)
            start_bootstrap
            ;;

        stop)
            stop_bootstrap
            ;;

        restart)
            restart_bootstrap
            ;;

        *)
            show_usage
            exit 1
            ;;
    esac
fi

exit 0
