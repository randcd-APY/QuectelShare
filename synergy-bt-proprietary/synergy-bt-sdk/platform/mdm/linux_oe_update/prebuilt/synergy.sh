###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

###############################################################################
#
# File: synergy.sh
#
# Description : Script to launch QTI Synergy Bluetooth application
#
# Version : 1.0
#
# Notes: The sequence to start synergy includes 3 steps
# (1) Power BT chip on, which is controlled through rfkill.
# (2) Start the application of bt_bootstrap, so as to boot up BT chip.
# (3) Start Synergy application.
#
###############################################################################

# !/bin/sh

set -e

enable_log=0

operation=$1
bt_device=$2

bt_device_default=phone

show_usage()
{
    echo "Usage: ./synergy {start|stop|restart|on|off|test} {carkit|phone}"
}

# verify operation
if [ -n "$operation" ]; then
    echo "$operation"
else
    show_usage
    exit 1
fi

# verify bt_device
if [ -n "$bt_device" ]; then
    echo "$bt_device"
else
    echo "$bt_device_default"
    bt_device=$bt_device_default
fi

use_bt_bootstrap=1

PLATFORM=MDM9628
MTP_NAME=MTP9X07

LOG_PATH=/data
BIN_PATH=/usr/bin

WIRESHARE_LOG=$LOG_PATH/pcap.cap
HCI_LOG=$LOG_PATH/hci.cfa
BT_BOOTSTRAP_CONSOLE_LOG=$LOG_PATH/bt_console.log
BT_BOOTSTRAP_HCI_LOG=$LOG_PATH/hci_bootstrap.cfa

BT_BOOTSTRAP_APP=bt_bootstrap
BT_BOOTSTRAP_FULL_NAME=$BIN_PATH/$BT_BOOTSTRAP_APP

BT_AUDIO_SERVICE_APP=bt_audio_service
BT_AUDIO_SERVICE_FULL_NAME=$BIN_PATH/$BT_AUDIO_SERVICE_APP

SYNERGY_APP=csr_bt_app_default_hci
SYNERGY_APP_FULL_NAME=$BIN_PATH/$SYNERGY_APP

PHONEBOOK_APP=hci_pbapc_app
PHONEBOOK_APP_FULL_NAME=$BIN_PATH/$PHONEBOOK_APP

# hciattach
if [ -f "/usr/bin/hciattach" ]; then
    HCIATTACH=/usr/bin/hciattach
fi

# New baudrate after BT chip reset.
NEW_BAUDRATE=3000000

# UART port
UART_PORT=/dev/ttyHS0

# 0: Disable H4 IBS, 1: Enable 
IBS=0

# 0: Disable UART Low Power Mode, 1: Enable
LPM=0

# 0: Disable Keyinput, 1: Enable
KEY_INPUT=1

if [ "$use_bt_bootstrap" = "1" ]; then
    TRANSPORT=h4ibs
    PORT=$UART_PORT
    INIT_BAUDRATE=$NEW_BAUDRATE
else
    TRANSPORT=bluez
    PORT=hci0
    INIT_BAUDRATE=115200
fi

remove_synergy_log()
{
    echo "remove synergy log"

    # remove bt_bootstrap console log
    if [ -f "$BT_BOOTSTRAP_CONSOLE_LOG" ]; then
        rm $BT_BOOTSTRAP_CONSOLE_LOG
    fi

    # remove bt_bootstrap hci log
    if [ -f "$BT_BOOTSTRAP_HCI_LOG" ]; then
        rm $BT_BOOTSTRAP_HCI_LOG
    fi

    # remove wireshark log
    if [ -f "$WIRESHARE_LOG" ]; then
        rm $WIRESHARE_LOG
    fi

    # remove hci dump log
    if [ -f "$HCI_LOG" ]; then
        rm $HCI_LOG
    fi
}

init_platform_info()
{
    PLATFORM=`cat /proc/cpuinfo | grep Hardware | cut -d ' ' -f5`
    echo "Platform: $PLATFORM"

    case "$PLATFORM" in
        MDM9640)
            MTP_NAME=MTP9X45
            ;;

        MSMZIRC)
            MTP_NAME=MTP9X45
            ;;

        MDM9628)
            MTP_NAME=MTP9X07
            ;;

        MDM9650)
            MTP_NAME=MTP9X50
            ;;

        MDMCALIFORNIUM)
            # FIXME
            MTP_NAME=MTP9X50
            ;;

        *)
            echo "Unknown platform: $PLATFORM"
            # should not exit since it is not a fatal error
            MTP_NAME=MTP_UNKNOWN
            ;;
    esac

    echo "$MTP_NAME"

    remove_synergy_log
}

init()
{
    init_platform_info

    # create /etc/bluetooth for storing bluetooth configuration files
    if [ ! -d "/etc/bluetooth" ]; then
        mkdir /etc/bluetooth -p
    fi
}

power_on()
{
    echo "power qca chip on"
    echo 1 > /sys/class/rfkill/rfkill0/state
}

power_off()
{
    echo "power qca chip off"
    echo 0 > /sys/class/rfkill/rfkill0/state
}

start_bt_bootstrap()
{
    if [ "$enable_log" = "0" ]; then
        $BT_BOOTSTRAP_FULL_NAME --bt-chip qca --bt-port $UART_PORT --bt-bs-baud $NEW_BAUDRATE --flow-control 1 > /dev/null 2>&1
    else
        $BT_BOOTSTRAP_FULL_NAME --bt-chip qca --bt-port $UART_PORT --bt-bs-baud $NEW_BAUDRATE --flow-control 1 > $BT_BOOTSTRAP_CONSOLE_LOG 2&>1
    fi
}

