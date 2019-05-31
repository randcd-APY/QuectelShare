/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include <stdlib.h>

void init_amix_hf_audio_for_9x45_le()
{
    /* Configure Rx audio path */
    system("amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia5' 1");

    /* Configure Tx audio path */
    system("amix 'MultiMedia5 Mixer SEC_AUX_PCM_TX' 1");

    /* Configure for local audio playing */
    system("amix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 1");
    system("amix 'MI2S_RX Channels' 'One'");
    system("amix 'SLIM RX1 MUX' 'AIF1_PB'");
    system("amix 'RX1 MIX1 INP1' 'RX1'");
    system("amix 'CLASS_H_DSM MUX' 'DSM_HPHL_RX1'");
    system("amix 'DAC1 Switch' 1");
    system("amix 'RX1 Digital Volume' '67%'");

    /* Configure for local audio  recording */
    system("amix 'MultiMedia1 Mixer PRI_MI2S_TX' 1");
    system("amix 'MI2S_TX Channels' 'One'");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 1");
    system("amix 'SLIM TX7 MUX' 'DEC10'");
    system("amix 'DEC10 MUX' 'DMIC3'");
    system("amix 'DEC10 Volume' '75%'");
}

void deinit_amix_hf_audio_for_9x45_le()
{
    /* Deconfigure Rx audio path */
    system("amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia5' 0");

    /* Deconfigure Tx audio path */
    system("amix 'MultiMedia5 Mixer SEC_AUX_PCM_TX' 0");

    /* Deconfigure for local audio playing */
    system("amix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 0");
    system("amix 'MI2S_RX Channels' 0");
    system("amix 'SLIM RX1 MUX' 0");
    system("amix 'RX1 MIX1 INP1' 0");
    system("amix 'CLASS_H_DSM MUX' 0");
    system("amix 'DAC1 Switch' 0");

    /* Deconfigure for local audio recording */
    system("amix 'MultiMedia1 Mixer PRI_MI2S_TX' 0");
    system("amix 'MI2S_TX Channels' 0");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 0");
    system("amix 'SLIM TX7 MUX' 0");
    system("amix 'DEC10 MUX' 0");
}

void init_amix_hf_audio_for_9x07_le()
{
    /* Configure Rx audio path */
    system("amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia5' 1");

    /* Configure Tx audio path */
    system("amix 'MultiMedia5 Mixer SEC_AUX_PCM_TX' 1");

    /* Configure for local audio playing */
    system("amix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 1");
    system("amix 'MI2S_RX Channels' 'One'");
    system("amix 'SLIM RX1 MUX' 'AIF1_PB'");
    system("amix 'RX1 MIX1 INP1' 'RX1'");
    system("amix 'CLASS_H_DSM MUX' 'DSM_HPHL_RX1'");
    system("amix 'DAC1 Switch' 1");
    system("amix 'RX1 Digital Volume' '67%'");

    /* Configure for local audio  recording */
    system("amix 'MI2S_TX Channels' 'One'");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 1");
    system("amix 'SLIM TX7 MUX' 'DEC5'");
    system("amix 'DEC5 MUX' 'ADC2'");
    system("amix 'DEC5 Volume' '67%'");
    system("amix 'ADC2 Volume' '100'");
    system("amix 'MultiMedia1 Mixer PRI_MI2S_TX' 1");
}

void deinit_amix_hf_audio_for_9x07_le()
{
    /* Deconfigure Rx audio path */
    system("amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia5' 0");

    /* Deconfigure Tx audio path */
    system("amix 'MultiMedia5 Mixer SEC_AUX_PCM_TX' 0");

    /* Deconfigure for local audio playing */
    system("amix 'PRI_MI2S_RX Audio Mixer MultiMedia1' 0");
    system("amix 'MI2S_RX Channels' 0");
    system("amix 'SLIM RX1 MUX' 0");
    system("amix 'RX1 MIX1 INP1' 0");
    system("amix 'CLASS_H_DSM MUX' 0");
    system("amix 'DAC1 Switch' 0");

    /* Deconfigure for local audio recording */
    system("amix 'MI2S_TX Channels' 0");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 0");
    system("amix 'SLIM TX7 MUX' 0");
    system("amix 'DEC5 MUX' 0");
    system("amix 'MultiMedia1 Mixer PRI_MI2S_TX' 0");
}

