/*
* Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
// $QTI_LICENSE_QDN_C$


#ifndef __QAPI_OTP_TLV_H__
#define __QAPI_OTP_TLV_H__

#ifdef   __cplusplus
extern "C" {
#endif


/*
 * @file qapi_otp_tlv.h
 *
 * @addtogroup qapi_otp_tlv
 * @{
 * The OTP_TLV module supports Tag/Length/Value storage within one-time programmable memory.
 *
 * This module allows writes, typically manufacturing, of TLVs to OTP
 * and run-time retrieval of TLV value by tag. TLVs are stored sequentially
 * in the order in which they are written, from low to high offsets within a
 * designated area of OTP. A given tag should appear at most once within the
 * area. If a tag appears more than once, only the first instance may be
 * retrieved. If a TLV is deleted, its tag is set to a deleted
 * tag value, but the TLV continues to consume space in OTP. Currently, tags
 * are 4-bit values: \n
 *   0 --> Unused TLV \n
 *   1..10 --> System tag \n
 *   11..14 --> Application tag \n
 *   15 --> Deleted TLV
 *
 * This module is divided into several object files. This allows APIs to
 * be included or excluded as desired. For instance, one may choose to
 * include OTP_TLV write in manufacturing firmware but not in Mission
 * Mode firmware.
 * @}
 */

#include "qapi/qapi_status.h"

/** @addtogroup qapi_otp_tlv
@{ */

/* The following definitions are the tag values used by the system. */
#define QAPI_OTP_TLV_TAG_BLE_ADDRESS            (1)  /**< OTP TLV TAG for the BLE address (BD_ADDR). */
#define QAPI_OTP_TLV_TAG_I15P4_ADDRESS          (2)  /**< OTP TLV TAG for the 15.4 address (EUI64). */
#define QAPI_OTP_TLV_TAG_XTAL_CAL  		        (3)  /**< OTP TLV TAG for the XTAL Calibration value. */
#define QAPI_OPT_TLV_TAG_EXT_BDF				(4)  /**< OTP TLV TAG for the ext board data file select>**/
#define QAPI_OTP_TLV_TAG_APP_MIN                (11) /**< Minimum tag value available for application specific OTP tags. Tags less
                                                          than this value are reserved. */

#define QAPI_OTP_TLV_TAG_APP_MAX                (14) /**< Minimum tag value available for application specific OTP tags. Tags
                                                          greater than this value are reserved. */

#define QAPI_OTL_TLV_TAG_BLE_ADDRESS_LENGTH     (6)  /**< Length of the OTP BLE address. */
#define QAPI_OTP_TLV_TAG_I15P4_ADDRESS_LENGTH   (8)  /**< Length of the OTP 15.4 address. */


/**
 * @brief Initializes the OTP_TLV module to work within a given range of OTP.
 *
 * If the Read, Write or Status APIs are called before this function is
 * called, the module auto-initializes to platform default values.
 *
 * Each time this API is called after the first time, it reinitializes,
 * using the new parameters.
 *
 * @param[in] otp_start       Start of the OTP region. If  0, the module uses system default values.
 * 
 * @param[out] otp_num_bytes  Size of the OTP region. If  0, the module uses system default values.
 *
 * @return
 * QAPI_ERR_INVALID_PARAM if start+num_bytes is too large. \n
 * QAPI_OK otherwise.
 *
 */
extern qapi_Status_t qapi_OTP_TLV_Init(
        unsigned int otp_start,
        unsigned int otp_num_bytes);


/**
 * @brief Searches for an OTP TLV with the specified tag and return ups to
 * max_length bytes in the supplied buffer. The actual length of the OTP TLV is
 * returned in the actual_length parameter regardless of whether the actual length
 * of the OTP TLV is larger than or smaller than max_length.
 *
 * @param[in] tag             Tag to identify the TLV.
 * 
 * @param[out] buffer         Buffer supplied by the caller for the TLV.
 * 
 * @param[in] max_length      Maximum allowed length.
 *
 * @param[out] actual_length  Actual length of the returned TLV.
 *
 * @return
 * QAPI_ERR_INVALID_PARAM if the tag is not valid. \n
 * QAPI_ERR_NO_ENTRY if the tag is not found. \n
 * QAPI_OK otherwise.
 *
 */
extern qapi_Status_t qapi_OTP_TLV_Read(
        unsigned int tag,
        uint8_t *buffer,
        unsigned int max_length,
        unsigned int *actual_length);



