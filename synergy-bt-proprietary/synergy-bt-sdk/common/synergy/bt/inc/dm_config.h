/*******************************************************************************

Copyright (c) 2007-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*******************************************************************************/
#ifndef _DM_CONFIG_H_
#define _DM_CONFIG_H_

#include "csr_synergy.h"

#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_FOR_HOST
/* Default Synergy from-chip buffer configuration */
#ifndef DM_CONFIG_HOST_ACL_DATA_PACKET_LENGTH
#define DM_CONFIG_HOST_ACL_DATA_PACKET_LENGTH           ((CsrUint16)1021)
#endif
#ifndef DM_CONFIG_HOST_SCO_DATA_PACKET_LENGTH
#define DM_CONFIG_HOST_SCO_DATA_PACKET_LENGTH           ((CsrUint8) 128)
#endif
#ifndef DM_CONFIG_HOST_TOTAL_NUM_ACL_DATA_PACKETS
#define DM_CONFIG_HOST_TOTAL_NUM_ACL_DATA_PACKETS       ((CsrUint16)100)
#endif
#ifndef DM_CONFIG_HOST_TOTAL_NUM_SCO_DATA_PACKETS
#define DM_CONFIG_HOST_TOTAL_NUM_SCO_DATA_PACKETS       ((CsrUint16)16)
#endif
#endif /* BUILD_FOR_HOST */

#define DM_MAX_NUM_DEVICES_IN_BLACK_LIST                (CsrUint8)5

#ifndef DM_CONFIG_POWERUP_TIMEOUT
#define DM_CONFIG_POWERUP_TIMEOUT                       (5 * CSR_SCHED_SECOND)
#endif

/* Security detach timer. Allows enough time for encryption event to
   arrive before detaching the ACL. */
#ifndef DM_CONFIG_SM_DETACH_TIMEOUT
#define DM_CONFIG_SM_DETACH_TIMEOUT                     (2 * CSR_SCHED_SECOND)
#endif

/* Authenitcation repeat timer. Allows enough time before retrying
   authentication to avoid HCI_ERROR_AUTH_REPEATED failures. */
#ifndef DM_CONFIG_SM_REAUTHENTICATION_TIMEOUT
#define DM_CONFIG_SM_REAUTHENTICATION_TIMEOUT           (1 * CSR_SCHED_SECOND)
#endif

/* Encryption retry timer. Allows the LM enough time to recover
   between encryption requests. */
#ifndef DM_CONFIG_SM_REENCRYPTION_TIMEOUT
#define DM_CONFIG_SM_REENCRYPTION_TIMEOUT               (200 * CSR_SCHED_MILLISECOND)
#endif

/* Mode change holdoff times after a failed locally-initiated mode
   change request and after a remotely-initiated mode change. */
#ifndef DM_CONFIG_MODE_CHANGE_FAILED_HOLDOFF_TIME
#define DM_CONFIG_MODE_CHANGE_FAILED_HOLDOFF_TIME       (1 * CSR_SCHED_SECOND)
#endif
#ifndef DM_CONFIG_MODE_CHANGE_REMOTE_HOLDOFF_TIME
#define DM_CONFIG_MODE_CHANGE_REMOTE_HOLDOFF_TIME       (5 * CSR_SCHED_SECOND)
#endif

/* Basic unit of ACL idle timeout used internally by ACL Manager. */
#ifndef DM_CONFIG_ACL_IDLE_TIMEOUT_BASE
#define DM_CONFIG_ACL_IDLE_TIMEOUT_BASE                 (250 * CSR_SCHED_MILLISECOND)
#endif

/* Defaults for timeout multipliers specified by clients when closing ACLs.
   Defined here is how many times you need to run consecutively a timeout
   of duration ACL_IDLE_TIMEOUT_BASE before closing the ACL. */

/* Timeout to allow for rapid automatic processes, e.g., L2CAP reconnect attempt */
#ifndef DM_CONFIG_ACL_IDLE_TIMEOUT_TINY
#define DM_CONFIG_ACL_IDLE_TIMEOUT_TINY     1                                    /* Wait for ACL_IDLE_TIMEOUT_BASE */
#endif

/* Timeout to allow for slower automatic processes, e.g. link key/encrypton messages. */
#ifndef DM_CONFIG_ACL_IDLE_TIMEOUT_SHORT
#define DM_CONFIG_ACL_IDLE_TIMEOUT_SHORT    ((2*CSR_SCHED_SECOND)/DM_CONFIG_ACL_IDLE_TIMEOUT_BASE)    /* Wait for 2 seconds. */
#endif

/* Timeout to allow for rapid user interaction - YES/NO option. */
#ifndef DM_CONFIG_ACL_IDLE_TIMEOUT_MEDIUM
#define DM_CONFIG_ACL_IDLE_TIMEOUT_MEDIUM   ((10*CSR_SCHED_SECOND)/DM_CONFIG_ACL_IDLE_TIMEOUT_BASE)   /* Wait for 10 seconds. */
#endif

/* Timeout to allow for slower user interaction - entering PIN codes/numerical comparision. */
#ifndef DM_CONFIG_ACL_IDLE_TIMEOUT_LONG
#define DM_CONFIG_ACL_IDLE_TIMEOUT_LONG     ((30*CSR_SCHED_SECOND)/DM_CONFIG_ACL_IDLE_TIMEOUT_BASE)   /* Wait for 30 seconds. */
#endif

/* Timeout after successful completion of bonding. Should really be instant. */
#ifndef DM_CONFIG_ACL_IDLE_TIMEOUT_BONDING
#define DM_CONFIG_ACL_IDLE_TIMEOUT_BONDING  0
#endif

/* Configurable timeout multiplier to keep the idle link alive post pairing */
#ifndef DM_CONFIG_ACL_IDLE_TIMEOUT_LE_PAIRING
#define DM_CONFIG_ACL_IDLE_TIMEOUT_LE_PAIRING    ((2*CSR_SCHED_SECOND)/DM_CONFIG_ACL_IDLE_TIMEOUT_BASE)    /* Wait for 2 seconds. */
#endif

/* Configurable timeout for disconnection of ACL after LE fixed(for now) channel */
#ifndef DM_CONFIG_ACL_IDLE_TIMEOUT_LE_TINY
#define DM_CONFIG_ACL_IDLE_TIMEOUT_LE_TINY ACL_IDLE_TIMEOUT_AT_LEAST_TINY
#endif

#ifdef __cplusplus
}
#endif
/*=============================== END OF FILE ================================*/
#endif

