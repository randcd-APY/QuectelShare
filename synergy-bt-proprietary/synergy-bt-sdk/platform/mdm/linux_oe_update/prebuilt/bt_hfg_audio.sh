###############################################################################
#
#    Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#    All Rights Reserved. 
#    Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

###############################################################################
#
# File: bt_hfg_audio.sh
#
# Description : Script to transfer Bluetooth HFG audio.
#
# Version : 1.0
#
# Notes:
# (1) The script is launched if SCO/eSCO is established during running HFG.
# (2) The script is based on ALSA application ("arec" & "aplay").
# (3) In MDM platform, sec aux pcm is connected with PCM interface in BT chip.
#
###############################################################################

#!/bin/sh

echo "bt_hfg_audio"

cmd=$1
sample_rate=$2
PLATFORM=`cat /proc/cpuinfo | grep Hardware | cut -d ' ' -f5`

if [ "$cmd" = "start" ]; then
    echo "$cmd hfg audio $sample_rate"
else
    echo "$cmd hfg audio"
fi

init_hfg_audio()
{
    echo "configure hfg audio path"

    # This is a workaround for an issue in kernel audio driver
    # In detail, if we deconfigure the audio route for the first time after device is power up,
    # we will block in doing amix.
    # Drop this workaourd when the audio issue is fixed
    configure_local_audio
    deconfigure_local_audio
    configure_bluetooth_audio
    deconfigure_bluetooth_audio
}

deinit_hfg_audio()
{
    echo "deconfigure hfg audio path"
}

configure_afe_port_sample_rate()
{
    if [ "$sample_rate" == 16000 ]; then
        amix 'AUX PCM SampleRate' 'rate_16000'
    else
        amix 'AUX PCM SampleRate' 'rate_8000'
    fi
}

start_audio_service_for_9x45_le_1x()
{
    # TX line
    aplay -D hw:0,2 -P -R$sample_rate -C 1 &

    # RX line
    arec -D hw:0,2 -P -R$sample_rate -C 1 &
}


start_audio_service_for_9x45_le_2x()
{
    # TX line
    aplay -D hw:0,17 -P -R$sample_rate -C 1 &

    # RX line
    arec -D hw:0,17 -P -R$sample_rate -C 1 &
}

start_audio_service_for_9x07()
{
    # TX line
    aplay -D hw:0,2 -P -R$sample_rate -C 1 &

    # RX line
    arec -D hw:0,2 -P -R$sample_rate -C 1 &
}

start_audio_service_for_9x50()
{
    # TX line
    aplay -D hw:0,17 -P -R$sample_rate -C 1 &

    # RX line
    arec -D hw:0,17 -P -R$sample_rate -C 1 &
}


start_audio_service()
{
    case "$PLATFORM" in
        MDM9640)
            start_audio_service_for_9x45_le_1x
            ;;

        MSMZIRC)
            start_audio_service_for_9x45_le_2x
            ;;

        MDM9628)
            start_audio_service_for_9x07
            ;;

        MDM9650)
            start_audio_service_for_9x50
            ;;
        *)
            echo "Unknown platform: $PLATFORM"
            ;;
    esac
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
    configure_afe_port_sample_rate
    start_audio_service
}

