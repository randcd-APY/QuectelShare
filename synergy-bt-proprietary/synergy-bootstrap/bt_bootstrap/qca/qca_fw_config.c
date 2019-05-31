/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #5 $
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "connx_setting.h"
#include "boot_qca.h"


#define MAX_QCA_FW_CONFIG_SIZE              8192

#define QCA_FW_CONFIG_FORMAT                "%s = %x \n\n"
/* Tag 36 - Power Configuration: Tx output power step control (12 bytes).  */
#define QCA_POWER_CFG_FORMAT                "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x"
/* Tag 83 - LE Tx Power Control: LE Tx output power configuration (8 bytes). */
#define QCA_LE_TX_POWER_CTRL_FORMAT         "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x"
/* Tag 28 - Xtal Fine Tuning for v3.1: offset CDACIN/CDACOUT from OTP (bytes 0x03 and 0x05) up to +- 15 values away (20 bytes).  */
#define QCA_XTAL_FINE_TUNING_FORMAT         "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x"
/* Tag 82 - Xtal Fine Tuning for v3.2: offset CDACIN/CDACOUT from OTP (bytes 0x03 and 0x05) up to +- 15 values away (16 bytes).  */
#define QCA_XTAL_FINE_TUNING2_FORMAT        "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x"

#define QCA_FW_CONFIG_KEY_PCM               "PCM"
#define QCA_FW_CONFIG_KEY_INVERT_BCLK       "INVERT_BCLK"
#define QCA_FW_CONFIG_KEY_NBS_BCLK          "NBS_BCLK"
#define QCA_FW_CONFIG_KEY_WBS_BCLK          "WBS_BCLK"
#define QCA_FW_CONFIG_KEY_NBS_SCLK          "NBS_SCLK"
#define QCA_FW_CONFIG_KEY_WBS_SCLK          "WBS_SCLK"
#define QCA_FW_CONFIG_KEY_IBS               "IBS"
#define QCA_FW_CONFIG_KEY_DEEP_SLEEP        "DEEP_SLEEP"
#define QCA_FW_CONFIG_KEY_PCM_LOOPBACK      "PCM_LOOPBACK"
#define QCA_FW_CONFIG_KEY_FW_LOG            "FW_LOG"
#define QCA_FW_CONFIG_KEY_POWER_CFG         "POWER_CONFIGURATION"
#define QCA_FW_CONFIG_KEY_LE_TX_POWER_CTRL  "LE_TX_POWER_CONTROL"
#define QCA_FW_CONFIG_KEY_XTAL_FINE_TUNING  "XTAL_FINE_TUNING"
#define QCA_FW_CONFIG_KEY_XTAL_FINE_TUNING2 "XTAL_FINE_TUNING2"

#define PCM_SETTING_COMMENT                 CONNX_COMMENT "0: PCM master, 1: PCM slave" CONNX_NEW_LINE
#define INVERT_BCLK_SETTING_COMMENT         CONNX_COMMENT "0: Normal bit clock, 1: Invert bit clock (Tx data in falling edge; Rx data in rising edge)" CONNX_NEW_LINE
#define NBS_BCLK_SETTING_COMMENT            CONNX_COMMENT "Decimal value for NBS bit clock, for example 256000 for 256khz bit clock" CONNX_NEW_LINE
#define WBS_BCLK_SETTING_COMMENT            CONNX_COMMENT "Decimal value for WBS bit clock, for example 256000 for 256khz bit clock" CONNX_NEW_LINE
#define NBS_SCLK_SETTING_COMMENT            CONNX_COMMENT "Decimal value for NBS sync clock, for example 8000 for 8khz sync clock" CONNX_NEW_LINE
#define WBS_SCLK_SETTING_COMMENT            CONNX_COMMENT "Decimal value for WBS sync clock, for example 16000 for 16khz sync clock" CONNX_NEW_LINE
#define IBS_SETTING_COMMENT                 CONNX_COMMENT "0: Disable S/W IBS, 1: Enable" CONNX_NEW_LINE
#define DEEP_SLEEP_SETTING_COMMENT          CONNX_COMMENT "0: Disable deep sleep, 1: Enable" CONNX_NEW_LINE
#define FW_LOG_SETTING_COMMENT              CONNX_COMMENT "0: Disable FW LOG, 1: Enable" CONNX_NEW_LINE
#define POWER_CFG_SETTING_COMMENT           CONNX_COMMENT "Tag 36 - Power Configuration: Tx output power step control (12 bytes)" CONNX_NEW_LINE
#define LE_TX_POWER_CTRL_SETTING_COMMENT    CONNX_COMMENT "Tag 83 - LE Tx Power Control: LE Tx output power configuration (3 bytes)" CONNX_NEW_LINE
#define XTAL_FINE_TUNING_COMMENT            CONNX_COMMENT "Tag 28 - Xtal Fine Tuning for v3.1: offset CDACIN/CDACOUT from OTP (bytes 0x03 and 0x05) up to +-5 values away (20 bytes)" CONNX_NEW_LINE
#define XTAL_FINE_TUNING2_COMMENT           CONNX_COMMENT "Tag 82 - Xtal Fine Tuning for v3.2: offset CDACIN/CDACOUT from OTP (bytes 0x03 and 0x05) up to +-15 values away (16 bytes)" CONNX_NEW_LINE


