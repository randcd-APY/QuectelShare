###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

###############################################################################
#
# File: mdm_test.sh
#
# Description : Script to test modem, which is based on mcm_ril_service.
#
# Version : 1.0
#
###############################################################################

#!/bin/sh

echo "modem test"

SCRIPT_PATH=/data

MDM_RIL_DAEMON=$SCRIPT_PATH/mdm_ril_daemon.sh
MDM_AUDIO_SERVICE=$SCRIPT_PATH/mdm_audio_service.sh

if [ -z "${MDM_RIL_DAEMON}" ]; then
   echo "Error : mdm ril daemon not exist ..."
   exit 1;
fi

if [ -z "${MDM_AUDIO_SERVICE}" ]; then
   echo "Error : mdm audio service not exist ..."
   exit 1;
fi

if [ -f "/usr/bin/IoEConsoleClient" ]; then
   MODEM_APP=IoEConsoleClient
fi

if [ -z "${MODEM_APP}" ]; then
   echo "Error : modem app not exist ..."
   exit 1;
fi

# Step1. Launch mdm ril daemon (once)
. $MDM_RIL_DAEMON

sleep 1

# Step2. Start mdm audio service
. $MDM_AUDIO_SERVICE

# Step3. Run modem app
echo "Run $MODEM_APP"

$MODEM_APP

echo "$MODEM_APP exit"

# [TODO] Step4. Stop mdm audio service

# [TODO] Step5. Stop mdm ril daemon