#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dsrc_dcc.h"

#define DCC_NDL_ACTIVE_STATE_SIZE 4

dcc_ndl_chan g_chan[2];
dcc_ndl_active_state_config g_state_cfg[8];

int str_to_ac(char *token)
{
    if (!strcasecmp(token, "AC_BK")) {
        return DCC_AC_BK;
    } else if (!strcasecmp(token, "AC_BE")) {
        return DCC_AC_BE;
    } else if (!strcasecmp(token, "AC_VI")) {
        return DCC_AC_VI;
    } else if (!strcasecmp(token, "AC_VO")) {
        return DCC_AC_VO;
    }

    return DCC_AC_BK;
}

int str_to_mcs(char *token)
{
    if (!strcasecmp(token, "MCS0")) {
        return 0;
    } else if (!strcasecmp(token, "MCS1")) {
        return 1;
    } else if (!strcasecmp(token, "MCS2")) {
        return 2;
    } else if (!strcasecmp(token, "MCS3")) {
        return 3;
    } else if (!strcasecmp(token, "MCS4")) {
        return 4;
    } else if (!strcasecmp(token, "MCS5")) {
        return 5;
    } else if (!strcasecmp(token, "MCS6")) {
        return 6;
    } else if (!strcasecmp(token, "MCS7")) {
        return 7;
    }

    return 0;
}

int str_to_state_id(char *token)
{
    if (!strcasecmp(token, "ActiveState_1")) {
        return 1;
    }
    if (!strcasecmp(token, "ActiveState_2")) {
        return 2;
    }
    if (!strcasecmp(token, "ActiveState_3")) {
        return 3;
    }
    if (!strcasecmp(token, "ActiveState_4")) {
        return 4;
    }

    return 1;
}

int tokenize_string(char *line, char **tokens)
{
    char *token;
    int i = 0;

    token =  strtok(line, "=,- \n");
    while (token) {
        if (token[0] == '#') {
            return 0;
        }
        tokens[i++] = token;
        DCC_DEBUG_PRINT("%d: %lu, %s\n", i, (unsigned long) strlen(token), token);
        token = strtok(NULL, "=,- \n");
    }

    return i;
}

unsigned char g_num_channels = 0;