void init_amix_hf_audio_for_9x50_le()
{
    /* Downlink BT-> Speak */
    system("amix 'SLIM RX0 MUX' 'AIF1_PB'");
    system("amix 'SLIM RX1 MUX' 'AIF1_PB'");
    system("amix 'MI2S_RX Channels' 'Two'");
    system("amix 'RX INT1_1 MIX1 INP0' 'RX0'");
    system("amix 'RX INT2_1 MIX1 INP0' 'RX1'");
    system("amix 'RX INT1 DEM MUX' 'CLSH_DSM_OUT'");
    system("amix 'RX INT2 DEM MUX' 'CLSH_DSM_OUT'");
    system("amix 'COMP1 Switch' 1");
    system("amix 'COMP2 Switch' 1");
    system("amix 'PRI_MI2S_RX Port Mixer SEC_AUX_PCM_UL_TX' 1");
    
    /* Uplink MIC->BT */
    system("amix 'MI2S_TX Channels' 'One'");
    system("amix 'AIF1_CAP Mixer SLIM TX6' 1");
    system("amix 'SLIM TX6 MUX' 'DEC6'");
    system("amix 'ADC MUX6' 'AMIC'");
    system("amix 'AMIC MUX6' 'ADC2'");
    system("amix 'DEC6 Volume' 84");
    system("amix 'SLIM RX0 MUX' 'AIF1_PB'");
    system("amix 'SLIM RX1 MUX' 'AIF1_PB'");
    system("amix 'MI2S_RX Channels' 'Two'");
    system("amix 'RX INT1_1 MIX1 INP0' 'RX0'");
    system("amix 'RX INT2_1 MIX1 INP0' 'RX1'");
    system("amix 'RX INT1 DEM MUX' 'CLSH_DSM_OUT'");
    system("amix 'RX INT2 DEM MUX' 'CLSH_DSM_OUT'");
    system("amix 'COMP1 Switch' 1");
    system("amix 'COMP2 Switch' 1");
    system("amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia6' 1");
    system("amix 'MultiMedia6 Mixer PRI_MI2S_TX' 1");
}

void deinit_amix_hf_audio_for_9x50_le()
{
    /* Downlink BT-> Speak */
    system("amix 'SLIM RX0 MUX' 0");
    system("amix 'SLIM RX1 MUX' 0");
    system("amix 'MI2S_RX Channels' 0");
    system("amix 'RX INT1_1 MIX1 INP0' 0");
    system("amix 'RX INT2_1 MIX1 INP0' 0");
    system("amix 'RX INT1 DEM MUX' 0");
    system("amix 'RX INT2 DEM MUX' 0");
    system("amix 'COMP1 Switch' 0");
    system("amix 'COMP2 Switch' 0");
    system("amix 'PRI_MI2S_RX Port Mixer SEC_AUX_PCM_UL_TX' 0");
    
    /* Uplink MIC->BT */
    system("amix 'MI2S_TX Channels' 0");
    system("amix 'AIF1_CAP Mixer SLIM TX6' 0");
    system("amix 'SLIM TX6 MUX' 0");
    system("amix 'ADC MUX6' 0");
    system("amix 'AMIC MUX6' 0");
    system("amix 'DEC6 Volume' 0");
    system("amix 'SLIM RX0 MUX' 0");
    system("amix 'SLIM RX1 MUX' 0");
    system("amix 'MI2S_RX Channels' 0");
    system("amix 'RX INT1_1 MIX1 INP0' 0");
    system("amix 'RX INT2_1 MIX1 INP0' 0");
    system("amix 'RX INT1 DEM MUX' 0");
    system("amix 'RX INT2 DEM MUX' 0");
    system("amix 'COMP1 Switch' 0");
    system("amix 'COMP2 Switch' 0");
    system("amix 'SEC_AUX_PCM_RX Audio Mixer MultiMedia6' 0");
    system("amix 'MultiMedia6 Mixer PRI_MI2S_TX' 0");


}

