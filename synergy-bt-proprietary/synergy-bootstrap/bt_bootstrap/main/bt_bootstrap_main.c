/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "connx_arg_search.h"
#include "bt_bootstrap.h"


#define MAX_BOOT_RETRY_TIME         1

/* Initial UART baudrate. */
#define BT_UART_DEFAULT_BAUDRATE    115200

/* Actual UART baudrate after HCI RESET. */
#define BT_UART_RESET_BAUDRATE      3000000


static uint16_t GetBtChipType(void)
{
    char *parameter, *value;

    if (ConnxArgSearch(NULL, "--bt-chip", &parameter, &value) && (value != NULL))
    {
        if (!strcmp(value, "qca"))
        {
            return BT_CHIP_QCA;
        }
        else if (!strcmp(value, "rome"))
        {
            return BT_CHIP_ROME;
        }
        else if (!strcmp(value, "bluecore"))
        {
            return BT_CHIP_BLUECORE;
        }
        else if (!strcmp(value, "csr8311"))
        {
            return BT_CHIP_CSR8311;
        }
    }

    return BT_CHIP_UNKNOWN;
}

static bool InitRegisterInfo(BtBootstrapRegisterInfo *registerInfo)
{
    char *parameter, *value;
    char *port = NULL;
    uint32_t init_baudrate = BT_UART_DEFAULT_BAUDRATE;
    uint32_t reset_baudrate = BT_UART_RESET_BAUDRATE;
    bool flow_control = false;
    char *firmware_path = NULL;
    char *config_path = NULL;
    uint32_t tmp_val = 0;

    if (!registerInfo)
        return false;

    memset(registerInfo, 0, sizeof(BtBootstrapRegisterInfo));

    if (ConnxArgSearch(NULL, "--bt-port", &parameter, &value))
    {
        if (value == NULL)
        {
            return false;
        }

        port = value;
    }

    if (ConnxArgSearch(NULL, "--bt-baud", &parameter, &value))
    {
        if ((value == NULL) || (sscanf(value, "%d", &tmp_val) != 1))
        {
            return false;
        }

        init_baudrate = tmp_val;
    }

    if (ConnxArgSearch(NULL, "--bt-bs-baud", &parameter, &value))
    {
        if ((value == NULL) || (sscanf(value, "%d", &tmp_val) != 1))
        {
            return false;
        }

        reset_baudrate = tmp_val;
    }

    if (ConnxArgSearch(NULL, "--flow-control", &parameter, &value))
    {
        if ((value == NULL) || (sscanf(value, "%x", &tmp_val) != 1))
        {
            return false;
        }

        flow_control = tmp_val ? true : false;
    }

    if (ConnxArgSearch(NULL, "--fw-path", &parameter, &value))
    {
        if (value == NULL)
        {
            return false;
        }

        firmware_path = value;
    }

    if (ConnxArgSearch(NULL, "--config-path", &parameter, &value))
    {
        if (value == NULL)
        {
            return false;
        }

        config_path = value;
    }

    registerInfo->size           = sizeof(BtBootstrapRegisterInfo);
    registerInfo->chip_type      = GetBtChipType();
    registerInfo->serial_port    = port;
    registerInfo->init_baudrate  = init_baudrate;
    registerInfo->reset_baudrate = reset_baudrate;
    registerInfo->flow_control   = flow_control;
    registerInfo->fw_path        = firmware_path;       /* If NULL, default firmware path "/lib/firmware" is used internally. */
    registerInfo->config_path    = config_path;         /* If NULL, default config path "/etc/bluetooth" is used internally. */

    return true;
}

int main(int argc, char *argv[])
{
    BtBootstrapRegisterInfo registerInfo;
    uint32_t retry_time;
    bool result = false;

    ConnxArgSearchInit((uint32_t) argc, (char **) argv);

    if (!InitRegisterInfo(&registerInfo))
    {
        /* Invalid input parameter. */
        exit(EXIT_FAILURE);
    }

    for (retry_time = 0; retry_time < MAX_BOOT_RETRY_TIME; retry_time++)
    {
        if (QcomBtBootstrap(&registerInfo))
        {
            /* Succeed to boot up BT chip. */
            result = true;
            break;
        }
    }

    if (result)
        exit(EXIT_SUCCESS);
    else
        exit(EXIT_FAILURE);
}