configure_local_audio_for_9x45_le_1x()
{
    # Configure for both playing and recording
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

configure_local_audio_for_9x45_le_2x()
{
    # Configure for both playing and recording
    amix 'SLIM RX1 MUX' 'AIF1_PB'
    amix 'SLIM RX2 MUX' 'AIF1_PB'
    amix 'AIF1_CAP Mixer SLIM TX7' 1
    amix 'RX1 MIX1 INP1' 'RX1'
    amix 'RX2 MIX1 INP1' 'RX2'
    amix 'HPHL DAC Switch' 1
    amix 'HPHL Volume' '80%'
    amix 'HPHR Volume' '80%'
    amix 'RX1 Digital Volume' '67%'
    amix 'RX2 Digital Volume' '67%'
    amix 'AIF1_CAP Mixer SLIM TX7' 1
    amix 'SLIM TX7 MUX' 'DEC5'
    amix 'CLASS_H_DSM MUX' 'DSM_HPHL_RX1'
    amix 'DEC5 MUX' 'ADC2'
    amix 'DEC5 Volume' '67%'
    amix 'ADC2 Volume' '65'
    amix 'PRI_MI2S_RX_Voice Mixer VoiceMMode1' 1
    amix 'VoiceMMode1_Tx Mixer PRI_MI2S_TX_MMode1' 1
}

configure_local_audio_for_9x07()
{
    # Configure for both playing and recording
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

configure_local_audio_for_9x50()
{
    # Configure for both playing and recording
    amix 'MI2S_TX Channels' 'One'
    amix 'AIF1_CAP Mixer SLIM TX6' 1
    amix 'SLIM TX6 MUX' 'DEC6'
    amix 'ADC MUX6' 'AMIC'
    amix 'AMIC MUX6' 'ADC2'
    amix 'DEC6 Volume' 67
    amix 'SLIM RX0 MUX' 'AIF1_PB'
    amix 'SLIM RX1 MUX' 'AIF1_PB'
    amix 'MI2S_RX Channels' 'Two'
    amix 'RX INT1_1 MIX1 INP0' 'RX0'
    amix 'RX INT2_1 MIX1 INP0' 'RX1'
    amix 'RX INT1 DEM MUX' 'CLSH_DSM_OUT'
    amix 'RX INT2 DEM MUX' 'CLSH_DSM_OUT'
    amix 'PRI_MI2S_RX_Voice Mixer VoiceMMode1' 1
    amix 'VoiceMMode1_Tx Mixer PRI_MI2S_TX_MMode1' 1
}

configure_local_audio()
{
    case "$PLATFORM" in
        MDM9640)
            configure_local_audio_for_9x45_le_1x
            ;;

        MSMZIRC)
            configure_local_audio_for_9x45_le_2x
            ;;

        MDM9628)
            configure_local_audio_for_9x07
            ;;
			
	MDM9650)
            configure_local_audio_for_9x50
            ;;
        *)
            echo "Unknown platform: $PLATFORM"
            ;;
    esac
}

deconfigure_local_audio_for_9x45_le_1x()
{
    # Deconfigure for both playing and recording
    amix 'SLIM RX1 MUX' 0
    amix 'SLIM RX2 MUX' 0
    amix 'MI2S_RX Channels' 0
    amix 'RX1 MIX1 INP1' 0
    amix 'RX2 MIX1 INP1' 0
    amix 'HPHL DAC Switch' 0
    # Keep the volume
    # mix 'HPHL Volume' '80%'
    # amix 'HPHR Volume' '80%'
    amix 'CLASS_H_DSM MUX' 0
    # Keep the volume
    # amix 'RX1 Digital Volume' '67%'
    # amix 'RX2 Digital Volume' '67%'
    amix 'MI2S_TX Channels' 0
    amix 'AIF1_CAP Mixer SLIM TX7' 0
    amix 'SLIM TX7 MUX' 0
    amix 'DEC5 MUX' 0
    # Keep the volume
    # amix 'DEC5 Volume' '67%'
    # amix 'ADC2 Volume' '100'
    amix 'PRI_MI2S_RX_Voice Mixer CSVoice' 0
    amix 'Voice_Tx Mixer PRI_MI2S_TX_Voice' 0
}


deconfigure_local_audio_for_9x45_le_2x()
{
    # Deconfigure for both playing and recording
    amix 'SLIM RX1 MUX'  0
    amix 'SLIM RX2 MUX' 0
    amix 'AIF1_CAP Mixer SLIM TX7' 0
    amix 'RX1 MIX1 INP1' 0
    amix 'RX2 MIX1 INP1' 0
    amix 'HPHL DAC Switch' 0
    # Keep the volume
    # amix 'HPHL Volume' '80%'
    # amix 'HPHR Volume' '80%'
    # amix 'RX1 Digital Volume' '67%'
    # amix 'RX2 Digital Volume' '67%'
    amix 'AIF1_CAP Mixer SLIM TX7' 0
    amix 'SLIM TX7 MUX' 0
    amix 'CLASS_H_DSM MUX' 0
    amix 'DEC5 MUX' 0
    # Keep the volume
    # amix 'DEC5 Volume' '67%'
    # amix 'ADC2 Volume' '65'
    amix 'PRI_MI2S_RX_Voice Mixer VoiceMMode1' 0
    amix 'VoiceMMode1_Tx Mixer PRI_MI2S_TX_MMode1' 0
}