static void QCA_ParseFwConfig(ConnxContext context, char *key, char *val)
{
    QcaExtraConfig *extra_config = (QcaExtraConfig *)context;
    uint32_t tmp_val = 0;

    if (!context || !key || !val)
        return;

    if (!strcmp(key, QCA_FW_CONFIG_KEY_PCM))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            extra_config->valid_pcm = true;
            extra_config->pcm_role  = (uint8_t)(tmp_val & 0xFF);
        }
        else
        {
            extra_config->valid_pcm = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_INVERT_BCLK))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            extra_config->valid_invert_bclk = true;
            extra_config->invert_bclk       = tmp_val ? true : false;
        }
        else
        {
            extra_config->valid_invert_bclk = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_NBS_BCLK))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            extra_config->valid_nbs_bclk = true;
            extra_config->nbs_bclk       = tmp_val;
        }
        else
        {
            extra_config->valid_nbs_bclk = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_WBS_BCLK))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            extra_config->valid_wbs_bclk = true;
            extra_config->wbs_bclk       = tmp_val;
        }
        else
        {
            extra_config->valid_wbs_bclk = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_NBS_SCLK))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            extra_config->valid_nbs_sclk = true;
            extra_config->nbs_sclk       = tmp_val;
        }
        else
        {
            extra_config->valid_nbs_sclk = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_WBS_SCLK))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            extra_config->valid_wbs_sclk = true;
            extra_config->wbs_sclk       = tmp_val;
        }
        else
        {
            extra_config->valid_wbs_sclk = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_IBS))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            extra_config->valid_ibs  = true;
            extra_config->enable_ibs = tmp_val ? true : false;
        }
        else
        {
            extra_config->valid_ibs = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_DEEP_SLEEP))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            extra_config->valid_deep_sleep  = true;
            extra_config->enable_deep_sleep = tmp_val ? true : false;
        }
        else
        {
            extra_config->valid_deep_sleep = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_PCM_LOOPBACK))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            extra_config->valid_pcm_lp  = true;
            extra_config->enable_pcm_lp = tmp_val ? true : false;
        }
        else
        {
            extra_config->valid_pcm_lp = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_FW_LOG))
    {
        if (ConnxSettingGetUint32(val, &tmp_val))
        {
            extra_config->valid_fw_log  = true;
            extra_config->enable_fw_log = tmp_val ? true : false;
        }
        else
        {
            extra_config->valid_fw_log = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_POWER_CFG))
    {
        uint32_t tmp_power_cfg[NVM_TAG_NUM_36_LENGTH];

        /* E.g. 'FF,03,08,09,09,09,00,00,09,07,01,00' */
        if (sscanf(val, QCA_POWER_CFG_FORMAT, &tmp_power_cfg[0], &tmp_power_cfg[1],
            &tmp_power_cfg[2], &tmp_power_cfg[3], &tmp_power_cfg[4], &tmp_power_cfg[5],
            &tmp_power_cfg[6], &tmp_power_cfg[7], &tmp_power_cfg[8], &tmp_power_cfg[9],
            &tmp_power_cfg[10], &tmp_power_cfg[11]) == NVM_TAG_NUM_36_LENGTH)
        {
            uint32_t index = 0;

            extra_config->valid_power_cfg = true;

            for (index = 0; index < NVM_TAG_NUM_36_LENGTH; index++)
            {
                extra_config->power_cfg[index] = (uint8_t)(tmp_power_cfg[index] & 0xFF);
            }
        }
        else
        {
            extra_config->valid_power_cfg = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_LE_TX_POWER_CTRL))
    {
        uint32_t tmp_le_tx_power_ctrl[NVM_TAG_NUM_83_LENGTH];

        /* E.g. '07,07,07,00,01,03,05,07' */
        if (sscanf(val, QCA_LE_TX_POWER_CTRL_FORMAT, &tmp_le_tx_power_ctrl[0],
            &tmp_le_tx_power_ctrl[1], &tmp_le_tx_power_ctrl[2], &tmp_le_tx_power_ctrl[3],
            &tmp_le_tx_power_ctrl[4], &tmp_le_tx_power_ctrl[5], &tmp_le_tx_power_ctrl[6],
            &tmp_le_tx_power_ctrl[7]) == NVM_TAG_NUM_83_LENGTH)
        {
            uint32_t index = 0;

            extra_config->valid_le_tx_power_ctrl = true;

            for (index = 0; index < NVM_TAG_NUM_83_LENGTH; index++)
            {
                extra_config->le_tx_power_ctrl[index] = (uint8_t)(tmp_le_tx_power_ctrl[index] & 0xFF);
            }
        }
        else
        {
            extra_config->valid_le_tx_power_ctrl = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_XTAL_FINE_TUNING))
    {
        uint32_t xtal_fine_tuning[NVM_TAG_NUM_28_LENGTH];

        /* E.g. '00,10,00,00,2C,01,02,08,14,F4,00,00,33,F4,00,00,00,00,00,00' */
        if (sscanf(val, QCA_XTAL_FINE_TUNING_FORMAT, &xtal_fine_tuning[0], &xtal_fine_tuning[1],
            &xtal_fine_tuning[2], &xtal_fine_tuning[3], &xtal_fine_tuning[4], &xtal_fine_tuning[5],
            &xtal_fine_tuning[6], &xtal_fine_tuning[7], &xtal_fine_tuning[8], &xtal_fine_tuning[9],
            &xtal_fine_tuning[10], &xtal_fine_tuning[11], &xtal_fine_tuning[12], &xtal_fine_tuning[13],
            &xtal_fine_tuning[14], &xtal_fine_tuning[15], &xtal_fine_tuning[16], &xtal_fine_tuning[17],
            &xtal_fine_tuning[18], &xtal_fine_tuning[19]) == NVM_TAG_NUM_28_LENGTH)
        {
            uint32_t index = 0;

            extra_config->valid_xtal_fine_tuning = true;

            for (index = 0; index < NVM_TAG_NUM_28_LENGTH; index++)
            {
                extra_config->xtal_fine_tuning[index] = (uint8_t)(xtal_fine_tuning[index] & 0xFF);
            }
        }
        else
        {
            extra_config->valid_xtal_fine_tuning = false;
        }
    }
    else if (!strcmp(key, QCA_FW_CONFIG_KEY_XTAL_FINE_TUNING2))
    {
        uint32_t xtal_fine_tuning2[NVM_TAG_NUM_82_LENGTH];

        /* E.g. '12,12,00,80,00,00,00,00,00,00,00,00,00,00,00,00' */
        if (sscanf(val, QCA_XTAL_FINE_TUNING2_FORMAT, &xtal_fine_tuning2[0], &xtal_fine_tuning2[1],
            &xtal_fine_tuning2[2], &xtal_fine_tuning2[3], &xtal_fine_tuning2[4], &xtal_fine_tuning2[5],
            &xtal_fine_tuning2[6], &xtal_fine_tuning2[7], &xtal_fine_tuning2[8], &xtal_fine_tuning2[9],
            &xtal_fine_tuning2[10], &xtal_fine_tuning2[11], &xtal_fine_tuning2[12], &xtal_fine_tuning2[13],
            &xtal_fine_tuning2[14], &xtal_fine_tuning2[15]) == NVM_TAG_NUM_82_LENGTH)
        {
            uint32_t index = 0;

            extra_config->valid_xtal_fine_tuning2 = true;

            for (index = 0; index < NVM_TAG_NUM_82_LENGTH; index++)
            {
                extra_config->xtal_fine_tuning2[index] = (uint8_t)(xtal_fine_tuning2[index] & 0xFF);
            }
        }
        else
        {
            extra_config->valid_xtal_fine_tuning2 = false;
        }
    }
    else
    {
        /* Unknown property. */
    }
}

