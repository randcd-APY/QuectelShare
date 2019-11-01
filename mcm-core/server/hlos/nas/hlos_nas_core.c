
/*************************************************************************************
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#include "hlos_nas_core.h"
#include "hlos_core.h"
#include "cri_utils.h"



typedef struct {
    cri_nas_rte_type voice_rte_cache;
    cri_nas_rte_type data_rte_cache;
     mcm_nw_voice_registration_event_ind_msg_v01 voice_reg_ind_cache;
    mcm_nw_data_registration_event_ind_msg_v01 data_reg_ind_cache;
    mcm_nw_signal_strength_event_ind_msg_v01 signal_strengh_ind_cache;
    mcm_nw_cell_access_state_change_event_ind_msg_v01 cell_access_state_ind_cache;
    mcm_nw_nitz_time_ind_msg_v01 nitz_time_ind_cache;
 }hlos_nas_cache_type;


hlos_nas_cache_type hlos_nas_cache;

int is_3gpp(cri_nas_rte_type rte)
{
    int ret = FALSE;
    if ( rte == CRI_NAS_RTE_GSM
            || rte == CRI_NAS_RTE_WCDMA
            || rte == CRI_NAS_RTE_LTE
            || rte == CRI_NAS_RTE_TDSCDMA )
    {
        ret = TRUE;
    }
    return ret;
}


void hlos_nas_core_fill_data_registration_indication(mcm_nw_data_registration_event_ind_msg_v01 *ind_resp)
{
    void *reg_info;
    cri_nas_rte_gsm_reg_info_type *gsm_data_reg_info;
    cri_nas_rte_wcdma_reg_info_type *wcdma_data_reg_info;
    cri_nas_rte_cdma_reg_info_type *cdma_data_reg_info;
    cri_nas_rte_hdr_reg_info_type *hdr_data_reg_info;
    cri_nas_rte_lte_reg_info_type *lte_data_reg_info;
    cri_nas_rte_tdscdma_reg_info_type *tdscdma_data_reg_info;

    cri_nas_rte_type data_rte;

    reg_info = cri_nas_retrieve_data_rte(&data_rte);

    if ( reg_info == NULL )
    {
        data_rte = CRI_NAS_RTE_UNKNOWN;
    }

    switch(data_rte)
    {
        case CRI_NAS_RTE_GSM:

            gsm_data_reg_info = (cri_nas_rte_gsm_reg_info_type*)reg_info;
            ind_resp->data_registration_valid = TRUE;
            ind_resp->data_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &ind_resp->data_registration,
                                                        &gsm_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(data_rte,
                                        &ind_resp->data_registration_details_3gpp,
                                        gsm_data_reg_info);
            break;

        case CRI_NAS_RTE_WCDMA:
            wcdma_data_reg_info = (cri_nas_rte_wcdma_reg_info_type*)reg_info;
            ind_resp->data_registration_valid = TRUE;
            ind_resp->data_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &ind_resp->data_registration,
                                                        &wcdma_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(data_rte,
                                        &ind_resp->data_registration_details_3gpp,
                                        wcdma_data_reg_info);

            break;

        case CRI_NAS_RTE_CDMA:
            cdma_data_reg_info = (cri_nas_rte_cdma_reg_info_type*)reg_info;
            ind_resp->data_registration_valid = TRUE;
            ind_resp->data_registration_details_3gpp2_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(FALSE,
                                                        &ind_resp->data_registration,
                                                        &cdma_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp2_info(data_rte,
                                        &ind_resp->data_registration_details_3gpp2,
                                        cdma_data_reg_info);


            break;

        case CRI_NAS_RTE_HDR:
            hdr_data_reg_info = (cri_nas_rte_hdr_reg_info_type*)reg_info;
            ind_resp->data_registration_valid = TRUE;
            ind_resp->data_registration_details_3gpp2_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(FALSE,
                                                        &ind_resp->data_registration,
                                                        &hdr_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp2_info(data_rte,
                                        &ind_resp->data_registration_details_3gpp2,
                                        hdr_data_reg_info);


            break;

        case CRI_NAS_RTE_LTE:
            lte_data_reg_info = (cri_nas_rte_lte_reg_info_type*)reg_info;
            ind_resp->data_registration_valid = TRUE;
            ind_resp->data_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &ind_resp->data_registration,
                                                        &lte_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(data_rte,
                                        &ind_resp->data_registration_details_3gpp,
                                        lte_data_reg_info);

            break;

        case CRI_NAS_RTE_TDSCDMA:
            tdscdma_data_reg_info = (cri_nas_rte_tdscdma_reg_info_type*)reg_info;
            ind_resp->data_registration_valid = TRUE;
            ind_resp->data_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &ind_resp->data_registration,
                                                        &tdscdma_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(data_rte,
                                        &ind_resp->data_registration_details_3gpp,
                                        tdscdma_data_reg_info);
            break;

        case CRI_NAS_RTE_UNKNOWN:
            UTIL_LOG_MSG("\nUnknown voice RTE\n");
            break;
    }

}


void hlos_nas_core_fill_voice_registration_indication(mcm_nw_voice_registration_event_ind_msg_v01 *ind_resp)
{

    void *reg_info;
    cri_nas_rte_gsm_reg_info_type *gsm_voice_reg_info;
    cri_nas_rte_wcdma_reg_info_type *wcdma_voice_reg_info;
    cri_nas_rte_cdma_reg_info_type *cdma_voice_reg_info;
    cri_nas_rte_hdr_reg_info_type *hdr_voice_reg_info;
    cri_nas_rte_lte_reg_info_type *lte_voice_reg_info;
    cri_nas_rte_tdscdma_reg_info_type *tdscdma_voice_reg_info;

    cri_nas_rte_type voice_rte = CRI_NAS_RTE_UNKNOWN;

    reg_info = cri_nas_retrieve_voice_rte(&voice_rte);

    if ( reg_info == NULL )
    {
        voice_rte = CRI_NAS_RTE_UNKNOWN;
    }


    switch(voice_rte)
    {
        case CRI_NAS_RTE_GSM:

            gsm_voice_reg_info = (cri_nas_rte_gsm_reg_info_type*)reg_info;
            ind_resp->voice_registration_valid = TRUE;
            ind_resp->voice_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &ind_resp->voice_registration,
                                                        &gsm_voice_reg_info->reg_info);
            hlos_nas_core_fill_3gpp_info(voice_rte,
                                        &ind_resp->voice_registration_details_3gpp,
                                        gsm_voice_reg_info);

            break;

        case CRI_NAS_RTE_WCDMA:
            wcdma_voice_reg_info = (cri_nas_rte_wcdma_reg_info_type*)reg_info;
            ind_resp->voice_registration_valid = TRUE;
            ind_resp->voice_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &ind_resp->voice_registration,
                                                        &wcdma_voice_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(voice_rte,
                                        &ind_resp->voice_registration_details_3gpp,
                                        wcdma_voice_reg_info);

            break;

        case CRI_NAS_RTE_CDMA:
            cdma_voice_reg_info = (cri_nas_rte_cdma_reg_info_type*)reg_info;
            ind_resp->voice_registration_valid = TRUE;
            ind_resp->voice_registration_details_3gpp2_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(FALSE,
                                                        &ind_resp->voice_registration,
                                                        &cdma_voice_reg_info->reg_info);

            hlos_nas_core_fill_3gpp2_info(voice_rte,
                                        &ind_resp->voice_registration_details_3gpp2,
                                        cdma_voice_reg_info);

            break;

        case CRI_NAS_RTE_HDR:
            hdr_voice_reg_info = (cri_nas_rte_hdr_reg_info_type*)reg_info;
            ind_resp->voice_registration_valid = TRUE;
            ind_resp->voice_registration_details_3gpp2_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(FALSE,
                                                        &ind_resp->voice_registration,
                                                        &hdr_voice_reg_info->reg_info);

            hlos_nas_core_fill_3gpp2_info(voice_rte,
                                        &ind_resp->voice_registration_details_3gpp2,
                                        hdr_voice_reg_info);


            break;

        case CRI_NAS_RTE_LTE:
            lte_voice_reg_info = (cri_nas_rte_lte_reg_info_type*)reg_info;
            ind_resp->voice_registration_valid = TRUE;
            ind_resp->voice_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &ind_resp->voice_registration,
                                                        &lte_voice_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(voice_rte,
                                        &ind_resp->voice_registration_details_3gpp,
                                        lte_voice_reg_info);

            break;

        case CRI_NAS_RTE_TDSCDMA:
            tdscdma_voice_reg_info = (cri_nas_rte_tdscdma_reg_info_type*)reg_info;
            ind_resp->voice_registration_valid = TRUE;
            ind_resp->voice_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &ind_resp->voice_registration,
                                                        &tdscdma_voice_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(voice_rte,
                                        &ind_resp->voice_registration_details_3gpp,
                                        tdscdma_voice_reg_info);
            break;

        case CRI_NAS_RTE_UNKNOWN:
            UTIL_LOG_MSG("\nUnknown voice RTE\n");
            break;
    }


}

void hlos_nas_send_network_time_update_to_client()
{
    uint8_t nitz_updated = FALSE;
    mcm_nw_nitz_time_ind_msg_v01 nitz_time_ind_msg;
    memset(&nitz_time_ind_msg,NIL,sizeof(nitz_time_ind_msg));

    nitz_updated = cri_nas_fill_network_time_ind(&nitz_time_ind_msg.nw_nitz_time_valid, &nitz_time_ind_msg.nw_nitz_time.nitz_time, &nitz_time_ind_msg.abs_time_valid,
                        &nitz_time_ind_msg.abs_time, &nitz_time_ind_msg.leap_sec_valid, &nitz_time_ind_msg.leap_sec);

    if(nitz_updated)
    {
        UTIL_LOG_MSG("network time NITZ info valid %d",nitz_time_ind_msg.nw_nitz_time_valid);
        if(TRUE == nitz_time_ind_msg.nw_nitz_time_valid)
        {
            UTIL_LOG_MSG("network time NITZ info %s",&nitz_time_ind_msg.nw_nitz_time.nitz_time);
        }

        UTIL_LOG_MSG("abs time valid %d",nitz_time_ind_msg.abs_time_valid);
        if(TRUE == nitz_time_ind_msg.abs_time_valid)
        {
            UTIL_LOG_MSG("abs time (%x, %x)", (uint32_t)(nitz_time_ind_msg.abs_time >> 32), (uint32_t)nitz_time_ind_msg.abs_time);
        }

        UTIL_LOG_MSG("Leap sec valid %d",nitz_time_ind_msg.leap_sec_valid);
        if(TRUE == nitz_time_ind_msg.leap_sec_valid)
        {
            UTIL_LOG_MSG("Leap sec %d",nitz_time_ind_msg.leap_sec);
        }

        hlos_core_send_indication(NIL,
                            MCM_NW_NITZ_TIME_IND_V01,
                            &nitz_time_ind_msg,
                            sizeof(nitz_time_ind_msg));
    }
}

void hlos_nas_send_sib16_network_time_update_to_client()
{
    uint8_t nitz_updated = FALSE;
    mcm_nw_nitz_time_ind_msg_v01 nitz_time_ind_msg;
    memset(&nitz_time_ind_msg,NIL,sizeof(nitz_time_ind_msg));

    nitz_updated = cri_nas_fill_sib16_network_time_ind(&nitz_time_ind_msg.nw_nitz_time_valid, &nitz_time_ind_msg.nw_nitz_time.nitz_time, &nitz_time_ind_msg.abs_time_valid,
                        &nitz_time_ind_msg.abs_time, &nitz_time_ind_msg.leap_sec_valid, &nitz_time_ind_msg.leap_sec);

    if(nitz_updated)
    {
        UTIL_LOG_MSG("network time NITZ info valid %d",nitz_time_ind_msg.nw_nitz_time_valid);
        if(TRUE == nitz_time_ind_msg.nw_nitz_time_valid)
        {
            UTIL_LOG_MSG("network time NITZ info %s",&nitz_time_ind_msg.nw_nitz_time.nitz_time);
        }

        UTIL_LOG_MSG("abs time valid %d",nitz_time_ind_msg.abs_time_valid);
        if(TRUE == nitz_time_ind_msg.abs_time_valid)
        {
            UTIL_LOG_MSG("abs time (%x, %x)", (uint32_t)(nitz_time_ind_msg.abs_time >> 32), (uint32_t)nitz_time_ind_msg.abs_time);
        }

        UTIL_LOG_MSG("Leap sec valid %d",nitz_time_ind_msg.leap_sec_valid);
        if(TRUE == nitz_time_ind_msg.leap_sec_valid)
        {
            UTIL_LOG_MSG("Leap sec %d",nitz_time_ind_msg.leap_sec);
        }

        hlos_core_send_indication(NIL,
                            MCM_NW_NITZ_TIME_IND_V01,
                            &nitz_time_ind_msg,
                            sizeof(nitz_time_ind_msg));
    }
}

mcm_nw_service_t_v01 hlos_nas_convert_cri_service_to_mcm(cri_nas_tech_reg_status_type cri_reg_status)
{
    mcm_nw_service_t_v01 mcm_reg_status;
    switch(cri_reg_status)
    {
        case CRI_NAS_TECH_REG_STATUS_REGISTERED_HOME:
        case CRI_NAS_TECH_REG_STATUS_REGISTERED_ROAMING:
            mcm_reg_status = MCM_NW_SERVICE_FULL_V01;
            break;

        case CRI_NAS_TECH_REG_STATUS_NOT_REGISTERED_LTD_SRV:
        case CRI_NAS_TECH_REG_STATUS_DENIED_LTD_SRV:
            mcm_reg_status = MCM_NW_SERVICE_LIMITED_V01;
            break;

        case CRI_NAS_TECH_REG_STATUS_NOT_REGISTERED_SEARCHING:
        case CRI_NAS_TECH_REG_STATUS_DENIED:
        case CRI_NAS_TECH_REG_STATUS_NOT_REGISTERED_NOT_SEARCHING:
            mcm_reg_status = MCM_NW_SERVICE_NONE_V01;
            break;
        default:
            mcm_reg_status = MCM_NW_SERVICE_NONE_V01;
            break;
    }
    return mcm_reg_status;
}


cri_nas_tech_type hlos_nas_convert_mcm_rat_to_cri(mcm_nw_radio_tech_t_v01 mcm_rat)
{
    cri_nas_tech_type cri_rat = MCM_NW_RADIO_TECH_NONE_V01;
    switch(mcm_rat)
    {
        case MCM_NW_RADIO_TECH_GSM_V01:
            cri_rat = CRI_NAS_TECH_GSM;
            break;

        case MCM_NW_RADIO_TECH_GPRS_V01:
            cri_rat = CRI_NAS_TECH_GPRS;
            break;

        case MCM_NW_RADIO_TECH_EDGE_V01:
            cri_rat = CRI_NAS_TECH_EDGE;
            break;

        case MCM_NW_RADIO_TECH_UMTS_V01:
            cri_rat = CRI_NAS_TECH_WCDMA;
            break;

        case MCM_NW_RADIO_TECH_TD_SCDMA_V01:
            cri_rat = CRI_NAS_TECH_TDSCDMA;
            break;

        case MCM_NW_RADIO_TECH_HSDPA_V01:
            cri_rat = CRI_NAS_TECH_HSDPA;
            break;

        case MCM_NW_RADIO_TECH_HSUPA_V01:
            cri_rat = CRI_NAS_TECH_HSUPA;
            break;

        case MCM_NW_RADIO_TECH_HSPA_V01:
            cri_rat = CRI_NAS_TECH_HSPA;
            break;

        case MCM_NW_RADIO_TECH_HSPAP_V01:
            cri_rat = CRI_NAS_TECH_HSPA_PLUS;
            break;

        case MCM_NW_RADIO_TECH_LTE_V01:
            cri_rat = CRI_NAS_TECH_LTE;
            break;

        case MCM_NW_RADIO_TECH_IS95A_V01:
            cri_rat = CRI_NAS_TECH_IS95A;
            break;

        case MCM_NW_RADIO_TECH_IS95B_V01:
            cri_rat = CRI_NAS_TECH_IS95B;
            break;

        case MCM_NW_RADIO_TECH_1xRTT_V01:
            cri_rat = CRI_NAS_TECH_1xRTT;
            break;

        case MCM_NW_RADIO_TECH_EVDO_0_V01:
            cri_rat = CRI_NAS_TECH_EVDO_0;
            break;

        case MCM_NW_RADIO_TECH_EVDO_A_V01:
            cri_rat = CRI_NAS_TECH_EVDO_A;
            break;

        case MCM_NW_RADIO_TECH_EVDO_B_V01:
            cri_rat = CRI_NAS_TECH_EVDO_B;
            break;

        case MCM_NW_RADIO_TECH_EHRPD_V01:
            cri_rat = CRI_NAS_TECH_EHRPD;
            break;

        default:
            break;
    }
    return cri_rat;
}



mcm_nw_radio_tech_t_v01 hlos_nas_convert_cri_radiotech_to_mcm_radiotech(cri_nas_tech_type cri_rat)
{
    mcm_nw_radio_tech_t_v01 mcm_rat = MCM_NW_RADIO_TECH_NONE_V01;
    switch(cri_rat)
    {
        case CRI_NAS_TECH_GSM:
            mcm_rat = MCM_NW_RADIO_TECH_GSM_V01;
            break;
        case CRI_NAS_TECH_GPRS:
            mcm_rat = MCM_NW_RADIO_TECH_GPRS_V01;
            break;
        case CRI_NAS_TECH_EDGE:
            mcm_rat = MCM_NW_RADIO_TECH_EDGE_V01;
            break;

        case CRI_NAS_TECH_WCDMA:
            mcm_rat = MCM_NW_RADIO_TECH_UMTS_V01;
            break;

        case CRI_NAS_TECH_TDSCDMA:
            mcm_rat = MCM_NW_RADIO_TECH_TD_SCDMA_V01;
            break;

        case CRI_NAS_TECH_HSDPA:
            mcm_rat = MCM_NW_RADIO_TECH_HSDPA_V01;
            break;

        case CRI_NAS_TECH_HSUPA:
            mcm_rat = MCM_NW_RADIO_TECH_HSUPA_V01;
            break;

        case CRI_NAS_TECH_HSPA:
            mcm_rat = MCM_NW_RADIO_TECH_HSPA_V01;
            break;

        case CRI_NAS_TECH_HSPA_PLUS:
            mcm_rat = MCM_NW_RADIO_TECH_HSPAP_V01;
            break;

        case CRI_NAS_TECH_LTE:
            mcm_rat = MCM_NW_RADIO_TECH_LTE_V01;
            break;

        case CRI_NAS_TECH_IS95A:
            mcm_rat = MCM_NW_RADIO_TECH_IS95A_V01;
            break;

        case CRI_NAS_TECH_IS95B:
            mcm_rat = MCM_NW_RADIO_TECH_IS95B_V01;
            break;

        case CRI_NAS_TECH_1xRTT:
            mcm_rat = MCM_NW_RADIO_TECH_1xRTT_V01;
            break;

        case CRI_NAS_TECH_EVDO_0:
            mcm_rat = MCM_NW_RADIO_TECH_EVDO_0_V01;
            break;

        case CRI_NAS_TECH_EVDO_A:
            mcm_rat = MCM_NW_RADIO_TECH_EVDO_A_V01;
            break;

        case CRI_NAS_TECH_EVDO_B:
            mcm_rat = MCM_NW_RADIO_TECH_EVDO_B_V01;
            break;

        case CRI_NAS_TECH_EHRPD:
            mcm_rat = MCM_NW_RADIO_TECH_EHRPD_V01;
            break;

        default:
            break;
    }
    return mcm_rat;
}





mcm_nw_cell_access_state_t_v01 hlos_nas_get_cell_access_state()
{
    // TODO: do we report cell access state only for voice?

    void *reg_info;
    cri_nas_rte_gsm_reg_info_type *gsm_voice_reg_info;
    cri_nas_rte_wcdma_reg_info_type *wcdma_voice_reg_info;
    cri_nas_rte_cdma_reg_info_type *cdma_voice_reg_info;
    cri_nas_rte_hdr_reg_info_type *hdr_voice_reg_info;
    cri_nas_rte_lte_reg_info_type *lte_voice_reg_info;
    cri_nas_rte_tdscdma_reg_info_type *tdscdma_voice_reg_info;
    cri_nas_rte_type voice_rte = CRI_NAS_RTE_UNKNOWN;
    nas_cell_access_status_e_type_v01 qmi_cell_access_state = NAS_CELL_ACCESS_UNKNOWN_V01;
    mcm_nw_cell_access_state_t_v01 mcm_cell_access_state = MCM_NW_CELL_ACCESS_NONE_V01;


    reg_info = cri_nas_retrieve_voice_rte(&voice_rte);

    if ( reg_info == NULL )
    {
        voice_rte = CRI_NAS_RTE_UNKNOWN;
    }

    switch(voice_rte)
    {
        case CRI_NAS_RTE_GSM:
            gsm_voice_reg_info = (cri_nas_rte_gsm_reg_info_type*)reg_info;
            qmi_cell_access_state = gsm_voice_reg_info->cell_access_status;
            break;

        case CRI_NAS_RTE_WCDMA:
            wcdma_voice_reg_info = (cri_nas_rte_wcdma_reg_info_type*)reg_info;
            qmi_cell_access_state = wcdma_voice_reg_info->cell_access_status;
            break;

        case CRI_NAS_RTE_LTE:
            lte_voice_reg_info = (cri_nas_rte_lte_reg_info_type*)reg_info;
            qmi_cell_access_state = lte_voice_reg_info->cell_access_status;
            break;

        case CRI_NAS_RTE_TDSCDMA:
            tdscdma_voice_reg_info = (cri_nas_rte_tdscdma_reg_info_type*)reg_info;
            qmi_cell_access_state = tdscdma_voice_reg_info->cell_access_status;
            break;

        case CRI_NAS_RTE_UNKNOWN:
            UTIL_LOG_MSG("\nUnknown voice RTE\n");
            break;
    }

    switch( qmi_cell_access_state )
    {
        case NAS_CELL_ACCESS_NORMAL_ONLY_V01:
            mcm_cell_access_state = MCM_NW_CELL_ACCESS_NORMAL_ONLY_V01;
            break;

        case NAS_CELL_ACCESS_EMERGENCY_ONLY_V01:
            mcm_cell_access_state = MCM_NW_CELL_ACCESS_EMERGENCY_ONLY_V01;
            break;

        case NAS_CELL_ACCESS_NO_CALLS_V01:
            mcm_cell_access_state = MCM_NW_CELL_ACCESS_NO_CALLS_V01;
            break;

        case NAS_CELL_ACCESS_ALL_CALLS_V01:
            mcm_cell_access_state = MCM_NW_CELL_ACCESS_ALL_CALLS_V01;
            break;

        default:
            mcm_cell_access_state = MCM_NW_CELL_ACCESS_NONE_V01;
    }

    UTIL_LOG_MSG("\nhlos cell access state - %d\n",mcm_cell_access_state);

    return mcm_cell_access_state;

}



void hlos_nas_core_fill_common_registration_info(int is3gpp,
                                            mcm_nw_common_registration_t_v01 *hlos_reg_info,
                                            cri_nas_rte_common_reg_info_type *reg_info)
{

    if ( reg_info != NULL )
    {
        if ( is3gpp )
        {
            hlos_reg_info->tech_domain = MCM_NW_TECH_DOMAIN_3GPP_V01;
        }
        else
        {
            hlos_reg_info->tech_domain = MCM_NW_TECH_DOMAIN_3GPP2_V01;
        }

        hlos_reg_info->radio_tech
            = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(reg_info->tech);
        hlos_reg_info->roaming = reg_info->roam_status;
        hlos_reg_info->deny_reason = reg_info->reg_reject_reason;
        hlos_reg_info->registration_state =
            hlos_nas_convert_cri_service_to_mcm(reg_info->reg_status);
    }

}


void hlos_nas_core_fill_3gpp2_info( cri_nas_rte_type rte,
                                    mcm_nw_3gpp2_registration_t_v01 *hlos_3gpp2_reg_info,
                                    void *reg_info)
{
    cri_nas_rte_cdma_reg_info_type *cdma_voice_reg_info;
    cri_nas_rte_hdr_reg_info_type *hdr_voice_reg_info;

    switch(rte)
    {
        case CRI_NAS_RTE_CDMA:
            cdma_voice_reg_info = (cri_nas_rte_cdma_reg_info_type*)reg_info;

            hlos_3gpp2_reg_info->tech_domain = MCM_NW_TECH_DOMAIN_3GPP2_V01;
            hlos_3gpp2_reg_info->radio_tech
                = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(cdma_voice_reg_info->reg_info.tech);
            strlcpy(hlos_3gpp2_reg_info->mcc,cdma_voice_reg_info->reg_info.mcc,
                                                        sizeof(hlos_3gpp2_reg_info->mcc));
            strlcpy(hlos_3gpp2_reg_info->mnc,cdma_voice_reg_info->reg_info.mnc,
                                                        sizeof(hlos_3gpp2_reg_info->mnc));
            hlos_3gpp2_reg_info->roaming = cdma_voice_reg_info->reg_info.roam_status;
            hlos_3gpp2_reg_info->forbidden = cdma_voice_reg_info->forbidden;
            hlos_3gpp2_reg_info->inPRL = cdma_voice_reg_info->inPRL;
            hlos_3gpp2_reg_info->css = cdma_voice_reg_info->css;
            hlos_3gpp2_reg_info->sid = atoi(cdma_voice_reg_info->sid);
            hlos_3gpp2_reg_info->nid = atoi(cdma_voice_reg_info->nid);
            hlos_3gpp2_reg_info->bsid = atoi(cdma_voice_reg_info->bsid);

            break;

        case CRI_NAS_RTE_HDR:
            hdr_voice_reg_info = (cri_nas_rte_hdr_reg_info_type*)reg_info;

            hlos_3gpp2_reg_info->tech_domain = MCM_NW_TECH_DOMAIN_3GPP2_V01;
            hlos_3gpp2_reg_info->radio_tech
                = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(hdr_voice_reg_info->reg_info.tech);
            strlcpy(hlos_3gpp2_reg_info->mcc,hdr_voice_reg_info->reg_info.mcc,
                                                        sizeof(hlos_3gpp2_reg_info->mcc));
            strlcpy(hlos_3gpp2_reg_info->mnc,hdr_voice_reg_info->reg_info.mnc,
                                                        sizeof(hlos_3gpp2_reg_info->mnc));
            hlos_3gpp2_reg_info->roaming = hdr_voice_reg_info->reg_info.roam_status;
            hlos_3gpp2_reg_info->forbidden = hdr_voice_reg_info->forbidden;
            hlos_3gpp2_reg_info->inPRL = hdr_voice_reg_info->inPRL;
            hlos_3gpp2_reg_info->css = NIL;
            hlos_3gpp2_reg_info->sid = atoi(hdr_voice_reg_info->is856_sys_id);
            hlos_3gpp2_reg_info->nid = NIL;
            hlos_3gpp2_reg_info->bsid = NIL;

            break;

        case CRI_NAS_RTE_UNKNOWN:
            UTIL_LOG_MSG("\nUnknown voice RTE\n");
            break;

    }
}




void hlos_nas_core_fill_3gpp_info( cri_nas_rte_type rte,
                                mcm_nw_3gpp_registration_t_v01 *hlos_3gpp_reg_info,
                                void *reg_info)
{
    cri_nas_rte_gsm_reg_info_type *gsm_voice_reg_info;
    cri_nas_rte_wcdma_reg_info_type *wcdma_voice_reg_info;
    cri_nas_rte_cdma_reg_info_type *cdma_voice_reg_info;
    cri_nas_rte_hdr_reg_info_type *hdr_voice_reg_info;
    cri_nas_rte_lte_reg_info_type *lte_voice_reg_info;
    cri_nas_rte_tdscdma_reg_info_type *tdscdma_voice_reg_info;

    switch(rte)
    {
        case CRI_NAS_RTE_GSM:

            gsm_voice_reg_info = (cri_nas_rte_gsm_reg_info_type*)reg_info;

            // 3gpp
            hlos_3gpp_reg_info->tech_domain = MCM_NW_TECH_DOMAIN_3GPP_V01;
            hlos_3gpp_reg_info->radio_tech
                = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(gsm_voice_reg_info->reg_info.tech);
            strlcpy(hlos_3gpp_reg_info->mcc,gsm_voice_reg_info->reg_info.mcc,
                                                         sizeof(hlos_3gpp_reg_info->mcc));
            strlcpy(hlos_3gpp_reg_info->mnc,gsm_voice_reg_info->reg_info.mnc,
                                                         sizeof(hlos_3gpp_reg_info->mnc));
            hlos_3gpp_reg_info->forbidden = NIL;

            hlos_3gpp_reg_info->roaming = gsm_voice_reg_info->reg_info.roam_status;
            hlos_3gpp_reg_info->cid = atoi(gsm_voice_reg_info->cid);
            hlos_3gpp_reg_info->lac = atoi(gsm_voice_reg_info->lac);
            hlos_3gpp_reg_info->psc = NIL;
            hlos_3gpp_reg_info->tac = NIL;

            break;

        case CRI_NAS_RTE_WCDMA:
            wcdma_voice_reg_info = (cri_nas_rte_wcdma_reg_info_type*)reg_info;

            hlos_3gpp_reg_info->tech_domain = MCM_NW_TECH_DOMAIN_3GPP_V01;
            hlos_3gpp_reg_info->radio_tech
                = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(wcdma_voice_reg_info->reg_info.tech);
            strlcpy(hlos_3gpp_reg_info->mcc,wcdma_voice_reg_info->reg_info.mcc,
                                                         sizeof(hlos_3gpp_reg_info->mcc));
            strlcpy(hlos_3gpp_reg_info->mnc,wcdma_voice_reg_info->reg_info.mnc,
                                                         sizeof(hlos_3gpp_reg_info->mnc));
            hlos_3gpp_reg_info->roaming = wcdma_voice_reg_info->reg_info.roam_status;
            hlos_3gpp_reg_info->forbidden = NIL;
            hlos_3gpp_reg_info->cid = atoi(wcdma_voice_reg_info->cid);
            hlos_3gpp_reg_info->lac = atoi(wcdma_voice_reg_info->lac);
            hlos_3gpp_reg_info->psc = atoi(wcdma_voice_reg_info->psc);
            hlos_3gpp_reg_info->tac = NIL;

            break;

        case CRI_NAS_RTE_LTE:
            lte_voice_reg_info = (cri_nas_rte_lte_reg_info_type*)reg_info;

            hlos_3gpp_reg_info->tech_domain = MCM_NW_TECH_DOMAIN_3GPP_V01;
            hlos_3gpp_reg_info->radio_tech
                = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(lte_voice_reg_info->reg_info.tech);
            strlcpy(hlos_3gpp_reg_info->mcc,lte_voice_reg_info->reg_info.mcc,
                                                         sizeof(hlos_3gpp_reg_info->mcc));
            strlcpy(hlos_3gpp_reg_info->mnc,lte_voice_reg_info->reg_info.mnc,
                                                         sizeof(hlos_3gpp_reg_info->mnc));
            hlos_3gpp_reg_info->roaming = lte_voice_reg_info->reg_info.roam_status;
            hlos_3gpp_reg_info->forbidden = NIL;
            hlos_3gpp_reg_info->cid = atoi(lte_voice_reg_info->cid);
            hlos_3gpp_reg_info->lac = atoi(lte_voice_reg_info->lac);
            hlos_3gpp_reg_info->psc = NIL;
            hlos_3gpp_reg_info->tac = atoi(lte_voice_reg_info->tac);

            break;

        case CRI_NAS_RTE_TDSCDMA:
            tdscdma_voice_reg_info = (cri_nas_rte_tdscdma_reg_info_type*)reg_info;

            hlos_3gpp_reg_info->tech_domain = MCM_NW_TECH_DOMAIN_3GPP_V01;
            hlos_3gpp_reg_info->radio_tech
                = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(tdscdma_voice_reg_info->reg_info.tech);
            strlcpy(hlos_3gpp_reg_info->mcc,tdscdma_voice_reg_info->reg_info.mcc,
                                                         sizeof(hlos_3gpp_reg_info->mcc));
            strlcpy(hlos_3gpp_reg_info->mnc,tdscdma_voice_reg_info->reg_info.mnc,
                                                         sizeof(hlos_3gpp_reg_info->mnc));
            hlos_3gpp_reg_info->roaming = tdscdma_voice_reg_info->reg_info.roam_status;
            hlos_3gpp_reg_info->forbidden = NIL;
            hlos_3gpp_reg_info->cid = atoi(tdscdma_voice_reg_info->cid);
            hlos_3gpp_reg_info->lac = atoi(tdscdma_voice_reg_info->lac);
            hlos_3gpp_reg_info->psc = NIL;
            hlos_3gpp_reg_info->tac = NIL;

            break;

        case CRI_NAS_RTE_UNKNOWN:
            UTIL_LOG_MSG("\nUnknown voice RTE\n");
            break;
    }

}



int hlos_nas_is_voice_rte_changed()
{
    int ret_val = FALSE;
    cri_nas_rte_type current_voice_rte;
    cri_nas_retrieve_voice_rte(&current_voice_rte);
    if ( current_voice_rte != hlos_nas_cache.voice_rte_cache )
    {
        hlos_nas_cache.voice_rte_cache = current_voice_rte;
        ret_val = TRUE;
    }
    return ret_val;
}

int hlos_nas_is_data_rte_changed()
{
    int ret_val = FALSE;
    cri_nas_rte_type current_data_rte;
    cri_nas_retrieve_voice_rte(&current_data_rte);
    if ( current_data_rte != hlos_nas_cache.data_rte_cache )
    {
        hlos_nas_cache.data_rte_cache = current_data_rte;
        ret_val = TRUE;
    }
    return ret_val;
}

uint8_t hlos_nas_is_registration_info_changed(uint8_t is_voice, void *ind_msg)
{
    uint8_t ret_val = FALSE;
    uint8_t is_same = FALSE;

    if ( is_voice )
    {
        is_same = CRI_MEMORY_COMPARISION(ind_msg,&hlos_nas_cache.voice_reg_ind_cache,sizeof(mcm_nw_voice_registration_event_ind_msg_v01));
    }
    else
    {
        is_same = CRI_MEMORY_COMPARISION(ind_msg,&hlos_nas_cache.data_reg_ind_cache,sizeof(mcm_nw_data_registration_event_ind_msg_v01));
    }
    ret_val = is_same ? FALSE: TRUE; // if same, info is not changed.

    return ret_val;
}


uint8_t hlos_nas_is_cell_access_state_info_changed(void *ind_msg)
{
    uint8_t ret_val = FALSE;
    uint8_t is_same = FALSE;

    is_same = CRI_MEMORY_COMPARISION(ind_msg,&hlos_nas_cache.cell_access_state_ind_cache,
                  sizeof(mcm_nw_cell_access_state_change_event_ind_msg_v01));

    ret_val = is_same ? FALSE: TRUE; // if same, info is not changed.

    return ret_val;
}



void hlos_nas_fill_signal_info_ind(mcm_nw_signal_strength_event_ind_msg_v01 *ss_info)
{
    void *reg_info;
    cri_nas_rte_type voice_rte;
    cri_nas_rte_gsm_reg_info_type *gsm_reg_info;
    cri_nas_rte_wcdma_reg_info_type *wcdma_reg_info;
    cri_nas_rte_tdscdma_reg_info_type *tdscdma_reg_info;
    cri_nas_rte_lte_reg_info_type *lte_reg_info;
    cri_nas_rte_cdma_reg_info_type *cdma_reg_info;
    cri_nas_rte_hdr_reg_info_type *hdr_reg_info;

    memset(ss_info,NIL,sizeof(*ss_info));

    reg_info = cri_nas_retrieve_voice_rte(&voice_rte);

    if ( reg_info == NULL )
    {
        voice_rte = CRI_NAS_RTE_UNKNOWN;
    }

    switch(voice_rte)
    {
        case CRI_NAS_RTE_GSM:
            gsm_reg_info = (cri_nas_rte_gsm_reg_info_type*)reg_info;
            ss_info->gsm_sig_info_valid = TRUE;
            ss_info->gsm_sig_info.rssi = gsm_reg_info->gsm_sig_info;
            break;

        case CRI_NAS_RTE_WCDMA:
            wcdma_reg_info = (cri_nas_rte_wcdma_reg_info_type*)reg_info;
            ss_info->wcdma_sig_info_valid = TRUE;
            ss_info->wcdma_sig_info.rssi = wcdma_reg_info->wcdma_sig_info.rssi;
            ss_info->wcdma_sig_info.ecio = wcdma_reg_info->wcdma_sig_info.ecio;
            break;

        case CRI_NAS_RTE_LTE:
            lte_reg_info = (cri_nas_rte_lte_reg_info_type*)reg_info;
            ss_info->lte_sig_info_valid = TRUE;
            ss_info->lte_sig_info.rssi = lte_reg_info->lte_sig_info.rssi;
            ss_info->lte_sig_info.rsrp = lte_reg_info->lte_sig_info.rsrp;
            ss_info->lte_sig_info.rsrq = lte_reg_info->lte_sig_info.rsrq;
            ss_info->lte_sig_info.snr = lte_reg_info->lte_sig_info.snr;
            break;

        case CRI_NAS_RTE_TDSCDMA:
            tdscdma_reg_info = (cri_nas_rte_tdscdma_reg_info_type*)reg_info;
            ss_info->tdscdma_sig_info_valid = TRUE;
            ss_info->tdscdma_sig_info.rssi = tdscdma_reg_info->tdscdma_sig_info.rssi;
            ss_info->tdscdma_sig_info.rscp = tdscdma_reg_info->tdscdma_sig_info.rscp;
            ss_info->tdscdma_sig_info.ecio = tdscdma_reg_info->tdscdma_sig_info.ecio;
            ss_info->tdscdma_sig_info.sinr = tdscdma_reg_info->tdscdma_sig_info.sinr;
            break;

        case CRI_NAS_RTE_CDMA:
            cdma_reg_info = (cri_nas_rte_cdma_reg_info_type*)reg_info;
            ss_info->cdma_sig_info_valid = TRUE;
            ss_info->cdma_sig_info.rssi = cdma_reg_info->cdma_sig_info.rssi;
            ss_info->cdma_sig_info.ecio = cdma_reg_info->cdma_sig_info.ecio;
            break;

        case CRI_NAS_RTE_HDR:
            hdr_reg_info = (cri_nas_rte_hdr_reg_info_type*)reg_info;
            ss_info->hdr_sig_info_valid = TRUE;
            ss_info->hdr_sig_info.rssi = hdr_reg_info->hdr_sig_info.common_sig_str.rssi;
            ss_info->hdr_sig_info.ecio = hdr_reg_info->hdr_sig_info.common_sig_str.ecio;
            ss_info->hdr_sig_info.sinr = hdr_reg_info->hdr_sig_info.sinr;
            ss_info->hdr_sig_info.io = hdr_reg_info->hdr_sig_info.io;
            break;

        case CRI_NAS_RTE_UNKNOWN:
            UTIL_LOG_MSG("\nUnknown RTE\n");
            break;
    }

}



void hlos_nas_fill_signal_info_resp(mcm_nw_get_signal_strength_resp_msg_v01 *ss_info)
{
    void *reg_info;
    cri_nas_rte_type rte;
    cri_nas_tech_reg_status_type reg_state;
    cri_nas_rte_gsm_reg_info_type *gsm_reg_info;
    cri_nas_rte_wcdma_reg_info_type *wcdma_reg_info;
    cri_nas_rte_tdscdma_reg_info_type *tdscdma_reg_info;
    cri_nas_rte_lte_reg_info_type *lte_reg_info;
    cri_nas_rte_cdma_reg_info_type *cdma_reg_info;
    cri_nas_rte_hdr_reg_info_type *hdr_reg_info;

    memset(ss_info,NIL,sizeof(*ss_info));

    do
    {
        // If Data is registered, fill data rte signal strength.
        reg_state = cri_nas_retrieve_reg_state(FALSE);
        if ( reg_state == CRI_NAS_TECH_REG_STATUS_REGISTERED_HOME ||
             reg_state == CRI_NAS_TECH_REG_STATUS_REGISTERED_ROAMING )
        {
            reg_info = cri_nas_retrieve_data_rte(&rte);
            if ( reg_info == NULL )
                rte = CRI_NAS_RTE_UNKNOWN;
            else
                break;
        }

        // Only when Data is not registered, retrieve voice rte irrespective
        // of confidence and send what ever signal strength received from modem.
        reg_info = cri_nas_retrieve_voice_rte(&rte);
        if ( reg_info == NULL )
            rte = CRI_NAS_RTE_UNKNOWN;
    }while(FALSE);

    switch(rte)
    {
        case CRI_NAS_RTE_GSM:
            gsm_reg_info = (cri_nas_rte_gsm_reg_info_type*)reg_info;
            ss_info->gsm_sig_info_valid = TRUE;
            ss_info->gsm_sig_info.rssi = gsm_reg_info->gsm_sig_info;
            break;

        case CRI_NAS_RTE_WCDMA:
            wcdma_reg_info = (cri_nas_rte_wcdma_reg_info_type*)reg_info;
            ss_info->wcdma_sig_info_valid = TRUE;
            ss_info->wcdma_sig_info.rssi = wcdma_reg_info->wcdma_sig_info.rssi;
            ss_info->wcdma_sig_info.ecio = wcdma_reg_info->wcdma_sig_info.ecio;
            break;

        case CRI_NAS_RTE_LTE:
            lte_reg_info = (cri_nas_rte_lte_reg_info_type*)reg_info;
            ss_info->lte_sig_info_valid = TRUE;
            ss_info->lte_sig_info.rssi = lte_reg_info->lte_sig_info.rssi;
            ss_info->lte_sig_info.rsrp = lte_reg_info->lte_sig_info.rsrp;
            ss_info->lte_sig_info.rsrq = lte_reg_info->lte_sig_info.rsrq;
            ss_info->lte_sig_info.snr = lte_reg_info->lte_sig_info.snr;
            break;

        case CRI_NAS_RTE_TDSCDMA:
            tdscdma_reg_info = (cri_nas_rte_tdscdma_reg_info_type*)reg_info;
            ss_info->tdscdma_sig_info_valid = TRUE;
            ss_info->tdscdma_sig_info.rssi = tdscdma_reg_info->tdscdma_sig_info.rssi;
            ss_info->tdscdma_sig_info.rscp = tdscdma_reg_info->tdscdma_sig_info.rscp;
            ss_info->tdscdma_sig_info.ecio = tdscdma_reg_info->tdscdma_sig_info.ecio;
            ss_info->tdscdma_sig_info.sinr = tdscdma_reg_info->tdscdma_sig_info.sinr;
            break;

        case CRI_NAS_RTE_CDMA:
            cdma_reg_info = (cri_nas_rte_cdma_reg_info_type*)reg_info;
            ss_info->cdma_sig_info_valid = TRUE;
            ss_info->cdma_sig_info.rssi = cdma_reg_info->cdma_sig_info.rssi;
            ss_info->cdma_sig_info.ecio = cdma_reg_info->cdma_sig_info.ecio;
            break;

        case CRI_NAS_RTE_HDR:
            hdr_reg_info = (cri_nas_rte_hdr_reg_info_type*)reg_info;
            ss_info->hdr_sig_info_valid = TRUE;
            ss_info->hdr_sig_info.rssi = hdr_reg_info->hdr_sig_info.common_sig_str.rssi;
            ss_info->hdr_sig_info.ecio = hdr_reg_info->hdr_sig_info.common_sig_str.ecio;
            ss_info->hdr_sig_info.sinr = hdr_reg_info->hdr_sig_info.sinr;
            ss_info->hdr_sig_info.io = hdr_reg_info->hdr_sig_info.io;
            break;

        case CRI_NAS_RTE_UNKNOWN:
            UTIL_LOG_MSG("\nUnknown RTE\n");
            break;
    }

}


void hlos_nas_send_signal_update_to_client()
{
    mcm_nw_signal_strength_event_ind_msg_v01 ss_ind_msg;
    memset(&ss_ind_msg,NIL,sizeof(ss_ind_msg));

    hlos_nas_fill_signal_info_ind(&ss_ind_msg);

    hlos_core_send_indication(NIL,
                            MCM_NW_SIGNAL_STRENGTH_EVENT_IND_V01,
                            &ss_ind_msg,
                            sizeof(ss_ind_msg));
}


void hlos_nas_send_registration_update_to_client()
{
    void *voice_reg;
    void *data_reg;
    cri_nas_rte_type voice_rte;
    cri_nas_rte_type data_rte;

    mcm_nw_voice_registration_event_ind_msg_v01 voice_ind_msg;
    mcm_nw_data_registration_event_ind_msg_v01 data_ind_msg;
    mcm_nw_cell_access_state_change_event_ind_msg_v01 cell_access_ind_msg;

    memset(&voice_ind_msg, NIL,sizeof(voice_ind_msg));
    memset(&data_ind_msg, NIL,sizeof(data_ind_msg));
    memset(&cell_access_ind_msg,NIL,sizeof(cell_access_ind_msg));

    voice_ind_msg.voice_registration_valid = TRUE;

    voice_reg = cri_nas_retrieve_voice_rte(&voice_rte);

    hlos_nas_core_fill_voice_registration_indication(&voice_ind_msg);

    if ( hlos_nas_is_registration_info_changed(TRUE,&voice_ind_msg) )
    {
        UTIL_LOG_MSG("voice rte changed.. send indication to client\n");

        //copy to cache
        memcpy(&hlos_nas_cache.voice_reg_ind_cache,
                &voice_ind_msg,
                sizeof(voice_ind_msg));

        hlos_core_send_indication(NIL,
                                MCM_NW_VOICE_REGISTRATION_EVENT_IND_V01,
                                &voice_ind_msg,
                                sizeof(voice_ind_msg));
    }

    data_reg = cri_nas_retrieve_data_rte(&data_rte);

    hlos_nas_core_fill_data_registration_indication(&data_ind_msg);

    if ( hlos_nas_is_registration_info_changed(FALSE,&data_ind_msg) )
    {
        UTIL_LOG_MSG("data rte changed.. send indication to client\n");

        //copy to cache
        memcpy(&hlos_nas_cache.data_reg_ind_cache,
                &data_ind_msg,
                sizeof(data_ind_msg));

        hlos_core_send_indication(NIL,
                                MCM_NW_DATA_REGISTRATION_EVENT_IND_V01,
                                &data_ind_msg,
                                sizeof(data_ind_msg));
    }

    // check for cell access state change.
    cell_access_ind_msg.nw_cell_access_state =
        hlos_nas_get_cell_access_state();

    if ( hlos_nas_is_cell_access_state_info_changed(&cell_access_ind_msg))
    {
        UTIL_LOG_MSG("Cell access state changed to - %d\n",
            cell_access_ind_msg.nw_cell_access_state);

        //copy to cache
        memcpy(&hlos_nas_cache.cell_access_state_ind_cache,
                &cell_access_ind_msg,
                sizeof(cell_access_ind_msg));

        hlos_core_send_indication(NIL,
                                MCM_NW_CELL_ACCESS_STATE_CHANGE_EVENT_IND_V01,
                                &cell_access_ind_msg,
                                sizeof(cell_access_ind_msg));
    }

}


void hlos_nas_unsol_ind_handler(unsigned long message_id,
                                void *ind_data,
                                int ind_data_len)
{
    UTIL_LOG_MSG("hlos_nas_unsol_ind_handler enter: message_id - %x\n",message_id);

    switch(message_id)
    {
        case CRI_NAS_NW_STATE_INFO_IND:
            UTIL_LOG_MSG("Received CRI state info indication\n");
            hlos_nas_send_registration_update_to_client();
            break;

        case CRI_NAS_NW_SIGNAL_INFO_IND:
            UTIL_LOG_MSG("Received CRI sig info indication\n");
            hlos_nas_send_signal_update_to_client();
            break;

        case CRI_NAS_NETWORK_TIME_IND:
            UTIL_LOG_MSG("Received CRI NW Time info indication\n");
            hlos_nas_send_network_time_update_to_client();
            break;

        case CRI_NAS_LTE_SIB16_NETWORK_TIME_IND:
            UTIL_LOG_MSG("Received CRI LTE SIB16 NW Time info indication\n");
            hlos_nas_send_sib16_network_time_update_to_client();
            break;

        case CRI_NAS_DEVICE_STATE_INFO_IND:
            UTIL_LOG_MSG("Received CRI state info indication\n");
            break;

        default:
            UTIL_LOG_MSG("unhandled CRI NAS indication\n");
    }
}


void hlos_nas_network_config_response_handler(cri_core_context_type context,
                                            cri_core_error_type cri_core_error,
                                            void *hlos_cb_data,
                                            void *cri_resp_data)
{

    mcm_nw_set_config_resp_msg_v01 resp_msg;

    UTIL_LOG_MSG("hlos_nas_network_config_response_handler enter\n");

    memset(&resp_msg, 0, sizeof(resp_msg));
    if ( cri_core_error != MCM_SUCCESS_V01 )
    {
        resp_msg.response.result = MCM_RESULT_FAILURE_V01;
    }
    else
    {
        resp_msg.response.result = MCM_SUCCESS_V01;
    }
    resp_msg.response.error = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            cri_core_error,
                            hlos_cb_data,
                            &resp_msg,
                            sizeof(resp_msg));

}

void hlos_nas_network_scan_response_handler(cri_core_context_type context,
                                            cri_core_error_type cri_core_error,
                                            void *hlos_cb_data,
                                            void *cri_resp_data)
{

    uint32_t iter = 0;
    cri_nas_nw_scan_resp_type *scan_resp;
    mcm_nw_scan_resp_msg_v01 resp_msg;

    UTIL_LOG_MSG("hlos_nas_network_scan_response_handler enter\n");

    memset(&resp_msg, NIL, sizeof(resp_msg));
    UTIL_LOG_MSG("cri_core_error: %d", cri_core_error);

    if ( cri_core_error == CRI_ERR_NONE_V01 )
    {
        do
        {
            UTIL_LOG_MSG("cri_resp_data: %x", cri_resp_data);
            if ( !cri_resp_data )
            {
                // if response is null, break.
                // TODO: Whats the proper error to return here?
                cri_core_error = CRI_ERR_GENERAL_V01;
                break;
            }

            scan_resp = (cri_nas_nw_scan_resp_type*)cri_resp_data;
            resp_msg.entry_valid = TRUE;
            for( iter=0;iter<CRI_NAS_3GPP_NETWORK_INFO_LIST_MAX;iter++)
            {
                UTIL_LOG_MSG("iter: %d, eons: %s[%s], mcc: %s, mnc: %s", cri_resp_data,resp_msg.entry[iter].operator_name.long_eons,resp_msg.entry[iter].operator_name.short_eons,resp_msg.entry[iter].operator_name.mcc,resp_msg.entry[iter].operator_name.mnc);
                strlcpy(resp_msg.entry[iter].operator_name.long_eons,
                        scan_resp->long_eons[iter],
                        sizeof(resp_msg.entry[iter].operator_name.long_eons));
                strlcpy(resp_msg.entry[iter].operator_name.short_eons,
                        scan_resp->short_eons[iter],
                        sizeof(resp_msg.entry[iter].operator_name.short_eons));
                strlcpy(resp_msg.entry[iter].operator_name.mcc,
                        scan_resp->mcc[iter],
                        sizeof(resp_msg.entry[iter].operator_name.mcc));
                strlcpy(resp_msg.entry[iter].operator_name.mnc,
                        scan_resp->mnc[iter],
                        sizeof(resp_msg.entry[iter].operator_name.mnc));

                resp_msg.entry[iter].rat
                    =  hlos_nas_convert_cri_radiotech_to_mcm_radiotech(scan_resp->rat[iter]);
                UTIL_LOG_MSG("RAT: %d", resp_msg.entry[iter].rat);

                if ( scan_resp->network_status[iter] == CRI_NAS_NW_SCAN_RES_ENTRY_AVAILABLE )
                {
                    resp_msg.entry[iter].network_status = MCM_NW_NETWORK_STATUS_AVAILABLE_V01;
                }
                if ( scan_resp->network_status[iter] == CRI_NAS_NW_SCAN_RES_ENTRY_FORBIDDEN)
                {
                    resp_msg.entry[iter].network_status = MCM_NW_NETWORK_STATUS_FORBIDDEN_V01;
                }
                if ( scan_resp->network_status[iter] == CRI_NAS_NW_SCAN_RES_ENTRY_CUR_SERVING)
                {
                    resp_msg.entry[iter].network_status = MCM_NW_NETWORK_STATUS_CURRENT_SERVING_V01;
                }
            }
            resp_msg.entry_len = scan_resp->nw_scan_info_len;
            resp_msg.response.result = MCM_RESULT_SUCCESS_V01;
        }while(FALSE);
    }
    else
    {
        resp_msg.response.result = MCM_RESULT_FAILURE_V01;
        UTIL_LOG_MSG("Response result: %d", resp_msg.response.result);
    }

    resp_msg.response.error = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            cri_core_error,
                            hlos_cb_data,
                            &resp_msg,
                            sizeof(resp_msg));

}




void hlos_nas_network_scan_request_handler(void *event_data)
{
    cri_core_error_type ret_val = QMI_ERR_INTERNAL_V01;
    cri_core_context_type cri_core_context = NIL;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data = NULL;
    mcm_nw_scan_resp_msg_v01 *hlos_resp;

    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;
    hlos_resp = (mcm_nw_scan_resp_msg_v01*)util_memory_alloc(sizeof(*hlos_resp));

    if(event_data)
    {

            ret_val = cri_nas_core_network_scan_request_handler(cri_core_context,
                                                            event_data,
                                                            hlos_nas_network_scan_response_handler);

            if(QMI_ERR_NONE_V01 != ret_val )
            {
                if ( hlos_resp )
                {
                    hlos_resp->response.result = MCM_RESULT_FAILURE_V01;
                    hlos_resp->response.error = hlos_map_qmi_ril_error_to_mcm_error(ret_val);
                    // in case of error send response immediately. if success, async resp cb will send response to client.
                    UTIL_LOG_MSG("hlos_nas_network_scan_request_handler Error - %d\n", ret_val);
                    hlos_core_send_response(NIL,
                                            NIL,
                                            hlos_core_hlos_request_data,
                                            &hlos_resp,
                                            sizeof(hlos_resp));
                }
            }
    }

}




void hlos_nas_get_operator_name_request_handler(void *event_data)
{
    cri_core_error_type ret_val = QMI_ERR_INTERNAL_V01;
    cri_core_context_type cri_core_context;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data = NULL;
    mcm_nw_get_operator_name_resp_msg_v01 hlos_resp;

    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;
    memset(&hlos_resp,NIL,sizeof(hlos_resp));

    if(event_data)
    {

            ret_val = cri_nas_get_operator_name_request_handler(&hlos_resp.operator_name.long_eons,
                                                                &hlos_resp.operator_name.short_eons,
                                                                &hlos_resp.operator_name.mcc,
                                                                &hlos_resp.operator_name.mnc);

            if(QMI_ERR_NONE_V01 != ret_val )
            {
                hlos_resp.response.result = MCM_RESULT_FAILURE_V01;
                hlos_resp.response.error = hlos_map_qmi_ril_error_to_mcm_error(ret_val);
                // in case of error send response immediately. if success, async resp cb will send response to client.
                UTIL_LOG_MSG("hlos_nas_get_operator_name_request_handler Error - %d\n", ret_val);
            }
            else
            {
                hlos_resp.operator_name_valid = TRUE;
            }
            hlos_core_send_response(NIL,
                                    NIL,
                                    hlos_core_hlos_request_data,
                                    &hlos_resp,
                                    sizeof(hlos_resp));

    }

}



void hlos_nas_network_config_request_handler(void *event_data)
{
    uint32_t is_changed;
    uint32_t pref_mode = 0;
    uint32_t roaming_pref = MCM_NW_ROAM_STATE_OFF_V01;
    mcm_nw_set_config_req_msg_v01 *hlos_req_msg;
    cri_core_error_type ret_val = QMI_ERR_INTERNAL_V01;
    cri_core_context_type cri_core_context = NIL;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data = NULL;
    mcm_nw_set_config_resp_msg_v01 hlos_resp;
    cri_nas_rte_type rte;


    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

    if(event_data)
    {
        hlos_req_msg = (mcm_nw_set_config_req_msg_v01*)hlos_core_hlos_request_data->data;
        if ( hlos_req_msg->preferred_nw_mode_valid == TRUE ||
            hlos_req_msg->roaming_pref_valid == TRUE )
        {
            if ( hlos_req_msg->preferred_nw_mode_valid == TRUE )
            {
                UTIL_LOG_MSG("Set preferred mode valid - %d mode - %d: %x\n",
                                                                    hlos_req_msg->preferred_nw_mode_valid,
                                                                    hlos_req_msg->preferred_nw_mode);
                pref_mode = hlos_nas_convert_mcm_prefmode_to_qmi_prefmode(hlos_req_msg->preferred_nw_mode);
            }

            if ( hlos_req_msg->roaming_pref_valid == TRUE )
            {
                UTIL_LOG_MSG("Roaming preference valid - %d roaming pref - %d: %x\n",
                                                                    hlos_req_msg->roaming_pref_valid,
                                                                    hlos_req_msg->roaming_pref);
                if (hlos_req_msg->roaming_pref == MCM_NW_ROAM_STATE_OFF_V01)
                {
                    roaming_pref = CRI_NAS_ROAMING_PREF_OFF;
                }
                else if (hlos_req_msg->roaming_pref == MCM_NW_ROAM_STATE_ON_V01)
                {
                    roaming_pref = CRI_NAS_ROAMING_PREF_ANY;
                }
            }

            ret_val = cri_nas_set_pref_mode_request_handler(cri_core_context,
                                                            hlos_req_msg->preferred_nw_mode_valid,
                                                            pref_mode,
                                                            hlos_req_msg->roaming_pref_valid,
                                                            roaming_pref,
                                                            event_data,
                                                            hlos_nas_network_config_response_handler,
                                                            &is_changed);

            if(QMI_ERR_NONE_V01 != ret_val || is_changed == FALSE)
            {
                hlos_resp.response.result = MCM_RESULT_FAILURE_V01;
                hlos_resp.response.error = hlos_map_qmi_ril_error_to_mcm_error(ret_val);
                // in case of error send response immediately. if success, async resp cb will send response to client.
                UTIL_LOG_MSG("hlos_nas_network_config_request_handler Error - %d\n", ret_val);
                hlos_core_send_response(NIL,
                                        NIL,
                                        hlos_core_hlos_request_data,
                                        &hlos_resp,
                                        sizeof(hlos_resp));
            }
        }

    }

}

void hlos_nas_core_populate_data_registration_info(mcm_nw_get_registration_status_resp_msg_v01 *hlos_resp)
{

    void *reg_info;
    cri_nas_rte_gsm_reg_info_type *gsm_data_reg_info;
    cri_nas_rte_wcdma_reg_info_type *wcdma_data_reg_info;
    cri_nas_rte_cdma_reg_info_type *cdma_data_reg_info;
    cri_nas_rte_hdr_reg_info_type *hdr_data_reg_info;
    cri_nas_rte_lte_reg_info_type *lte_data_reg_info;
    cri_nas_rte_tdscdma_reg_info_type *tdscdma_data_reg_info;

    cri_nas_rte_type data_rte;

    reg_info = cri_nas_retrieve_data_rte(&data_rte);

    if ( reg_info == NULL )
    {
        data_rte = CRI_NAS_RTE_UNKNOWN;
    }

    switch(data_rte)
    {
        case CRI_NAS_RTE_GSM:

            gsm_data_reg_info = (cri_nas_rte_gsm_reg_info_type*)reg_info;
            hlos_resp->data_registration_valid = TRUE;
            hlos_resp->data_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &hlos_resp->data_registration,
                                                        &gsm_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(data_rte,
                                         &hlos_resp->data_registration_details_3gpp,
                                         gsm_data_reg_info);
            break;

        case CRI_NAS_RTE_WCDMA:
            wcdma_data_reg_info = (cri_nas_rte_wcdma_reg_info_type*)reg_info;
            hlos_resp->data_registration_valid = TRUE;
            hlos_resp->data_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &hlos_resp->data_registration,
                                                        &wcdma_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(data_rte,
                                        &hlos_resp->data_registration_details_3gpp,
                                        wcdma_data_reg_info);

            break;

        case CRI_NAS_RTE_CDMA:
            cdma_data_reg_info = (cri_nas_rte_cdma_reg_info_type*)reg_info;
            hlos_resp->data_registration_valid = TRUE;
            hlos_resp->data_registration_details_3gpp2_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(FALSE,
                                                        &hlos_resp->data_registration,
                                                        &cdma_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp2_info(data_rte,
                                          &hlos_resp->data_registration_details_3gpp2,
                                          cdma_data_reg_info);


            break;

        case CRI_NAS_RTE_HDR:
            hdr_data_reg_info = (cri_nas_rte_hdr_reg_info_type*)reg_info;
            hlos_resp->data_registration_valid = TRUE;
            hlos_resp->data_registration_details_3gpp2_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(FALSE,
                                                        &hlos_resp->data_registration,
                                                        &hdr_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp2_info(data_rte,
                                          &hlos_resp->data_registration_details_3gpp2,
                                          hdr_data_reg_info);


            break;

        case CRI_NAS_RTE_LTE:
            lte_data_reg_info = (cri_nas_rte_lte_reg_info_type*)reg_info;
            hlos_resp->data_registration_valid = TRUE;
            hlos_resp->data_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &hlos_resp->data_registration,
                                                        &lte_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(data_rte,
                                         &hlos_resp->data_registration_details_3gpp,
                                         lte_data_reg_info);

            break;

        case CRI_NAS_RTE_TDSCDMA:
            tdscdma_data_reg_info = (cri_nas_rte_tdscdma_reg_info_type*)reg_info;
            hlos_resp->data_registration_valid = TRUE;
            hlos_resp->data_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &hlos_resp->data_registration,
                                                        &tdscdma_data_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(data_rte,
                                        &hlos_resp->data_registration_details_3gpp,
                                        tdscdma_data_reg_info);
            break;

        case CRI_NAS_RTE_UNKNOWN:
            UTIL_LOG_MSG("\nUnknown voice RTE\n");
            break;
    }


}


void hlos_nas_core_populate_voice_registration_info(mcm_nw_get_registration_status_resp_msg_v01 *hlos_resp)
{

    void *reg_info;
    cri_nas_rte_gsm_reg_info_type *gsm_voice_reg_info;
    cri_nas_rte_wcdma_reg_info_type *wcdma_voice_reg_info;
    cri_nas_rte_cdma_reg_info_type *cdma_voice_reg_info;
    cri_nas_rte_hdr_reg_info_type *hdr_voice_reg_info;
    cri_nas_rte_lte_reg_info_type *lte_voice_reg_info;
    cri_nas_rte_tdscdma_reg_info_type *tdscdma_voice_reg_info;

    cri_nas_rte_type voice_rte = CRI_NAS_RTE_UNKNOWN;

    reg_info = cri_nas_retrieve_voice_rte(&voice_rte);

    if ( reg_info == NULL )
    {
        voice_rte = CRI_NAS_RTE_UNKNOWN;
    }

    switch(voice_rte)
    {
        case CRI_NAS_RTE_GSM:

            gsm_voice_reg_info = (cri_nas_rte_gsm_reg_info_type*)reg_info;
            hlos_resp->voice_registration_valid = TRUE;
            hlos_resp->voice_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &hlos_resp->voice_registration,
                                                        &gsm_voice_reg_info->reg_info);
            hlos_nas_core_fill_3gpp_info(voice_rte,
                                         &hlos_resp->voice_registration_details_3gpp,
                                         gsm_voice_reg_info);

            break;

        case CRI_NAS_RTE_WCDMA:
            wcdma_voice_reg_info = (cri_nas_rte_wcdma_reg_info_type*)reg_info;
            hlos_resp->voice_registration_valid = TRUE;
            hlos_resp->voice_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &hlos_resp->voice_registration,
                                                        &wcdma_voice_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(voice_rte,
                                         &hlos_resp->voice_registration_details_3gpp,
                                         wcdma_voice_reg_info);

            break;

        case CRI_NAS_RTE_CDMA:
            cdma_voice_reg_info = (cri_nas_rte_cdma_reg_info_type*)reg_info;
            hlos_resp->voice_registration_valid = TRUE;
            hlos_resp->voice_registration_details_3gpp2_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(FALSE,
                                                        &hlos_resp->voice_registration,
                                                        &cdma_voice_reg_info->reg_info);

            hlos_nas_core_fill_3gpp2_info(voice_rte,
                                          &hlos_resp->voice_registration_details_3gpp2,
                                          cdma_voice_reg_info);

            break;

        case CRI_NAS_RTE_HDR:
            hdr_voice_reg_info = (cri_nas_rte_hdr_reg_info_type*)reg_info;
            hlos_resp->voice_registration_valid = TRUE;
            hlos_resp->voice_registration_details_3gpp2_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(FALSE,
                                                        &hlos_resp->voice_registration,
                                                        &hdr_voice_reg_info->reg_info);

            hlos_nas_core_fill_3gpp2_info(voice_rte,
                                        &hlos_resp->voice_registration_details_3gpp2,
                                        hdr_voice_reg_info);


            break;

        case CRI_NAS_RTE_LTE:
            lte_voice_reg_info = (cri_nas_rte_lte_reg_info_type*)reg_info;
            hlos_resp->voice_registration_valid = TRUE;
            hlos_resp->voice_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(FALSE,
                                                        &hlos_resp->voice_registration,
                                                        &lte_voice_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(voice_rte,
                                         &hlos_resp->voice_registration_details_3gpp,
                                         lte_voice_reg_info);

            break;

        case CRI_NAS_RTE_TDSCDMA:
            tdscdma_voice_reg_info = (cri_nas_rte_tdscdma_reg_info_type*)reg_info;
            hlos_resp->voice_registration_valid = TRUE;
            hlos_resp->voice_registration_details_3gpp_valid = TRUE;

            hlos_nas_core_fill_common_registration_info(TRUE,
                                                        &hlos_resp->voice_registration,
                                                        &tdscdma_voice_reg_info->reg_info);

            hlos_nas_core_fill_3gpp_info(voice_rte,
                                         &hlos_resp->voice_registration_details_3gpp,
                                         tdscdma_voice_reg_info);
            break;

        case CRI_NAS_RTE_UNKNOWN:
            UTIL_LOG_MSG("\nUnknown voice RTE\n");
            break;
    }


}



void hlos_nas_network_screen_state_handler(void *event_data)
{
    cri_core_error_type ret_val;
    uint8_t is_screen_off = FALSE;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    mcm_nw_screen_on_off_req_msg_v01 *hlos_req;
    mcm_nw_screen_on_off_resp_msg_v01 hlos_resp;

    UTIL_LOG_MSG("\n hlos_nas_network_screen_state_handler entry\n");

    memset(&hlos_resp, NIL, sizeof(hlos_resp));
    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;
    hlos_req = (mcm_nw_screen_on_off_req_msg_v01*)hlos_core_hlos_request_data->data;

    UTIL_LOG_MSG("\nrequest to set screen off state - %d\n", hlos_req->turn_off_screen);

    is_screen_off = hlos_req->turn_off_screen;

    ret_val = cri_nas_change_screen_state(is_screen_off);

    hlos_resp.response.result = MCM_RESULT_SUCCESS_V01;
    hlos_resp.response.error = hlos_map_qmi_ril_error_to_mcm_error(ret_val);
    hlos_core_send_response(NIL,
                            ret_val,
                            hlos_core_hlos_request_data,
                            &hlos_resp,
                            sizeof(hlos_resp));
}



void hlos_nas_network_get_status_request_handler(void *event_data)
{

    cri_core_error_type ret_val;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    mcm_nw_get_registration_status_resp_msg_v01 hlos_resp;

    UTIL_LOG_MSG("\n hlos_nas_network_get_status_request_handler entry\n");

    memset(&hlos_resp, NIL, sizeof(hlos_resp));
    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;

    hlos_nas_core_populate_voice_registration_info(&hlos_resp);
    hlos_nas_core_populate_data_registration_info(&hlos_resp);

    hlos_resp.response.result = 0;
    hlos_resp.response.error = 0;//hlos_map_qmi_ril_error_to_mcm_error(ret_val);
    hlos_core_send_response(NIL,
                            ret_val,
                            hlos_core_hlos_request_data,
                            &hlos_resp,
                            sizeof(hlos_resp));

}


void hlos_nas_network_get_config_request_handler(void *event_data)
{
    uint32_t qmi_pref_mode;
    uint32_t qmi_roaming_pref_mode;
    uint32_t mcm_pref_mode;
    uint32_t mcm_roaming_pref;
    cri_core_error_type ret_val;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    mcm_nw_get_config_resp_msg_v01 hlos_resp;

    UTIL_LOG_MSG("\n hlos_nas_network_get_config_request_handler entry\n");

    memset(&hlos_resp, NIL, sizeof(hlos_resp));
    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;

    ret_val = cri_nas_get_mode_preferrence(&qmi_pref_mode);
    if ( ret_val != CRI_ERR_NONE_V01 )
    {
        UTIL_LOG_MSG("\nFailed to get mode preference - %d\n", ret_val);
    }

    hlos_resp.preferred_nw_mode_valid = TRUE;
    hlos_resp.preferred_nw_mode = hlos_nas_convert_qmi_prefmode_to_mcm_prefmode(qmi_pref_mode);

    ret_val = cri_nas_get_roaming_preferrence(&qmi_roaming_pref_mode);
    if ( ret_val != CRI_ERR_NONE_V01 )
    {
        UTIL_LOG_MSG("\nFailed to get mode preference - %d\n", ret_val);
    }

    hlos_resp.roaming_pref_valid = TRUE;
    if ( qmi_roaming_pref_mode == CRI_NAS_ROAMING_PREF_OFF )
    {
        hlos_resp.roaming_pref = MCM_NW_ROAM_STATE_OFF_V01;
    }
    else if ( qmi_roaming_pref_mode == CRI_NAS_ROAMING_PREF_ANY )
    {
        hlos_resp.roaming_pref = MCM_NW_ROAM_STATE_ON_V01;
    }

    hlos_resp.response.result = 0;
    hlos_resp.response.error = hlos_map_qmi_ril_error_to_mcm_error(ret_val);
    hlos_core_send_response(NIL,
                            ret_val,
                            hlos_core_hlos_request_data,
                            &hlos_resp,
                            sizeof(hlos_resp));

}


void hlos_nas_nw_scan_request_handler(void *event_data)
{
    mcm_nw_scan_req_msg_v01 *hlos_req_msg;
    cri_core_error_type ret_val = QMI_ERR_INTERNAL_V01;
    cri_core_context_type cri_core_context = NIL;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data = NULL;

    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;

    if(event_data)
    {
        ret_val = cri_nas_core_network_scan_request_handler(cri_core_context,
                                                            event_data,
                                                            hlos_nas_network_config_response_handler);
    }

}


void hlos_nas_get_cell_access_state_handler(void *event_data)
{
    cri_core_error_type ret_val = CRI_ERR_NONE_V01;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    mcm_nw_get_cell_access_state_req_msg_v01 *hlos_req_msg;
    mcm_nw_get_cell_access_state_resp_msg_v01 hlos_resp_msg;

    UTIL_LOG_MSG("hlos_nas_get_cell_access_state_handler Enter");

    memset(&hlos_resp_msg,NIL,sizeof(hlos_resp_msg));
    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;

    hlos_resp_msg.nw_cell_access_state_valid = TRUE;

    hlos_resp_msg.nw_cell_access_state =
        hlos_nas_get_cell_access_state();

    hlos_resp_msg.response.error = MCM_SUCCESS_V01;
    hlos_resp_msg.response.result = MCM_RESULT_SUCCESS_V01;

    hlos_core_send_response(NIL,
                            ret_val,
                            hlos_core_hlos_request_data,
                            &hlos_resp_msg,
                            sizeof(hlos_resp_msg));

}


void hlos_nas_network_get_signal_strength_handler(void *event_data)
{
    cri_core_error_type ret_val = CRI_ERR_NONE_V01;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    mcm_nw_get_signal_strength_req_msg_v01 *hlos_req_msg;
    mcm_nw_get_signal_strength_resp_msg_v01 hlos_resp_msg;

    UTIL_LOG_MSG("\nhlos_nas_network_get_signal_strength_handler entry\n");

    memset(&hlos_resp_msg, NIL, sizeof(hlos_resp_msg));
    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;

    hlos_nas_fill_signal_info_resp(&hlos_resp_msg);

    hlos_resp_msg.response.result = MCM_RESULT_SUCCESS_V01;
    hlos_resp_msg.response.error = MCM_SUCCESS_V01;

    hlos_core_send_response(NIL,
                            ret_val,
                            hlos_core_hlos_request_data,
                            &hlos_resp_msg,
                            sizeof(hlos_resp_msg));


}

void hlos_nas_network_get_nitz_time_info_handler(void *event_data)
{
    cri_core_error_type ret_val = CRI_ERR_NONE_V01;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    mcm_nw_get_nitz_time_info_req_msg_v01 *hlos_req_msg;
    mcm_nw_get_nitz_time_info_resp_msg_v01 hlos_resp_msg;

    UTIL_LOG_MSG("\nhlos_nas_network_get_nitz_time_info_handler entry\n");

    memset(&hlos_resp_msg, NIL, sizeof(hlos_resp_msg));
    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;

    cri_nas_fetch_update_lte_sib16_nitz_time_info();
    cri_nas_fetch_update_nitz_time_info();
    cri_nas_fill_nitz_time_resp(&hlos_resp_msg.nw_nitz_time_valid, &hlos_resp_msg.nw_nitz_time.nitz_time, &hlos_resp_msg.abs_time_valid, &hlos_resp_msg.abs_time,
	    &hlos_resp_msg.leap_sec_valid, &hlos_resp_msg.leap_sec);

    hlos_resp_msg.response.result = MCM_RESULT_SUCCESS_V01;
    hlos_resp_msg.response.error = MCM_SUCCESS_V01;

    UTIL_LOG_MSG("Get NITZ time info valid %d",hlos_resp_msg.nw_nitz_time_valid);
    if(TRUE == hlos_resp_msg.nw_nitz_time_valid)
    {
        UTIL_LOG_MSG("Get NITZ time info %s",&hlos_resp_msg.nw_nitz_time.nitz_time);
    }

    UTIL_LOG_MSG("abs time valid %d",hlos_resp_msg.abs_time_valid);
    if(TRUE == hlos_resp_msg.abs_time_valid)
    {
        UTIL_LOG_MSG("abs time (%x, %x)", (uint32_t)(hlos_resp_msg.abs_time >> 32), (uint32_t)hlos_resp_msg.abs_time);
    }

    UTIL_LOG_MSG("Leap sec valid %d",hlos_resp_msg.leap_sec_valid);
    if(TRUE == hlos_resp_msg.leap_sec_valid)
    {
        UTIL_LOG_MSG("Leap sec %d",hlos_resp_msg.leap_sec);
    }

    hlos_core_send_response(NIL,
                            ret_val,
                            hlos_core_hlos_request_data,
                            &hlos_resp_msg,
                            sizeof(hlos_resp_msg));


}

void hlos_nas_network_selection_response_handler(cri_core_context_type context,
                                            cri_core_error_type cri_core_error,
                                            void *hlos_cb_data,
                                            void *cri_resp_data)
{
    mcm_nw_selection_resp_msg_v01 resp_msg;

    UTIL_LOG_MSG("hlos_nas_network_selection_response_handler enter\n");

    memset(&resp_msg, NIL, sizeof(resp_msg));

    if ( cri_core_error == CRI_ERR_NONE_V01 )
    {
        resp_msg.response.result = MCM_RESULT_SUCCESS_V01;
    }
    else
    {
        resp_msg.response.result = MCM_RESULT_FAILURE_V01;
    }

    resp_msg.response.error = hlos_map_qmi_ril_error_to_mcm_error(cri_core_error);
    hlos_core_send_response(NIL,
                            cri_core_error,
                            hlos_cb_data,
                            &resp_msg,
                            sizeof(resp_msg));

}


void hlos_nas_network_selection_handler(void *event_data)
{
    cri_nas_nw_selection_request_type selection_info;
    mcm_nw_selection_req_msg_v01 *hlos_req_msg;
    cri_core_error_type ret_val = QMI_ERR_INTERNAL_V01;
    cri_core_context_type cri_core_context = NIL;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data = NULL;

    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*)event_data;
    hlos_req_msg = (mcm_nw_selection_req_msg_v01*)hlos_core_hlos_request_data->data;

    if ( hlos_req_msg->nw_selection_info.nw_selection_type == MCM_NW_SELECTION_AUTOMATIC_V01 )
    {
        selection_info.is_automatic = TRUE;
    }
    else
    {
        selection_info.is_automatic = FALSE;
        selection_info.mcc = atoi(hlos_req_msg->nw_selection_info.mcc);
        selection_info.mnc = atoi(hlos_req_msg->nw_selection_info.mnc);
        selection_info.rat
            = hlos_nas_convert_mcm_rat_to_cri(hlos_req_msg->nw_selection_info.rat);
    }

    if(event_data)
    {
        ret_val = cri_nas_network_selection_handler(cri_core_context,
                                                    &selection_info,
                                                    event_data,
                                                    hlos_nas_network_selection_response_handler);
    }

}



uint32_t hlos_nas_convert_mcm_prefmode_to_qmi_prefmode(uint32_t mcm_pref_mode)
{

    int qmi_pref_mode = NIL;

    if ( NAS_IS_RAT_SET(mcm_pref_mode,MCM_MODE_CDMA_V01))
    {
        NAS_SET_BIT(qmi_pref_mode,CRI_NAS_RAT_CDMA_BIT);
    }
    if ( NAS_IS_RAT_SET(mcm_pref_mode,MCM_MODE_EVDO_V01))
    {
        NAS_SET_BIT(qmi_pref_mode,CRI_NAS_RAT_HRPD_BIT);
    }
    if ( NAS_IS_RAT_SET(mcm_pref_mode,MCM_MODE_GSM_V01))
    {
        NAS_SET_BIT(qmi_pref_mode,CRI_NAS_RAT_GSM_BIT);
    }
    if ( NAS_IS_RAT_SET(mcm_pref_mode,MCM_MODE_WCDMA_V01))
    {
        NAS_SET_BIT(qmi_pref_mode,CRI_NAS_RAT_WCDMA_BIT);
    }
    if ( NAS_IS_RAT_SET(mcm_pref_mode,MCM_MODE_LTE_V01))
    {
        NAS_SET_BIT(qmi_pref_mode,CRI_NAS_RAT_LTE_BIT);
    }
    if ( NAS_IS_RAT_SET(mcm_pref_mode,MCM_MODE_TDSCDMA_V01))
    {
        NAS_SET_BIT(qmi_pref_mode,CRI_NAS_RAT_TDSCDMA_BIT);
    }
    if ( NAS_IS_RAT_SET(mcm_pref_mode,MCM_MODE_TDSCDMA_V01))
    {
        NAS_SET_BIT(qmi_pref_mode,CRI_NAS_RAT_TDSCDMA_BIT);
    }
    if ( NAS_IS_RAT_SET(mcm_pref_mode,MCM_MODE_PRL_V01))
    {
        NAS_SET_BIT(qmi_pref_mode,CRI_NAS_RAT_PRL_BIT);
    }

    return qmi_pref_mode;
}


uint32_t hlos_nas_convert_qmi_prefmode_to_mcm_prefmode(uint32_t qmi_pref_mode)
{

    int mcm_pref_mode = NIL;

    if ( NAS_IS_BIT_SET(qmi_pref_mode,CRI_NAS_RAT_CDMA_BIT))
    {
        NAS_RAT_SET(mcm_pref_mode,MCM_MODE_CDMA_V01);
    }
    if ( NAS_IS_BIT_SET(qmi_pref_mode,CRI_NAS_RAT_HRPD_BIT))
    {
        NAS_RAT_SET(mcm_pref_mode,MCM_MODE_EVDO_V01);
    }
    if ( NAS_IS_BIT_SET(qmi_pref_mode,CRI_NAS_RAT_GSM_BIT))
    {
        NAS_RAT_SET(mcm_pref_mode,MCM_MODE_GSM_V01);
    }
    if ( NAS_IS_BIT_SET(qmi_pref_mode,CRI_NAS_RAT_WCDMA_BIT))
    {
        NAS_RAT_SET(mcm_pref_mode,MCM_MODE_WCDMA_V01);
    }
    if ( NAS_IS_BIT_SET(qmi_pref_mode,CRI_NAS_RAT_LTE_BIT))
    {
        NAS_RAT_SET(mcm_pref_mode,MCM_MODE_LTE_V01);
    }
     if ( NAS_IS_BIT_SET(qmi_pref_mode,CRI_NAS_RAT_TDSCDMA_BIT))
    {
        NAS_RAT_SET(mcm_pref_mode,MCM_MODE_TDSCDMA_V01);
    }
/*    if ( NAS_IS_BIT_SET(qmi_pref_mode,CRI_NAS_RAT_PRL_BIT))
    {
        NAS_RAT_SET(mcm_pref_mode,MCM_MODE_PRL_V01);
    }
*/
    return mcm_pref_mode;
}

