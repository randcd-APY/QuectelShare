/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $

DESCRIPTION:   Header file to define the API for Bluetooth HFG RIL(MODEM).

NOTE:          The API for Bluetooth specific AT command isn't defined.

******************************************************************************/

#ifndef _CONNX_HFG_RIL_H_
#define _CONNX_HFG_RIL_H_

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif


#define CONNX_HFG_MAX_VOICE_CALL                    8

#define CONNX_HFG_MAX_PHONE_NUM                     (82 + 1)

typedef uint32_t        ConnxHfgRilResult;
#define CONNX_HFG_RIL_RESULT_SUCCESS                ((ConnxHfgRilResult) (0x0))
#define CONNX_HFG_RIL_RESULT_FAIL                   ((ConnxHfgRilResult) (0x1))
#define CONNX_HFG_RIL_RESULT_NOT_READY              ((ConnxHfgRilResult) (0x2))
#define CONNX_HFG_RIL_RESULT_INVALID_PARAMETER      ((ConnxHfgRilResult) (0x3))
#define CONNX_HFG_RIL_RESULT_NOT_IMPLEMENTED        ((ConnxHfgRilResult) (0x4))
#define CONNX_HFG_RIL_RESULT_SERVICE_BUSY           ((ConnxHfgRilResult) (0x5))

#define IS_HFG_RIL_SUCCESS(r)   ((r) == CONNX_HFG_RIL_RESULT_SUCCESS)

typedef uint8_t         ConnxHfgCallDir;
#define CONNX_HFG_CALL_MOBILE_ORIGINATED            ((ConnxHfgCallDir) 0)
#define CONNX_HFG_CALL_MOBILE_TERMINATED            ((ConnxHfgCallDir) 1)

typedef uint8_t         ConnxHfgCallStatus;
#define CONNX_HFG_CALL_STATUS_ACTIVE                ((ConnxHfgCallStatus) 0)
#define CONNX_HFG_CALL_STATUS_HELD                  ((ConnxHfgCallStatus) 1)
#define CONNX_HFG_CALL_STATUS_OUTGOING              ((ConnxHfgCallStatus) 2)
#define CONNX_HFG_CALL_STATUS_ALERT                 ((ConnxHfgCallStatus) 3)
#define CONNX_HFG_CALL_STATUS_INCOMING              ((ConnxHfgCallStatus) 4)
#define CONNX_HFG_CALL_STATUS_WAITING               ((ConnxHfgCallStatus) 5)
/* Extended HFG call status, which match with the definition of MCM. */
#define CONNX_HFG_CALL_STATUS_END                   ((ConnxHfgCallStatus) 0xFE)
#define CONNX_HFG_CALL_STATUS_UNKNOWN               ((ConnxHfgCallStatus) 0xFF)

typedef uint8_t         ConnxHfgCallMode;
#define CONNX_HFG_CALL_MODE_VOICE                   ((ConnxHfgCallMode) 0)
#define CONNX_HFG_CALL_MODE_DATA                    ((ConnxHfgCallMode) 1)
#define CONNX_HFG_CALL_MODE_FAX                     ((ConnxHfgCallMode) 2)

/* Phone number type: without/with international access code. */
#define CONNX_HFG_NUM_TYPE_NO_INT_ACCESS_CODE       129
#define CONNX_HFG_NUM_TYPE_WITH_INT_ACCESS_CODE     145

/* Call handling (AT+CHLD) commands from headset */
typedef uint8_t         ConnxHfgCallHandlingCommand;
#define CONNX_HFG_RELEASE_ALL_HELD_CALL             ((ConnxHfgCallHandlingCommand) 0)     /* AT+CHLD=0  */
#define CONNX_HFG_RELEASE_ACTIVE_ACCEPT             ((ConnxHfgCallHandlingCommand) 1)     /* AT+CHLD=1  */
#define CONNX_HFG_RELEASE_SPECIFIED_CALL            ((ConnxHfgCallHandlingCommand) 2)     /* AT+CHLD=1x */
#define CONNX_HFG_HOLD_ACTIVE_ACCEPT                ((ConnxHfgCallHandlingCommand) 3)     /* AT+CHLD=2  */
#define CONNX_HFG_REQUEST_PRIVATE_WITH_SPECIFIED    ((ConnxHfgCallHandlingCommand) 4)     /* AT+CHLD=2x */
#define CONNX_HFG_ADD_CALL                          ((ConnxHfgCallHandlingCommand) 5)     /* AT+CHLD=3  */
#define CONNX_HFG_CONNECT_TWO_CALLS                 ((ConnxHfgCallHandlingCommand) 6)     /* AT+CHLD=4  */

