
###############################################################################
#
#     Copyright (c) 2016 Qualcomm Technologies International, Ltd.
#     All Rights Reserved. 
#     Qualcomm Technologies International, Ltd. Confidential and Proprietary.
#
###############################################################################

###############################################################################
#
# File: bt_hf_audio.sh
#
# Description : Script to transfer Bluetooth HF audio.
#
# Version : 1.0
#
# Notes:
# (1) The script is launched if SCO/eSCO is established during running HF.
# (2) The script is based on ALSA application ("arec" & "aplay").
# (3) In MDM platform, sec aux pcm is connected with PCM interface in BT chip.
#
###############################################################################

#!/bin/sh

echo "bt_hf_audio"

cmd=$1
sample_rate=$2
PLATFORM=`cat /proc/cpuinfo | grep Hardware | cut -d ' ' -f5`

if [ "$cmd" = "start" ]; then
    echo "$cmd hf audio $sample_rate"
else
    echo "$cmd hf audio"
fi

init_hf_audio_for_9x45_le()
{
    echo "configure hf audio path"

    # Configure Rx audio path
    amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia5' 1

    # Configure Tx audio path
    amix 'MultiMedia5 Mixer SEC_AUX_PCM_TX' 1

    # Configure for local audio playing
    amix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 1
    amix 'MI2S_RX Channels' 'One'
    amix 'SLIM RX1 MUX' 'AIF1_PB'
    amix 'RX1 MIX1 INP1' 'RX1'
    amix 'CLASS_H_DSM MUX' 'DSM_HPHL_RX1'
    amix 'DAC1 Switch' 1
    amix 'RX1 Digital Volume' '67%'

    # Configure for local audio recording
    amix 'MultiMedia1 Mixer PRI_MI2S_TX' 1
    amix 'MI2S_TX Channels' 'One'
    amix 'AIF1_CAP Mixer SLIM TX7' 1
    amix 'SLIM TX7 MUX' 'DEC10'
    amix 'DEC10 MUX' 'DMIC3'
    amix 'DEC10 Volume' '75%'
}

init_hf_audio_for_9x07_le()
{
    echo "configure hf audio path"

    # Configure Rx audio path
    amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia5' 1

    # Configure Tx audio path
    amix 'MultiMedia5 Mixer SEC_AUX_PCM_TX' 1

    # Configure for local audio playing
    amix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 1
    amix 'MI2S_RX Channels' 'One'
    amix 'SLIM RX1 MUX' 'AIF1_PB'
    amix 'RX1 MIX1 INP1' 'RX1'
    amix 'CLASS_H_DSM MUX' 'DSM_HPHL_RX1'
    amix 'DAC1 Switch' 1
    amix 'RX1 Digital Volume' '67%'

    # Configure for local audio recording
    amix 'MI2S_TX Channels' 'One'
    amix 'AIF1_CAP Mixer SLIM TX7' 1
    amix 'SLIM TX7 MUX' 'DEC5'
    amix 'DEC5 MUX' 'ADC2'
    amix 'DEC5 Volume' '67%'
    amix 'ADC2 Volume' '100'
    amix 'MultiMedia1 Mixer PRI_MI2S_TX' 1
}

init_hf_audio_for_9x50_le()
{
    amix 'SLIM RX0 MUX' 'AIF1_PB'
    amix 'SLIM RX1 MUX' 'AIF1_PB'
    amix 'MI2S_RX Channels' 'Two'
    amix 'RX INT1_1 MIX1 INP0' 'RX0'
    amix 'RX INT2_1 MIX1 INP0' 'RX1'
    amix 'RX INT1 DEM MUX' 'CLSH_DSM_OUT'
    amix 'RX INT2 DEM MUX' 'CLSH_DSM_OUT'
    amix 'COMP1 Switch' 1
    amix 'COMP2 Switch' 1
    amix 'PRI_MI2S_RX Port Mixer SEC_AUX_PCM_UL_TX' 1
    
    amix 'MI2S_TX Channels' 'One'
    amix 'AIF1_CAP Mixer SLIM TX6' 1
    amix 'SLIM TX6 MUX' 'DEC6'
    amix 'ADC MUX6' 'AMIC'
    amix 'AMIC MUX6' 'ADC2'
    amix 'DEC6 Volume' 84
    amix 'SLIM RX0 MUX' 'AIF1_PB'
    amix 'SLIM RX1 MUX' 'AIF1_PB'
    amix 'MI2S_RX Channels' 'Two'
    amix 'RX INT1_1 MIX1 INP0' 'RX0'
    amix 'RX INT2_1 MIX1 INP0' 'RX1'
    amix 'RX INT1 DEM MUX' 'CLSH_DSM_OUT'
    amix 'RX INT2 DEM MUX' 'CLSH_DSM_OUT'
    amix 'COMP1 Switch' 1
    amix 'COMP2 Switch' 1
    amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia6' 1
    amix 'MultiMedia6 Mixer PRI_MI2S_TX' 1
}