/* 2019/10/23 added by tommy.zhang */
void hlos_nas_core_populate_serving_rate_info(mcm_nw_get_cell_info_resp_msg_v01 *resp)
{
    void *reg_info;
    cri_nas_rte_gsm_reg_info_type *gsm_data_reg_info;
    cri_nas_rte_wcdma_reg_info_type *wcdma_data_reg_info;
    cri_nas_rte_cdma_reg_info_type *cdma_data_reg_info;
    cri_nas_rte_hdr_reg_info_type *hdr_data_reg_info;
    cri_nas_rte_lte_reg_info_type *lte_data_reg_info;
    cri_nas_rte_tdscdma_reg_info_type *tdscdma_data_reg_info;

    cri_nas_rte_type data_rte;

    reg_info = cri_nas_retrieve_data_rte(&data_rte);

    if ( reg_info == NULL )
    {
        data_rte = CRI_NAS_RTE_UNKNOWN;
    }

    switch(data_rte)
    {
        case CRI_NAS_RTE_GSM:
            gsm_data_reg_info = (cri_nas_rte_gsm_reg_info_type*)reg_info;
            resp->serving_rat = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(gsm_data_reg_info->reg_info.tech);
            break;

        case CRI_NAS_RTE_WCDMA:
            wcdma_data_reg_info = (cri_nas_rte_wcdma_reg_info_type*)reg_info;
            resp->serving_rat = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(wcdma_data_reg_info->reg_info.tech);
            break;

        case CRI_NAS_RTE_CDMA:
            cdma_data_reg_info = (cri_nas_rte_cdma_reg_info_type*)reg_info;
            resp->serving_rat = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(cdma_data_reg_info->reg_info.tech);
            break;

        case CRI_NAS_RTE_HDR:
            hdr_data_reg_info = (cri_nas_rte_hdr_reg_info_type*)reg_info;
            resp->serving_rat = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(hdr_data_reg_info->reg_info.tech);
            break;

        case CRI_NAS_RTE_LTE:
            lte_data_reg_info = (cri_nas_rte_lte_reg_info_type*)reg_info;
            resp->serving_rat = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(lte_data_reg_info->reg_info.tech);
            break;

        case CRI_NAS_RTE_TDSCDMA:
            tdscdma_data_reg_info = (cri_nas_rte_tdscdma_reg_info_type*)reg_info;
            resp->serving_rat = hlos_nas_convert_cri_radiotech_to_mcm_radiotech(tdscdma_data_reg_info->reg_info.tech);
            break;

        case CRI_NAS_RTE_UNKNOWN:
            UTIL_LOG_MSG("\nUnknown voice RTE\n");
            break;
    }
}