void init_amix_hfg_local_audio_for_9x07()
{
    /* Configure for both playing and recording */
    system("amix 'SLIM RX1 MUX' 'AIF1_PB'");
    system("amix 'SLIM RX2 MUX' 'AIF1_PB'");
    system("amix 'MI2S_RX Channels' 'Two'");
    system("amix 'RX1 MIX1 INP1' 'RX1'");
    system("amix 'RX2 MIX1 INP1' 'RX2'");
    system("amix 'HPHL DAC Switch' 1");
    system("amix 'HPHL Volume' '80%'");
    system("amix 'HPHR Volume' '80%'");
    system("amix 'CLASS_H_DSM MUX' 'DSM_HPHL_RX1'");
    system("amix 'RX1 Digital Volume' '67%'");
    system("amix 'RX2 Digital Volume' '67%'");
    system("amix 'MI2S_TX Channels' 'One'");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 1");
    system("amix 'SLIM TX7 MUX' 'DEC5'");
    system("amix 'DEC5 MUX' 'ADC2'");
    system("amix 'DEC5 Volume' '67%'");
    system("amix 'ADC2 Volume' '100'");
    system("amix 'PRI_MI2S_RX_Voice Mixer CSVoice' 1");
    system("amix 'Voice_Tx Mixer PRI_MI2S_TX_Voice' 1");
}

void deinit_amix_hfg_local_audio_for_9x07()
{
    /* Deconfigure for both playing and recording */
    system("amix 'SLIM RX1 MUX' 0");
    system("amix 'SLIM RX2 MUX' 0");
    system("amix 'MI2S_RX Channels' 0");
    system("amix 'RX1 MIX1 INP1' 0");
    system("amix 'RX2 MIX1 INP1' 0");
    system("amix 'HPHL DAC Switch' 0");
    system("amix 'CLASS_H_DSM MUX' 0");
    system("amix 'MI2S_TX Channels' 0");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 0");
    system("amix 'SLIM TX7 MUX' 0");
    system("amix 'DEC5 MUX' 0");
    system("amix 'PRI_MI2S_RX_Voice Mixer CSVoice' 0");
    system("amix 'Voice_Tx Mixer PRI_MI2S_TX_Voice' 0");
}

void init_amix_hfg_local_audio_for_9x50()
{
    /* Configure for both playing and recording */
    system("amix 'MI2S_TX Channels' 'One'");
    system("amix 'AIF1_CAP Mixer SLIM TX6' 1");
    system("amix 'SLIM TX6 MUX' 'DEC6'");
    system("amix 'ADC MUX6' 'AMIC'");
    system("amix 'AMIC MUX6' 'ADC2'");
    system("amix 'DEC6 Volume' 67");
    system("amix 'SLIM RX0 MUX' 'AIF1_PB'");
    system("amix 'SLIM RX1 MUX' 'AIF1_PB'");
    system("amix 'MI2S_RX Channels' 'Two'");
    system("amix 'RX INT1_1 MIX1 INP0' 'RX0'");
    system("amix 'RX INT2_1 MIX1 INP0' 'RX1'");
    system("amix 'RX INT1 DEM MUX' 'CLSH_DSM_OUT'");
    system("amix 'RX INT2 DEM MUX' 'CLSH_DSM_OUT'");
    system("amix 'PRI_MI2S_RX_Voice Mixer VoiceMMode1' 1");
    system("amix 'VoiceMMode1_Tx Mixer PRI_MI2S_TX_MMode1' 1");
}