void update_dcc_config(int num_tokens, char **tokens,
                        dcc_ndl_chan *chan,
                        dcc_ndl_active_state_config *state_cfg)
{
    int i = 0, num_chan = 0, state_id;

    if (!strcasecmp(tokens[0], "DCC_Channels")) {
        g_num_channels = num_tokens - 1;

        for (i = 0; i < g_num_channels; i++) {
            DCC_CHAN_FREQ_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "DCC_ControlLoopEnable")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_DCC_ENABLE_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "DCC_StatsEnable")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_DCC_STATS_ENABLE_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "DCC_MinDccSampling")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MIN_DCC_SAMPLING_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "DCC_MeasurementInterval")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MEASURE_INTERVAL_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "DCC_StatsInterval")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_DCC_STATS_INTERVAL_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_minTxPower")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MIN_TX_POWER_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_MaxTxPower")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MAX_TX_POWER_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_defTxPower")) {
        num_chan = num_tokens - 2;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_DEF_TX_POWER_SET(&chan[i], str_to_ac(tokens[1]), atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_maxPacketDuration")) {
        num_chan = num_tokens - 2;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MAX_PACKET_DURATION_SET(&chan[i], str_to_ac(tokens[1]), atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_minPacketInterval")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MIN_PACKET_INTERVAL_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_maxPacketInterval")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MAX_PACKET_INTERVAL_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_defPacketInterval")) {
        num_chan = num_tokens - 2;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_DEF_PACKET_INTERVAL_SET(&chan[i], str_to_ac(tokens[1]), atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_minDatarate")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MIN_DATARATE_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_maxDatarate")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MAX_DATARATE_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_defDatarate")) {
        num_chan = num_tokens - 2;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_DEF_DATARATE_SET(&chan[i], str_to_ac(tokens[1]), atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_minCarrierSense")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MIN_CARRIER_SENSE_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_maxCarrierSense")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MAX_CARRIER_SENSE_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_defCarrierSense")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_DEF_CARRIER_SENSE_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_defDccSensitivity")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_DEF_DCC_SENSITIVITY_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_maxCsRange")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MAX_CS_RANGE_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_refPathloss")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_REF_PATH_LOSS_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_minSNR")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MIN_SNR_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_snrBackoff")) {
        num_chan = num_tokens - 2;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_SNR_BACKOFF_SET(&chan[i], str_to_mcs(tokens[1]), atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_tmPacketArrivalRate")) {
        num_chan = num_tokens - 2;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_TM_PACKET_ARRIVAL_RATE_SET(&chan[i], str_to_ac(tokens[1]), atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_tmPacketAvgDuration")) {
        num_chan = num_tokens - 2;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_TM_PACKET_AVG_DURATION_SET(&chan[i], str_to_ac(tokens[1]), atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_tmSignalAvgPower")) {
        num_chan = num_tokens - 2;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_TM_SIGNAL_AVG_POWER_SET(&chan[i], str_to_ac(tokens[1]), atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_maxChannelUse")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_TM_MAX_CHANNEL_USE_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_tmChannelUse")) {
        num_chan = num_tokens - 2;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_TM_CHANNEL_USE_SET(&chan[i], str_to_ac(tokens[1]), atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_minChannelLoad")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MIN_CHANNEL_LOAD_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_maxChannelLoad")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_MAX_CHANNEL_LOAD_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_numQueue")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_NUM_QUEUE_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_refQueueStatus")) {
        num_chan = num_tokens - 2;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_REF_QUEUE_STATUS_SET(&chan[i], str_to_ac(tokens[1]), atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_queueLen")) {
        num_chan = num_tokens - 2;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_REF_QUEUE_LEN_SET(&chan[i], str_to_ac(tokens[1]), atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_timeUp")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_TIME_UP_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_timeDown")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_TIME_DOWN_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_NumActiveStates")) {
        num_chan = num_tokens - 1;

        for (i = 0; i < num_chan; i++) {
            for (state_id = 1; state_id < 5; state_id++) {
                DCC_NDL_AS_STATE_ID_SET(&state_cfg[i*4+state_id-1], state_id);
            }
            DCC_NDL_NUM_ACTIVE_STATE_SET(&chan[i], atoi(tokens[i+1]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_asChanLoad")) {
        num_chan = num_tokens - 2;

        if (num_tokens < 3) {
            return;
        }

        state_id = str_to_state_id(tokens[1]);

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_AS_CHAN_LOAD_SET(&state_cfg[i*4+state_id-1], atoi(tokens[i+2]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_asDcc")) {
        num_chan = num_tokens - 3;

        if (num_tokens < 4) {
            return;
        }

        state_id = str_to_state_id(tokens[2]);

        for (i = 0; i < num_chan; i++) {
            DCC_NDL_AS_DCC_SET(&state_cfg[i*4+state_id-1], str_to_ac(tokens[1]), atoi(tokens[i+3]));
        }
    } else if (!strcasecmp(tokens[0], "NDL_asTxPower")) {
        num_chan = num_tokens - 3;

        if (num_tokens < 4) {
            return;
        }

        state_id = str_to_state_id(tokens[2]);

        for (i = 0; i < num_chan; i++) {
            if (!strcasecmp(tokens[i+3],"ref")) {
                DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+state_id-1], str_to_ac(tokens[1]), INVALID_TXPOWER);
            } else {
                DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+state_id-1], str_to_ac(tokens[1]), atoi(tokens[i+3]));
            }
        }
    } else if (!strcasecmp(tokens[0], "NDL_asPacketInterval")) {
        num_chan = num_tokens - 3;

        if (num_tokens < 4) {
            return;
        }

        state_id = str_to_state_id(tokens[2]);

        for (i = 0; i < num_chan; i++) {
            if (!strcasecmp(tokens[i+3],"ref")) {
                DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+state_id-1], str_to_ac(tokens[1]), INVALID_PACKETINTERVAL);
            } else {
                DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+state_id-1], str_to_ac(tokens[1]), atoi(tokens[i+3]));
            }
        }
    } else if (!strcasecmp(tokens[0], "NDL_asDatarate")) {
        num_chan = num_tokens - 3;

        if (num_tokens < 4) {
            return;
        }

        state_id = str_to_state_id(tokens[2]);

        for (i = 0; i < num_chan; i++) {
            if (!strcasecmp(tokens[i+3],"ref")) {
                DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+state_id-1], str_to_ac(tokens[1]), INVALID_DATARATE);
            } else {
                DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+state_id-1], str_to_ac(tokens[1]), atoi(tokens[i+3]));
            }
        }
    } else if (!strcasecmp(tokens[0], "NDL_asCarrierSense")) {
        num_chan = num_tokens - 3;

        if (num_tokens < 4) {
            return;
        }

        state_id = str_to_state_id(tokens[2]);

        for (i = 0; i < num_chan; i++) {
            if (!strcasecmp(tokens[i+3],"ref")) {
                DCC_NDL_AS_CARRIER_SENSE_SET(&state_cfg[i*4+state_id-1], str_to_ac(tokens[1]), INVALID_RXPOWER);
            } else {
                DCC_NDL_AS_CARRIER_SENSE_SET(&state_cfg[i*4+state_id-1], str_to_ac(tokens[1]), atoi(tokens[i+3]));
            }
        }
    }


}


