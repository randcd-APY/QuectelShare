/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QS_COMMMON_H__
#define __QS_COMMMON_H__

#include <stdint.h>
#include "qsPack.h"
#include "qsOSAL.h"


/**********************************************************************************************************************/
/*************************************************   CONSTANTS   ******************************************************/
/**********************************************************************************************************************/

#define QS_ASSERT(_x)                     (TRUE)
#define UNUSED(_x)                        ((void)(_x))

#define QS_NUM_DEVICES                    2

/**
  Values of technology modules that will use this API.
  */
#define QS_MODULE_WLAN                    (0x00)
#define QS_MODULE_BLE                     (0x01)
#define QS_MODULE_HMI                     (0x02)
#define QS_MODULE_ZIGBEE                  (0x03)
#define QS_MODULE_THREAD                  (0x04)
#define QS_MODULE_COEX                    (0x05)
#define QS_MODULE_HERH                    (0x06)
#define QS_MODULE_FWUPGRADE               (0x07)
#define QS_MODULE_TLMM                    (0x08)
#define QS_MODULE_MAX                     (0x09)

/* Signal mask for worker threads */
#define QS_THREAD_READY                   0x0001
#define QS_WLAN_SIG                       0x0002
#define QS_BLE_SIG                        0x0004
#define QS_HMI_SIG                        0x0008
#define QS_ZB_SIG                         0x0010
#define QS_TWN_SIG                        0x0020
#define QS_COEX_SIG                       0x0040
#define QS_HERH_SIG                       0x0080
#define QS_FWUPGRADE_SIG                  0x0100
#define QS_TLMM_SIG                       0x0200

#define QS_POINTER_HEADER_SIZE            (0x01)
#define QS_CALLBACK_HEADER_SIZE           (0x00)


#ifdef CONFIG_DAEMON_MODE

#define QS_HTC_HEADER_LEN                 4
#define QS_MML_HEADER_LEN                 6
#define QS_MML_FLAGS_OFFSET               5   /*Offset to MML flags byte from start of header*/
#define QS_IOTD_COOKIE_OFFSET             6   /*Offset to cookie from start of header*/
#define QS_IOTD_HEADER_LEN                (QS_HTC_HEADER_LEN + QS_MML_HEADER_LEN)

#define QS_TOTAL_LEN(x)                   ((x) + QS_IOTD_HEADER_LEN)
#define QS_TOTAL_LENGTH_FIELD_SIZE        2
#define QS_MOD_ID_OFFSET                  (QS_IOTD_HEADER_LEN + QS_TOTAL_LENGTH_FIELD_SIZE)

/*Control message definitions: exchanged between App and daemon*/
#define QS_CONTROL_BIT_SHIFT              (0x00)  /*Bit 0*/
#define QS_SET_CONTROL_BIT(_x)            ((((char *)_x)[QS_MML_FLAGS_OFFSET]) |=  (1<<QS_CONTROL_BIT_SHIFT))
#define QS_RESET_CONTROL_BIT(_x)          ((((char *)_x)[QS_MML_FLAGS_OFFSET]) &= ~(1<<QS_CONTROL_BIT_SHIFT))
#define QS_GET_CONTROL_BIT(_x)            (((((char *)_x)[QS_MML_FLAGS_OFFSET]) & (1<<QS_CONTROL_BIT_SHIFT)) >> QS_CONTROL_BIT_SHIFT)

#define QS_CONTROL_MSG_TYPE_OFFSET        (QS_IOTD_HEADER_LEN)
#define QS_HELLO_REQ                      (0x00)
#define QS_HELLO_RESP                     (0x01)
#define QS_CLIENT_EXIT                    (0x02)
#define QS_SERVER_EXIT                    (0x04)
#define QS_TARGET_ASSERT                  (0x05)


