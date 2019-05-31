#ifndef BNEP_PRIM_H__
#define BNEP_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2001-2015 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/


#include "csr_types.h"
#include "csr_mblk.h"
#include "bluetooth.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************

#defines for BNEP magic numbers

******************************************************************************/

#define BNEP_UUID_PANU 0x1115 /* www.bluetooth.org/assigned-numbers/sdp.htm */
#define BNEP_UUID_NAP 0x1116 /* www.bluetooth.org/assigned-numbers/sdp.htm */
#define BNEP_UUID_GN 0x1117 /* www.bluetooth.org/assigned-numbers/sdp.htm */


/******************************************************************************

    struct ETHER_ADDR  --  an Ethernet address

******************************************************************************/

typedef struct ETHER_ADDR
{
    CsrUint16 w[3]; /* Big-endian */
} ETHER_ADDR;


/******************************************************************************

    enum BNEP_MSG_TYPE  --  BNEP task message types
    typedefs for BNEP task messages
    enums for BNEP task message parameters

******************************************************************************/

#define BNEP_PRIM_BASE 0x00fe

#define ENUM_SEP_BNEP_FIRST_PRIM BNEP_PRIM_BASE
#define ENUM_SEP_BNEP_LAST_PRIM (BNEP_PRIM_BASE + 25)

enum BNEP_MSG_TYPE
{
#ifndef BLUELAB
    BNEP_FLOW_REQ = BNEP_PRIM_BASE + 1,
    BNEP_FLOW_IND, /* HERE: rename to PAN_FLOW_* */
#endif
    BNEP_REGISTER_REQ = BNEP_PRIM_BASE + 3,
    BNEP_EXTENDED_REGISTER_REQ,
    BNEP_CONNECT_REQ,
    BNEP_CONNECT_CFM,
    BNEP_CONNECT_IND,
#ifndef BLUELAB
    BNEP_LISTEN_REQ,
    BNEP_IGNORE_REQ,
    BNEP_UP_IND,
    BNEP_DOWN_IND,
    BNEP_DATA_REQ,
    BNEP_EXTENDED_DATA_REQ,
    BNEP_EXTENDED_DATA_CFM,
    BNEP_EXTENDED_MULTICAST_DATA_REQ,
    BNEP_DATA_IND,
    BNEP_EXTENDED_DATA_IND,
    BNEP_MULTICAST_DATA_IND,
    BNEP_LOOPBACK_DATA_IND,
#endif
    BNEP_DISCONNECT_REQ = BNEP_PRIM_BASE + 19,
    BNEP_DISCONNECT_IND,
#ifndef BLUELAB
    BNEP_DEBUG_REQ,
    BNEP_DEBUG_IND,
#endif
    BNEP_HCI_SWITCH_ROLE_RSP = BNEP_PRIM_BASE + 23,
    BNEP_ROLE_CHANGE_IND
};
#define PAN_FLOW_REQ BNEP_FLOW_REQ
#define PAN_FLOW_IND BNEP_FLOW_IND

#ifndef BTCHIP
#ifndef BLUELAB
#define BNEP_REGISTER_REQ_FLAG_DISABLE_EXTENDED 0x0004
#define BNEP_REGISTER_REQ_FLAG_MANUAL_BRIDGE 0x0002 /* reserved for future use */
#endif
#define BNEP_REGISTER_REQ_FLAG_DISABLE_STACK 0x0001
#endif

#define BNEP_NO_CID 0 /* BT1.1/D/2.1 */
#define BNEP_SIGNALLING_CID 1 /* BT1.1/D/2.1 */
#define BNEP_ID_LOCAL BNEP_NO_CID
#define BNEP_ID_DST BNEP_SIGNALLING_CID

typedef struct BNEP_REGISTER_REQ_T
{
    CsrUint16 type;
/*    bchs change
#ifdef BTCHIP*/
    unsigned int : 13;
    unsigned int disable_extended : 1;
    unsigned int manual_bridge : 1; /* reserved for future use */
    unsigned int disable_stack : 1;
/*#else*/
    CsrUint16 flags;
/*#endif*/
    CsrSchedQid phandle;
} BNEP_REGISTER_REQ_T;