static void QCA_GetDefaultExtraConfig(QcaExtraConfig *extra_config)
{
    if (!extra_config)
        return;

    /* Ignore setting BT_ADDR. */

    extra_config->valid_pcm         = true;
    extra_config->pcm_role          = FW_CONF_PCM_MASTER;   /* Configure PCM as master. */

    extra_config->valid_invert_bclk = true;
    extra_config->invert_bclk       = false;    /* Normal bit clock. */

    extra_config->valid_nbs_bclk    = false;
    extra_config->nbs_bclk          = FW_CONF_PCM_BCLK_256K;    /* 256KHz bit clock. */    

    extra_config->valid_wbs_bclk    = false;
    extra_config->wbs_bclk          = FW_CONF_PCM_BCLK_256K;    /* 256KHz bit clock. */    

    extra_config->valid_nbs_sclk    = false;
    extra_config->nbs_sclk          = FW_CONF_PCM_SYNC_CLOCK_8K;    /* 8KHz sync clock. */ 

    extra_config->valid_wbs_sclk    = false;
    extra_config->wbs_sclk          = FW_CONF_PCM_SYNC_CLOCK_16K;   /* 16KHz sync clock. */ 
    
    extra_config->valid_ibs         = true;
    extra_config->enable_ibs        = false;    /* Disable IBS. */

    extra_config->valid_deep_sleep  = true;
    extra_config->enable_deep_sleep = false;    /* Disable deep sleep. */

    extra_config->valid_pcm_lp      = false;
    extra_config->enable_pcm_lp     = false;    /* Disable pcm loopback. */

    extra_config->valid_fw_log      = false;
    extra_config->enable_fw_log     = false;

    extra_config->valid_power_cfg         = false;
    extra_config->valid_le_tx_power_ctrl  = false;
    extra_config->valid_xtal_fine_tuning  = false;
    extra_config->valid_xtal_fine_tuning2 = false;
}

