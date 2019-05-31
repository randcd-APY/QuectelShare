#ifndef CSR_BT_SDC_SUPPORT_H     /* Once is enough */
#define CSR_BT_SDC_SUPPORT_H
/******************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Defines the Type Description */
#define CSR_BT_DATA_ELEMENT_NIL_TYPE                                        0x00
#define CSR_BT_DATA_ELEMENT_SIZE_DESCRIPTOR_MASK                            0x07
#define CSR_BT_DATA_ELEMENT_UNSIGNED_INTEGER_TYPE                           0x08
#define CSR_BT_DATA_ELEMENT_SIGNED_INTEGER_TYPE                             0x10
#define CSR_BT_DATA_ELEMENT_UUID_TYPE                                       0x18
#define CSR_BT_DATA_ELEMENT_TEXT_STRING_TYPE                                0x20
#define CSR_BT_DATA_ELEMENT_BOOLEAN_TYPE                                    0x28
#define CSR_BT_DATA_ELEMENT_SEQUENCE_TYPE                                   0x30
#define CSR_BT_DATA_ELEMENT_ALTERNATIVE_TYPE                                0x38
#define CSR_BT_DATA_ELEMENT_URL                                             0x40
#define CSR_BT_DATA_ELEMENT_TYPE_DESCRIPTOR_MASK                            0xF8

#define CSR_BT_DATA_ELEMENT_SIZE_1_BYTE                                     0x00
#define CSR_BT_DATA_ELEMENT_SIZE_2_BYTES                                    0x01
#define CSR_BT_DATA_ELEMENT_SIZE_4_BYTES                                    0x02
#define CSR_BT_DATA_ELEMENT_SIZE_8_BYTES                                    0x03
#define CSR_BT_DATA_ELEMENT_SIZE_16_BYTES                                   0x04
#define CSR_BT_DATA_ELEMENT_SIZE_NEXT_1_BYTE                                0x05
#define CSR_BT_DATA_ELEMENT_SIZE_NEXT_2_BYTES                               0x06
#define CSR_BT_DATA_ELEMENT_SIZE_NEXT_4_BYTES                               0x07


/* Defines the Attribute Identifier Codes */
#define CSR_BT_SERVICE_RECORD_HANDLE_ATTRIBUTE_IDENTIFIER                   0x0000
#define CSR_BT_SERVICE_CLASS_ID_LIST                                        0x0001
#define CSR_BT_SERVICE_RECORD_STATE_ATTRIBUTE_IDENTIFIER                    0x0002
#define CSR_BT_SERVICE_ID_ATTRIBUTE_IDENTIFIER                              0x0003
#define CSR_BT_PROTOCOL_DESCRIPTOR_LIST_ATTRIBUTE_IDENTIFIER                0x0004
#define CSR_BT_BROWSE_GROUP_LIST                                            0x0005
#define CSR_BT_LANGUAGE_BASE_ATTRIBUTE_ID_LIST_ATTRIBUTE_IDENTIFIER         0x0006
#define CSR_BT_SERVICE_AVAILABILITY_ATTRIBUTE_IDENTIFIER                    0x0008
#define CSR_BT_BLUETOOTH_PROFILE_DESCRIPTOR_LIST_ATTRIBUTE_IDENTIFIER       0x0009
#define CSR_BT_ADDITIONAL_PROTOCOL_DESCRIPTOR_LIST_ATTRIBUTE_IDENTIFIER     0x000D

#define CSR_BT_SERVICE_NAME_ATTRIBUTE_IDENTIFIER                            0x0100
#define CSR_BT_SERVICE_DESCRIPTION_ATTRIBUTE_IDENTIFIER                     0x0101
#define CSR_BT_SERVICE_PROVIDER_NAME                                        0x0102

#define CSR_BT_SPECIFICATION_ID_ATTRIBUTE_IDENTIFIER                        0x0200
#define CSR_BT_VENDOR_ID_ATTRIBUTE_IDENTIFIER                               0x0201
#define CSR_BT_PRODUCT_ID_ATTRIBUTE_IDENTIFIER                              0x0202
#define CSR_BT_VERSION_ATTRIBUTE_IDENTIFIER                                 0x0203
#define CSR_BT_PRIMARY_RECORD_ATTRIBUTE_IDENTIFIER                          0x0204
#define CSR_BT_VENDOR_ID_SOURCE_ATTRIBUTE_IDENTIFIER                        0x0205
#define CSR_BT_DOCUMENTATION_URL_ATTRIBUTE_IDENTIFIER                       0x000A
#define CSR_BT_CLIENT_EXECUTABLE_URL_ATTRIBUTE_IDENTIFIER                   0x000B