/*    bchs change. New signal added with addr included */
typedef struct BNEP_EXTENDED_REGISTER_REQ_T
{
    CsrUint16 type;
    BD_ADDR_T    addr;
    unsigned int : 13;
    unsigned int disable_extended : 1;
    unsigned int manual_bridge : 1; /* reserved for future use */
    unsigned int disable_stack : 1;
    CsrUint16 flags;
    CsrSchedQid phandle;
} BNEP_EXTENDED_REGISTER_REQ_T;

/*    Synergy BT change
#ifndef BTCHIP
#define BNEP_CONNECT_REQ_FLAG_RNAP 0x4000
#define BNEP_CONNECT_REQ_FLAG_RGN 0x2000
#define BNEP_CONNECT_REQ_FLAG_RPANU 0x1000
#define BNEP_CONNECT_REQ_FLAG_LNAP 0x0400
#define BNEP_CONNECT_REQ_FLAG_LGN 0x0200
#define BNEP_CONNECT_REQ_FLAG_LPANU 0x0100
#define BNEP_CONNECT_REQ_FLAG_NO_SWITCH 0x0004
#define BNEP_CONNECT_REQ_FLAG_SINGLE_USER 0x0002
#define BNEP_CONNECT_REQ_FLAG_ACCEPT_IN 0x0001
#else
*/
typedef struct BNEP_CONNECT_REQ_FLAGS
{
    unsigned int reserved15 : 1;
    unsigned int rnap : 1;
    unsigned int rgn : 1;
    unsigned int rpanu : 1;
    unsigned int reserved11 : 1;
    unsigned int lnap : 1;
    unsigned int lgn : 1;
    unsigned int lpanu : 1;
    /* Not all combinations are valid! */
    unsigned int reserved7 : 1;
    unsigned int reserved6 : 1;
    unsigned int reserved5 : 1;
    unsigned int persist : 1; /* reserved for future use */
    unsigned int on_demand : 1; /* reserved for future use */
    unsigned int no_switch : 1;
    unsigned int single_user : 1;
    unsigned int accept_in : 1; /* only for passive */
} BNEP_CONNECT_REQ_FLAGS;
/*#endif*/
typedef struct BNEP_CONNECT_REQ_T
{
    CsrUint16 type;
/*    Synergy BT change
#ifdef BTCHIP*/
    BNEP_CONNECT_REQ_FLAGS flags;
/*#else
    CsrUint16 flags;
/ *#endif*/
    ETHER_ADDR rem_addr; /* set msw to ETHER_UNKNOWN for passive */
} BNEP_CONNECT_REQ_T;

enum BNEP_CONNECT_CFM_STATUS
{
    BNEP_CONNECT_CFM_STATUS_SUCCESS,
    BNEP_CONNECT_CFM_STATUS_BADARG,
    BNEP_CONNECT_CFM_STATUS_NOMEM,
    BNEP_CONNECT_CFM_STATUS_CONFLICT,
    BNEP_CONNECT_CFM_STATUS_TRYLATER
};
typedef struct BNEP_CONNECT_CFM_T
{
    CsrUint16 type;
    enum BNEP_CONNECT_CFM_STATUS status;
} BNEP_CONNECT_CFM_T;

enum BNEP_CONNECT_IND_STATUS
{
    /*
     * Some of these must match the L2CA_CONNECTION codes in l2cap_prim.h.
 */
    BNEP_CONNECT_IND_STATUS_SUCCESSFUL_OUT = 0x0000, /* must match */
    BNEP_CONNECT_IND_STATUS_REJ_PSM = 0x0002, /* must match */
    BNEP_CONNECT_IND_STATUS_REJ_SECURITY = 0x0003, /* must match */
    BNEP_CONNECT_IND_STATUS_REJ_RESOURCES = 0x0004, /* must match */
    BNEP_CONNECT_IND_STATUS_REJ_INVALID_SRC_CID = 0x0006, /*must match */
    BNEP_CONNECT_IND_STATUS_REJ_SRC_CID_INUSE = 0x0007, /*must match */    
    BNEP_CONNECT_IND_STATUS_NOT_MASTER = (int) 0xdda5,
    BNEP_CONNECT_IND_STATUS_ABEND = (int) 0xddab,
    BNEP_CONNECT_IND_STATUS_REJ_ROLES = (int) 0xddb2, /* HERE: pack in reason? */
    BNEP_CONNECT_IND_STATUS_REJ_NOT_READY = (int) 0xdddd, /* must match */
    BNEP_CONNECT_IND_STATUS_FAILED = (int) 0xddde, /* must match */
    BNEP_CONNECT_IND_STATUS_TIMEOUT = (int) 0xeeee, /* must match */
    BNEP_CONNECT_IND_STATUS_SUCCESSFUL_IN = (int) 0xffff
};
typedef struct BNEP_CONNECT_IND_T
{
    CsrUint16 type;
    enum BNEP_CONNECT_IND_STATUS status;
    CsrUint16 id;
    ETHER_ADDR rem_addr;
    CsrUint16 rem_uuid;
    CsrUint16 loc_uuid;
} BNEP_CONNECT_IND_T;