/* Response/hold (AT+BTRH) commands from headset. These are a continuation of the AT+CHLD.  */
#define CONNX_HFG_BTRH_PUT_ON_HOLD                  ((ConnxHfgCallHandlingCommand) 7)     /* AT+BTRH=0 */
#define CONNX_HFG_BTRH_ACCEPT_INCOMING              ((ConnxHfgCallHandlingCommand) 8)     /* AT+BTRH=1 */
#define CONNX_HFG_BTRH_REJECT_INCOMING              ((ConnxHfgCallHandlingCommand) 9)     /* AT+BTRH=2 */
#define CONNX_HFG_BTRH_READ_STATUS                  ((ConnxHfgCallHandlingCommand) 10)    /* AT+BTRH? query status */

/* CME ERRORS. Refer to chapter4.33.2 in Bluetooth Handsfree Profile(HFP) 1.6 specification. */
typedef uint16_t        ConnxHfgCmeeResultCode;
#define CONNX_HFG_CME_AG_FAILURE                                    ((ConnxHfgCmeeResultCode) 0)
#define CONNX_HFG_CME_NO_CONNECTION_TO_PHONE                        ((ConnxHfgCmeeResultCode) 1)
#define CONNX_HFG_CME_OPERATION_NOT_ALLOWED                         ((ConnxHfgCmeeResultCode) 3)
#define CONNX_HFG_CME_OPERATION_NOT_SUPPORTED                       ((ConnxHfgCmeeResultCode) 4)
#define CONNX_HFG_CME_PH_SIM_PIN_REQUIRED                           ((ConnxHfgCmeeResultCode) 5)
#define CONNX_HFG_CME_PH_FSIM_PIN                                   ((ConnxHfgCmeeResultCode) 6)   /* not defined in spec */
#define CONNX_HFG_CME_PH_FSIM_PUK                                   ((ConnxHfgCmeeResultCode) 7)   /* not defined in spec */
#define CONNX_HFG_CME_SIM_NOT_INSERTED                              ((ConnxHfgCmeeResultCode) 10)
#define CONNX_HFG_CME_SIM_PIN_REQUIRED                              ((ConnxHfgCmeeResultCode) 11)
#define CONNX_HFG_CME_SIM_PUK_REQUIRED                              ((ConnxHfgCmeeResultCode) 12)
#define CONNX_HFG_CME_SIM_FAILURE                                   ((ConnxHfgCmeeResultCode) 13)
#define CONNX_HFG_CME_SIM_BUSY                                      ((ConnxHfgCmeeResultCode) 14)
#define CONNX_HFG_CME_SIM_WRONG                                     ((ConnxHfgCmeeResultCode) 15)  /* not defined in spec */
#define CONNX_HFG_CME_INCORRECT_PASSWORD                            ((ConnxHfgCmeeResultCode) 16)
#define CONNX_HFG_CME_SIM_PIN2_REQUIRED                             ((ConnxHfgCmeeResultCode) 17)
#define CONNX_HFG_CME_PUK2_REQUIRED                                 ((ConnxHfgCmeeResultCode) 18)
#define CONNX_HFG_CME_MEMORY_FULL                                   ((ConnxHfgCmeeResultCode) 20)
#define CONNX_HFG_CME_INVALID_INDEX                                 ((ConnxHfgCmeeResultCode) 21)
#define CONNX_HFG_CME_NOT_FOUND                                     ((ConnxHfgCmeeResultCode) 22)  /* not defined in spec */
#define CONNX_HFG_CME_MEMORY_FAILURE                                ((ConnxHfgCmeeResultCode) 23)
#define CONNX_HFG_CME_TEXT_STRING_TOO_LONG                          ((ConnxHfgCmeeResultCode) 24)
#define CONNX_HFG_CME_INVALID_CHARACTERS_IN_TEXT_STRING             ((ConnxHfgCmeeResultCode) 25)
#define CONNX_HFG_CME_DIAL_STRING_TOO_LONG                          ((ConnxHfgCmeeResultCode) 26)
#define CONNX_HFG_CME_INVALID_CHARACTERS_IN_DIAL_STRING             ((ConnxHfgCmeeResultCode) 27)
#define CONNX_HFG_CME_NO_NETWORK_SERVICE                            ((ConnxHfgCmeeResultCode) 30)
#define CONNX_HFG_CME_NETWORK_TIMEOUT                               ((ConnxHfgCmeeResultCode) 31)  /* not defined in spec */
#define CONNX_HFG_CME_NETWORK_NOT_ALLOWED_EMERGENCY_CALLS_ONLY      ((ConnxHfgCmeeResultCode) 32)
#define CONNX_HFG_CME_NETWORK_PERSONALIZATION_PIN_REQUIRED          ((ConnxHfgCmeeResultCode) 40)  /* not defined in spec */
#define CONNX_HFG_CME_NETWORK_PERSONALIZATION_PUK_REQUIRED          ((ConnxHfgCmeeResultCode) 41)  /* not defined in spec */
#define CONNX_HFG_CME_NETWORK_SUBSET_PERSONALIZATION_PIN_REQUIRED   ((ConnxHfgCmeeResultCode) 42)  /* not defined in spec */
#define CONNX_HFG_CME_NETWORK_SUBSET_PERSONALIZATION_PUK_REQUIRED   ((ConnxHfgCmeeResultCode) 43)  /* not defined in spec */
#define CONNX_HFG_CME_SERVICE_PROVIDER_PERSONALIZATION_PIN_REQUIRED ((ConnxHfgCmeeResultCode) 44)  /* not defined in spec */
#define CONNX_HFG_CME_SERVICE_PROVIDER_PERSONALIZATION_PUK_REQUIRED ((ConnxHfgCmeeResultCode) 45)  /* not defined in spec */
#define CONNX_HFG_CME_CORPORATE_PERSONALIZATION_PIN_REQUIRED        ((ConnxHfgCmeeResultCode) 46)  /* not defined in spec */
#define CONNX_HFG_CME_CORPORATE_PERSONALIZATION_PUK_REQUIRED        ((ConnxHfgCmeeResultCode) 47)  /* not defined in spec */
#define CONNX_HFG_CME_HIDDEN_KEY_REQUIRED                           ((ConnxHfgCmeeResultCode) 48)  /* not defined in spec */
#define CONNX_HFG_CME_EAP_METHOD_NOT_SUPPORTED                      ((ConnxHfgCmeeResultCode) 49)  /* not defined in spec */
#define CONNX_HFG_CME_INCORRECT_PARAMETERS                          ((ConnxHfgCmeeResultCode) 50)  /* not defined in spec */
#define CONNX_HFG_CME_UNKNOWN                                       ((ConnxHfgCmeeResultCode) 100) /* not defined in spec */
#define CONNX_HFG_CME_BUSY                                          ((ConnxHfgCmeeResultCode) 101) /* not defined in GSM spec */
#define CONNX_HFG_CME_NO_CARRIER                                    ((ConnxHfgCmeeResultCode) 102) /* not defined in GSM spec */
#define CONNX_HFG_CME_NO_ANSWER                                     ((ConnxHfgCmeeResultCode) 103) /* not defined in GSM spec */
#define CONNX_HFG_CME_DELAYED                                       ((ConnxHfgCmeeResultCode) 104) /* not defined in GSM spec */
#define CONNX_HFG_CME_BLACKLISTED                                   ((ConnxHfgCmeeResultCode) 105) /* not defined in GSM spec */
#define CONNX_HFG_CME_SUCCESS                                       ((ConnxHfgCmeeResultCode) 0xffff) /* not defined in spec, internal CSR */

