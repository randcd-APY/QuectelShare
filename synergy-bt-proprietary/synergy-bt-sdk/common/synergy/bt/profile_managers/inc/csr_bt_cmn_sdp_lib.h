#ifndef CSR_BT_CMN_SDP_LIB_H__
#define CSR_BT_CMN_SDP_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_profiles.h"
#include "csr_bt_sd_prim.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Values taken from the Service Discovery Specification, section 3.3 */
#define SDP_DATA_ELEMENT_SIZE_1_BYTE          0
#define SDP_DATA_ELEMENT_SIZE_2_BYTES         1
#define SDP_DATA_ELEMENT_SIZE_4_BYTES         2
#define SDP_DATA_ELEMENT_SIZE_8_BYTES         3

#define SDP_DATA_ELEMENT_SIZE_NEXT_1_BYTE     5
#define SDP_DATA_ELEMENT_SIZE_NEXT_2_BYTES    6
#define SDP_DATA_ELEMENT_SIZE_NEXT_4_BYTES    7

#define SDP_DATA_ELEMENT_SIZE_8_BITS          0
#define SDP_DATA_ELEMENT_SIZE_16_BITS         1
#define SDP_DATA_ELEMENT_SIZE_32_BITS         2
#define SDP_DATA_ELEMENT_SIZE_64_BITS         3

#define SDP_DATA_ELEMENT_SIZE_NEXT_8_BITS     5
#define SDP_DATA_ELEMENT_SIZE_NEXT_16_BITS    6
#define SDP_DATA_ELEMENT_SIZE_NEXT_32_BITS    7


#define SDP_DATA_ELEMENT_BYTELENGTH_UUID16             3
#define SDP_DATA_ELEMENT_BYTELENGTH_SEQUENCE_1_BYTE    2

#ifdef CSR_BT_INSTALL_128_BIT_SERVICE_SEARCH
#define SDP_DATA_ELEMENT_SIZE_16_BYTES        4
#define SDP_DATA_ELEMENT_SIZE_128_BITS        4
#endif

CsrBool CsrBtUtilSdpCombineMemoryAndFree(CsrUint8   **data1,
                               CsrUint16    data1Len,
                               CsrUint8   **data2,
                               CsrUint16    data2Len,
                               CsrUint8   **resultingData,
                               CsrUint16   *resultingDataLen);

/* **************************************************** */
/* **************************************************** */
/* CREATE SDP DATA ELEMENTS */
/* **************************************************** */
/* **************************************************** */


    /* Creates a CsrUint16 data element with value 'value',
       according to the SDP specification.
       The pointer to the data element is returned in the 'data' pointer,
       and the length of the data is returned in the 'dataLen' variable.
       TRUE is returned if everything went ok, otherwise FALSE.
    */
    void CsrBtUtilSdpCreateUint16DataElement(CsrUint8  **data,
                                             CsrUint16 *dataLen,
                                             CsrUint16 value);

    void CsrBtUtilSdpCreateUuid16DataElement(CsrUint8  **data,
                                             CsrUint16 *dataLen,
                                             CsrUint16 value);

    void CsrBtUtilSdpCreateBooleanDataElement(CsrUint8  **data,
                                              CsrUint16 *dataLen,
                                              CsrBool   value);

    CsrBool CsrBtUtilSdpCreateServiceClassIdList(CsrUint8  **data,
                                          CsrUint16  *dataLen,
                                          CsrUint16   nofUuid,
                                          CsrUint16  *uuid);

    CsrBool CsrBtUtilSdpCreateUint16String(CsrUint8  **dataElement,
                                    CsrUint16  *dataElementLen,
                                    CsrUint16   uuid,
                                    CsrUint8   *string,
                                    CsrUint16   stringLen,
                                    CsrUint8    typeDescriptor);

    CsrBool CsrBtUtilSdpCreateUint16Uint16(CsrUint16    value1,
                                    CsrUint16    value2,
                                    CsrUint8   **data,
                                    CsrUint16   *dataLen);

    CsrBool CsrBtUtilSdpCreateUint16Boolean(CsrUint16    value1,
                                     CsrBool      value2,
                                     CsrUint8   **data,
                                     CsrUint16   *dataLen);

/* **************************************************** */
/* **************************************************** */
/* EXTRACT DATA FROM SDP DATA ELEMENTS */
/* **************************************************** */
/* **************************************************** */

    CsrBool CsrBtUtilSdpExtractDataElementSequence(CsrUint8   *attData,
                                            CsrUint16   attDataLen,
                                            CsrUint32  *sequenceSize,
                                            CsrUint16  *consumedBytes);

    CsrBool CsrBtUtilSdpUintData(CsrUint8   *attData_p,
                                 CsrUint16  attDataLen,
                                 CsrUint32  *uintDataSize,
                                 CsrUint16  *consumedBytes);

    CsrBool CsrBtUtilSdpUuidData(CsrUint8   *attData_p,
                             CsrUint16  attDataLen,
                             CsrUint32  *uintDataSize,
                             CsrUint16  *consumedBytes);

    CsrBool CsrBtUtilSdpExtractUint(CsrUint8   *attData,
                             CsrUint16   attDataLen,
                             CsrUint32  *uuid,
                             CsrUint16  *consumedBytes,
                             CsrBool     allowDataElementSequence);

    CsrBool CsrBtUtilSdpExtractString(CsrUint8   *attData,
                               CsrUint16   attDataLen,
                               CsrUint8  **string,
                               CsrUint16  *stringLen,
                               CsrUint16  *consumedBytes);

    CsrBool CsrBtUtilSdpExtractServiceClassIdList(CsrUint8   *attData,
                                           CsrUint16   attDataLen,
                                           CsrUint16  *nofServiceClassIds,
                                           CsrUint32 **serviceClassIdList,
                                           CsrUint16  *consumedBytes);

#ifdef __cplusplus
}
#endif

#endif