#ifndef BLUELAB
typedef struct BNEP_LISTEN_REQ_T
{
    CsrUint16 type;
    CsrSchedQid phandle;
    CsrUint16 ether_type;
    ETHER_ADDR addr;
} BNEP_LISTEN_REQ_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_IGNORE_REQ_T
{
    CsrUint16 type;
    CsrUint16 ether_type;
    ETHER_ADDR addr;
} BNEP_IGNORE_REQ_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_DATA_REQ_T
{
    CsrUint16 type;
    CsrUint16 ether_type;
    ETHER_ADDR rem_addr; /* HERE: this could be multicast -- probably get BNEP to transform into a BNEP_EXTENDED_DATA_REQ if not PANU */
    CsrMblk *mblk;
} BNEP_DATA_REQ_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_EXTENDED_DATA_REQ_T
{
    CsrUint16 type;
    CsrUint16 id; /* HERE: if 0 then to local stack, 1 to go to dst_addr */
    CsrUint16 ether_type;
    ETHER_ADDR dst_addr; /* typically a PANU's address, but allow future exp */
    ETHER_ADDR src_addr;
    CsrMblk *mblk;
} BNEP_EXTENDED_DATA_REQ_T;

typedef struct BNEP_EXTENDED_DATA_CFM_T
{
    CsrUint16 type;
    CsrUint16 id;
} BNEP_EXTENDED_DATA_CFM_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_EXTENDED_MULTICAST_DATA_REQ_T
{
    CsrUint16 type;
    CsrUint16 id_not; /* HERE: if 0 then not to local stack, 1 to go to all */
    CsrUint16 ether_type;
    ETHER_ADDR dst_addr;
    ETHER_ADDR src_addr;
    CsrMblk *mblk;
} BNEP_EXTENDED_MULTICAST_DATA_REQ_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_DATA_IND_T
{
    CsrUint16 type;
    CsrSchedQid phandle;
    CsrMblk *mblk;
} BNEP_DATA_IND_T;
typedef BNEP_DATA_IND_T BNEP_MULTICAST_DATA_IND_T;
typedef BNEP_DATA_IND_T BNEP_LOOPBACK_DATA_IND_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_EXTENDED_DATA_IND_T
{
    CsrUint16 type;
    CsrUint16 id; /* HERE: if 0 then from local stack */
    CsrUint16 ether_type;
    ETHER_ADDR dst_addr; /* note may be multicast */
    ETHER_ADDR src_addr; /* should be a PANU's address, but who knows? */
    CsrMblk *mblk;
} BNEP_EXTENDED_DATA_IND_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_UP_IND_T
{
    CsrUint16 type;
    CsrSchedQid phandle;
    CsrUint16 rem_uuid;
    CsrUint16 loc_uuid;
} BNEP_UP_IND_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_DOWN_IND_T /* This must be compatible with BNEP_UP_IND_T */
{
    CsrUint16 type;
    CsrSchedQid phandle;
} BNEP_DOWN_IND_T;
#endif

typedef struct BNEP_DISCONNECT_REQ_T
{
    CsrUint16 type;
    CsrUint16 flags;
    CsrUint16 id;
} BNEP_DISCONNECT_REQ_T;

