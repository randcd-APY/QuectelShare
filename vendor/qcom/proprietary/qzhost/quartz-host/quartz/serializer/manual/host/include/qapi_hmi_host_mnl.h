/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

typedef PACKED struct HMI_Key_Descriptor_s
{
   uint64_t Source;
   uint8_t  IdMode;
   uint8_t  Index;
   uint16_t FrameUsage;
   uint8_t  Key[16];
} PACKED_END HMI_Key_Descriptor_t;

#define HMI_KEY_DESCRIPTOR_SIZE                                (sizeof(HMI_Key_Descriptor_t))

typedef PACKED struct HMI_Device_Descriptor_s
{
   uint64_t ExtAddress;
   uint16_t ShortAddress;
   uint16_t PanId;
   uint32_t FrameCounter;
   uint16_t Flags;
   uint8_t  KeyTableIndex;
} PACKED_END HMI_Device_Descriptor_t;

#define HMI_DEVICE_DESCRIPTOR_FLAG_EXEMPT                      (0x0001)

#define HMI_DEVICE_DESCRIPTOR_SIZE                             (sizeof(HMI_Device_Descriptor_t))

typedef PACKED struct HMI_SecurityLevelDescriptor_s
{
   uint16_t FrameUsage;
   uint16_t OverrideMask;
   uint8_t  SecurityMinimum;
} PACKED_END HMI_Security_Level_Descriptor_t;

#define  HMI_SECURITY_LEVEL_DESCRIPTOR_SIZE                    (sizeof(HMI_Security_Level_Descriptor_t))

qapi_Status_t Mnl_qapi_HMI_MLME_Get_Request(uint8_t TargetID, uint32_t Interface_ID, uint8_t PIBAttribute, uint8_t PIBAttributeIndex, uint8_t *PIBAttributeLength, uint8_t *PIBAttributeValue, uint8_t *Status);

qapi_Status_t Mnl_qapi_HMI_MLME_Set_Request(uint8_t TargetID, uint32_t Interface_ID, uint8_t PIBAttribute, uint8_t PIBAttributeIndex, uint8_t PIBAttributeLength, const void *PIBAttributeValue, uint8_t *Status);
