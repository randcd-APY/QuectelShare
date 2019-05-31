/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary. 

*****************************************************************************/

#ifndef CSR_TRANSPORT_H__
#define CSR_TRANSPORT_H__

#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Message struct used when data is sent/received to/from the host controller */
typedef struct
{
    CsrUint8 *buf;
    CsrUint32 buflen;
    CsrUint32 dex;
    CsrUint8  chan;
} MessageStructure;

typedef struct txmsg
{
    struct txmsg    *next;
    MessageStructure m;     /* The message (reference) itself. */
    CsrUint8         chan;  /* BCSP channel. */
    CsrUint8         seq;   /* Sequence number. */
} TXMSG;


/* Definitions for transport channels */

/* Send primitive on a unreliable channel */
#define TRANSPORT_UNRELIABLE_CHANNEL         (0)

/* Send primitive on a reliable channel */
#define TRANSPORT_RELIABLE_CHANNEL           (1)

#define TRANSPORT_CHANNEL_BCCMD              (2)
#define TRANSPORT_CHANNEL_HQ                 (3)
#define TRANSPORT_CHANNEL_DM                 (4)
#define TRANSPORT_CHANNEL_HCI                (5)
#define TRANSPORT_CHANNEL_ACL                (6)
#define TRANSPORT_CHANNEL_SCO                (7)
#define TRANSPORT_CHANNEL_L2CAP              (8)
#define TRANSPORT_CHANNEL_RFCOMM             (9)
#define TRANSPORT_CHANNEL_SDP                (10)
#define TRANSPORT_CHANNEL_DFU                (12)
#define TRANSPORT_CHANNEL_VM                 (13)
#define TRANSPORT_CHANNEL_ACCMD              (21)
#define TRANSPORT_CHANNEL_CME                (24)
#define TRANSPORT_CHANNEL_QC_HCIVS           (25)
#define TRANSPORT_CHANNEL_H4IBS              (26)

/* Defines for setting/resetting the msb of the handle for downstream/upstream primitives */
#define TRANSPORT_HANDLE_ON_CHIP_FLAG_SET    (0x8000)
#define TRANSPORT_HANDLE_ON_CHIP_FLAG_RESET  (0x7FFF)

/* Definitions of transport types */
#define TRANSPORT_TYPE_UNKNOWN               (0x0)
#define TRANSPORT_TYPE_BCSP                  (0x1)
#define TRANSPORT_TYPE_USB                   (0x2)
#define TRANSPORT_TYPE_H4_UART               (0x3)
#define TRANSPORT_TYPE_H5_THREE_WIRE         (0x6) /* Unsupported - Use the BCSP protocol */
#define TRANSPORT_TYPE_H4_DS                 (0x7)
#define TRANSPORT_TYPE_TYPE_A                (0x8)
#define TRANSPORT_TYPE_PIPE                  (0x9)
#define TRANSPORT_TYPE_RAW                   (0xA) /* Raw - internal usage only */
#define TRANSPORT_TYPE_H4_I                  (0xB) /* Infineon H4+ variant */
#define TRANSPORT_TYPE_BLUEZ                 (0xC) /* BlueZ raw socket/device */
#define TRANSPORT_TYPE_HTRANS                (0xD) /* Hydra Transports */
#define TRANSPORT_TYPE_H4_IBS                (0xE) /* H4 IBS(In-Band Sleep) specific with QCA chip. */
void *CsrHciTransportCreate(void *type);

#ifdef __cplusplus
}
#endif


#endif /* CSR_TRANSPORT_H__ */
