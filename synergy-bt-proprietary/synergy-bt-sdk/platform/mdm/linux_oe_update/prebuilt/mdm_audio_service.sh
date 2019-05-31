###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

###############################################################################
#
# File: mdm_audio_service.sh
#
# Description : Script to transfer voice between MODEM and Bluetooth chip.
#
# Version : 1.0
#
# Notes:
# (1) The script is based on ALSA application ("arec" & "aplay").
# (2) In MDM platform, sec aux pcm is connected with PCM interface in BT chip.
#
###############################################################################

#!/bin/sh

echo "mdm_audio_service"

cmd=$1

if [ -n "$cmd" ]; then
    echo "$cmd audio service"
else
    cmd=restart
fi

init_audio_service()
{
    echo "init audio service"

    amix 'SLIM RX1 MUX' 'AIF1_PB'
    amix 'SLIM RX2 MUX' 'AIF1_PB'
    amix 'MI2S_RX Channels' 'Two'
    amix 'RX1 MIX1 INP1' 'RX1'
    amix 'RX2 MIX1 INP1' 'RX2'
    amix 'HPHL DAC Switch' 1
    amix 'HPHL Volume' '80%'
    amix 'HPHR Volume' '80%'
    amix 'CLASS_H_DSM MUX' 'DSM_HPHL_RX1'
    amix 'RX1 Digital Volume' '67%'
    amix 'RX2 Digital Volume' '67%'
    amix 'MI2S_TX Channels' 'One'
    amix 'AIF1_CAP Mixer SLIM TX7' 1
    amix 'SLIM TX7 MUX' 'DEC5'
    amix 'DEC5 MUX' 'ADC2'
    amix 'DEC5 Volume' '67%'
    amix 'ADC2 Volume' '100'
    amix 'PRI_MI2S_RX_Voice Mixer CSVoice' 1
    amix 'Voice_Tx Mixer PRI_MI2S_TX_Voice' 1
}

deinit_audio_service()
{
    echo "deinit audio service"
}

start_audio_service()
{
    init_audio_service

    echo "start audio service"

    aplay -D hw:0,2 -P &
    arec -D hw:0,2 -P -R 8000 -C 1 &
}

stop_audio_service()
{
    # TX line
    pid1=`ps aux | grep aplay | grep -v grep`;

    if [ "$pid1" == "" ]; then
        echo "no aplay is in running state"
    else
        kill -9 $pid1
    fi

    # RX line
    pid2=`ps aux | grep arec | grep -v grep`;

    if [ "$pid2" == "" ]; then
        echo "no arec is in running state"
    else
        kill -9 $pid2
    fi
}

restart_audio_service()
{
    stop_audio_service

    sleep 1

    start_audio_service
}

case "$cmd" in
    init)
        init_audio_service
        ;;

    deinit)
        deinit_audio_service
        ;;

    start)
        start_audio_service
        ;;

    stop)
        stop_audio_service
        ;;

    restart)
        restart_audio_service
        ;;

    *)
        echo "unknown para: $cmd"
        exit 1
        ;;
esac