###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

###############################################################################
#
# File: robot_build.sh
#
# Description : Utility script to build synergy bt application.
#
# Version : 1.0
#
# Notes:
# (1) The script is based on "bitbake" used in MDM_LE (OpenEmbedded).
# (2) Before running the script, make sure the variables below are set.
#     "mdm_platform"
#     "mdm_root_path"
#
###############################################################################

#!/bin/bash

readonly usage="\
Usage: 

Before using the script tool, please copy it to your Android BSP base folder.

    $0 <-m> [type] [-c]
                s      : build synergy (csr_bt_app_default_hci)

"[-c]" is a optional parameter, if you want to clean the project before build, 
need to add parameter "[-c]"

Examples:

    $0 -m s    : build synergy [incremental compilation]
    $0 -m s -c : clean the synergy and build synergy
"

#
# synergy build type: all, sdk
#
synergy_build_type=sdk

#
# mdm platform: mdm9607, mdm9640
#
mdm_platform=mdm9607

#
# mdm root path
#
mdm_root_path=~/mdm

oe_path=$mdm_root_path/oe-core
oe_build_path=$oe_path/build
oe_stamps_path=$oe_build_path/tmp-glibc/stamps

mdm_arch=$mdm_platform-oe-linux-gnueabi
mdm_perf_arch=$mdm_platform_perf-oe-linux-gnueabi
mdmcalifornium_arch=mdmcalifornium-oe-linux-gnueabi

synergy_bt_package=synergy-bt
synergy_bt_sdk_package=synergy-bt-sdk

synergy_do_fetch_file=git-invalidgit-r0.do_fetch

synergy_bt_do_fetch=$oe_stamps_path/$mdm_arch/$synergy_bt_package/$synergy_do_fetch_file
synergy_bt_sdk_do_fetch=$oe_stamps_path/$mdm_arch/$synergy_bt_sdk_package/$synergy_do_fetch_file

oe_synergy_relative_path=tmp-glibc/work/$mdm_arch/$synergy_bt_sdk_package
oe_synergy_path=$oe_build_path/$oe_synergy_relative_path

synergy_code_path=$mdm_root_path/synergy-bt-proprietary


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

#
# set up build environment
#
env_setup()
{
    cd $oe_path
    source build/conf/set_bb_env.sh
}

#
# Before compile, clean code
#
clean()
{
    if [ "$1" = "-c" ]; then
        echo "===================== clean ===================="

        if [ "$synergy_build_type" = "all" ]; then
            bitbake -c clean $synergy_bt_package
        fi

        bitbake -c clean $synergy_bt_sdk_package
    fi
}

#
# install synergy package
#
install()
{
    if [ "$synergy_build_type" = "all" ]; then
        echo "rm $synergy_bt_do_fetch"
        rm $synergy_bt_do_fetch

        echo "install synergy bt" 
        bitbake $synergy_bt_package
    fi

    echo "rm $synergy_bt_sdk_do_fetch"
    rm $synergy_bt_sdk_do_fetch

    echo "install synergy bt sdk" 
    bitbake $synergy_bt_sdk_package
}

#
# Compile synergy package
#
compile()
{
    if [ "$synergy_build_type" = "all" ]; then
        echo "rm $synergy_bt_do_fetch"
        rm $synergy_bt_do_fetch

        echo "compile synergy bt" 
        bitbake -c compile $synergy_bt_package
    fi

    echo "rm $synergy_bt_sdk_do_fetch"
    rm $synergy_bt_sdk_do_fetch

    echo "compile synergy bt sdk"
    bitbake -c compile $synergy_bt_sdk_package
}

#
# build synergy
#
build_synergy()
{
    echo "build synergy"
    clean $1
    compile
}

#
# list synergy app
#
list_synergy_app()
{
    find $oe_synergy_relative_path -name "csr_bt_app_default_hci" | xargs ls -al
}

#
# chmod synergy code with r/w permission
#
chmod_synergy_code()
{
    echo "synergy code: $synergy_code_path"
    chmod -R 777 $synergy_code_path
}

while getopts "m:l:o" arg
do
    case $arg in
        m)
            echo "build's arg:$OPTARG"

            if [ "$OPTARG" = "s" ]; then
                build_synergy $3
            else
                echo "unknown option argument"
            fi
            ;;
        l)
            echo "list synergy app"
            list_synergy_app
            ;;
        o)
            echo "chmod synergy code with r/w"
            chmod_synergy_code
            ;;
        ?)
            echo "unknown argument"
            exit 1
            ;;
    esac
done