void initialize_dcc_config(dcc_ndl_chan *chan,
                            dcc_ndl_active_state_config *state_cfg)
{
    unsigned long i = 0, ac, num_chan = 2, state_id;
    unsigned long chan_freq[2] = {5860, 5870};
    unsigned long max_pkt_dur[2]= {125,125};
    unsigned long min_pkt_intr[2] = {4,4};
    unsigned long max_pkt_intr[2] = {200,200};
    unsigned long def_pkt_intr[2] = {50,50};
    unsigned long min_dr[2] = {2,2};
    unsigned long max_dr[2] = {5,5};
    unsigned long def_dr[2] = {2,2};
    unsigned long min_cs[2] = {110,110};
    unsigned long max_cs[2] = {50,50};
    unsigned long def_cs[2] = {90,90};
    unsigned long def_dcc_sens[2] = {90,90};
    unsigned long max_cs_r[2] = {1000,1000};
    unsigned long ref_pathl[2] = {10,10};
    unsigned long min_snr[2] = {40,40};
    unsigned long bo_mcs0[2] = {20,20};
    unsigned long bo_mcs1[2] = {22,22};
    unsigned long bo_mcs2[2] = {26,26};
    unsigned long bo_mcs3[2] = {30,30};
    unsigned long bo_mcs4[2] = {36,36};
    unsigned long bo_mcs5[2] = {44,44};
    unsigned long bo_mcs6[2] = {52,52};
    unsigned long bo_mcs7[2] = {54,54};
    unsigned long pkt_arr_r[2] = {0, 0};
    unsigned long pkt_avg_dur[2] = {0,0};
    unsigned long signal_avg_pwr[2] = {0,0};
    unsigned long max_chan_use[2] = {0,0};
    unsigned long chan_use[2] = {0,0};
    unsigned long min_channelld[2] = {150,200};
    unsigned long max_channelld[2] = {400,500};
    unsigned long num_q[2] = {4,4};
    unsigned long ref_q_status[2] = {1,1};
    unsigned long ref_q_len[2] = {2,8};
    unsigned long time_up[2] = {100,100};
    unsigned long time_dn[2] = {500,500};
    unsigned long num_a_states[2] = {4,4};

    unsigned long as1_chanload[2] = {250,250};
    unsigned long as1_dcc_BK[2] = {1,1};
    unsigned long as1_dcc_BE[2] = {1,1};
    unsigned long as1_dcc_VI[2] = {0,0};
    unsigned long as1_dcc_VO[2] = {0,0};
    unsigned long as1_txpwr_BK[2] = {80,80};
    unsigned long as1_txpwr_BE[2] = {90,90};
    unsigned long as1_txpwr_VI[2] = {INVALID_TXPOWER,INVALID_TXPOWER};
    unsigned long as1_txpwr_VO[2] = {INVALID_TXPOWER,INVALID_TXPOWER};
    unsigned long as1_pkt_intr[2] = {INVALID_PACKETINTERVAL,INVALID_PACKETINTERVAL};
    unsigned long as1_dr[2] = {INVALID_DATARATE,INVALID_DATARATE};
    unsigned long as_cs[2] = {INVALID_RXPOWER,INVALID_RXPOWER};

    unsigned long as2_chanload[2] = {300,300};
    unsigned long as2_dcc_BK[2] = {3,3};
    unsigned long as2_dcc_BE[2] = {1,1};
    unsigned long as2_dcc_VI[2] = {1,1};
    unsigned long as2_dcc_VO[2] = {1,1};
    unsigned long as2_txpwr_BK[2] = {60,60};
    unsigned long as2_txpwr_BE[2] = {80,80};
    unsigned long as2_txpwr_VI[2] = {90,90};
    unsigned long as2_txpwr_VO[2] = {90,90};
    unsigned long as2_pkt_intr_BK[2] = {100,100};
    unsigned long as2_pkt_intr_BE[2] = {INVALID_PACKETINTERVAL,INVALID_PACKETINTERVAL};
    unsigned long as2_pkt_intr_VI[2] = {INVALID_PACKETINTERVAL,INVALID_PACKETINTERVAL};
    unsigned long as2_pkt_intr_VO[2] = {INVALID_PACKETINTERVAL,INVALID_PACKETINTERVAL};
    unsigned long as2_dr[2] = {INVALID_DATARATE,INVALID_DATARATE};

    unsigned long as3_chanload[2] = {350,350};
    unsigned long as3_dcc_BK[2] = {7,7};
    unsigned long as3_dcc_BE[2] = {7,7};
    unsigned long as3_dcc_VI[2] = {1,1};
    unsigned long as3_dcc_VO[2] = {1,1};
    unsigned long as3_txpwr_BK[2] = {50,50};
    unsigned long as3_txpwr_BE[2] = {60,60};
    unsigned long as3_txpwr_VI[2] = {70,70};
    unsigned long as3_txpwr_VO[2] = {70,70};
    unsigned long as3_pkt_intr_BK[2] = {150,150};
    unsigned long as3_pkt_intr_BE[2] = {100,100};
    unsigned long as3_pkt_intr_VI[2] = {INVALID_PACKETINTERVAL,INVALID_PACKETINTERVAL};
    unsigned long as3_pkt_intr_VO[2] = {INVALID_PACKETINTERVAL,INVALID_PACKETINTERVAL};
    unsigned long as3_dr_BK[2] = {3,3};
    unsigned long as3_dr_BE[2] = {3,3};
    unsigned long as3_dr_VI[2] = {INVALID_DATARATE,INVALID_DATARATE};
    unsigned long as3_dr_VO[2] = {INVALID_DATARATE,INVALID_DATARATE};

    unsigned long as4_chanload[2] = {400,400};
    unsigned long as4_dcc_BK[2] = {7,7};
    unsigned long as4_dcc_BE[2] = {7,7};
    unsigned long as4_dcc_VI[2] = {7,7};
    unsigned long as4_dcc_VO[2] = {5,5};
    unsigned long as4_txpwr_BK[2] = {20,20};
    unsigned long as4_txpwr_BE[2] = {30,30};
    unsigned long as4_txpwr_VI[2] = {40,40};
    unsigned long as4_txpwr_VO[2] = {10,10};
    unsigned long as4_pkt_intr_BK[2] = {200,200};
    unsigned long as4_pkt_intr_BE[2] = {150,150};
    unsigned long as4_pkt_intr_VI[2] = {100,100};
    unsigned long as4_pkt_intr_VO[2] = {INVALID_PACKETINTERVAL,INVALID_PACKETINTERVAL};
    unsigned long as4_dr_BK[2] = {5,5};
    unsigned long as4_dr_BE[2] = {5,5};
    unsigned long as4_dr_VI[2] = {4,4};
    unsigned long as4_dr_VO[2] = {4,4};

    for (i = 0; i < num_chan; i++) {

        DCC_CHAN_FREQ_SET(&chan[i], chan_freq[i]);
        DCC_NDL_NUM_ACTIVE_STATE_SET(&chan[i], 0);
        DCC_NDL_DCC_ENABLE_SET(&chan[i], 0);
        DCC_NDL_DCC_STATS_ENABLE_SET(&chan[i], 0);
        DCC_NDL_MIN_DCC_SAMPLING_SET(&chan[i], 1);
        DCC_NDL_MEASURE_INTERVAL_SET(&chan[i], 500);
        DCC_NDL_DCC_STATS_INTERVAL_SET(&chan[i], 500);
        DCC_NDL_MIN_TX_POWER_SET(&chan[i], 20);
        DCC_NDL_MAX_TX_POWER_SET(&chan[i], 106);

        for (ac = DCC_AC_VO; ac < DCC_AC_MAX; ac++) {
            DCC_NDL_DEF_TX_POWER_SET(&chan[i], ac, 86);
            DCC_NDL_MAX_PACKET_DURATION_SET(&chan[i], ac, max_pkt_dur[i]);
            DCC_NDL_DEF_PACKET_INTERVAL_SET(&chan[i], ac, def_pkt_intr[i]);
            DCC_NDL_DEF_DATARATE_SET(&chan[i], ac, def_dr[i]);
            DCC_NDL_TM_PACKET_ARRIVAL_RATE_SET(&chan[i], ac, pkt_arr_r[i]);
            DCC_NDL_TM_PACKET_AVG_DURATION_SET(&chan[i], ac, pkt_avg_dur[i]);
            DCC_NDL_TM_SIGNAL_AVG_POWER_SET(&chan[i], ac, signal_avg_pwr[i]);
            DCC_NDL_TM_CHANNEL_USE_SET(&chan[i], ac, chan_use[i]);
            DCC_NDL_REF_QUEUE_STATUS_SET(&chan[i], ac, ref_q_status[i]);
            DCC_NDL_REF_QUEUE_LEN_SET(&chan[i], ac, ref_q_len[i]);
        }

        DCC_NDL_MIN_PACKET_INTERVAL_SET(&chan[i], min_pkt_intr[i]);
        DCC_NDL_MAX_PACKET_INTERVAL_SET(&chan[i], max_pkt_intr[i]);
        DCC_NDL_MIN_DATARATE_SET(&chan[i], min_dr[i]);
        DCC_NDL_MAX_DATARATE_SET(&chan[i], max_dr[i]);
        DCC_NDL_MIN_CARRIER_SENSE_SET(&chan[i], min_cs[i]);
        DCC_NDL_MAX_CARRIER_SENSE_SET(&chan[i], max_cs[i]);
        DCC_NDL_DEF_CARRIER_SENSE_SET(&chan[i], def_cs[i]);
        DCC_NDL_DEF_DCC_SENSITIVITY_SET(&chan[i], def_dcc_sens[i]);
        DCC_NDL_MAX_CS_RANGE_SET(&chan[i], max_cs_r[i]);
        DCC_NDL_REF_PATH_LOSS_SET(&chan[i], ref_pathl[i]);
        DCC_NDL_MIN_SNR_SET(&chan[i], min_snr[i]);

        DCC_NDL_SNR_BACKOFF_SET(&chan[i], 0, bo_mcs0[i]);
        DCC_NDL_SNR_BACKOFF_SET(&chan[i], 1, bo_mcs1[i]);
        DCC_NDL_SNR_BACKOFF_SET(&chan[i], 2, bo_mcs2[i]);
        DCC_NDL_SNR_BACKOFF_SET(&chan[i], 3, bo_mcs3[i]);
        DCC_NDL_SNR_BACKOFF_SET(&chan[i], 4, bo_mcs4[i]);
        DCC_NDL_SNR_BACKOFF_SET(&chan[i], 5, bo_mcs5[i]);
        DCC_NDL_SNR_BACKOFF_SET(&chan[i], 6, bo_mcs6[i]);
        DCC_NDL_SNR_BACKOFF_SET(&chan[i], 7, bo_mcs7[i]);

        DCC_NDL_TM_MAX_CHANNEL_USE_SET(&chan[i], max_chan_use[i]);
        DCC_NDL_MIN_CHANNEL_LOAD_SET(&chan[i], min_channelld[i]);
        DCC_NDL_MAX_CHANNEL_LOAD_SET(&chan[i], max_channelld[i]);
        DCC_NDL_NUM_QUEUE_SET(&chan[i], num_q[i]);
        DCC_NDL_TIME_UP_SET(&chan[i], time_up[i]);
        DCC_NDL_TIME_DOWN_SET(&chan[i], time_dn[i]);

        DCC_NDL_AS_STATE_ID_SET(&state_cfg[i*4+0], 1);
        DCC_NDL_AS_STATE_ID_SET(&state_cfg[i*4+1], 2);
        DCC_NDL_AS_STATE_ID_SET(&state_cfg[i*4+2], 3);
        DCC_NDL_AS_STATE_ID_SET(&state_cfg[i*4+3], 4);

        DCC_NDL_NUM_ACTIVE_STATE_SET(&chan[i], num_a_states[i]);

        DCC_NDL_AS_CHAN_LOAD_SET(&state_cfg[i*4+0], as1_chanload[i]);
        DCC_NDL_AS_CHAN_LOAD_SET(&state_cfg[i*4+1], as2_chanload[i]);
        DCC_NDL_AS_CHAN_LOAD_SET(&state_cfg[i*4+2], as3_chanload[i]);
        DCC_NDL_AS_CHAN_LOAD_SET(&state_cfg[i*4+3], as4_chanload[i]);

        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+0], DCC_AC_BK, as1_dcc_BK[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+0], DCC_AC_BE, as1_dcc_BE[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+0], DCC_AC_VI, as1_dcc_VI[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+0], DCC_AC_VO, as1_dcc_VO[i]);

        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+1], DCC_AC_BK, as2_dcc_BK[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+1], DCC_AC_BE, as2_dcc_BE[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+1], DCC_AC_VI, as2_dcc_VI[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+1], DCC_AC_VO, as2_dcc_VO[i]);

        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+2], DCC_AC_BK, as3_dcc_BK[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+2], DCC_AC_BE, as3_dcc_BE[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+2], DCC_AC_VI, as3_dcc_VI[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+2], DCC_AC_VO, as3_dcc_VO[i]);

        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+3], DCC_AC_BK, as4_dcc_BK[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+3], DCC_AC_BE, as4_dcc_BE[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+3], DCC_AC_VI, as4_dcc_VI[i]);
        DCC_NDL_AS_DCC_SET(&state_cfg[i*4+3], DCC_AC_VO, as4_dcc_VO[i]);

        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+0], DCC_AC_BK, as1_txpwr_BK[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+0], DCC_AC_BE, as1_txpwr_BE[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+0], DCC_AC_VI, as1_txpwr_VI[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+0], DCC_AC_VO, as1_txpwr_VO[i]);

        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+1], DCC_AC_BK, as2_txpwr_BK[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+1], DCC_AC_BE, as2_txpwr_BE[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+1], DCC_AC_VI, as2_txpwr_VI[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+1], DCC_AC_VO, as2_txpwr_VO[i]);

        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+2], DCC_AC_BK, as3_txpwr_BK[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+2], DCC_AC_BE, as3_txpwr_BE[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+2], DCC_AC_VI, as3_txpwr_VI[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+2], DCC_AC_VO, as3_txpwr_VO[i]);

        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+3], DCC_AC_BK, as4_txpwr_BK[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+3], DCC_AC_BE, as4_txpwr_BE[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+3], DCC_AC_VI, as4_txpwr_VI[i]);
        DCC_NDL_AS_TX_POWER_SET(&state_cfg[i*4+3], DCC_AC_VO, as4_txpwr_VO[i]);

        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+0], DCC_AC_BK, as1_pkt_intr[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+0], DCC_AC_BE, as1_pkt_intr[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+0], DCC_AC_VI, as1_pkt_intr[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+0], DCC_AC_VO, as1_pkt_intr[i]);

        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+1], DCC_AC_BK, as2_pkt_intr_BK[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+1], DCC_AC_BE, as2_pkt_intr_BE[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+1], DCC_AC_VI, as2_pkt_intr_VI[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+1], DCC_AC_VO, as2_pkt_intr_VO[i]);

        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+2], DCC_AC_BK, as3_pkt_intr_BK[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+2], DCC_AC_BE, as3_pkt_intr_BE[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+2], DCC_AC_VI, as3_pkt_intr_VI[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+2], DCC_AC_VO, as3_pkt_intr_VO[i]);

        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+3], DCC_AC_BK, as4_pkt_intr_BK[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+3], DCC_AC_BE, as4_pkt_intr_BE[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+3], DCC_AC_VI, as4_pkt_intr_VI[i]);
        DCC_NDL_AS_PACKET_INTERVAL_SET(&state_cfg[i*4+3], DCC_AC_VO, as4_pkt_intr_VO[i]);

        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+0], DCC_AC_BK, as1_dr[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+0], DCC_AC_BE, as1_dr[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+0], DCC_AC_VI, as1_dr[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+0], DCC_AC_VO, as1_dr[i]);

        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+1], DCC_AC_BK, as2_dr[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+1], DCC_AC_BE, as2_dr[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+1], DCC_AC_VI, as2_dr[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+1], DCC_AC_VO, as2_dr[i]);

        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+2], DCC_AC_BK, as3_dr_BK[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+2], DCC_AC_BE, as3_dr_BE[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+2], DCC_AC_VI, as3_dr_VI[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+2], DCC_AC_VO, as3_dr_VO[i]);

        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+3], DCC_AC_BK, as4_dr_BK[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+3], DCC_AC_BE, as4_dr_BE[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+3], DCC_AC_VI, as4_dr_VI[i]);
        DCC_NDL_AS_DATARATE_SET(&state_cfg[i*4+3], DCC_AC_VO, as4_dr_VO[i]);

        for (state_id = 1; state_id < 5; state_id++) {
            for (ac = DCC_AC_VI; ac < DCC_AC_MAX; ac++) {
                DCC_NDL_AS_CARRIER_SENSE_SET(&state_cfg[i*4+state_id-1], ac, as_cs[i]);
            }
        }
    }
}