init_hf_audio()
{
    case "$PLATFORM" in
        MDM9640)
            init_hf_audio_for_9x45_le
            ;;

        MSMZIRC)
            init_hf_audio_for_9x45_le
            ;;

        MDM9628)
            init_hf_audio_for_9x07_le
            ;;

        MDM9650)
            init_hf_audio_for_9x50_le
            ;;
        *)
            echo "Unknown platform: $PLATFORM"
            ;;
    esac
}

deinit_hf_audio_for_9x45_le()
{
    echo "deconfigure hf audio path"

    # Deconfigure Rx audio path
    amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia5' 0

    # Deconfigure Tx audio path
    amix 'MultiMedia5 Mixer SEC_AUX_PCM_TX' 0

    # Deconfigure for local audio playing
    amix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 0
    amix 'MI2S_RX Channels' 0
    amix 'SLIM RX1 MUX' 0
    amix 'RX1 MIX1 INP1' 0
    amix 'CLASS_H_DSM MUX' 0
    amix 'DAC1 Switch' 0
    # Just keep the changed volume
    # amix 'RX1 Digital Volume' '67%'

    # Deconfigure for local audio recording
    amix 'MultiMedia1 Mixer PRI_MI2S_TX' 0
    amix 'MI2S_TX Channels' 0
    amix 'AIF1_CAP Mixer SLIM TX7' 0
    amix 'SLIM TX7 MUX' 0
    amix 'DEC10 MUX' 0
    # Just keep the changed volume
    # amix 'DEC10 Volume' '75%'
}

deinit_hf_audio_for_9x07_le()
{
    echo "deconfigure hf audio path"

    # Deconfigure Rx audio path
    amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia5' 0

    # Deconfigure Tx audio path
    amix 'MultiMedia5 Mixer SEC_AUX_PCM_TX' 0

    # Deconfigure for local audio playing
    amix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 0
    amix 'MI2S_RX Channels' 0
    amix 'SLIM RX1 MUX' 0
    amix 'RX1 MIX1 INP1' 0
    amix 'CLASS_H_DSM MUX' 0
    amix 'DAC1 Switch' 0
    # Just keep the changed volume
    # amix 'RX1 Digital Volume' '67%'

    # Deconfigure for local audio recording
    amix 'MI2S_TX Channels' 'One'
    amix 'AIF1_CAP Mixer SLIM TX7' 1
    amix 'SLIM TX7 MUX' 'DEC5'
    amix 'DEC5 MUX' 'ADC2'
    # Just keep the changed volume
    # amix 'DEC5 Volume' '67%'
    # amix 'ADC2 Volume' '100'
    amix 'MultiMedia1 Mixer PRI_MI2S_TX' 1
}

deinit_hf_audio_for_9x50_le()
{
    amix 'SLIM RX0 MUX' 0
    amix 'SLIM RX1 MUX' 0
    amix 'MI2S_RX Channels' 0
    amix 'RX INT1_1 MIX1 INP0' 0
    amix 'RX INT2_1 MIX1 INP0' 0
    amix 'RX INT1 DEM MUX' 0
    amix 'RX INT2 DEM MUX' 0
    amix 'COMP1 Switch' 0
    amix 'COMP2 Switch' 0
    amix 'PRI_MI2S_RX Port Mixer SEC_AUX_PCM_UL_TX' 0
    
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
    amix 'COMP1 Switch' 0
    amix 'COMP2 Switch' 1
    amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia6' 0
    amix 'MultiMedia6 Mixer PRI_MI2S_TX' 0

}

deinit_hf_audio()
{
    case "$PLATFORM" in
        MDM9640)
            deinit_hf_audio_for_9x45_le
            ;;

        MSMZIRC)
            deinit_hf_audio_for_9x45_le
            ;;

        MDM9628)
            deinit_hf_audio_for_9x07_le
            ;;

        MDM9650)
            deinit_hf_audio_for_9x50_le
            ;;
        *)
            echo "Unknown platform: $PLATFORM"
            ;;
    esac
}

start_audio_service()
{
    case "$PLATFORM" in
        MDM9650)
            arec -D hw:0,33 -P -R 8000 -C 1 | aplay -D hw:0,3 -P -R 8000 -C 1 &        
            arec -D hw:0,15 -P | aplay -D hw:0,15 -P &        
	    ;;
	*)
    	    arec -C1 -R$sample_rate -FS16_LE | aplay -Dhw:0,16 -C1 -R$sample_rate -FS16_LE &
    	    arec -Dhw:0,16 -C1 -R$sample_rate -FS16_LE | aplay -C1 -R$sample_rate -FS16_LE &
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

start_hf_audio()
{
    stop_audio_service
    start_audio_service
}

stop_hf_audio()
{
    stop_audio_service
}

case "$cmd" in
    init)
        init_hf_audio
        ;;

    deinit)
        deinit_hf_audio
        ;;

    start)
        start_hf_audio
        ;;

    stop)
        stop_hf_audio
        ;;

    *)
        echo "unknown para: $cmd"
        exit 1
        ;;
esac

