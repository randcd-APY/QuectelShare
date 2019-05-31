/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <sys/stat.h>

#include "bt_bootstrap_util.h"
#include "connx_serial_com.h"
#include "bt_h4.h"


#define DEFAULT_OUTPUT_FILE     BT_BOOTSTRAP_LOG_FILE


static uint8_t Map2TransportType(uint16_t chipType);
static void GenerateBtAddr(BT_ADDR_T *addr);
static bool ReadBtAddrFile(const char *file_name, BT_ADDR_T *addr);
static bool WriteBtAddrFile(BT_ADDR_T *addr, const char *fileName);


static uint8_t Map2TransportType(uint16_t chipType)
{
    switch (chipType)
    {
        /* QCA chip */
        case BT_CHIP_QCA:
        case BT_CHIP_ROME:
            return HCI_TRANSPORT_TYPE_H4;

        /* BlueCore chip */
        case BT_CHIP_BLUECORE:
        case BT_CHIP_CSR8311:
            return HCI_TRANSPORT_TYPE_BCSP;

        default:
            return HCI_TRANSPORT_TYPE_UNKNOWN;
    }
}

static void GenerateBtAddr(BT_ADDR_T *addr)
{
    if (!addr)
        return;

    srand((unsigned) time(NULL));

    /* Generate BT_ADDR in which lap is in random. */
    addr->nap = (uint16_t) (0x0002);
    addr->uap = (uint8_t)  (0x5b);
    addr->lap = (uint24_t) (rand() & BT_ADDR_LAP_MASK);
}

static bool ReadBtAddrFile(const char *file_name, BT_ADDR_T *addr)
{
    FILE *fp = NULL;
    uint32_t nap, uap, lap;

    if (!file_name || !addr)
    {
        return false;
    }

    fp = fopen(file_name, "r");

    if (!fp)
    {
        return false;
    }

    if (fscanf(fp, BT_ADDR_FORMAT, &nap, &uap, &lap) != BT_ADDR_FIELD_COUNT)
    {
        fclose(fp);
        return false;
    }

    addr->nap = (uint16_t)(nap & BT_ADDR_NAP_MASK);
    addr->uap = (uint8_t) (uap & BT_ADDR_UAP_MASK);
    addr->lap = (uint24_t)(lap & BT_ADDR_LAP_MASK);

    fclose(fp);

    return true;
}

static bool WriteBtAddrFile(BT_ADDR_T *addr, const char *fileName)
{
    FILE *fp = NULL;

    if (!addr || !fileName)
    {
        return false;
    }

    fp = fopen(fileName, "w");

    if (!fp)
    {
        return false;
    }

    fprintf(fp, BT_ADDR_FORMAT, addr->nap, addr->uap, addr->lap);

    fflush(fp);

    fclose(fp);

    return true;
}

/* -------------------------------------------------------------------------------- */

bool IsValidRegisterInfo(BtBootstrapRegisterInfo *registerInfo)
{
    return ((registerInfo != NULL) &&
            (registerInfo->size == sizeof(BtBootstrapRegisterInfo)) &&
            (IS_QCA_CHIP(registerInfo->chip_type) || IS_BLUECORE_CHIP(registerInfo->chip_type)) &&
            (registerInfo->serial_port != NULL) &&
            (registerInfo->init_baudrate != 0) &&
            (registerInfo->reset_baudrate != 0)) ? true : false;
}

/* [QTI] Fix KW issue#16794. */
void CpyRegisterInfo(BtBootstrapRegisterInfo *dst, BtBootstrapRegisterInfo *src)
{
    if (!dst || !src)
        return;

    memcpy(dst, src, sizeof(BtBootstrapRegisterInfo));

    dst->serial_port = ConnxStrDup(src->serial_port);

    dst->fw_path = src->fw_path ? ConnxStrDup(src->fw_path) : ConnxStrDup(BT_FIRMWARE_PATH);

    dst->config_path = src->config_path ? ConnxStrDup(src->config_path) : ConnxStrDup(BT_CONFIG_PATH);
}

void FreeRegisterInfo(BtBootstrapRegisterInfo *registerInfo)
{
    if (!registerInfo)
        return;

    if (registerInfo->serial_port)
    {
        free(registerInfo->serial_port);
        registerInfo->serial_port = NULL;
    }

    if (registerInfo->fw_path)
    {
        free(registerInfo->fw_path);
        registerInfo->fw_path = NULL;
    }

    if (registerInfo->config_path)
    {
        free(registerInfo->config_path);
        registerInfo->config_path = NULL;
    }
}

bool ReadBluetoothAddress(const char *path, const char *file_name, BT_ADDR_T *bt_addr)
{
    char *full_file_name;
    bool res = true;

    if (!path || !file_name || !bt_addr)
        return false;

    full_file_name = ConnxCreateFullFileName(path, file_name);

    if (!full_file_name)
        return false;

    do
    {
        res = ReadBtAddrFile(full_file_name, bt_addr);

        if (res)
        {
            /* BT_ADDR is read from the file successfully. */
            break;
        }

        /* Generate new BT_ADDR. */
        GenerateBtAddr(bt_addr);

        res = WriteBtAddrFile(bt_addr, full_file_name);

        if (res)
        {
            /* BT_ADDR newly generated is written into the file successfully. */
            break;
        }

        /* No BT_ADDR is found. */
        res = false;
    }
    while (0);

    free(full_file_name);

    return res;
}

uint32_t FindValidH4Head(const uint8_t *ptr, uint32_t len)
{
    uint32_t offset = 0;

    if (!ptr || !len)
    {
        return 0;
    }

    do
    {
        if (IS_H4_EVT(ptr[offset]))
        {
            return offset;
        }

        offset++;
    }
    while (offset < len);

    return offset;
}