#define IS_HFG_CME_SUCCESS(cmee_result)     ((cmee_result) == CONNX_HFG_CME_SUCCESS)

#define INVALID_HFG_CALL_ID                 0xFFFFFFFF


typedef struct
{      
    uint8_t     idx;        /* call index */
    uint8_t     dir;        /* direction (0=out, 1=in) */
    uint8_t     stat;       /* call status (0,1,2,3,4 or 5) */
    uint8_t     mode;       /* call mode (0=voice, 1=data, 2=fax) */
    uint8_t     mpy;        /* multiparty (0=no, 1=yes) */
    uint8_t     numType;    /* type of number */
    char        number[CONNX_HFG_MAX_PHONE_NUM];    /* number of remote peer */
} ConnxHfgCallInfo;

/* HFG RIL common response callback. */
typedef void (* HfgRilCmnRespCallback)(ConnxContext context, uint16_t cmee_result);

typedef void (* HfgRilGetSubscriberNumRespCallback)(ConnxContext context, uint16_t cmee_result, const char *phone_number);

typedef void (* HfgRilCurrentCallListCallback)(ConnxContext context, uint16_t cmee_result, ConnxHfgCallInfo *call_list, uint32_t call_list_count);

typedef void (* HfgRilGetCopsRespCallback)(ConnxContext context, uint16_t cmee_result, char *cops);

typedef void (* HfgRilDialRespCallback)(ConnxContext context, uint16_t cmee_result, uint32_t call_id);

