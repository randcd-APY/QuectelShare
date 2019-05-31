#ifndef CSR_BT_UI_STRINGS_H__
#define CSR_BT_UI_STRINGS_H__
/****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/
#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_util.h"
#include "csr_unicode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TECH_BT CsrUtf8StrDup((const CsrUtf8String *) "BT")
#define TECH_BTLE CsrUtf8StrDup((const CsrUtf8String *)"BT-LE")
#define CONVERT_TEXT_STRING_2_UCS2(_x)  CsrUtf82Utf16String((CsrUint8 *)_x)

#define TEXT_THREE_NEW_LINE_STRING              "\n\n\n"
#define TEXT_TWO_NEW_LINE_STRING                "\n\n"
#define TEXT_NEW_LINE_STRING                    "\n"
#define TEXT_SELECT_STRING                      "Select"
#define TEXT_OPTIONS_STRING                     "Options"
#define TEXT_YES_STRING                         "Yes"
#define TEXT_NO_STRING                          "No"
#define TEXT_SUCCESS_STRING                     "Success"
#define TEXT_FAILED_STRING                      "Failed"
#define TEXT_OK_STRING                          "Ok"
#define TEXT_SEARCH_STRING                      "Search"
#define TEXT_PROXIMITY_SEARCH_STRING            "Proximity Search"
#define TEXT_SEARCHING_STRING                   "Searching"
#define TEXT_FOUND_DEVICES_STRING               "Found Devices"
#define TEXT_START_STRING                       "Start"
#define TEXT_PLEASE_WAIT_STRING                 "Please Wait"
#define TEXT_CANCEL_STRING                      "Cancel"
#define TEXT_ABORT_STRING                       "Abort"
#define TEXT_EDIT_STRING                        "Edit"
#define TEXT_STOP_STRING                        "Stop"

/* Common OBEX text */
#define TEXT_OBEX_AUTH_STRING                   "OBEX Authentication"
#define TEXT_OBEX_PASSKEY_STRING                "Insert passkey:"
#define TEXT_OBEX_USERID_STRING                 "Insert user id:"

/* UI text defines                          */
#define TEXT_THREE_NEW_LINE_UCS2            CONVERT_TEXT_STRING_2_UCS2(TEXT_THREE_NEW_LINE_STRING)
#define TEXT_TWO_NEW_LINE_UCS2              CONVERT_TEXT_STRING_2_UCS2(TEXT_TWO_NEW_LINE_STRING)
#define TEXT_NEW_LINE_UCS2                  CONVERT_TEXT_STRING_2_UCS2(TEXT_NEW_LINE_STRING)
#define TEXT_SELECT_UCS2                    CONVERT_TEXT_STRING_2_UCS2(TEXT_SELECT_STRING)
#define TEXT_OPTIONS_UCS2                   CONVERT_TEXT_STRING_2_UCS2(TEXT_OPTIONS_STRING)
#define TEXT_YES_UCS2                       CONVERT_TEXT_STRING_2_UCS2(TEXT_YES_STRING)
#define TEXT_NO_UCS2                        CONVERT_TEXT_STRING_2_UCS2(TEXT_NO_STRING)
#define TEXT_OK_UCS2                        CONVERT_TEXT_STRING_2_UCS2(TEXT_OK_STRING)
#define TEXT_SUCCESS_UCS2                   CONVERT_TEXT_STRING_2_UCS2(TEXT_SUCCESS_STRING)
#define TEXT_FAILED_UCS2                    CONVERT_TEXT_STRING_2_UCS2(TEXT_FAILED_STRING)
#define TEXT_START_UCS2                     CONVERT_TEXT_STRING_2_UCS2(TEXT_START_STRING)
#define TEXT_SEARCH_UCS2                    CONVERT_TEXT_STRING_2_UCS2(TEXT_SEARCH_STRING)
#define TEXT_PROXIMITY_SEARCH_UCS2          CONVERT_TEXT_STRING_2_UCS2(TEXT_PROXIMITY_SEARCH_STRING)
#define TEXT_SEARCHING_UCS2                 CONVERT_TEXT_STRING_2_UCS2(TEXT_SEARCHING_STRING)
#define TEXT_FOUND_DEVICES_UCS2             CONVERT_TEXT_STRING_2_UCS2(TEXT_FOUND_DEVICES_STRING)
#define TEXT_PLEASE_WAIT_UCS2               CONVERT_TEXT_STRING_2_UCS2(TEXT_PLEASE_WAIT_STRING)
#define TEXT_CANCEL_UCS2                    CONVERT_TEXT_STRING_2_UCS2(TEXT_CANCEL_STRING)
#define TEXT_ABORT_UCS2                     CONVERT_TEXT_STRING_2_UCS2(TEXT_ABORT_STRING)
#define TEXT_EDIT_UCS2                      CONVERT_TEXT_STRING_2_UCS2(TEXT_EDIT_STRING)
#define TEXT_STOP_UCS2                      CONVERT_TEXT_STRING_2_UCS2(TEXT_STOP_STRING)

#define TEXT_OBEX_AUTH_UCS2                 CONVERT_TEXT_STRING_2_UCS2(TEXT_OBEX_AUTH_STRING)
#define TEXT_OBEX_PASSKEY_UCS2              CONVERT_TEXT_STRING_2_UCS2(TEXT_OBEX_PASSKEY_STRING)
#define TEXT_OBEX_USERID_UCS2               CONVERT_TEXT_STRING_2_UCS2(TEXT_OBEX_USERID_STRING)


#ifdef __cplusplus
}
#endif



#endif