/**
 * @brief Searches for an OTP TLV with the specified tag and returns the
 * offset in the OTP of the start of the data value along with the length of the
 * value according to the TLV header in OTP. This QAPI always finds the first
 * matching tag.
 *
 * @details This API is not intended for typical Mission Mode use, but it may
 * be useful for special situations and for debug.
 *
 * @param[in] tag       Tag to identify the TLV
 * 
 * @param[out] pOffset  If the tag is found, pOffset is updated to the starting offset
 *                      of the valueor data associated with the tag. If the tag is not found,
 *                      pOffset is updated to the offset where data for the next OTP TLV
 *                      is to be placed. Note that if the OTP TLV area is full, this may
 *                      be beyond the end of the OTP TLV area.
 * 
 * @param[out] pLength  Length of the OPT TLV. This is only updated if the tag is found.
 *
 * @return
 * QAPI_ERR_INVALID_PARAM if the tag is not valid. \n
 * QAPI_ERR_NO_ENTRY if the tag is not found. \n
 * QAPI_ERROR if the module is not initialized. \n
 * QAPI_OK otherwise
 *
 */
extern qapi_Status_t qapi_OTP_TLV_Find(
        unsigned int tag,
        unsigned int *pOffset,
        unsigned int *pLength);



/**
 * @brief Appends a new OTP TLV to the OTP TLV area. It is the caller's
 * responsibility to manage or avoid duplicate tags. This QAPI simply appends the new
 * OTP TLV to the end of the OTP TLV list.
 *
 * @param[in] tag     Tag to identify the TLV.
 * 
 * @param[in] buffer  Buffer containing the new TLV.
 * 
 * @param[in] length  Length of new TLV.
 *
 * @return
 * QAPI_ERROR if the module is not initialized. \n
 * QAPI_ERR_NOT_SUPPORTED if the OTP TLV area is currently invalid. \n
 * QAPI_ERR_INVALID_PARAM if the length of this OTP TLV exceeds
 *     the size of the OTP TLV area. \n
 * QAPI_ERR_NO_RESOURCE if there is insufficient remaining space in
 *      the OTP TLV area to hold this OTP TLV.
 * QAPI_OK if the new OTP TLV was committed.
 *
 */
extern qapi_Status_t qapi_OTP_TLV_Write(
        unsigned int tag,
        uint8_t *buffer,
        unsigned int length);



/**
 * @brief Deletes an OTP TLV from the OTP TLV area.
 * Deleted OTP TLVs continue to consume space indefinitely in the OTP TLV area.
 *
 * @param[in] tag  Tag to identify the TLV.
 *
 * @return
 * QAPI_ERR_INVALID_PARAM if the tag is not valid. \n
 * QAPI_ERR_NO_ENTRY if the tag was not found. \n
 * QAPI_OK if the tag was found the the corresponding OTP TLV was deleted.
 *
 */
extern qapi_Status_t qapi_OTP_TLV_Delete(unsigned int tag);

/**
 * @brief Reflects the current status of the OTP TLV area. 
 */
struct qapi_OTP_TLV_Status_s {
    uint32_t area_start_offset; /**< Offset into the start of the OTP TLV area. */
    uint32_t area_size;         /**< Size in bytes of the OTP TLV area. */
    uint32_t bytes_consumed;    /**< Bytes of the OTP consumed in the OTP TLV area. */
    uint32_t num_SYS_TLV;       /**< Number of SYS-tagged TLVs. */
    uint32_t num_SYS_byte;      /**< Number of bytes in the SYS-tagged TLVs. */
    uint32_t num_APP_TLV;       /**< Number of application-tagged TLVs. */
    uint32_t num_APP_byte;      /**< Number of bytes in the application-tagged TLVs. */
    uint32_t num_DEL_TLV;       /**< Number of deleted TLVs. */
    uint32_t num_DEL_byte;      /**< Number of bytes in deleted TLVs. */
};

/* Reflects XTAL value for capin and capout. */
typedef struct OTP_TLV_xtal_cal_s {
    uint8_t capin;
    uint8_t capout;
}OTP_TLV_XTAL_CAL;


/**
 * @brief Returns the current status of the OTP TLV area.
 *
 * @param[out] otp_tlv_status  Current status of the OTP TLV area defined by #qapi_OTP_TLV_Status_s.
 *
 * @return
 * QAPI_ERROR if the module is not initialized. \n
 * QAPI_ERR_NOT_SUPPORTED if the OTP TLV area is currently invalid
 *                           (otp_tlv_status is updated regardless).
 * QAPI_OK if the OTP TLV area is in good condition.
 *
 */
extern qapi_Status_t qapi_OTP_TLV_Status(struct qapi_OTP_TLV_Status_s *otp_tlv_status);

#ifdef   __cplusplus
}
#endif

/** @} */

#endif /* __QAPI_OTP_TLV_H__ */