#define QSPACKET_COOKIE(_x)               ((char *)(&((uint8_t *)(_x))[QS_IOTD_COOKIE_OFFSET]))
#define APPLY_DEV_ID(buf, devid)          (*((uint8_t*)(buf) + QS_HTC_HEADER_LEN) |= ((devid) << 4))
#define APPLY_SQ_ID(buf, sqid)            (*((uint8_t*)(buf) + QS_HTC_HEADER_LEN) |= ((sqid) & 0x0F))


/* Service IDs */
#define QAPI_SQ_ID                        0
#define BLE_DATA_SQ_ID                    1
#define WLAN_DATA_SQ_ID                   2
#define THREAD_DATA_SQ_ID                 3
#define MGMT_SQ_ID                        4
#define DIAG_SQ_ID                        5
#define NUM_SQ_ID                         6


#else

#define QS_HTC_HEADER_LEN                 0
#define QS_MML_HEADER_LEN                 0
#define QS_IOTD_HEADER_LEN                0
#define QS_TOTAL_LEN(x)                   (x)
#define QS_TOTAL_LENGTH_FIELD_SIZE        2

#endif /* CONFIG_DAEMON_MODE */


/**********************************************************************************************************************/
/****************************************************   ENUMS  * ******************************************************/
/**********************************************************************************************************************/

/* Transport types.  */
typedef enum qsTransport_e
{
    QS_UART_E = 0,
    QS_SPI_E,
    QS_SDIO_E,
    QS_TRANSPORT_MAX_E
} qsTransport_t;


/* Packet types.  */
typedef enum qsPacketType_e
{
    QS_PACKET_E,
    QS_RETURN_E,
    QS_ASYNC_E,
} qsPacketType_t;


/* Function return values */
typedef enum SerStatus_e
{
    ssSuccess,
    ssFailure,
    ssUnknownFile,
    ssUnkownCommand,
    ssInvalidParameter,
    ssInvalidLength,
    ssAllocationError,
    ssIncompleteData
} SerStatus_t;


/**********************************************************************************************************************/
/**************************************************   STRUCTURES   ****************************************************/
/**********************************************************************************************************************/

#ifdef CONFIG_DAEMON_MODE
/* Following structure are used for HTC and MML header which are used only in daemon mode */

/* HTC header */
typedef PACKED struct HtcHeader_s
{
    uint16_t TotalLength;  /* Total packet length */
    uint16_t Reserved;     /* Reserved for future use */
} PACKED_END HtcHeader_t;
#define HTC_HEADER(_x)       ((char *)(&((uint8_t *)(_x))[0]))

typedef PACKED struct MmlHeader_s
{
    uint8_t SqId;      /* Service ID */
    uint8_t Reserved;  /* Reserved for future use */
    uint32_t Cookie;   /* Unique ID used by the daemon */
} PACKED_END MmlHeader_t;
#define MML_HEADER(_x)           ((char *)(&((uint8_t *)(_x))[QS_HTC_HEADER_LEN]))

#endif /* CONFIG_DAEMON_MODE */

#define QSPACKET_HEADER(_x)      ((char *)(&((uint8_t *)(_x))[QS_IOTD_HEADER_LEN]))

/* Defines the packed QAPI header structure. */
typedef PACKED struct qsPacketHeader_s
{
    uint16_t TotalLength;  /* Total QAPI command packet length, excludes HTC and MML header lengths */
    uint8_t  ModuleID;     /* Module ID, example - QS_MODULE_WLAN */
    uint8_t  PacketType;   /* Type of packet, qsPacketType_t */
    uint8_t  UID[8];       /* Unique ID used by the host, should be copied from QAPI request packet to response packet */
    uint16_t FileID;       /* File ID which includes requested QAPI command */
    uint16_t FunctionID;   /* Requested QAPI */
    uint16_t DataLength;   /* Total length of command arguments */
} PACKED_END qsQapiPacketHeader_t;

/* Defines the packed QAPI command structure. */
typedef PACKED struct qsQapiReq_s
{
    qsQapiPacketHeader_t Header;  /* Command header */
    uint8_t  Data[1];             /* Command arguments */
} PACKED_END qsQapiReq_t;