typedef struct
{
    uint32_t                              size;                   /* Total size in byte for HFG RIL register information. */
    ConnxContext                          context;

    HfgRilCmnRespCallback                 call_end_resp_cb;
    HfgRilCmnRespCallback                 answer_resp_cb;
    HfgRilGetSubscriberNumRespCallback    get_subscriber_num_resp_cb;
    HfgRilCurrentCallListCallback         get_clcc_resp_cb;
    HfgRilCmnRespCallback                 set_call_notification_ind_resp_cb;
    HfgRilCmnRespCallback                 set_call_waiting_notification_resp_cb;
    HfgRilCmnRespCallback                 generate_dtmf_resp_cb;
    HfgRilCmnRespCallback                 call_handling_resp_cb;
    HfgRilGetCopsRespCallback             get_cops_resp_cb;
    HfgRilDialRespCallback                dial_resp_cb;

    HfgRilCurrentCallListCallback         voice_call_ind_cb;
} ConnxHfgRilRegisterInfo; 


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilInit
 *
 *  DESCRIPTION
 *        Init RIL(MODEM) globally
 *
 *  PARAMETERS
 *
 *  RETURN
 *        init result: true for success, false for failure.
 *----------------------------------------------------------------------------*/

bool ConnxHfgRilInit(void);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilDeinit
 *
 *  DESCRIPTION
 *        Deinit RIL(MODEM)
 *
 *  PARAMETERS
 *
 *  RETURN
 *
 *----------------------------------------------------------------------------*/

void ConnxHfgRilDeinit(void);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilOpen
 *
 *  DESCRIPTION
 *        Open RIL(MODEM) for Bluetooth HFG
 *
 *  PARAMETERS
 *        registerInfo: HFG RIL register information.
 *
 *  RETURN
 *        RIL handle. NULL for failure.
 *----------------------------------------------------------------------------*/

ConnxHandle ConnxHfgRilOpen(ConnxHfgRilRegisterInfo *registerInfo);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilClose
 *
 *  DESCRIPTION
 *        Close RIL(MODEM) for Bluetooth HFG
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *
 *  RETURN
 *        N/A
 *----------------------------------------------------------------------------*/

void ConnxHfgRilClose(ConnxHandle handle);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilCallEnd
 *
 *  DESCRIPTION
 *        Terminate phone call
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        call_id:    Call index
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilCallEnd(ConnxHandle handle, uint32_t call_id);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilAnswer
 *
 *  DESCRIPTION
 *        Answer incoming call
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilAnswer(ConnxHandle handle);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilGetSubscriberNumInfo
 *
 *  DESCRIPTION
 *        Get subscriber number information
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilGetSubscriberNumInfo(ConnxHandle handle);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilGetCurrentCallList
 *
 *  DESCRIPTION
 *        Get current call list
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilGetCurrentCallList(ConnxHandle handle);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilSetCallNotificationIndication
 *
 *  DESCRIPTION
 *        Send AT+CLIP to MODEM
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        enable:     Enable/disable "Calling Line Identification notification"
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilSetCallNotificationIndication(ConnxHandle handle, bool enable);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilSetCallWaitingNotification
 *
 *  DESCRIPTION
 *        Send AT+CCWA to MODEM
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        enable:     Enable/disable "Call Waiting notification"
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilSetCallWaitingNotification(ConnxHandle handle, bool enable);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilGenerateDTMF
 *
 *  DESCRIPTION
 *        Generate DTMF tone
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        call_id:    Call ID
 *        dtmf:       DTMF tone/value to generate
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilGenerateDTMF(ConnxHandle handle, uint32_t call_id, char dtmf);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilCallHandling
 *
 *  DESCRIPTION
 *        Send call handling command to MODEM
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        command:    Call handling command
 *        index:      Index of call handling command if applied
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilCallHandling(ConnxHandle handle, ConnxHfgCallHandlingCommand command, uint8_t index);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilGetCurrentOperatorSelection
 *
 *  DESCRIPTION
 *        Get current operator selection
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilGetCurrentOperatorSelection(ConnxHandle handle);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilSetExtendedErrorResultCode
 *
 *  DESCRIPTION
 *        Enable/disable extended error result code
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        enable:     true to enable, false to disable
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilSetExtendedErrorResultCode(ConnxHandle handle, bool enable);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilSetStatusIndicatorUpdate
 *
 *  DESCRIPTION
 *        Enable/disable status indicator update
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        enable:     true to enable, false to disable
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilSetStatusIndicatorUpdate(ConnxHandle handle, bool enable);


/*----------------------------------------------------------------------------*
 *  NAME
 *      ConnxHfgRilDial
 *
 *  DESCRIPTION
 *        Make outgoing call
 *
 *  PARAMETERS
 *        handle:     RIL handle
 *        number:     Phone number to dial
 *
 *  RETURN
 *        Result code for RIL operation in asynchronous mode
 *----------------------------------------------------------------------------*/

uint32_t ConnxHfgRilDial(ConnxHandle handle, char *number);


#ifdef __cplusplus
extern "C" }
#endif

#endif  /* _CONNX_HFG_RIL_H_ */