/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_SETTING_H_
#define _CONNX_SETTING_H_

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif


#define CONNX_KEY_VAL_FORMAT            "%s = %s \n\n"

#define CONNX_KEY_VAL_FIELD_COUNT       2

#define CONNX_COMMENT                   "# "
#define CONNX_NEW_LINE                  "\n"


typedef void (* ParseKeyValCb)(ConnxContext context, char *key, char *val);

typedef struct
{
    char           *fileName;           /* File name of the setting. */
    char           *defaultSetting;     /* Default setting. */
    ConnxContext    context;            /* Context to parse setting. */
    ParseKeyValCb   parseCb;            /* Callback to parse key-value. */
} ConnxSettingRegisterInfo;


ConnxHandle ConnxSettingOpen(ConnxSettingRegisterInfo *registerInfo);

void ConnxSettingClose(ConnxHandle handle);

bool ConnxSettingParse(ConnxHandle handle);

bool ConnxSettingGetUint32(char *val_str, uint32_t *value);


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_SETTING_H_ */