#ifndef CSR_APP_PRIM_H__
#define CSR_APP_PRIM_H__
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_prim_defs.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrAppPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim CsrAppPrim;

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/
#define CSR_APP_PRIM_DOWNSTREAM_LOWEST                      (0x0000)

#define CSR_APP_REGISTER_REQ                  ((CsrAppPrim) (0x0000 + CSR_APP_PRIM_DOWNSTREAM_LOWEST))
#define CSR_APP_BACKLOG_REQ                   ((CsrAppPrim) (0x0001 + CSR_APP_PRIM_DOWNSTREAM_LOWEST))

#define CSR_APP_PRIM_DOWNSTREAM_HIGHEST                     (0x0001 + CSR_APP_PRIM_DOWNSTREAM_LOWEST)

/*******************************************************************************/

#define CSR_APP_PRIM_UPSTREAM_LOWEST                        (0x0000 + CSR_PRIM_UPSTREAM)

#define CSR_APP_REGISTER_CFM                  ((CsrAppPrim) (0x0000 + CSR_APP_PRIM_UPSTREAM_LOWEST))
#define CSR_APP_TAKE_CONTROL_IND              ((CsrAppPrim) (0x0001 + CSR_APP_PRIM_UPSTREAM_LOWEST))

#define CSR_APP_PRIM_UPSTREAM_HIGHEST                       (0x0001 + CSR_APP_PRIM_UPSTREAM_LOWEST)

#define CSR_APP_PRIM_DOWNSTREAM_COUNT         (CSR_APP_PRIM_DOWNSTREAM_HIGHEST + 1 - CSR_APP_PRIM_DOWNSTREAM_LOWEST)
#define CSR_APP_PRIM_UPSTREAM_COUNT           (CSR_APP_PRIM_UPSTREAM_HIGHEST + 1 - CSR_APP_PRIM_UPSTREAM_LOWEST)
/*******************************************************************************
 * End primitive definitions
 *******************************************************************************/

typedef struct
{
    CsrAppPrim     type;
    CsrSchedQid    phandle;
    CsrUtf8String *menuName; /* Normally technology name */
    CsrUtf8String *subMenuName; /* Normally profile name */
    CsrUint16      menuIcon;
    CsrUint16      subMenuIcon;
    CsrBool        top; /* Set to TRUE to add to top of the sub menu or FALSE to add to bottom */
} CsrAppRegisterReq;

typedef struct
{
    CsrAppPrim type;
    CsrUint16  controlId; /* Identifier given to the registred menu item */
} CsrAppRegisterCfm;

typedef struct
{
    CsrAppPrim type;
    CsrUint16  controlId; /* Identifier given to the registred menu item, the app can use this to find out which menu item the user selected */
} CsrAppTakeControlInd;

typedef struct
{
    CsrAppPrim     type;
    CsrUtf8String *menuName; /* Normally technology name */
    CsrUtf8String *subMenuName; /* Normally profile name */
    CsrUtf8String *logText; /* Actual string which should be stored in the backlog */
    CsrBool        causePopup; /* TRUE to display popup with the message */
    CsrBool        causeBacklog; /* TRUE to add message to backlog */
} CsrAppBacklogReq;

#ifdef __cplusplus
}
#endif

#endif
