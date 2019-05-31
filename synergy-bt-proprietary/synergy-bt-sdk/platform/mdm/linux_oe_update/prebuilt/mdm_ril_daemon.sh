###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

###############################################################################
#
# File: mdm_ril_daemon.sh
#
# Description : Script to launch mcm_ril_service.
#
# Version : 1.0
#
###############################################################################

#!/bin/sh

echo " "

cmd=$1

if [ -n "$cmd" ]; then
    echo "$cmd mcm ril service"
else
    cmd=start
fi

if [ -f "/usr/bin/mcm_ril_service" ]; then
   MCM_RIL_SERVICE=/usr/bin/mcm_ril_service
fi

if [ -z "${MCM_RIL_SERVICE}" ]; then
   echo "Error : mcm ril service is not present ..."
   exit 1;
fi

start_mcm_ril_service()
{
    process=`ps aux | grep mcm_ril_service | grep -v grep`;
  
    if [ "$process" == "" ]; then
        mcm_ril_service & > /dev/null 2>&1
    else
        echo "mcm ril service already run"
    fi
}

stop_mcm_ril_service()
{
    # TODO
    echo " "
}

case "$cmd" in
    start)
        start_mcm_ril_service
        ;;

    stop)
        stop_mcm_ril_service
        ;;

    *)
        echo "unknown cmd: $cmd"
        exit 1
        ;;
esac