static char *QCA_CreateExtraConfigString(QcaExtraConfig *extra_config)
{
    char *total_str = NULL;
    char line[CONNX_MAX_PATH_LENGTH];

    if (!extra_config)
        return NULL;

    total_str = (char *)malloc(MAX_QCA_FW_CONFIG_SIZE);

    if (!total_str)
        return NULL;

    memset(total_str, 0, MAX_QCA_FW_CONFIG_SIZE);

    /* Add PCM setting. */
    ConnxStrLCat(total_str, PCM_SETTING_COMMENT, MAX_QCA_FW_CONFIG_SIZE);

    if (extra_config->valid_pcm)
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_PCM, extra_config->pcm_role);
    }
    else
    {
        snprintf(line, sizeof(line), CONNX_COMMENT QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_PCM, FW_CONF_PCM_MASTER);
    }

    ConnxStrLCat(total_str, line, MAX_QCA_FW_CONFIG_SIZE);

    /* Add invert BCLK setting. */
    ConnxStrLCat(total_str, INVERT_BCLK_SETTING_COMMENT, MAX_QCA_FW_CONFIG_SIZE);

    if (extra_config->valid_invert_bclk)
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_INVERT_BCLK, extra_config->invert_bclk);
    }
    else
    {
        snprintf(line, sizeof(line), CONNX_COMMENT QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_INVERT_BCLK, false);
    }

    /* Add NBS BCLK value setting. */
    ConnxStrLCat(total_str, NBS_BCLK_SETTING_COMMENT, MAX_QCA_FW_CONFIG_SIZE);

    if (extra_config->valid_nbs_bclk)
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_NBS_BCLK, extra_config->nbs_bclk);
    }
    else
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_NBS_BCLK, FW_CONF_PCM_BCLK_256K);
    }

    ConnxStrLCat(total_str, line, MAX_QCA_FW_CONFIG_SIZE);

    /* Add WBS BCLK value setting. */
    ConnxStrLCat(total_str, WBS_BCLK_SETTING_COMMENT, MAX_QCA_FW_CONFIG_SIZE);

    if (extra_config->valid_nbs_bclk)
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_WBS_BCLK, extra_config->wbs_bclk);
    }
    else
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_WBS_BCLK, FW_CONF_PCM_BCLK_256K);
    }

    ConnxStrLCat(total_str, line, MAX_QCA_FW_CONFIG_SIZE);

    /* Add NBS SCLK value setting. */
    ConnxStrLCat(total_str, NBS_SCLK_SETTING_COMMENT, MAX_QCA_FW_CONFIG_SIZE);

    if (extra_config->valid_nbs_sclk)
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_NBS_SCLK, extra_config->nbs_sclk);
    }
    else
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_NBS_SCLK, FW_CONF_PCM_SYNC_CLOCK_8K);
    } 

    ConnxStrLCat(total_str, line, MAX_QCA_FW_CONFIG_SIZE);

    /* Add WBS SCLK value setting. */
    ConnxStrLCat(total_str, WBS_SCLK_SETTING_COMMENT, MAX_QCA_FW_CONFIG_SIZE);

    if (extra_config->valid_wbs_sclk)
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_WBS_SCLK, extra_config->wbs_sclk);
    }
    else
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_WBS_SCLK, FW_CONF_PCM_SYNC_CLOCK_16K);
    }

    ConnxStrLCat(total_str, line, MAX_QCA_FW_CONFIG_SIZE);

    /* Add S/W IBS setting. */
    ConnxStrLCat(total_str, IBS_SETTING_COMMENT, MAX_QCA_FW_CONFIG_SIZE);

    if (extra_config->valid_ibs)
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_IBS, extra_config->enable_ibs);
    }
    else
    {
        snprintf(line, sizeof(line), CONNX_COMMENT QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_IBS, 0);
    }

    ConnxStrLCat(total_str, line, MAX_QCA_FW_CONFIG_SIZE);

    /* Add deep sleep setting. */
    ConnxStrLCat(total_str, DEEP_SLEEP_SETTING_COMMENT, MAX_QCA_FW_CONFIG_SIZE);

    if (extra_config->valid_deep_sleep)
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_DEEP_SLEEP, extra_config->enable_deep_sleep);
    }
    else
    {
        snprintf(line, sizeof(line), CONNX_COMMENT QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_DEEP_SLEEP, 0);
    }

    ConnxStrLCat(total_str, line, MAX_QCA_FW_CONFIG_SIZE);

    /* Add core dump setting. */
    ConnxStrLCat(total_str, FW_LOG_SETTING_COMMENT, MAX_QCA_FW_CONFIG_SIZE);

    if (extra_config->valid_fw_log)
    {
        snprintf(line, sizeof(line), QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_FW_LOG, extra_config->enable_fw_log);
    }
    else
    {
        snprintf(line, sizeof(line), CONNX_COMMENT QCA_FW_CONFIG_FORMAT, QCA_FW_CONFIG_KEY_FW_LOG, 0);
    }

    ConnxStrLCat(total_str, line, MAX_QCA_FW_CONFIG_SIZE);

    return total_str;
}