deconfigure_local_audio_for_9x07()
{
    # Deconfigure for both playing and recording
    amix 'SLIM RX1 MUX' 0
    amix 'SLIM RX2 MUX' 0
    amix 'MI2S_RX Channels' 0
    amix 'RX1 MIX1 INP1' 0
    amix 'RX2 MIX1 INP1' 0
    amix 'HPHL DAC Switch' 0
    # Keep the volume
    # mix 'HPHL Volume' '80%'
    # amix 'HPHR Volume' '80%'
    amix 'CLASS_H_DSM MUX' 0
    # Keep the volume
    # amix 'RX1 Digital Volume' '67%'
    # amix 'RX2 Digital Volume' '67%'
    amix 'MI2S_TX Channels' 0
    amix 'AIF1_CAP Mixer SLIM TX7' 0
    amix 'SLIM TX7 MUX' 0
    amix 'DEC5 MUX' 0
    # Keep the volume
    # amix 'DEC5 Volume' '67%'
    # amix 'ADC2 Volume' '100'
    amix 'PRI_MI2S_RX_Voice Mixer CSVoice' 0
    amix 'Voice_Tx Mixer PRI_MI2S_TX_Voice' 0
}

deconfigure_local_audio_for_9x50()
{
    # Deconfigure for both playing and recording
    amix 'MI2S_TX Channels' 0
    amix 'AIF1_CAP Mixer SLIM TX6' 0
    amix 'SLIM TX6 MUX' 0
    amix 'ADC MUX6' 0
    amix 'AMIC MUX6' 0
    amix 'DEC6 Volume' 0
    amix 'SLIM RX0 MUX' 0
    amix 'SLIM RX1 MUX' 0
    amix 'MI2S_RX Channels' 0
    amix 'RX INT1_1 MIX1 INP0' 0
    amix 'RX INT2_1 MIX1 INP0' 0
    amix 'RX INT1 DEM MUX' 0
    amix 'RX INT2 DEM MUX' 0
    amix 'PRI_MI2S_RX_Voice Mixer VoiceMMode1' 0
    amix 'VoiceMMode1_Tx Mixer PRI_MI2S_TX_MMode1' 0
}

deconfigure_local_audio()
{
    case "$PLATFORM" in
        MDM9640)
            deconfigure_local_audio_for_9x45_le_1x
            ;;

        MSMZIRC)
            deconfigure_local_audio_for_9x45_le_2x
            ;;

        MDM9628)
            deconfigure_local_audio_for_9x07
            ;;

        MDM9650)
            deconfigure_local_audio_for_9x50
            ;;
        *)
            echo "Unknown platform: $PLATFORM"
            ;;
    esac
}

configure_bluetooth_audio_for_9x45_le_1x()
{
    # Configure for Rx Line
    amix 'SEC_AUX_PCM_RX_Voice Mixer CSVoice' 1
    # Configure for Tx Line
    amix 'Voice_Tx Mixer SEC_AUX_PCM_TX_Voice' 1
}

configure_bluetooth_audio_for_9x45_le_2x()
{
    # Configure for Rx Line
    amix 'SEC_AUX_PCM_RX_Voice Mixer VoiceMMode1' 1
    # Configure for Tx Line
    amix 'VoiceMMode1_Tx Mixer SEC_AUX_PCM_TX_MMode1' 1
}

configure_bluetooth_audio_for_9x07()
{
    # Configure for Rx Line
    amix 'SEC_AUX_PCM_RX_Voice Mixer CSVoice' 1
    # Configure for Tx Line
    amix 'Voice_Tx Mixer SEC_AUX_PCM_TX_Voice' 1
}

