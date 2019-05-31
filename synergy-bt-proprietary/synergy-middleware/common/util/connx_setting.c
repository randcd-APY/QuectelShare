/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "connx_setting.h"
#include "connx_util.h"


#define MAX_LINE_SIZE       CONNX_MAX_PATH_LENGTH

#define MAX_SETTING_SIZE    (MAX_LINE_SIZE >> 1)


typedef struct
{
    ConnxSettingRegisterInfo    registerInfo;   /* Setting's register info. */
    uint8_t                    *buf;            /* Buffer to store data read from file. */
    uint32_t                    bufSize;        /* Buffer size in byte. */
} ConnxSettingInstance;


static bool SI_ValidRegisterInfo(ConnxSettingRegisterInfo *registerInfo);
static bool SI_CreateFile(char *fileName, char *defaultSetting);


static bool SI_ValidRegisterInfo(ConnxSettingRegisterInfo *registerInfo)
{
    return (registerInfo &&
            registerInfo->fileName &&
            registerInfo->defaultSetting &&
            registerInfo->parseCb) ? true : false;
}

static bool SI_CreateFile(char *fileName, char *defaultSetting)
{
    FILE *fp = NULL;
    uint32_t length = 0;
    uint32_t bytesWritten = 0;

    if (!fileName || !defaultSetting)
        return false;

    fp = fopen(fileName, "r");

    if (fp != NULL)
    {
        /* File already exist. */
        fclose(fp);
        return true;
    }

    /* Create new file to store default setting. */
    fp = fopen(fileName, "w");

    if (!fp)
    {
        return false;
    }

    length = strlen(defaultSetting);

    bytesWritten = fwrite(defaultSetting, 1, length, fp);

    if (bytesWritten != length)
    {
        fclose(fp);
        return false;
    }

    fflush(fp);

    fclose(fp);

    return true;
}

ConnxHandle ConnxSettingOpen(ConnxSettingRegisterInfo *registerInfo)
{
    ConnxSettingInstance *inst = NULL;
    ConnxSettingRegisterInfo *ri = NULL;
    bool res = false;

    /* Verify register info. */
    if (!SI_ValidRegisterInfo(registerInfo))
    {
        return NULL;
    }

    do
    {
        inst = (ConnxSettingInstance *) malloc(sizeof(ConnxSettingInstance));

        if (!inst)
        {
            break;
        }

        ri = &inst->registerInfo;

        memcpy(ri, registerInfo, sizeof(ConnxSettingRegisterInfo));

        ri->fileName       = ConnxStrDup(registerInfo->fileName);
        ri->defaultSetting = ConnxStrDup(registerInfo->defaultSetting);

        if (!SI_CreateFile(ri->fileName, ri->defaultSetting))
        {
            break;
        }

        inst->bufSize = MAX_LINE_SIZE;

        inst->buf = (uint8_t *) malloc(inst->bufSize);

        if (!inst->buf)
        {
            break;
        }

        res = true;
    }
    while (0);

    if (!res)
    {
        ConnxSettingClose((ConnxHandle) inst);
        inst = NULL;
    }

    return res ? (ConnxHandle) inst : NULL;
}

void ConnxSettingClose(ConnxHandle handle)
{
    ConnxSettingInstance *inst = (ConnxSettingInstance *) handle;
    ConnxSettingRegisterInfo *registerInfo = NULL;

    if (!inst)
        return;

    registerInfo = &inst->registerInfo;

    if (registerInfo->fileName)
    {
        free(registerInfo->fileName);
        registerInfo->fileName = NULL;
    }

    if (registerInfo->defaultSetting)
    {
        free(registerInfo->defaultSetting);
        registerInfo->defaultSetting = NULL;
    }

    if (inst->buf)
    {
        free(inst->buf);
        inst->buf = NULL;
    }

    free(inst);
}

bool ConnxSettingParse(ConnxHandle handle)
{
    ConnxSettingInstance *inst = (ConnxSettingInstance *) handle;
    ConnxSettingRegisterInfo *registerInfo = NULL;
    char *fileName = NULL;
    FILE *fp = NULL;
    char *line = NULL;
    uint32_t maxLineSize = 0;
    ParseKeyValCb parseCb = NULL;
    char key[MAX_SETTING_SIZE];
    char val[MAX_SETTING_SIZE];

    if (!inst)
        return false;

    registerInfo = &inst->registerInfo;
    fileName     = registerInfo->fileName;
    parseCb      = registerInfo->parseCb;

    if (!fileName || !parseCb)
        return false;

    fp = fopen(fileName, "r");

    if (!fp)
        return false;

    line = (char *) inst->buf;
    maxLineSize = inst->bufSize;

    while (fgets(line, maxLineSize, fp) != NULL)
    {
        if (sscanf(line, CONNX_KEY_VAL_FORMAT, key, val) == CONNX_KEY_VAL_FIELD_COUNT)
        {
            parseCb(registerInfo->context, key, val);
        }
    }

    fclose(fp);

    return true;
}

bool ConnxSettingGetUint32(char *val_str, uint32_t *value)
{
    if (!val_str || !value)
        return false;

    return (sscanf(val_str, "%x", value) == 1) ? true : false;
}