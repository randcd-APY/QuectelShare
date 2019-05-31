###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

###############################################################################
#
# File: robot.sh
#
# Description : Utility script to push synergy app and pull synergy log
#               into/from MDM platform, such as MDM9607.
#
# Version : 1.0
#
# Notes:
# (1) The script is based on the tool "adb" used in MDM_LE (OpenEmbedded).
# (2) Before running the script, make sure the variables below are set.
#     "LOCAL_LOG_PATH"
#     "MDM_ROOT_PATH"
#     "mdm_platform"
#
###############################################################################

#!/bin/bash

readonly usage="\
Usage: 

Note: Before using the robot.sh, need to modify the LOCAL_LOG_PATH and MDM_ROOT_PATH parameters.

   $0 <-p> [type]
                s   : push synergy app
                b   : push bt_bootstrap

   $0 <-d>  [type] 
                log : pull synergy log (hci.cfa, pcap.cap)
                b   : pull bt_bootstrap log
                pb  : pull phonebook (vcf file)

Examples:
    $0 -p s   : push synergy app
    $0 -d log : pull synergy log
"
#############################################################
#
# LOCAL_LOG_PATH
#
LOCAL_LOG_PATH=/mnt/hgfs/D/Log/MDM

#
# MDM_ROOT_PATH
#
MDM_ROOT_PATH=~/ssh/mdm

mdm_platform=mdm9607

mdm_arch=$mdm_platform-oe-linux-gnueabi
mdm_perf_arch=$mdm_platform_perf-oe-linux-gnueabi
mdmcalifornium_arch=mdmcalifornium-oe-linux-gnueabi

synergy_package=synergy-bt-sdk

BIN_PATH=$MDM_ROOT_PATH/oe-core/build/tmp-glibc/work/$mdm_arch/$synergy_package/git-invalidgit-r0/synergy-bt-proprietary/$synergy_package/sdk/output/host/bin/bt/app
PREBUILT_PATH=$MDM_ROOT_PATH/oe-core/build/tmp-glibc/work/$mdm_arch/$synergy_package/git-invalidgit-r0/synergy-bt-proprietary/$synergy_package/prebuilt

SYNERGY_PATH=/data

LOG_PATH=$SYNERGY_PATH
hcidump_log=$LOG_PATH/hci.cfa
wireshark_log=$LOG_PATH/pcap.cap

bt_bootstrap_log=$LOG_PATH/bt.log
bt_bootstrap_console_log=$LOG_PATH/bt_console.log
bt_bootstrap_snoop_log=$LOG_PATH/hci_bootstrap.cfa

PB_PATH=$SYNERGY_PATH
PB_VCF=$PB_PATH/pb.vcf
ICH_VCF=$PB_PATH/ich.vcf
OCH_VCF=$PB_PATH/och.vcf
MCH_VCF=$PB_PATH/mch.vcf
CCH_VCF=$PB_PATH/cch.vcf
FAV_VCF=$PB_PATH/fav.vcf
SPD_VCF=$PB_PATH/spd.vcf

###########################################################
# Synergy bins and libs
###########################################################

synergy_app=$BIN_PATH/csr_bt_app_default_hci
bootstrap_app=$PREBUILT_PATH/bt_bootstrap

show_help()
{
    echo -e "$usage"
}

if [ $# -lt 1 ]; then
    show_help
    exit 1
fi

if [ $# = 0 ] || [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    show_help
    exit 1
fi

pull_log()
{
    echo "rm temp hcidump log"
    rm $LOCAL_LOG_PATH/hci.frm
    rm $LOCAL_LOG_PATH/hci.fsc

    echo "pull $hcidump_log ==> $LOCAL_LOG_PATH"
    adb pull $hcidump_log $LOCAL_LOG_PATH

    echo "pull $wireshark_log ==> $LOCAL_LOG_PATH"
    adb pull $wireshark_log $LOCAL_LOG_PATH

#   adb shell rm $hcidump_log
#   adb shell rm $wireshark_log
#   adb shell sync

    echo "done"
}

pull_bt_bootstrap_log()
{
    echo "pull $bt_bootstrap_console_log ==> $LOCAL_LOG_PATH"
    adb pull $bt_bootstrap_console_log $LOCAL_LOG_PATH

    echo "rm temp btsnoop log"
    rm $LOCAL_LOG_PATH/hci_bootstrap.frm
    rm $LOCAL_LOG_PATH/hci_bootstrap.fsc

    echo "pull $bt_bootstrap_snoop_log ==> $LOCAL_LOG_PATH"
    adb pull $bt_bootstrap_snoop_log $LOCAL_LOG_PATH

#   echo "pull $bt_bootstrap_log ==> $LOCAL_LOG_PATH"
#   adb pull $bt_bootstrap_log $LOCAL_LOG_PATH

    echo "done"
}

pull_pb()
{
    echo "pull $PB_VCF ==> $LOCAL_LOG_PATH"
    adb pull $PB_VCF $LOCAL_LOG_PATH

    echo "pull $ICH_VCF ==> $LOCAL_LOG_PATH"
    adb pull $ICH_VCF $LOCAL_LOG_PATH

    echo "pull $OCH_VCF ==> $LOCAL_LOG_PATH"
    adb pull $OCH_VCF $LOCAL_LOG_PATH

    echo "pull $MCH_VCF ==> $LOCAL_LOG_PATH"
    adb pull $MCH_VCF $LOCAL_LOG_PATH

    echo "pull $CCH_VCF ==> $LOCAL_LOG_PATH"
    adb pull $CCH_VCF $LOCAL_LOG_PATH

    echo "pull $FAV_VCF ==> $LOCAL_LOG_PATH"
    adb pull $FAV_VCF $LOCAL_LOG_PATH

    echo "pull $SPD_VCF ==> $LOCAL_LOG_PATH"
    adb pull $SPD_VCF $LOCAL_LOG_PATH

    echo "done"
}

push_synergy_app()
{
    echo "push $synergy_app ==> $SYNERGY_PATH"
    adb push $synergy_app $SYNERGY_PATH

    adb shell sync

    echo "done"
}

push_bootstrap()
{
    echo "push $bootstrap_app ==> $SYNERGY_PATH"
    adb push $bootstrap_app $SYNERGY_PATH

    adb shell sync

    echo "done"
}

while getopts "p:d:" arg
do
    case $arg in
        d)
            echo "pull's arg:$OPTARG" 

            if [ "$OPTARG" = "log" ]; then
                pull_log
            elif [ "$OPTARG" = "b" ]; then
                pull_bt_bootstrap_log
            elif [ "$OPTARG" = "pb" ]; then
                pull_pb
            else
                echo "unkown option argument"
            fi
            ;;
        p)
            echo "push's arg:$OPTARG"

            if [ "$OPTARG" = "s" ]; then
                push_synergy_app
            elif [ "$OPTARG" = "b" ]; then
                push_bootstrap
            else
                echo "unkown option argument"
            fi
            ;;
         ?) 
            echo "unkonw argument"
            exit 1
            ;;
    esac
done
