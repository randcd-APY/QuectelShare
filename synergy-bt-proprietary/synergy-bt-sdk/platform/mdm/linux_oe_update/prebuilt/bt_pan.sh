###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

###############################################################################
#
# File: bt_pan.sh
#
# Description : Script to configure network for Bluetooth PAN
#
# Version : 1.0
#
###############################################################################

#!/bin/sh

#
# cmd: start, stop 
#
cmd=$1

#
# role: panu, nap
#
role=$2

#############################################################
#
# Internal settting/variable
#
#############################################################

#
# tun/tap interface
#
tap="tap0"

#
# bridge interface
#
bridge="bridge0"

#
# bridge utility
#
brctl_bin="/sbin/brctl"

#
# ifconfig command
#
ifconfig_bin="/sbin/ifconfig"

#
# dhcp client daemon
#
dhcpcd_bin="/usr/sbin/dhcpcd"

#
# tun/tap ip address
#
tap_ip="169.254.2.1"

#
# tun/tap netmask
#
tap_netmask="255.255.255.0"

#############################################################

show_error()
{
    echo "error: $1"
    exit 1
}

#############################################################
#
# PANU
#
#############################################################

start_panu()
{
    echo "start panu"

    # dhcpcd will bring up tun(tap0) before sending request to get ip address
    $dhcpcd_bin $tap 
}

stop_panu()
{
    echo "stop panu"

    # tun(tap0) will be freed when pan is disconnected
    # meanwhile, dhcpcd will be stopped automatically once tun(tap0) is freed
    # so no more actions is required
}

#############################################################
#
# NAP
#
#############################################################

start_nap()
{
    echo "start nap"

    # brctl addif bridge0 tap0
    $brctl_bin addif $bridge $tap

    # ifconfig tap0 169.254.2.1 netmask 255.255.255.0 up
    $ifconfig_bin $tap $tap_ip netmask $tap_netmask up
}

stop_nap()
{
    echo "stop nap"

    # tun(tap0) will be freed when pan is disconnected
    # meanwhile, related bridge will be deleted automatically once tun(tap0) is freed
    # so no more actions is required
}

#############################################################

case "$role" in
    panu)
        if [ "$cmd" = "start" ]; then
            start_panu &
        elif [ "$cmd" = "stop" ]; then
            stop_panu &
        else
            show_error "unknown cmd '$cmd' for panu"
        fi
        ;;

    nap)
        if [ "$cmd" = "start" ]; then
            start_nap &
        elif [ "$cmd" = "stop" ]; then
            stop_nap &
        else
            show_error "unknown cmd '$cmd' for nap"
        fi
        ;;

    *)
        show_error "unknown role '$role' "
        ;;
esac