enum BNEP_DISCONNECT_IND_REASON
{
    /* 0-255 reserved in case one day we get HCI reason codes from L2CAP */
    BNEP_DISCONNECT_IND_REASON_ABEND = 0x0100,
    BNEP_DISCONNECT_IND_REASON_DONE = 0x0101,
    BNEP_DISCONNECT_IND_REASON_NOT_MASTER = 0x0102,
    BNEP_DISCONNECT_IND_REASON_REMOTE_DISC = 0x0103
};
typedef struct BNEP_DISCONNECT_IND_T
{
    CsrUint16 type;
    CsrUint16 id;
    enum BNEP_DISCONNECT_IND_REASON reason;
} BNEP_DISCONNECT_IND_T;

#ifndef BLUELAB
typedef struct BNEP_DEBUG_REQ_T
{
    CsrUint16 type;
    CsrUint16 id;
    CsrUint16 test;
    CsrUint16 args[18];
} BNEP_DEBUG_REQ_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_DEBUG_IND_T
{
    CsrUint16 type;
    CsrUint16 id;
    CsrUint16 test;
    CsrUint16 args[18];
} BNEP_DEBUG_IND_T;
#endif

#ifndef BLUELAB
typedef struct BNEP_FLOW_REQ_T
{
    CsrUint16 type;
} BNEP_FLOW_REQ_T;
#define PAN_FLOW_REQ_T BNEP_FLOW_REQ_T
#endif

#ifndef BLUELAB
typedef struct BNEP_FLOW_IND_T
{
    CsrUint16 type;
    CsrUint16 free;
} BNEP_FLOW_IND_T;
#define PAN_FLOW_IND_T BNEP_FLOW_IND_T
#endif

typedef struct BNEP_HCI_SWITCH_ROLE_RSP_T /* DM_HCI_SWITCH_ROLE_CFM_T */
{
    CsrUint16 type;
    CsrSchedQid phandle; /* CSR_UNUSED */
    BD_ADDR_T bd_addr;
    CsrUint8 status;
    CsrUint8 role;
} BNEP_HCI_SWITCH_ROLE_RSP_T;

typedef struct BNEP_ROLE_CHANGE_IND_T
{
    CsrUint16 type;
    CsrUint16 rem_uuid;
    CsrUint16 loc_uuid;
} BNEP_ROLE_CHANGE_IND_T;

typedef union BNEP_UPRIM_T
{
    CsrUint16 type;
    /* HERE: put all the primitives in here */
} BNEP_UPRIM_T;


/******************************************************************************

#defines and macros for Ethernet addressing

******************************************************************************/
/*    Synergy BT change */
#define BNEP_MTU 1691 /* BNEP0.95a/2.2 */

#define ETHER_ADDR_LEN 6 /* in octets */
#define ETHER_MTU 1500 /* excluding header, CRC and any 802.1Q header */
#define ETHER_LOOPBACK 0x1bac /* First 16 bits -- note this is locally-assigned and multicast to ensure it won't be in use on the network */
#define ETHER_UNKNOWN 0x0b0f /* Ditto */
#define ETHER_MULTICAST_IPV4 0x01005e /* First 24 bits, and then a zero bit */
#define ETHER_MULTICAST_IPV6 0x3333 /* First 16 bits */
#define ETHER_BROADCAST_IPV4 0xffff /* Replicated across all 48 bits */
#define ETHER_TYPE_IPV4 0x0800
#define ETHER_TYPE_ARP 0x0806
#define ETHER_TYPE_8021Q 0x8100
#define ETHER_TYPE_IPV6 0x86dd
#define ETHER_IS_LOOPBACK(x) ((x)->w[0] == ETHER_LOOPBACK)
#define ETHER_IS_UNKNOWN(x) ((x)->w[0] == ETHER_UNKNOWN)
#define ETHER_IS_BROADCAST(x) \
    (!~(x)->w[0] && \
     !~(x)->w[1] && \
     !~(x)->w[2])
#define ETHER_IS_MULTICAST(x) ((x)->w[0] & 0x0100)
#define ETHER_IS_SAME_ADDR(x,y) \
    ((x)->w[0] == (y)->w[0] && \
     (x)->w[1] == (y)->w[1] && \
     (x)->w[2] == (y)->w[2])

#ifdef __cplusplus
}
#endif

#endif /* BNEP_PRIM_H */