#define QSCOMMAND_T_SIZE(_x)     (sizeof(qsQapiReq_t) - 1 + (_x))
#define QSCOMMAND_DATA(_x)       ((uint8_t *)((unsigned int)(_x) + ((unsigned int)((qsQapiReq_t *)0)->Data)))

/* Defines the packed QAPI response structure. */
typedef PACKED struct qsQapiResp_s {
    qsQapiPacketHeader_t Header;  /* Response header */
    uint8_t  Data[1];             /* Response arguments */
} PACKED_END qsQapiResp_t;

#define QSRESPONSE_T_SIZE(_x)    (sizeof(qsQapiResp_t) - 1 + (_x))
#define QSRESPONSE_DATA(_x)      ((uint8_t *)((unsigned int)(_x) + ((unsigned int)((qsQapiResp_t *)0)->Data)))

/* Defines the packed asynchronous event structure. */
typedef PACKED struct qsEvent_s {
    qsQapiPacketHeader_t Header;  /* Event header */
    uint8_t  Data[1];             /* Event arguments */
} PACKED_END qsEvent_t;

#define QSEVENT_T_SIZE(_x)       (sizeof(qsEvent_t) - 1 + (_x))
#define QSEVENT_DATA(_x)         ((uint8_t *)((unsigned int)(_x) + ((unsigned int)((qsEvent_t *)0)->Data)))

/* Defines the packed debug callback structure. */
typedef PACKED struct qsDebugEvent_s {
    uint8_t  ModuleID;     /* Module ID, example - QS_MODULE_WLAN */
    uint8_t  FileID;       /* File ID which includes requested QAPI command */
    uint16_t EventID;      /* Event ID */
} PACKED_END qsDebugEvent_t;
#define QSDEBUGEVENT_T_SIZE      (sizeof(qsDebugEvent_t))

#define QSDEBUGEVENT_FILEID      (0xFF)
#define QSDEBUGEVENT_EVENTID     (0xFFFF)

typedef struct PackedBuffer_s
{
    uint8_t        *Packet;     /* Data buffer. */
    qsPacketType_t PacketType;  /* Type of the packet. */
    uint32_t       Remaining;   /* Number of bytes left to be processed. */
    uint32_t       Length;      /* Length of data from 'Start'. */
    uint8_t        *Start;      /* Pointer to start of allocated buffer. */
    uint8_t        *CurrentPos; /* Current position in the buffer, used while processing. */
} PackedBuffer_t;

typedef struct BufferListEntry_s
{
    struct BufferListEntry_s *Next;
} BufferListEntry_t;


/**
  Initialize packed buffer.

  @param Buffer Pointer to the buffer to initialize
  @param Value  Pointer to value to be packed.

*/
void InitializePackedBuffer(PackedBuffer_t *Buffer, uint8_t *Start, uint32_t Length);

/**
  Allocates a buffer and adds it to the provided linked list.

  @param BufferList The buffer list to add the entry to.
  @param Size       Required size of the buffer.

  @return The newly allocated buffer or NULL if there was an error.
  */
void *AllocateBufferListEntry(BufferListEntry_t **BufferList, uint16_t Size);

/**
  Frees a buffer list.

  @param BufferList Buffer list to free.
  */
void FreeBufferList(BufferListEntry_t **BufferList);

/**
  Write a packed value (int).

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to value to be packed.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_int(PackedBuffer_t *Buffer, int *Value);

/**
  Write a packed value (8 bit unsigned value).

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to value to be packed.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_8(PackedBuffer_t *Buffer, uint8_t *Value);

/**
  Write a packed value (8 bit unsigned value).

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to value to be packed.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_16(PackedBuffer_t *Buffer, uint16_t *Value);

/**
  Write a packed value (32 bit unsigned value).

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to value to be packed.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_32(PackedBuffer_t *Buffer, uint32_t *Value);

/**
  Write a packed value (32 bit unsigned value).

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to value to be packed.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_64(PackedBuffer_t *Buffer, uint64_t *Value);

/**
  Write an array value.

  @param Buffer       Pointer to the buffer to pack the value
  @param Value        Pointer to value to be packed.
  @param ElementSize  Length of one element.
  @param ElementCount Number of elements.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_Array(PackedBuffer_t *Buffer, void *Value, uint32_t ElementSize, uint32_t ElementCount);

/**
  Writes a pointer header value. Accepts a NULL pointer.

  @param Buffer Pointer to the buffer to pack the value
  @param Value  Pointer to the value whose header to write.

  @return the status result of the write.
  */