#define CSR_BT_NETWORK_ATTRIBUTE_IDENTIFIER                                 0x0301
#define CSR_BT_SUPPORTED_DATA_STORES_LIST_ATTRIBUTE_IDENTIFIER              0x0301
#define CSR_BT_REMOTE_AUDIO_VOLUME_CONTROL_ATTRIBUTE_IDENTIFIER             0x0302
#define CSR_BT_SUPPORTED_FORMATS_LIST_ATTRIBUTE_IDENTIFIER                  0x0303
#define CSR_BT_SECURITY_DESCRIPTION_ATTRIBUTE_IDENTIFIER                    0x030A
#define CSR_BT_NET_ACCESS_TYPE_ATTRIBUTE_IDENTIFIER                         0x030B
#define CSR_BT_MAX_NET_ACCESS_RATE_ATTRIBUTE_IDENTIFIER                     0x030C
#define CSR_BT_IPV4_SUBNET_ATTRIBUTE_IDENTIFIER                             0x030D
#define CSR_BT_IPV6_SUBNET_ATTRIBUTE_IDENTIFIER                             0x030E
#define CSR_BT_SUPPORTED_CAPABILITIES_ATTRIBUTE_IDENTIFIER                  0x0310
#define CSR_BT_SUPPORTED_FEATURES_ATTRIBUTE_IDENTIFIER                      0x0311
#define CSR_BT_SUPPORTED_FUNCTIONS_ATTRIBUTE_IDENTIFIER                     0x0312
#define CSR_BT_TOTAL_IMAGING_DATA_CAPACITY_ATTRIBUTE_IDENTIFIER             0x0313
#define CSR_BT_SUPPORTED_REPOSITORIES_ATTRIBUTE_IDENTIFIER                  0x0314
#define CSR_BT_MAS_INSTANCE_ID_ATTRIBUTE_IDENTIFIER                         0x0315
#define CSR_BT_SUPPORTED_MESSAGE_TYPES_ATTRIBUTE_IDENTIFIER                 0x0316
#define CSR_BT_OBEX_SUPPORTED_FEATURES_ATTRIBUTE_IDENTIFIER                 0x0317
#define CSR_BT_DOCUMENT_FORMATS_SUPPORTED_ATTRIBUTE_IDENTIFIER              0x0350
#define CSR_BT_CHARACTER_REPERTOIRES_SUPPORTED_ATTRIBUTE_IDENTIFIER         0x0352
#define CSR_BT_XHTML_PRINT_IMAGE_FORMATS_SUPPORTED_ATTRIBUTE_IDENTIFIER     0x0354
#define CSR_BT_COLOR_SUPPORTED__ATTRIBUTE_IDENTIFIER                        0x0356
#define CSR_BT_PRINTER_1284ID_ATTRIBUTE_IDENTIFIER                          0x0358
#define CSR_BT_DUPLEX_SUPPORTED_ATTRIBUTE_IDENTIFIER                        0x035E
#define CSR_BT_MEDIA_TYPES_SUPPORTED_ATTRIBUTE_IDENTIFIER                   0x0360
#define CSR_BT_MAX_MEDIA_WIDTH_ATTRIBUTE_IDENTIFIER                         0x0362
#define CSR_BT_MAX_MEDIA_LENGTH_ATTRIBUTE_IDENTIFIER                        0x0364
#define CSR_BT_DATA_EXCHANGE_SPECIFICATION                                  0x0301
#define CSR_BT_MCAP_SUPPORTED_FEATURES                                      0x0302
#define CSR_BT_SUPPORTED_FEATURES                                           0x0200

/* Defines Protocol UUID16 */
#define CSR_BT_RFCOMM_PROTOCOL_UUID16_IDENTIFIER                            0x0003
#define CSR_BT_OBEX_PROTOCOL_UUID16_IDENTIFIER                              0x0008
#define CSR_BT_BNEP_PROTOCOL_UUID16_IDENTIFIER                              0x000F
#define CSR_BT_L2CAP_PROTOCOL_UUID16_IDENTIFIER                             0x0100
#define CSR_BT_ATT_PROTOCOL_UUID16_IDENTIFIER                               0x0007

