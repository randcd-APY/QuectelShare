/******************************************************************************
 *
 *  Copyright (C) 2011-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This is the implementation of the API for the advanced audio/video (AV)
 *  subsystem of BTA, Broadcom's Bluetooth application layer for mobile
 *  phones.
 *
 ******************************************************************************/

#include "bt_target.h"
#if defined(BTA_AV_INCLUDED) && (BTA_AV_INCLUDED == TRUE)

#include "bta_api.h"
#include "bta_sys.h"
#include "bta_avk_api.h"
#include "bta_avk_int.h"
#include "bt_common.h"
#include <string.h>
#include "osi/include/allocator.h"

/*****************************************************************************
**  Constants
*****************************************************************************/

static const tBTA_SYS_REG bta_avk_reg =
{
    bta_avk_hdl_event,
    BTA_AvkDisable
};

/*******************************************************************************
**
** Function         BTA_AvkEnable
**
** Description      Enable the advanced audio/video service. When the enable
**                  operation is complete the callback function will be
**                  called with a BTA_AVK_ENABLE_EVT. This function must
**                  be called before other function in the AV API are
**                  called.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkEnable(tBTA_SEC sec_mask, tBTA_AVK_FEAT features, tBTA_AVK_CBACK *p_cback)
{
    tBTA_AVK_API_ENABLE  *p_buf;

    /* register with BTA system manager */
    bta_sys_register(BTA_ID_AVK, &bta_avk_reg);

    if ((p_buf = (tBTA_AVK_API_ENABLE *) osi_malloc(sizeof(tBTA_AVK_API_ENABLE))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_API_ENABLE_EVT;
        p_buf->p_cback  = p_cback;
        p_buf->features = features;
        p_buf->sec_mask = sec_mask;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkDisable
**
** Description      Disable the advanced audio/video service.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkDisable(void)
{
    BT_HDR  *p_buf;

    bta_sys_deregister(BTA_ID_AVK);
    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL)
    {
        p_buf->event = BTA_AVK_API_DISABLE_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkRegister
**
** Description      Register the audio or video service to stack. When the
**                  operation is complete the callback function will be
**                  called with a BTA_AVK_REGISTER_EVT. This function must
**                  be called before AVDT stream is open.
**
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkRegister(tBTA_AVK_CHNL chnl, const char *p_service_name, UINT8 app_id, tBTA_AVK_DATA_CBACK  *p_data_cback,
                    UINT16 service_uuid)
{
    tBTA_AVK_API_REG  *p_buf;


    if ((p_buf = (tBTA_AVK_API_REG *) osi_malloc(sizeof(tBTA_AVK_API_REG))) != NULL)
    {
        p_buf->hdr.layer_specific   = chnl;
        p_buf->hdr.event = BTA_AVK_API_REGISTER_EVT;
        if(p_service_name)
        {
            strlcpy(p_buf->p_service_name, p_service_name, BTA_SERVICE_NAME_LEN);
        }
        else
        {
            p_buf->p_service_name[0] = 0;
        }
        p_buf->app_id = app_id;
        p_buf->p_app_data_cback = p_data_cback;
        p_buf->service_uuid = service_uuid;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkDeregister
**
** Description      Deregister the audio or video service
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkDeregister(tBTA_AVK_HNDL hndl)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL)
    {
        p_buf->layer_specific   = hndl;
        p_buf->event = BTA_AVK_API_DEREGISTER_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkOpen
**
** Description      Opens an advanced audio/video connection to a peer device.
**                  When connection is open callback function is called
**                  with a BTA_AVK_OPEN_EVT.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkOpen(BD_ADDR bd_addr, tBTA_AVK_HNDL handle, BOOLEAN use_rc, tBTA_SEC sec_mask,
                                                                             UINT16 uuid)
{
    tBTA_AVK_API_OPEN  *p_buf;

    if ((p_buf = (tBTA_AVK_API_OPEN *) osi_malloc(sizeof(tBTA_AVK_API_OPEN))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_API_OPEN_EVT;
        p_buf->hdr.layer_specific   = handle;
        bdcpy(p_buf->bd_addr, bd_addr);
        p_buf->use_rc = use_rc;
        p_buf->sec_mask = sec_mask;
        p_buf->switch_res = BTA_AVK_RS_NONE;
        p_buf->uuid = uuid;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkClose
**
** Description      Close the current streams.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkClose(tBTA_AVK_HNDL handle)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL)
    {
        p_buf->event = BTA_AVK_API_CLOSE_EVT;
        p_buf->layer_specific   = handle;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkDisconnect
**
** Description      Close the connection to the address.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkDisconnect(BD_ADDR bd_addr)
{
    tBTA_AVK_API_DISCNT  *p_buf;

    if ((p_buf = (tBTA_AVK_API_DISCNT *) osi_malloc(sizeof(tBTA_AVK_API_DISCNT))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_API_DISCONNECT_EVT;
        bdcpy(p_buf->bd_addr, bd_addr);
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkStart
**
** Description      Start audio/video stream data transfer.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkStart(tBTA_AVK_HNDL handle)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL)
    {
        p_buf->layer_specific   = handle;
        p_buf->event = BTA_AVK_API_START_EVT;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkEnable_Sink
**
** Description      Enable/Disable A2DP Sink..
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkEnable_Sink(int enable)
{
#if (BTA_AV_SINK_INCLUDED == TRUE)
    BT_HDR  *p_buf;
    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL)
    {
        p_buf->event = BTA_AVK_API_SINK_ENABLE_EVT;
        p_buf->layer_specific = enable;
        bta_sys_sendmsg(p_buf);
    }
#else
    return;
#endif
}

/*******************************************************************************
**
** Function         BTA_AvkStop
**
** Description      Stop audio/video stream data transfer.
**                  If suspend is TRUE, this function sends AVDT suspend signal
**                  to the connected peer(s).
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkStop(BOOLEAN suspend, tBTA_AVK_HNDL handle)
{
    tBTA_AVK_API_STOP  *p_buf;

    if ((p_buf = (tBTA_AVK_API_STOP *) osi_malloc(sizeof(tBTA_AVK_API_STOP))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_API_STOP_EVT;
        p_buf->hdr.layer_specific   = handle;
        p_buf->flush   = TRUE;
        p_buf->suspend = suspend;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkEnableMultiCast
**
** Description      Enable/Disable Avdtp MultiCast
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkEnableMultiCast(BOOLEAN state, tBTA_AVK_HNDL handle)
{
    tBTA_AVK_ENABLE_MULTICAST  *p_buf;

    if ((p_buf = (tBTA_AVK_ENABLE_MULTICAST *) osi_malloc(sizeof(tBTA_AVK_ENABLE_MULTICAST))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_ENABLE_MULTICAST_EVT;
        p_buf->hdr.layer_specific   = handle;
        p_buf->is_multicast_enabled = state;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkReconfig
**
** Description      Reconfigure the audio/video stream.
**                  If suspend is TRUE, this function tries the suspend/reconfigure
**                  procedure first.
**                  If suspend is FALSE or when suspend/reconfigure fails,
**                  this function closes and re-opens the AVDT connection.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkReconfig(tBTA_AVK_HNDL hndl, BOOLEAN suspend, UINT8 sep_info_idx,
                    UINT8 *p_codec_info, UINT8 num_protect, UINT8 *p_protect_info)
{
    tBTA_AVK_API_RCFG  *p_buf;

    if ((p_buf = (tBTA_AVK_API_RCFG *) osi_malloc((UINT16) (sizeof(tBTA_AVK_API_RCFG) + num_protect))) != NULL)
    {
        p_buf->hdr.layer_specific   = hndl;
        p_buf->hdr.event    = BTA_AVK_API_RECONFIG_EVT;
        p_buf->num_protect  = num_protect;
        p_buf->suspend      = suspend;
        p_buf->sep_info_idx = sep_info_idx;
        p_buf->p_protect_info = (UINT8 *)(p_buf + 1);
        memcpy(p_buf->codec_info, p_codec_info, AVDT_CODEC_SIZE);
        memcpy(p_buf->p_protect_info, p_protect_info, num_protect);
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkProtectReq
**
** Description      Send a content protection request.  This function can only
**                  be used if AV is enabled with feature BTA_AVK_FEAT_PROTECT.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkProtectReq(tBTA_AVK_HNDL hndl, UINT8 *p_data, UINT16 len)
{
    tBTA_AVK_API_PROTECT_REQ  *p_buf;

    if ((p_buf = (tBTA_AVK_API_PROTECT_REQ *) osi_malloc((UINT16) (sizeof(tBTA_AVK_API_PROTECT_REQ) + len))) != NULL)
    {
        p_buf->hdr.layer_specific   = hndl;
        p_buf->hdr.event = BTA_AVK_API_PROTECT_REQ_EVT;
        p_buf->len       = len;
        if (p_data == NULL)
        {
            p_buf->p_data = NULL;
        }
        else
        {
            p_buf->p_data = (UINT8 *) (p_buf + 1);
            memcpy(p_buf->p_data, p_data, len);
        }
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkProtectRsp
**
** Description      Send a content protection response.  This function must
**                  be called if a BTA_AVK_PROTECT_REQ_EVT is received.
**                  This function can only be used if AV is enabled with
**                  feature BTA_AVK_FEAT_PROTECT.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkProtectRsp(tBTA_AVK_HNDL hndl, UINT8 error_code, UINT8 *p_data, UINT16 len)
{
    tBTA_AVK_API_PROTECT_RSP  *p_buf;

    if ((p_buf = (tBTA_AVK_API_PROTECT_RSP *) osi_malloc((UINT16) (sizeof(tBTA_AVK_API_PROTECT_RSP) + len))) != NULL)
    {
        p_buf->hdr.layer_specific   = hndl;
        p_buf->hdr.event    = BTA_AVK_API_PROTECT_RSP_EVT;
        p_buf->len          = len;
        p_buf->error_code   = error_code;
        if (p_data == NULL)
        {
            p_buf->p_data = NULL;
        }
        else
        {
            p_buf->p_data = (UINT8 *) (p_buf + 1);
            memcpy(p_buf->p_data, p_data, len);
        }
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkRemoteCmd
**
** Description      Send a remote control command.  This function can only
**                  be used if AV is enabled with feature BTA_AVK_FEAT_RCCT.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkRemoteCmd(UINT8 rc_handle, UINT8 label, tBTA_AVK_RC rc_id, tBTA_AVK_STATE key_state)
{
    tBTA_AVK_API_REMOTE_CMD  *p_buf;

    if ((p_buf = (tBTA_AVK_API_REMOTE_CMD *) osi_malloc(sizeof(tBTA_AVK_API_REMOTE_CMD))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_API_REMOTE_CMD_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        p_buf->msg.op_id = rc_id;
        p_buf->msg.state = key_state;
        p_buf->msg.p_pass_data = NULL;
        p_buf->msg.pass_len = 0;
        p_buf->label = label;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkVendorCmd
**
** Description      Send a vendor dependent remote control command.  This
**                  function can only be used if AV is enabled with feature
**                  BTA_AVK_FEAT_VENDOR.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkVendorCmd(UINT8 rc_handle, UINT8 label, tBTA_AVK_CODE cmd_code, UINT8 *p_data, UINT16 len)
{
    tBTA_AVK_API_VENDOR  *p_buf;

    if ((p_buf = (tBTA_AVK_API_VENDOR *) osi_malloc((UINT16) (sizeof(tBTA_AVK_API_VENDOR) + len))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_API_VENDOR_CMD_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        p_buf->msg.hdr.ctype = cmd_code;
        p_buf->msg.hdr.subunit_type = AVRC_SUB_PANEL;
        p_buf->msg.hdr.subunit_id = 0;
        p_buf->msg.company_id = p_bta_avk_cfg->company_id;
        p_buf->label = label;
        p_buf->msg.vendor_len = len;
        if (p_data == NULL)
        {
            p_buf->msg.p_vendor_data = NULL;
        }
        else
        {
            p_buf->msg.p_vendor_data = (UINT8 *) (p_buf + 1);
            memcpy(p_buf->msg.p_vendor_data, p_data, len);
        }
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkVendorRsp
**
** Description      Send a vendor dependent remote control response.
**                  This function must be called if a BTA_AVK_VENDOR_CMD_EVT
**                  is received. This function can only be used if AV is
**                  enabled with feature BTA_AVK_FEAT_VENDOR.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkVendorRsp(UINT8 rc_handle, UINT8 label, tBTA_AVK_CODE rsp_code, UINT8 *p_data, UINT16 len, UINT32 company_id)
{
    tBTA_AVK_API_VENDOR  *p_buf;

    if ((p_buf = (tBTA_AVK_API_VENDOR *) osi_malloc((UINT16) (sizeof(tBTA_AVK_API_VENDOR) + len))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_API_VENDOR_RSP_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        p_buf->msg.hdr.ctype = rsp_code;
        p_buf->msg.hdr.subunit_type = AVRC_SUB_PANEL;
        p_buf->msg.hdr.subunit_id = 0;
        if(company_id)
            p_buf->msg.company_id = company_id;
        else
            p_buf->msg.company_id = p_bta_avk_cfg->company_id;
        p_buf->label = label;
        p_buf->msg.vendor_len = len;
        if (p_data == NULL)
        {
            p_buf->msg.p_vendor_data = NULL;
        }
        else
        {
            p_buf->msg.p_vendor_data = (UINT8 *) (p_buf + 1);
            memcpy(p_buf->msg.p_vendor_data, p_data, len);
        }
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkOpenRc
**
** Description      Open an AVRCP connection toward the device with the
**                  specified handle
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkOpenRc(tBTA_AVK_HNDL handle)
{
    tBTA_AVK_API_OPEN_RC  *p_buf;

    if ((p_buf = (tBTA_AVK_API_OPEN_RC *) osi_malloc(sizeof(tBTA_AVK_API_OPEN_RC))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_API_RC_OPEN_EVT;
        p_buf->hdr.layer_specific   = handle;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkCloseRc
**
** Description      Close an AVRCP connection
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkCloseRc(UINT8 rc_handle)
{
    tBTA_AVK_API_CLOSE_RC  *p_buf;

    if ((p_buf = (tBTA_AVK_API_CLOSE_RC *) osi_malloc(sizeof(tBTA_AVK_API_CLOSE_RC))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_API_RC_CLOSE_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkMetaRsp
**
** Description      Send a Metadata/Advanced Control response. The message contained
**                  in p_pkt can be composed with AVRC utility functions.
**                  This function can only be used if AV is enabled with feature
**                  BTA_AVK_FEAT_METADATA.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkMetaRsp(UINT8 rc_handle, UINT8 label, tBTA_AVK_CODE rsp_code,
                               BT_HDR *p_pkt)
{
    tBTA_AVK_API_META_RSP  *p_buf;

    if ((p_buf = (tBTA_AVK_API_META_RSP *) osi_malloc((UINT16) (sizeof(tBTA_AVK_API_META_RSP)))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_API_META_RSP_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        p_buf->rsp_code = rsp_code;
        p_buf->p_pkt = p_pkt;
        p_buf->is_rsp = TRUE;
        p_buf->label = label;

        bta_sys_sendmsg(p_buf);
    } else if (p_pkt) {
        osi_free(p_pkt);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkMetaCmd
**
** Description      Send a Metadata/Advanced Control command. The message contained
**                  in p_pkt can be composed with AVRC utility functions.
**                  This function can only be used if AV is enabled with feature
**                  BTA_AVK_FEAT_METADATA.
**                  This message is sent only when the peer supports the TG role.
*8                  The only command makes sense right now is the absolute volume command.
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkMetaCmd(UINT8 rc_handle, UINT8 label, tBTA_AVK_CMD cmd_code, BT_HDR *p_pkt)
{
    tBTA_AVK_API_META_RSP  *p_buf;

    if ((p_buf = (tBTA_AVK_API_META_RSP *) osi_malloc((UINT16) (sizeof(tBTA_AVK_API_META_RSP)))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_API_META_RSP_EVT;
        p_buf->hdr.layer_specific   = rc_handle;
        p_buf->p_pkt = p_pkt;
        p_buf->rsp_code = cmd_code;
        p_buf->is_rsp = FALSE;
        p_buf->label = label;

        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         BTA_AvkIsBrowsingSupported
**
** Description      Check to see if browsing is supported by local device.
**                  This API does not result in any message being posted, it just
**                  checks the local supported features for browsing support and
**                  returns TRUE or FALSE.
** Returns          TRUE/FALSE based on browse support in local device
**
*******************************************************************************/
BOOLEAN BTA_AvkIsBrowsingSupported (void)
{
    if (p_bta_avk_cfg->avrc_ct_cat & AVRC_SUPF_CT_BROWSE)
    {
        return TRUE;
    }
    return FALSE;
}


/*******************************************************************************
**
** Function         BTA_AvkUpdateCodecSupport
**
** Description      Update Avdtp Codec Support
**
** Returns          void
**
*******************************************************************************/
void BTA_AvkUpdateCodecSupport(UINT8 *p_codec_type_list, UINT8 *p_vnd_list, UINT8 *p_codec_id_list,
                                        UINT8 codec_info[][AVDT_CODEC_SIZE], UINT8 num_codec_configs)
{
    tBTA_AVK_UPDATE_SUPP_CODECS  *p_buf;

    if (p_codec_type_list == NULL || p_vnd_list == NULL || p_codec_id_list == NULL)
        return;

    if ((p_buf = (tBTA_AVK_UPDATE_SUPP_CODECS *) osi_calloc((UINT8) (sizeof
        (tBTA_AVK_UPDATE_SUPP_CODECS)))) != NULL)
    {
        p_buf->hdr.event = BTA_AVK_UPDATE_SUPP_CODECS;
        p_buf->num_codec_configs = num_codec_configs;
        memcpy(p_buf->codec_type, p_codec_type_list, num_codec_configs);
        memcpy(p_buf->vnd_id, p_vnd_list, num_codec_configs);
        memcpy(p_buf->codec_id, p_codec_id_list, num_codec_configs);
        memcpy(p_buf->codec_info, codec_info, num_codec_configs * AVDT_CODEC_SIZE);
        bta_sys_sendmsg(p_buf);
    }
}
#endif /* BTA_AV_INCLUDED */