SerStatus_t PackedWrite_PointerHeader(PackedBuffer_t *Buffer, void *Value);

/**
  Read a packed value (int).

  @param InputBuffer Pointer to the packed buffer to be
  allocated.
  @param BufferList  List of Buffers.
  @param Value       Pointer to place unpacked value.

  @return the status result of the read.
  */
SerStatus_t PackedRead_int(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, int *Value);

/**
  Read a packed value (8 bit unsigned value).

  @param InputBuffer Pointer to the packed buffer to be
  allocated.
  @param BufferList  List of Buffers.
  @param Value       Pointer to place unpacked value.

  @return the status result of the read.
  */
SerStatus_t PackedRead_8(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, uint8_t *Value);

/**
  Read a packed value (16 bit unsigned value).

  @param InputBuffer Pointer to the packed buffer to be
  allocated.
  @param BufferList  List of Buffers.
  @param Value       Pointer to place unpacked value.

  @return the status result of the read.
  */
SerStatus_t PackedRead_16(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, uint16_t *Value);

/**
  Read a packed value (32 bit unsigned value).

  @param InputBuffer Pointer to the packed buffer to be
  allocated.
  @param BufferList  List of Buffers.
  @param Value       Pointer to place unpacked value.

  @return the status result of the read.
  */
SerStatus_t PackedRead_32(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, uint32_t *Value);

/**
  Read a packed value (64 bit unsigned value).

  @param InputBuffer Pointer to the packed buffer to be
  allocated.
  @param BufferList  List of Buffers.
  @param Value       Pointer to place unpacked value.

  @return the status result of the read.
  */
SerStatus_t PackedRead_64(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, uint64_t *Value);

/**
  Read an array value.

  @param InputBuffer  Pointer to the packed buffer to be
  allocated.
  @param BufferList   List of Buffers.
  @param Value        Pointer to place unpacked value.
  @param ElementSize  Length of one element.
  @param ElementCount Number of elements.

  @return the status result of the read.
  */
SerStatus_t PackedRead_Array(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, void *Value, uint32_t ElementSize, uint32_t ElementCount);

/**
  Read a pointer header value and checks if the pointer is valid.

  @param InputBuffer  Pointer to the packed buffer to be
  allocated.
  @param BufferList   List of Buffers.
  @param PointerValid Output pointer to the pointer header
  validity, TRUE if non-NULL and FALSE
  if NULL.

  @return the status result of the read.
  */
SerStatus_t PackedRead_PointerHeader(PackedBuffer_t *InputBuffer, BufferListEntry_t **BufferList, Boolean_t *PointerValid);

/**
  Calculates the packed size of an integer value.

  @param Value       Pointer to value.

  @return the packed size of the value.
  */
uint32_t CalcPackedSize_int(int *Value);

/**
  Calculates the packed size of an 8 bit value.

  @param Value       Pointer to value.

  @return the packed size of the value.
  */
uint32_t CalcPackedSize_8(uint8_t *Value);

/**
  Calculates the packed size of a 16 bit value.

  @param Value       Pointer to value.

  @return the packed size of the value.
  */
uint32_t CalcPackedSize_16(uint16_t *Value);

/**
  Calculates the packed size of a 32 bit value.

  @param Value       Pointer to value.

  @return the packed size of the value.
  */
uint32_t CalcPackedSize_32(uint32_t *Value);

/**
  Calculates the packed size of a 64 bit value.

  @param Value       Pointer to value.

  @return the packed size of the value.
  */
uint32_t CalcPackedSize_64(uint64_t *Value);

#endif // __QS_COMMMON_H__