void dcc_get_config(unsigned char *num_channels, dcc_ndl_chan **chan,
                    unsigned char *num_active_states, dcc_ndl_active_state_config **state_cfg)
{
    FILE *fp = NULL;
    char *line = NULL;
    unsigned int linecount = 0, num_tokens;
    size_t len;
    char *tokens[128];
    unsigned int i;

    fp = fopen(DCC_CFG_FILE, "r");
    if (!fp) {
        DCC_PRINT("Failed to open file %s\n", DCC_CFG_FILE);
        return;
    }

    *chan = &g_chan[0];
    *state_cfg = &g_state_cfg[0];

    initialize_dcc_config(*chan, *state_cfg);

    while(0 < getline(&line, &len, fp)) {
        linecount++;
        DCC_DEBUG_PRINT("linecount %d\n", linecount);
        DCC_DEBUG_PRINT("%u : %s\n", linecount, line);
        num_tokens = tokenize_string(line, &tokens[0]);

        for (i = 0; i < num_tokens; i++) {
            DCC_DEBUG_PRINT("%d: %s\n", i, tokens[i]);
        }

        if (num_tokens > 0) {
            DCC_DEBUG_PRINT("Step %d\n", 5);
            update_dcc_config(num_tokens, &tokens[0],
                                *chan,
                                *state_cfg);
        }
    }

    if (num_active_states == NULL) {
        fclose(fp);
        return;
    }

    *num_active_states = 0;
    for (i = 0; i < g_num_channels; i++) {
        *num_active_states += DCC_NDL_NUM_ACTIVE_STATE_GET(&g_chan[i]);
        DCC_DEBUG_PRINT("DCC num_active_states for chan idx %u: %lu\n",
            i, DCC_NDL_NUM_ACTIVE_STATE_GET(&g_chan[i]));
    }

    /* Copy over state configurations for channel 2 to right after that of channel 1 to make it contiguous */
    if (g_num_channels > 1) {
        memcpy(&g_state_cfg[DCC_NDL_NUM_ACTIVE_STATE_GET(&g_chan[0])],
               &g_state_cfg[DCC_NDL_ACTIVE_STATE_SIZE],
               sizeof(dcc_ndl_active_state_config) * DCC_NDL_NUM_ACTIVE_STATE_GET(&g_chan[1]));
    }

    *num_channels = g_num_channels;

    fclose(fp);
}