static bool QCA_ReadFirmwareConfig(char *file_name, QcaExtraConfig *extra_config)
{
    ConnxSettingRegisterInfo registerInfo;
    ConnxSettingRegisterInfo *ri = &registerInfo;
    ConnxHandle handle = NULL;
    QcaExtraConfig defaultExtraConfig;
    char *defaultSetting = NULL;

    if (!file_name || !extra_config)
        return false;

    QCA_GetDefaultExtraConfig(&defaultExtraConfig);

    defaultSetting = QCA_CreateExtraConfigString(&defaultExtraConfig);

    ri->fileName       = file_name;
    ri->defaultSetting = defaultSetting;
    ri->context        = extra_config;
    ri->parseCb        = QCA_ParseFwConfig;

    handle = ConnxSettingOpen(ri);

    if (!handle)
    {
        free(defaultSetting);
        return false;
    }

    ConnxSettingParse(handle);

    ConnxSettingClose(handle);

    free(defaultSetting);

    return true;
}

bool QCA_GetFirmwareConfig(char *config_path, char *file_name, QcaExtraConfig *extra_config)
{
    char *full_file_name = NULL;

    if (!config_path || !file_name || !extra_config)
        return false;

    QCA_GetDefaultExtraConfig(extra_config);

    full_file_name = ConnxCreateFullFileName(config_path, file_name);

    QCA_ReadFirmwareConfig(full_file_name, extra_config);

    free(full_file_name);

    return true;
}