void deinit_amix_hfg_local_audio_for_9x50()
{
    /* Deconfigure for both playing and recording */
    system("amix 'MI2S_TX Channels' 0");
    system("amix 'AIF1_CAP Mixer SLIM TX6' 0");
    system("amix 'SLIM TX6 MUX' 0");
    system("amix 'ADC MUX6' 0");
    system("amix 'AMIC MUX6' 0");
    system("amix 'DEC6 Volume' 0");
    system("amix 'SLIM RX0 MUX' 0");
    system("amix 'SLIM RX1 MUX' 0");
    system("amix 'MI2S_RX Channels' 0");
    system("amix 'RX INT1_1 MIX1 INP0' 0");
    system("amix 'RX INT2_1 MIX1 INP0' 0");
    system("amix 'RX INT1 DEM MUX' 0");
    system("amix 'RX INT2 DEM MUX' 0");
    system("amix 'PRI_MI2S_RX_Voice Mixer VoiceMMode1' 0");
    system("amix 'VoiceMMode1_Tx Mixer PRI_MI2S_TX_MMode1' 0");
}

void init_amix_hfg_local_audio_for_9x45_le_1x()
{
    /* Configure for both playing and recording */
    system("amix 'SLIM RX1 MUX' 'AIF1_PB'");
    system("amix 'SLIM RX2 MUX' 'AIF1_PB'");
    system("amix 'MI2S_RX Channels' 'Two'");
    system("amix 'RX1 MIX1 INP1' 'RX1'");
    system("amix 'RX2 MIX1 INP1' 'RX2'");
    system("amix 'HPHL DAC Switch' 1");
    system("amix 'HPHL Volume' '80%'");
    system("amix 'HPHR Volume' '80%'");
    system("amix 'CLASS_H_DSM MUX' 'DSM_HPHL_RX1'");
    system("amix 'RX1 Digital Volume' '67%'");
    system("amix 'RX2 Digital Volume' '67%'");
    system("amix 'MI2S_TX Channels' 'One'");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 1");
    system("amix 'SLIM TX7 MUX' 'DEC5'");
    system("amix 'DEC5 MUX' 'ADC2'");
    system("amix 'DEC5 Volume' '67%'");
    system("amix 'ADC2 Volume' '100'");
    system("amix 'PRI_MI2S_RX_Voice Mixer CSVoice' 1");
    system("amix 'Voice_Tx Mixer PRI_MI2S_TX_Voice' 1");
}

void deinit_amix_hfg_local_audio_for_9x45_le_1x()
{
    /* Deconfigure for both playing and recording */
    system("amix 'SLIM RX1 MUX' 0");
    system("amix 'SLIM RX2 MUX' 0");
    system("amix 'MI2S_RX Channels' 0");
    system("amix 'RX1 MIX1 INP1' 0");
    system("amix 'RX2 MIX1 INP1' 0");
    system("amix 'HPHL DAC Switch' 0");
    system("amix 'CLASS_H_DSM MUX' 0");
    system("amix 'MI2S_TX Channels' 0");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 0");
    system("amix 'SLIM TX7 MUX' 0");
    system("amix 'DEC5 MUX' 0");
    system("amix 'PRI_MI2S_RX_Voice Mixer CSVoice' 0");
    system("amix 'Voice_Tx Mixer PRI_MI2S_TX_Voice' 0");
}

void init_amix_hfg_local_audio_for_9x45_le_2x()
{
    /* Configure for both playing and recording */
    system("amix 'SLIM RX1 MUX' 'AIF1_PB'");
    system("amix 'SLIM RX2 MUX' 'AIF1_PB'");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 1");
    system("amix 'RX1 MIX1 INP1' 'RX1'");
    system("amix 'RX2 MIX1 INP1' 'RX2'");
    system("amix 'HPHL DAC Switch' 1");
    system("amix 'HPHL Volume' '80%'");
    system("amix 'HPHR Volume' '80%'");
    system("amix 'RX1 Digital Volume' '67%'");
    system("amix 'RX2 Digital Volume' '67%'");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 1");
    system("amix 'SLIM TX7 MUX' 'DEC5'");
    system("amix 'CLASS_H_DSM MUX' 'DSM_HPHL_RX1'");
    system("amix 'DEC5 MUX' 'ADC2'");
    system("amix 'DEC5 Volume' '67%'");
    system("amix 'ADC2 Volume' '65'");
    system("amix 'PRI_MI2S_RX_Voice Mixer VoiceMMode1' 1");
    system("amix 'VoiceMMode1_Tx Mixer PRI_MI2S_TX_MMode1' 1");
}

