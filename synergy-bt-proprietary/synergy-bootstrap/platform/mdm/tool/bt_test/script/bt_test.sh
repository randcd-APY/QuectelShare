###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

###############################################################################
#
# File: bt_test.sh
#
# Description : Script to launch bt_test application
#
# Version : 1.0
#
# Notes:
# (1) The script is mainly used to test QCA BT chip.
# (2) Refer to "80-WL400-14_D_Bluetooth_HCI_Vendor-Specific_Command_Reference".
#
###############################################################################

# !/bin/sh

set -e

readonly usage="\
Usage:

Run the script:

    $0 <-c> [hci_cmd]           : send raw hci command in 'hex#0 hex#1 ...' (without h4 header)

    $0 <-d> [debug_command]
                rssi            : issue hci vs command of 'Read RSSI'

    $0 <-e> [enable_type]
                dut             : enable device under test mode

    $0 <-f> [hci_cmd_file]      : send raw hci command defined in the file (without h4 header)

    $0 <-k> [process]
                t               : kill bt_test

    $0 <-n> [nvm_tag (decimal)] : read nvm tag

    $0 <-p> [power_type]
                0               : power off bt chip
                1               : power on bt chip

    $0 <-s> [start_type]
                1               : start to boot up bt chip

    $0 <-t> [test_command]
                slave           : issue hci vs command of 'PROD_TEST_SLAVE'
                master          : issue hci vs command of 'PROD_TEST_MASTER'
                stats           : issue hci vs command of 'PROD_TEST_STATS'
                tx_burst        : issue hci vs command of 'PROD_TEST_TX_BURST'
                tx_continous    : issue hci vs command of 'PROD_TEST_TX_CONTINUOUS'
                rx_burst        : issue hci vs command of 'PROD_TEST_RX_BURST'
                rx_burst_config : issue hci vs command of 'PROD_TEST_RX_BURST_CONFIG'

Note:
* The setting for product test command in QCA BT chip is stored in '/data/bt_test.conf' by default.

* Need to boot up BT chip, before sending hci vs command (debug or prod test).

Examples:
    $0 -c '03,0c,00'            : issue hci reset command
    $0 -d rssi                  : read rssi
    $0 -e dut                   : enable device under test mode, with inquiry/page scan & auto-connect enable
    $0 -f '/data/hci_cmd.txt'   : issue hci command defined in '/data/hci_cmd.txt'
    $0 -k b                     : kill bt stack application
    $0 -n 2                     : read nvm tag '2' for bt device address
    $0 -p 0                     : power off bt chip
    $0 -p 1                     : power on bt chip
    $0 -s 1                     : power on and boot up bt chip
    $0 -t slave                 : send 'PROD_TEST_SLAVE' command
    $0 -t master                : send 'PROD_TEST_MASTER' command
    $0 -t stats                 : send 'PROD_TEST_STATS' command
    $0 -t tx_burst              : send 'PROD_TEST_TX_BURST' command
    $0 -t tx_continous          : send 'PROD_TEST_TX_CONTINUOUS' command
    $0 -t rx_burst              : send 'PROD_TEST_RX_BURST' command
    $0 -t rx_burst_config       : send 'PROD_TEST_RX_BURST_CONFIG' command
"

#
# Test folder which store bt_test application and config file
#
test_folder=/data

#
# Path to store bt_test application
#
bin_path=$test_folder

#
# Path to store bt_test configuration file
#
config_path=$test_folder

#
# Path to store qca bt firmware
#
firmware_path=/lib/firmware

#
# bt_bootstrap application
#
bt_bootstrap_bin=$bin_path/bt_bootstrap

#
# bt_test application
#
bt_test_bin=$bin_path/bt_test

#
# UART port
#
uart_port=/dev/ttyHS0

#
# New baudrate after BT chip reset.
#
new_baudrate=3000000


#############################################################
#
# Common Part
#
#############################################################

show_help()
{
    echo -e "$usage"
}

exit_info()
{
    echo "$1"
    show_help
    exit 1
}

show_error()
{
    echo "error: $1"
    exit 1
}

kill_process()
{
    echo "kill process: '$1' "
    pidof $1 | xargs kill -9
}

kill_test_app()
{
    kill_process bt_test
}

#############################################################
#
# Power on/off
#
#############################################################

power_off()
{
    echo "power qca chip off"
    echo 0 > /sys/class/rfkill/rfkill0/state
}

power_on()
{
    echo "power qca chip on"
    echo 1 > /sys/class/rfkill/rfkill0/state
}

