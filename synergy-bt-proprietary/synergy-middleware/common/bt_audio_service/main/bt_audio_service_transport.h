#ifndef _BT_AUDIO_SERVICE_TRANSPORT_H_
#define _BT_AUDIO_SERVICE_TRANSPORT_H_

/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#include "connx_common_def.h"
#include "connx_list.h"
#include "connx_bas_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BAS_EVENT_ADD_MSG_REQ                   0x0001
#define BAS_EVENT_QUIT_REQ                      0xFFFF

typedef struct ConnxBasDataElmTag
{
    struct ConnxBasDataElmTag *next;
    struct ConnxBasDataElmTag *prev;
    ConnxBtAudioServiceReq     message;
} ConnxBasDataElm_t;

typedef void (* ConnxBasHandleMsgCb)(void *context, void *msg);

typedef struct
{
    ConnxBasHandleMsgCb  handleMsgCb;
} ConnxBasCallbackT;

typedef struct
{
    bool                      init;                   /* true: instance is initialized, false: not. */
    bool                      quitThread;             /* true: quit thread, false: not. */
    ConnxHandle               eventHandle;            /* Event handle for API synchronous calling. */
    ConnxHandle               threadHandle;           /* Thread used to handle bt audio service message. */
    ConnxHandle 			  mutexHandle;	          /* Mutex handle. */
    ConnxCmnList_t            messageList;            /* BT audio service message list. */
    ConnxBasCallbackT         callback;               /* Handler for bt audio service event */
} BasTransportInstance;

BasTransportInstance *BasTransportGetInstance();
bool BasInitTransportInst(BasTransportInstance *inst);
void BasDeinitTransportInst(BasTransportInstance *inst);
ConnxHandle BasTransportOpen(ConnxContext appContext);
void BasTransportClose(ConnxHandle transportHandle);
void ConnxBasInitCallback(ConnxBasCallbackT *callback, ConnxBasHandleMsgCb  handleMsgCb);


#ifdef __cplusplus
}
#endif

#endif  /* _BT_AUDIO_SERVICE_TRANSPORT_H_ */

