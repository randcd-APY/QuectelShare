/*===========================================================================

                         M C M _ A T C O P_ CLIENT.H

DESCRIPTION

  The MCM ATCOP Client Module.

Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  when      who    what, where, why
--------    ---    ----------------------------------------------------------
 11/05/13   vm     Created module
 ===========================================================================*/

#include "mcm_client.h"


typedef struct {
   /* MCM_ATCOP service info */
   mcm_client_handle_type    mcm_client_handle;
   uint32_t                  mcm_atcop_handle;
} mcm_atcop_cb_t;


#define MCM_ATCOP_MAX_CMD_LEN 512
#define TRUE 1
#define FALSE 0


/*
Log Message Macros
*/

#define LOG_MSG_INFO1( fmtString, x, y, z)                        \
{                               \
  printf(fmtString, x, y, z);              \
}
#define LOG_MSG_INFO2( fmtString, x, y, z)                       \
{                                           \
  printf(fmtString, x, y, z);                   \
}
#define LOG_MSG_INFO3( fmtString, x, y, z)                            \
{                                                 \
  printf(fmtString, x, y, z);                \
}
#define LOG_MSG_ERROR( fmtString, x, y, z)                          \
{                                           \
  printf(fmtString, x, y, z);                  \
}


#define MCM_ATCOP_LOG(...)                         \
 LOG_MSG_INFO1( "%s %d:", __FILE__, __LINE__,0); \
 LOG_MSG_INFO1( __VA_ARGS__ ,0,0)

#define MCM_ATCOP_LOG_FUNC_ENTRY()  \
 MCM_ATCOP_LOG                   \
(                              \
       "Entering function %s\n",  \
       __FUNCTION__               \
)

#define MCM_ATCOP_LOG_FUNC_EXIT()   \
 MCM_ATCOP_LOG                   \
(                              \
       "Exiting function %s\n",   \
       __FUNCTION__ \
)