reset()
{
    power_off

    power_on

    sleep 1
}

#############################################################
#
# Boot QCA chip
#
#############################################################

boot_qca()
{
    reset

    echo "boot up qca chip"

    $bt_bootstrap_bin --bt-chip qca --bt-port $uart_port --bt-bs-baud $new_baudrate --fw-path $firmware_path --config-path $config_path --flow-control 1

    echo "done"
}

#############################################################
#
# Enable DUT command
#
#############################################################

enable_dut()
{
    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --enable dut
}

#############################################################
#
# Raw HCI command
#
#############################################################

send_hci_cmd()
{
    hci_cmd=$1
    
    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --hci-cmd $hci_cmd
}

send_hci_cmd_from_file()
{
    hci_cmd_file=$1
    
    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --hci-cmd-file $hci_cmd_file --delay 0
}

#############################################################
#
# NVM read command
#
#############################################################

read_nvm()
{
    tag=$1

    echo "read nvm: $tag"

    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --read-nvm $tag
}

#############################################################
#
# Debug command.
#
#############################################################

read_rssi()
{
    echo "read rssi"

    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --debug rssi
}

#############################################################
#
# Product test command
#
#############################################################

prod_test_slave()
{
    echo "send 'PROD_TEST_SLAVE' command"

    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --test slave
}

prod_test_master()
{
    echo "send 'PROD_TEST_MASTER' command"

    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --test master
}

prod_test_stats()
{
    echo "send 'PROD_TEST_STATS' command"

    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --test stats
}

prod_test_tx_burst()
{
    echo "send 'PROD_TEST_TX_BURST' command"

    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --test tx_burst
}

prod_test_tx_continous()
{
    echo "send 'PROD_TEST_TX_CONTINUOUS' command"

    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --test tx_continous
}

prod_test_rx_burst()
{
    echo "send 'PROD_TEST_RX_BURST' command"

    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --test rx_burst
}

prod_test_rx_burst_config()
{
    echo "send 'PROD_TEST_RX_BURST_CONFIG' command"

    $bt_test_bin --bt-port $uart_port --bt-bs-baud $new_baudrate --config-path $config_path --test rx_burst_config
}

#############################################################

if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    exit_info "show help"
fi

while getopts "c:d:e:f:k:n:p:s:t:" arg
do
    case $arg in
        c)
            echo "raw hci cmd: $OPTARG"

            send_hci_cmd $OPTARG
            ;;
        d)
            echo "debug: $OPTARG"
            
            if [ "$OPTARG" = "rssi" ]; then
                read_rssi
            else
                exit_info "unknown debug command $OPTARG"
            fi
            ;;
        e)
            echo "enable: $OPTARG"
            
            if [ "$OPTARG" = "dut" ]; then
                enable_dut
            else
                exit_info "unknown enable command $OPTARG"
            fi
            ;;
        f)
            echo "raw hci cmd file: $OPTARG"

            send_hci_cmd_from_file $OPTARG
            ;;
        k)
            echo "kill: $OPTARG"

            if [ "$OPTARG" = "t" ]; then
                kill_test_app
            else
                exit_info "unknown kill command $OPTARG"
            fi
            ;;
        n)
            echo "nvm: $OPTARG"

            read_nvm $OPTARG
            ;;
        p)
            echo "power: $OPTARG"

            if [ "$OPTARG" = "0" ]; then
                power_off
            elif [ "$OPTARG" = "1" ]; then
                power_on
            else
                exit_info "unknown power argument $OPTARG"
            fi
            ;;
        s)
            echo "start: $OPTARG"

            if [ "$OPTARG" = "1" ]; then
                boot_qca
            else
                exit_info "unknown start argument $OPTARG"
            fi
            ;;
        t)
            echo "test: $OPTARG"

            if [ "$OPTARG" = "slave" ]; then
                prod_test_slave
            elif [ "$OPTARG" = "master" ]; then
                prod_test_master
            elif [ "$OPTARG" = "stats" ]; then
                prod_test_stats
            elif [ "$OPTARG" = "tx_burst" ]; then
                prod_test_tx_burst
            elif [ "$OPTARG" = "tx_continous" ]; then
                prod_test_tx_continous
            elif [ "$OPTARG" = "rx_burst" ]; then
                prod_test_rx_burst
            elif [ "$OPTARG" = "rx_burst_config" ]; then
                prod_test_rx_burst_config
            else
                exit_info "unknown test argument $OPTARG"
            fi
            ;;
        ?)
            exit_info "unknown argument $arg"
            ;;
    esac
done