void deinit_amix_hfg_local_audio_for_9x45_le_2x()
{
    /* Deconfigure for both playing and recording */
    system("amix 'SLIM RX1 MUX'  0");
    system("amix 'SLIM RX2 MUX' 0");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 0");
    system("amix 'RX1 MIX1 INP1' 0");
    system("amix 'RX2 MIX1 INP1' 0");
    system("amix 'HPHL DAC Switch' 0");
    system("amix 'AIF1_CAP Mixer SLIM TX7' 0");
    system("amix 'SLIM TX7 MUX' 0");
    system("amix 'CLASS_H_DSM MUX' 0");
    system("amix 'DEC5 MUX' 0");
    system("amix 'PRI_MI2S_RX_Voice Mixer VoiceMMode1' 0");
    system("amix 'VoiceMMode1_Tx Mixer PRI_MI2S_TX_MMode1' 0");
}

void init_amix_hfg_bt_audio_for_9x07()
{
    /* Configure for Rx Line */
    system("amix 'SEC_AUX_PCM_RX_Voice Mixer CSVoice' 1");

    /* Configure for Tx Line */
    system("amix 'Voice_Tx Mixer SEC_AUX_PCM_TX_Voice' 1");
}

void deinit_amix_hfg_bt_audio_for_9x07()
{
    /* Deconfigure for Rx Line */
    system("amix 'SEC_AUX_PCM_RX_Voice Mixer CSVoice' 0");

    /* Deconfigure for Tx Line */
    system("amix 'Voice_Tx Mixer SEC_AUX_PCM_TX_Voice' 0");
}

void init_amix_hfg_bt_audio_for_9x45_le_1x()
{
    /* Configure for Rx Line */
    system("amix 'SEC_AUX_PCM_RX_Voice Mixer CSVoice' 1");

    /* Configure for Tx Line */
    system("amix 'Voice_Tx Mixer SEC_AUX_PCM_TX_Voice' 1");
}

void deinit_amix_hfg_bt_audio_for_9x45_le_1x()
{
    /* Deconfigure for Rx Line */
    system("amix 'SEC_AUX_PCM_RX_Voice Mixer CSVoice' 0");

    /* Deconfigure for Tx Line */
    system("amix 'Voice_Tx Mixer SEC_AUX_PCM_TX_Voice' 0");
}

void init_amix_hfg_bt_audio_for_9x45_le_2x()
{
    /* Configure for Rx Line */
    system("amix 'SEC_AUX_PCM_RX_Voice Mixer VoiceMMode1' 1");

    /* Configure for Tx Line */
    system("amix 'VoiceMMode1_TX Mixer SEC_AUX_PCM_TX_MMode1' 1");
}

void deinit_amix_hfg_bt_audio_for_9x45_le_2x()
{
    /* Deconfigure for Rx Line */
    system("amix 'SEC_AUX_PCM_RX_Voice Mixer VoiceMMode1' 0");

    /* Deconfigure for Tx Line */
    system("amix 'VoiceMMode1_TX Mixer SEC_AUX_PCM_TX_MMode1' 0");
}

void init_amix_hfg_bt_audio_for_9x50()
{
    /* Configure for Rx Line */
    system("amix 'SEC_AUX_PCM_RX_Voice Mixer VoiceMMode1' 1");

    /* Configure for Tx Line */
    system("amix 'VoiceMMode1_Tx Mixer SEC_AUX_PCM_TX_MMode1' 1");
}

void deinit_amix_hfg_bt_audio_for_9x50()
{
    /* Deconfigure for Rx Line */
    system("amix 'SEC_AUX_PCM_RX_Voice Mixer VoiceMMode1' 0");

    /* Deconfigure for Tx Line */
    system("amix 'VoiceMMode1_Tx Mixer SEC_AUX_PCM_TX_MMode1' 0");
}

void configure_afe_port_sample_rate(unsigned int sample_rate)
{
    if (sample_rate == 16000)
        system("amix 'AUX PCM SampleRate' 'rate_16000'");
    else if (sample_rate == 8000)
        system("amix 'AUX PCM SampleRate' 'rate_8000'");
}