void CsrBtSdcGetServiceName(CsrUint8 * nameDestination, CsrUint16 nameDestinationLength, CsrUint16 attributeListLength, CsrUint8 * attributeList);
CsrUint8 CsrBtSdcGetRfcommServerChannel(CsrUint16 attributeListLength, CsrUint8 * attributeList);
psm_t CsrBtSdcGetL2capPsm(CsrUint16 attributeListLength, CsrUint8 * attributeList);
CsrUint8 CsrBtSdcGetExtendedRfcommServerChannel(CsrUint16 attributeListLength, CsrUint8 * attributeList);
CsrBool CsrBtSdcGetTextStringDirect(CsrUint8 * nameDestination,
                              CsrUint16 nameDestinationLength,
                              CsrUint16 attributeListLength,
                              CsrUint8 * attributeList,
                              CsrUint16 attributeId,
                              CsrUint32 *theSize,
                              CsrUint32 * theIndex);
CsrBool CsrBtSdcGetUint16Direct(CsrUint16 attributeListLength, CsrUint8 * attributeList, CsrUint16 attributeId, CsrUint16 *theValue);
CsrBool CsrBtSdcGetUint32Direct(CsrUint16 attributeListLength, CsrUint8 * attributeList, CsrUint16 attributeId, CsrUint32 *theValue);
CsrBool CsrBtSdcGetUint8Direct(CsrUint16 attributeListLength, CsrUint8 * attributeList, CsrUint16 attributeId, CsrUint8 *theValue);
CsrBool CsrBtSdcGetBoolDirect(CsrUint16 attributeListLength, CsrUint8 * attributeList, CsrUint16 attributeId, CsrUint8 *theValue);
CsrBool CsrBtSdcGetUint64ByteStringDirect(CsrUint16 attributeListLength, CsrUint8 * attributeList, CsrUint16 attributeId, CsrUint8 *byteString);
CsrBool CsrBtSdcGetUint128ByteStringDirect(CsrUint16 attributeListLength, CsrUint8 * attributeList, CsrUint16 attributeId, CsrUint8 *byteString);
CsrBool CsrBtSdcGetAttribute(CsrUint16 attributeListLength, CsrUint8 * attributeList, CsrUint16 theAttribute, CsrUint8 *theType, CsrUint32 *theSize, CsrUint32 * theIndex);
CsrBool CsrBtSdcGetTypeDescriptorInsideList(CsrUint16 attributeListLength,
                                      CsrUint8 * attributeList,
                                      CsrUint32 startIndex,
                                      CsrUint8 *descriptorType,
                                      CsrUint32 *descriptorLength,
                                      CsrUint32  *numOfBytesRead);
CsrBool CsrBtSdcGetUint16Value(CsrUint16 attributeListLength,
                         CsrUint8 * attributeList,
                         CsrUint32 index,
                         CsrUint16 *returnValue);
CsrUint8 CsrBtReturnSizeDescriptorValue(CsrUint8 descriptorValue);
CsrBool CsrBtGetNumOfLanguageBaseAttributeId(CsrUint16 attributeListLength,
                                       CsrUint8 * attributeList,
                                       CsrUint32 *startIndex,
                                       CsrUint16 *numOfAttributeIdElements);
CsrBool CsrBtGetLanguageBaseAttributeIdElement(CsrUint16 attributeListLength,
                                         CsrUint8 * attributeList,
                                         CsrUint32 index,
                                         CsrBtLanguageElement *languageElement,
                                         CsrUint32 *nextIndex);
CsrBool CsrBtSdcFindServiceClassUuid(CsrUint16 attributeListLength,
                               CsrUint8 * attributeList,
                               CsrBtUuid32 serviceClassUuid);
CsrBool CsrBtSdcFindServiceClass128Uuid(CsrUint16 attributeListLength,
                                  CsrUint8 * attributeList,
                                  CsrBtUuid128 serviceClassUuid);
CsrBool CsrBtSdcIndexOfUuid(CsrUint16 attributeListLength,
                      CsrUint8 * attributeList,
                      CsrBtUuid32 serviceClassUuid,
                      CsrUint32 *uuidIndex);

#ifdef __cplusplus
}
#endif

#endif