void dcc_print_stats(unsigned char num_channels, dcc_ndl_stats_per_channel *stats)
{
    unsigned long i, ac, mcs;

    DCC_PRINT("DCC stats num_channels = %d\n", num_channels);

    for (i = 0; i < num_channels; i++) {

        DCC_PRINT("DCC NDL chan freq = %lu\n", DCC_NDL_STATS_CHAN_FREQ_GET(&stats[i]));
        DCC_PRINT("DCC NDL stats bitmap = %lu\n", DCC_NDL_STATS_DCC_STATS_BITMAP_GET(&stats[i]));

        for (mcs = 0; mcs < 8; mcs++) {
            DCC_PRINT("DCC NDL snr backoff (mcs=%lu) = %lu\n", mcs, DCC_NDL_STATS_SNR_BACKOFF_GET(&stats[i], mcs));
        }

        DCC_PRINT("DCC NDL tx power = %lu\n", DCC_TX_POWER_GET(&stats[i]));
        DCC_PRINT("DCC NDL datarate = %lu\n", DCC_TX_DATARATE_GET(&stats[i]));
        DCC_PRINT("DCC NDL carrier sense range = %lu\n", DCC_NDL_CARRIER_SENSE_RANGE_GET(&stats[i]));
        DCC_PRINT("DCC NDL communication range = %lu\n", DCC_NDL_EST_COMM_RANGE_GET(&stats[i]));
        DCC_PRINT("DCC NDL sensitivity = %lu\n", DCC_DCC_SENSITIVITY_GET(&stats[i]));
        DCC_PRINT("DCC NDL carrier sense = %lu\n", DCC_CARRIER_SENSE_GET(&stats[i]));
        DCC_PRINT("DCC NDL channel load = %lu\n", DCC_NDL_CHANNEL_LOAD_GET(&stats[i]));
        DCC_PRINT("DCC NDL arrival rate = %lu\n", DCC_NDL_PACKET_ARRIVAL_RATE_GET(&stats[i]));
        DCC_PRINT("DCC NDL avg duration = %lu\n", DCC_NDL_PACKET_AVG_DURATION_GET(&stats[i]));
        DCC_PRINT("DCC NDL channel busy time = %lu\n", DCC_NDL_CHANNEL_BUSY_TIME_GET(&stats[i]));

        for (ac = DCC_AC_VO; ac < DCC_AC_MAX; ac++) {
            DCC_PRINT("DCC NDL tx packet arrival rate (ac=%lu) = %lu\n", ac, DCC_NDL_TX_PACKET_ARRIVAL_RATE_GET(&stats[i], ac));
            DCC_PRINT("DCC NDL tx packet avg duration (ac=%lu) = %lu\n", ac, DCC_NDL_TX_PACKET_AVG_DURATION_GET(&stats[i], ac));
            DCC_PRINT("DCC NDL tx channel use (ac=%lu) = %lu\n", ac, DCC_NDL_TX_CHANNEL_USE_GET(&stats[i], ac));
            DCC_PRINT("DCC NDL tx signal avg power (ac=%lu) = %lu\n", ac, DCC_NDL_TX_SIGNAL_AVG_POWER_GET(&stats[i], ac));
        }
    }
}