configure_bluetooth_audio_for_9x50()
{
    # Configure for Rx Line
    amix 'SEC_AUX_PCM_RX_Voice Mixer VoiceMMode1' 1
    # Configure for Tx Line
    amix 'VoiceMMode1_Tx Mixer SEC_AUX_PCM_TX_MMode1' 1
}

configure_bluetooth_audio()
{
    case "$PLATFORM" in
        MDM9640)
            configure_bluetooth_audio_for_9x45_le_1x
            ;;

        MSMZIRC)
            configure_bluetooth_audio_for_9x45_le_2x
            ;;

        MDM9628)
            configure_bluetooth_audio_for_9x07
            ;;

        MDM9650)
            configure_bluetooth_audio_for_9x50
            ;;
        *)
            echo "Unknown platform: $PLATFORM"
            ;;
    esac
}

deconfigure_bluetooth_audio_for_9x45_le_1x()
{
    # Deconfigure for Rx Line
    amix 'SEC_AUX_PCM_RX_Voice Mixer CSVoice' 0
    # Deconfigure for Tx Line
    amix 'Voice_Tx Mixer SEC_AUX_PCM_TX_Voice' 0
}

deconfigure_bluetooth_audio_for_9x45_le_2x()
{
    # Deconfigure for Rx Line
    amix 'SEC_AUX_PCM_RX_Voice Mixer VoiceMMode1' 0
    # Deconfigure for Tx Line
    amix 'VoiceMMode1_Tx Mixer SEC_AUX_PCM_TX_MMode1' 0
}

deconfigure_bluetooth_audio_for_9x07()
{
    # Deconfigure for Rx Line
    amix 'SEC_AUX_PCM_RX_Voice Mixer CSVoice' 0
    # Deconfigure for Tx Line
    amix 'Voice_Tx Mixer SEC_AUX_PCM_TX_Voice' 0
}

deconfigure_bluetooth_audio_for_9x50()
{
    # Deconfigure for Rx Line
    amix 'SEC_AUX_PCM_RX_Voice Mixer VoiceMMode1' 0
    # Deconfigure for Tx Line
    amix 'VoiceMMode1_TX Mixer SEC_AUX_PCM_TX_MMode1' 0
}

deconfigure_bluetooth_audio()
{
    case "$PLATFORM" in
        MDM9640)
            deconfigure_bluetooth_audio_for_9x45_le_1x
            ;;

        MSMZIRC)
            deconfigure_bluetooth_audio_for_9x45_le_2x
            ;;

        MDM9628)
            deconfigure_bluetooth_audio_for_9x07
            ;;

        MDM9650)
            deconfigure_bluetooth_audio_for_9x50
            ;;
        *)
            echo "Unknown platform: $PLATFORM"
            ;;
    esac
}

start_local_audio()
{
    configure_local_audio
    # No need to start the audio service as audio service is started previously in start_bluetooth_audio
}

stop_local_audio()
{
    deconfigure_local_audio
    # No need to kill the audio service as audio service will be restared in start_bluetooth_audio
}

start_bluetooth_audio()
{
    configure_bluetooth_audio
    # It is necessary to restart the audio service because We don't know
    # if the arec/aplay is started by synergy for the first time we call start_bluetooth_audio
    restart_audio_service
}

stop_bluetooth_audio()
{
    deconfigure_bluetooth_audio
    # No need to kill the audio service as audio service will be used in start_local_audio
}

start_hfg_audio()
{
    stop_local_audio
    start_bluetooth_audio
}

stop_hfg_audio()
{
    stop_bluetooth_audio
    start_local_audio
}

case "$cmd" in
    init)
        init_hfg_audio
        ;;

    deinit)
        deinit_hfg_audio
        ;;

    start)
        start_hfg_audio
        ;;

    stop)
        stop_hfg_audio
        ;;

    *)
        echo "unknown para: $cmd"
        exit 1
        ;;
esac
