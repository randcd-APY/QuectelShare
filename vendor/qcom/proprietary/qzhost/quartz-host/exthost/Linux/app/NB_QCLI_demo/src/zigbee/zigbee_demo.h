/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __ZIGBEE_DEMO_H__
#define __ZIGBEE_DEMO_H__

#include "qcli_api.h"

#include "qapi_zb.h"
#include "qapi_zb_cl_identify.h"

/* This enumeration defines the device types that can be added to the demo's
   device list. */
typedef enum
{
   DEVICE_ID_TYPE_UNUSED,
   DEVICE_ID_TYPE_NWK_ADDR,
   DEVICE_ID_TYPE_GROUP_ADDR
} Device_ID_Type_t;

/* This structure represents a registered device/group address. */
typedef struct ZB_Device_ID_s
{
   qbool_t             InUse;
   qapi_ZB_Addr_Mode_t Type;
   qapi_ZB_Addr_t      Address;
   uint8_t             Endpoint;
} ZB_Device_ID_t;

/* The number of scenes to have space allocated for the Scenes cluster. */
#define APP_MAX_NUM_SCENES                                              (4)

/* The macro defines the TX option used by the ZigBee demo. */
#define ZIGBEE_DEMO_TX_OPTION_WITH_SECURITY                             (QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_SECURE_TRANSMISSION | \
                                                                         QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_USE_NWK_KEY | \
                                                                         QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_ACKNOWLEDGED_TRANSMISSION | \
                                                                         QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_FRAGMENTATION_PERMITTED)

#define ZIGBEE_DEMO_TX_OPTION_WITHOUT_SECURITY                          (QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_ACKNOWLEDGED_TRANSMISSION | \
                                                                         QAPI_ZB_APSDE_DATA_REQUEST_TX_OPTION_FRAGMENTATION_PERMITTED)


/* The following macros are used to read little endian data. */
#define READ_UNALIGNED_LITTLE_ENDIAN_UINT8(__src__)                     (((uint8_t *)(__src__))[0])

#define READ_UNALIGNED_LITTLE_ENDIAN_UINT16(__src__)                    ((uint16_t)((((uint16_t)(((uint8_t *)(__src__))[1])) << 8) | \
                                                                          ((uint16_t)(((uint8_t *)(__src__))[0]))))

#define READ_UNALIGNED_LITTLE_ENDIAN_UINT24(__src__)                    ((((uint32_t)(((uint8_t *)(__src__))[2])) << 16) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[1])) << 8) | \
                                                                          ((uint32_t)(((uint8_t *)(__src__))[0])))

#define READ_UNALIGNED_LITTLE_ENDIAN_UINT32(__src__)                    ((uint32_t)((((uint32_t)(((uint8_t *)(__src__))[3])) << 24) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[2])) << 16) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[1])) << 8) | \
                                                                          ((uint32_t)(((uint8_t *)(__src__))[0]))))

#define READ_UNALIGNED_LITTLE_ENDIAN_UINT40(__src__)                    ((uint64_t)((((uint64_t)(((uint8_t *)(__src__))[4])) << 32) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[3])) << 24) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[2])) << 16) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[1])) << 8) | \
                                                                          ((uint32_t)(((uint8_t *)(__src__))[0]))))

#define READ_UNALIGNED_LITTLE_ENDIAN_UINT48(__src__)                    ((uint64_t)((((uint64_t)(((uint8_t *)(__src__))[5])) << 40) | \
                                                                          (((uint64_t)(((uint8_t *)(__src__))[4])) << 32) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[3])) << 24) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[2])) << 16) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[1])) << 8) | \
                                                                          ((uint32_t)(((uint8_t *)(__src__))[0]))))

#define READ_UNALIGNED_LITTLE_ENDIAN_UINT56(__src__)                    ((uint64_t)((((uint64_t)(((uint8_t *)(__src__))[6])) << 48) | \
                                                                          (((uint64_t)(((uint8_t *)(__src__))[5])) << 40) | \
                                                                          (((uint64_t)(((uint8_t *)(__src__))[4])) << 32) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[3])) << 24) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[2])) << 16) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[1])) << 8) | \
                                                                          ((uint32_t)(((uint8_t *)(__src__))[0]))))

#define READ_UNALIGNED_LITTLE_ENDIAN_UINT64(__src__)                    ((uint64_t)((((uint64_t)(((uint8_t *)(__src__))[7])) << 56) | \
                                                                          (((uint64_t)(((uint8_t *)(__src__))[6])) << 48) | \
                                                                          (((uint64_t)(((uint8_t *)(__src__))[5])) << 40) | \
                                                                          (((uint64_t)(((uint8_t *)(__src__))[4])) << 32) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[3])) << 24) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[2])) << 16) | \
                                                                          (((uint32_t)(((uint8_t *)(__src__))[1])) << 8) | \
                                                                          ((uint32_t)(((uint8_t *)(__src__))[0]))))

/**
   @brief Registers the ZigBee interface commands with QCLI.
*/
void Initialize_ZigBee_Demo(void);

/**
   @brief Un-Registers the ZigBee interface commands with QCLI.
*/
void Cleanup_ZigBee_Demo(void);

/**
   @brief Helper function to format the send information for a packet.

   @param DeviceIndex is the index of the device to be sent.
   @param SendInfo    is a pointer to where the send information will be
                      formatted upon successful return.

   @return true if the send info was formatted successfully, false otherwise.
*/
qbool_t Format_Send_Info_By_Device(uint32_t DeviceIndex, qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Helper function to format the send information for a packet.

   @param ReceiveInfo is the receive information for an event.
   @param SendInfo    is a pointer to where the send information will be
                      formatted upon successful return.

   @return true if the send info was formatted successfully, false otherwise.
*/
qbool_t Format_Send_Info_By_Receive_Info(const qapi_ZB_CL_General_Receive_Info_t *ReceiveInfo, qapi_ZB_CL_General_Send_Info_t *SendInfo);

/**
   @brief Function to get a specified entry from the ZigBee demo's device list.

   @param DeviceID is the index of the device to retrieve.

   @return a pointer to the device list entry or NULL if either the DeviceID was
           not valid or not in use.
*/
ZB_Device_ID_t *GetDeviceListEntry(uint32_t DeviceID);

/**
   @brief Function to get the ZigBee stack's handle.

   @return The handle of the ZigBee stack.
*/
qapi_ZB_Handle_t GetZigBeeHandle(void);

/**
   @brief Function to get the next sequence number for sending packets.

   @return the next sequence number to be used for sending packets.
*/
uint8_t GetNextSeqNum(void);

/**
   @brief Function to get the QCLI handle for the ZigBee demo.

   @return The QCLI handled used by the ZigBee demo.
*/
QCLI_Group_Handle_t GetZigBeeQCLIHandle(void);

/**
   @brief Helper function that displays variable length value.

   @param Group_Handle is the QCLI group handle.
   @param Data_Length  is the length of the data to be displayed.
   @param Data         is the data to be displayed.
*/
void DisplayVariableLengthValue(QCLI_Group_Handle_t Group_Handle, uint16_t Data_Length, const uint8_t *Data);

#endif