void hlos_nas_get_cell_info_handler(void *event_data)
{
    int i;
    qmi_error_type_v01 ret_val = QMI_ERR_INTERNAL_V01;

    mcm_nw_get_cell_info_req_msg_v01 *hlos_req_msg;
    mcm_nw_get_cell_info_resp_msg_v01 hlos_resp_msg;
    hlos_core_hlos_request_data_type *hlos_core_hlos_request_data;
    hlos_core_hlos_request_data = (hlos_core_hlos_request_data_type*) event_data;
    hlos_req_msg = (void *)hlos_core_hlos_request_data->data;
    nas_get_cell_location_info_req_msg_v01 nas_req_msg;
    nas_get_cell_location_info_resp_msg_v01 nas_resp_msg;

    memset(&nas_req_msg, 0, sizeof(nas_req_msg));
    memset(&nas_resp_msg, 0, sizeof(nas_resp_msg));

    memset(&hlos_resp_msg, 0, sizeof(hlos_resp_msg));

    ret_val = cri_core_qmi_send_msg_sync( cri_nas_core_retrieve_client_id(),
            QMI_NAS_GET_CELL_LOCATION_INFO_REQ_MSG_V01,
            &nas_req_msg,
            sizeof(nas_req_msg),
            &nas_resp_msg,
            sizeof(nas_resp_msg),
            CRI_CORE_MAX_TIMEOUT );

    if(QMI_ERR_NONE_V01 != ret_val)
    {
        UTIL_LOG("cri_nas_core_network_scan_request_handler error - %d\n", ret_val);
    }
    else {
        if(nas_resp_msg.lte_intra_valid) {
            hlos_resp_msg.lte_info_valid = 1;
            hlos_resp_msg.lte_info_len = nas_resp_msg.lte_intra.cells_len + 1;
            hlos_resp_msg.lte_info[0].cid = nas_resp_msg.lte_intra.global_cell_id;
            memcpy(hlos_resp_msg.lte_info[0].plmn, nas_resp_msg.lte_intra.plmn, 3);
            hlos_resp_msg.lte_info[0].tac = nas_resp_msg.lte_intra.tac;
            hlos_resp_msg.lte_info[0].pci = 0;
            hlos_resp_msg.lte_info[0].earfcn = nas_resp_msg.lte_intra.earfcn;

            for(i=0; i<nas_resp_msg.lte_intra.cells_len; i++) {
                hlos_resp_msg.lte_info[i+1].pci = nas_resp_msg.lte_intra.cells[i].pci;
            }
        }

        if(nas_resp_msg.geran_info_valid) {
            nas_geran_cell_info_type_v01 *pinfo = &nas_resp_msg.geran_info;

            hlos_resp_msg.gsm_info_valid = 1;
            hlos_resp_msg.gsm_info_len = pinfo->nmr_cell_info_len + 1;
            hlos_resp_msg.gsm_info[0].cid = pinfo->cell_id;
            memcpy(hlos_resp_msg.gsm_info[0].plmn, pinfo->plmn, 3);
            hlos_resp_msg.gsm_info[0].lac = pinfo->lac;
            hlos_resp_msg.gsm_info[0].arfcn = pinfo->arfcn;
            hlos_resp_msg.gsm_info[0].bsic = pinfo->bsic;

            for(i=0; i<pinfo->nmr_cell_info_len; i++) {
                nas_nmr_cell_info_type_v01 *pi = &pinfo->nmr_cell_info[i];
                hlos_resp_msg.gsm_info[i+1].cid = pi->nmr_cell_id;
                memcpy(hlos_resp_msg.gsm_info[i+1].plmn, pi->nmr_plmn, 3);
                hlos_resp_msg.gsm_info[i+1].lac = pi->nmr_lac;
                hlos_resp_msg.gsm_info[i+1].arfcn = pi->nmr_arfcn;
                hlos_resp_msg.gsm_info[i+1].bsic = pi->nmr_bsic;
            }
        }

        if(nas_resp_msg.umts_info_valid) {
            int oft = 0;
            nas_umts_cell_info_type_v01 *pinfo = &nas_resp_msg.umts_info;
            hlos_resp_msg.umts_info_valid = 1;
            hlos_resp_msg.umts_info_len = pinfo->umts_monitored_cell_len + pinfo->umts_geran_nbr_cell_len + 1;

            hlos_resp_msg.umts_info[oft].cid = pinfo->cell_id;
            memcpy(hlos_resp_msg.umts_info[oft].plmn, pinfo->plmn, 3);
            hlos_resp_msg.umts_info[oft].lcid = 0;
            hlos_resp_msg.umts_info[oft].lac = pinfo->lac;
            hlos_resp_msg.umts_info[oft].uarfcn = pinfo->uarfcn;
            hlos_resp_msg.umts_info[oft].psc = pinfo->psc;
            oft++;

            for(i=0; i<pinfo->umts_monitored_cell_len; i++) {
                nas_umts_monitored_cell_set_info_type_v01 *pi = &pinfo->umts_monitored_cell[i];
                hlos_resp_msg.umts_info[oft].uarfcn = pi->umts_uarfcn;
                hlos_resp_msg.umts_info[oft].psc = pi->umts_psc;
                oft++;
            }

            for(i=0; i<pinfo->umts_geran_nbr_cell_len; i++) {
                nas_umts_geran_nbr_cell_set_info_type_v01 *pi = &pinfo->umts_geran_nbr_cell[i];
                hlos_resp_msg.umts_info[oft].uarfcn = pi->geran_arfcn;
                oft++;
            }
        }
    }

    hlos_nas_core_populate_serving_rate_info(&hlos_resp_msg);

    hlos_resp_msg.response.result = MCM_RESULT_SUCCESS_V01;
    hlos_resp_msg.response.error = MCM_SUCCESS_V01;

    hlos_core_send_response_handler(ret_val,
            hlos_core_hlos_request_data,
            &hlos_resp_msg,
            sizeof(hlos_resp_msg));
}