start_bt_audio_service()
{
    echo "start bt_audio_service"

    if [ "$enable_log" = "0" ]; then
        $BT_AUDIO_SERVICE_FULL_NAME --ipc-type socket > /dev/null 2>&1 &
    else
        $BT_AUDIO_SERVICE_FULL_NAME --ipc-type socket &
    fi
}

stop_bt_audio_service()
{
    echo "stop bt_audio_service"

    pidof $BT_AUDIO_SERVICE_APP | xargs kill -9
}

start_hci_attach()
{
    case "$PLATFORM" in
        MDM9640)
            # MTP9X45
            $HCIATTACH $UART_PORT qca $NEW_BAUDRATE -t120 flow > /dev/null 2>&1
            ;;

        *)
            # MTP9X07, MTP9X50
            $HCIATTACH $UART_PORT qca $NEW_BAUDRATE -t120 flow -r > /dev/null 2>&1
            ;;
    esac
}

start_bootstrap()
{
    # TODO: check whether BT chip is powered on
    sleep 1

    if [ "$use_bt_bootstrap" = "1" ]; then
        echo "start bt_bootstrap"
        start_bt_bootstrap
    else
        echo "start hciattach"
        start_hci_attach
    fi
}

stop_bootstrap()
{
    # Not necessary to kill "bt_bootstrap" if "use_bt_bootstrap" is set as "1".
    # The reason is that "bt_bootstrap" already exit after it boots up BT chip.
    if [ "$use_bt_bootstrap" = "0" ]; then
        echo "stop hciattach"
        pidof hciattach | xargs kill -9
    fi
}

start_bluetoothd()
{
    init

    power_on

    start_bootstrap
    
    start_bt_audio_service

    start_synergy_app
}

stop_bluetoothd()
{
    power_off

    stop_bootstrap

    stop_bt_audio_service

#   pidof $SYNERGY_APP | xargs kill -9
}

start_synergy_app_for_carkit()
{
    echo "carkit"

    if [ "$enable_log" = "0" ]; then
        $SYNERGY_APP_FULL_NAME --bc-transport $TRANSPORT --bc-port $PORT --bc-baud $INIT_BAUDRATE --bc-bs-baud $NEW_BAUDRATE --platform $PLATFORM --bc-bs-device-name $MTP_NAME --bt-dev carkit --bt-av-alsa-device hw:0,0  --bt-remote-address 2424:0E:CA7E44 --ibs $IBS --lpm $LPM --use-key $KEY_INPUT
    else
        $SYNERGY_APP_FULL_NAME --bc-transport $TRANSPORT --bc-port $PORT --bc-baud $INIT_BAUDRATE --bc-bs-baud $NEW_BAUDRATE --platform $PLATFORM --bc-bs-device-name $MTP_NAME --bt-dev carkit --bt-av-alsa-device hw:0,0 --bt-remote-address 2424:0E:CA7E44 --ibs $IBS --lpm $LPM --use-key $KEY_INPUT --log-cleartext-stdout --log-pcap-file $WIRESHARE_LOG --log-btsnoop-file $HCI_LOG --tui-nocls
    fi
}

start_synergy_app_for_phone()
{
    echo "phone"

    if [ "$enable_log" = "0" ]; then
        $SYNERGY_APP_FULL_NAME --bc-transport $TRANSPORT --bc-port $PORT --bc-baud $INIT_BAUDRATE --bc-bs-baud $NEW_BAUDRATE --platform $PLATFORM --bc-bs-device-name $MTP_NAME --bt-dev phone --bt-av-wav-file-in /data/csr.wav --bt-remote-address 0002:5B:00FF07 --ibs $IBS --lpm $LPM --use-key $KEY_INPUT
    else
        $SYNERGY_APP_FULL_NAME --bc-transport $TRANSPORT --bc-port $PORT --bc-baud $INIT_BAUDRATE --bc-bs-baud $NEW_BAUDRATE --platform $PLATFORM --bc-bs-device-name $MTP_NAME --bt-dev phone --bt-av-wav-file-in /data/csr.wav --bt-remote-address 0002:5B:00FF07 --ibs $IBS --lpm $LPM --use-key $KEY_INPUT --log-cleartext-stdout --log-pcap-file $WIRESHARE_LOG --log-btsnoop-file $HCI_LOG --tui-nocls
    fi
}

start_synergy_app()
{
    case "$bt_device" in
        carkit)
            start_synergy_app_for_carkit
            ;;

        phone)
            start_synergy_app_for_phone
            ;;

        *)
            # invalid device type
            stop_bluetoothd
            show_usage
            exit 1
            ;;
    esac
}

start_phonebook_app()
{
    echo "starting phonebook app ..."

    power_on

    start_bootstrap

    $PHONEBOOK_APP_FULL_NAME -t $TRANSPORT -c $PORT -x 0002:5b:123456
}

stop_phonebook_app()
{
    echo "stopping phonebook app ..."

    power_off

    stop_bootstrap

    pidof $PHONEBOOK_APP | xargs kill -9
}

case "$operation" in
    start)
        start_bluetoothd
        ;;

    stop)
        stop_bluetoothd
        ;;

    restart)
        stop_bluetoothd
        sleep 2
        start_bluetoothd
        ;;

    on)
        power_on
        ;;

    off)
        power_off
        ;;

    test)
        init
        power_on
        start_bootstrap
        ;;

    start_pb)
        start_phonebook_app
        ;;

    stop_pb)
        stop_phonebook_app
        ;;

    *)
        show_usage
        exit 1
        ;;
esac

exit 0
