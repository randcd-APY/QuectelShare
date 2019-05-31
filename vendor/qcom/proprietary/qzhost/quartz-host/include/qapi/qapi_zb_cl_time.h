/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_cl_time.h
   @brief QAPI for the ZigBee Time cluster.

   @addtogroup qapi_zb_cl_time
   @{

   This API provides definitions, commands, and events related to the ZigBee
   Time cluster.

   The time server cluster automatically exists on all registered endpoints
   (except endpoint zero, which is used by ZDP), and the majority of the
   functionality is handled internally by the ZigBee stack.  An API is provided
   to retrieve the handle for the time server to be used with the ZCL APIs.

   An API is provided to create a time client cluster that can be used for
   querying remote time servers.

   @}
*/

#ifndef  __QAPI_ZB_CL_TIME_H__ // [
#define  __QAPI_ZB_CL_TIME_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"
#include "qapi_zb_cl.h"

/** @addtogroup qapi_zb_cl_time
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/* The following definitions represents the attribute IDs for the time cluster. */
#define QAPI_ZB_CL_TIME_ATTR_ID_TIME                                    (0x0000) /**< Time cluster attribute ID for the current time (UTC). */
#define QAPI_ZB_CL_TIME_ATTR_ID_TIME_STATUS                             (0x0001) /**< Time cluster attribute ID for the time status (map8). */
#define QAPI_ZB_CL_TIME_ATTR_ID_TIME_ZONE                               (0x0002) /**< Time cluster attribute ID for the current time zone (int32). */
#define QAPI_ZB_CL_TIME_ATTR_ID_DST_START                               (0x0003) /**< Time cluster attribute ID for the time (in UTCTime format) when DST begins
                                                                                      (uint32). */
#define QAPI_ZB_CL_TIME_ATTR_ID_DST_END                                 (0x0004) /**< Time cluster attribute ID for the time (in UTCTime format) when DST ends
                                                                                      (uint32). */
#define QAPI_ZB_CL_TIME_ATTR_ID_DST_SHIFT                               (0x0005) /**< Time cluster attribute ID for the time offset from standard time for DST
                                                                                      (int32). */
#define QAPI_ZB_CL_TIME_ATTR_ID_STANDARD_TIME                           (0x0006) /**< Time cluster attribute ID for the standard time of the device (read only,
                                                                                      uint32). */
#define QAPI_ZB_CL_TIME_ATTR_ID_LOCAL_TIME                              (0x0007) /**< Time cluster attribute ID for the local time of the device (read only,
                                                                                      uint32). */
#define QAPI_ZB_CL_TIME_ATTR_ID_LAST_SET_TIME                           (0x0008) /**< Time cluster attribute ID for the last time the time attribute was updated
                                                                                      (read only, UTC). */
#define QAPI_ZB_CL_TIME_ATTR_ID_VALID_UNTIL_TIME                        (0x0009) /**< Time cluster attribute ID for the time at which the time value may not be
                                                                                      considered accurate (read only, UTC). */

/* The following definitions are the bit mask values for the Time Status
   attribute. */
#define QAPI_ZB_CL_TIME_TIME_STATUS_MASK_MASTER                         (0x01) /**< Bitmask for the time status attribute indicating that the time attribute
                                                                                    is internally set to the time standard.  This bit is read only. */
#define QAPI_ZB_CL_TIME_TIME_STATUS_MASK_SYNCHRONIZED                   (0x02) /**< Bitmask for the time status attribute indicating that the time has been
                                                                                    set over the ZigBee network to synchronize it to the time standard. This
                                                                                    bit will be zero if the master bit is one. */
#define QAPI_ZB_CL_TIME_TIME_STATUS_MASK_MASTER_ZONE_DST                (0x04) /**< Bitmask for the time status attribute indicating that the time zone,
                                                                                    DST start, DST end, and DST shift values are internally set to the correct
                                                                                    time for the location of the network. */
#define QAPI_ZB_CL_TIME_TIME_STATUS_MASK_SUPERSEDING                    (0x08) /**< Bitmask for the time status attribute indicating that the time value should
                                                                                    supersede other times on the network. */

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Reads an attribute from the local time cluster server.

   @param[in]     ZB_Handle Handle of the ZigBee instance.
   @param[in]     AttrId    ID of the attribute to read.
   @param[in,out] Length    Pointer to where the length of the requested
                            attribute is stored. If the value in it is not the
                            actual attribute length, this function will return
                            QAPI_ERR_BOUNDS and set the actual length into the
                            memory identified by this pointer. Otherwise, this
                            function will try to read the attribute specified.
   @param[out]    Data      Pointer to the buffer that is to store the requested
                            attribute upon successful return.

   @return
     - QAPI_OK          if the request executed successfully.
     - QAPI_ERR_BOUNDS  if the buffer holding the attribute is not big enough.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Time_Server_Read_Attribute(qapi_ZB_Handle_t ZB_Handle, uint16_t AttrId, uint16_t *Length, uint8_t *Data);

/**
   @brief Writes an attribute on the local time cluster server.

   This function can be used to set read only attributes for the local time
   cluster server.

   @param[in] ZB_Handle Handle of the ZigBee instance.
   @param[in] AttrId    Attribute ID that is to be written.
   @param[in] Length    Length of the attribute that is to be written.
   @param[in] Data      Pointer to the attribute value.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_CL_Time_Server_Write_Attribute(qapi_ZB_Handle_t ZB_Handle, uint16_t AttrId, uint16_t Length, const uint8_t *Data);

#endif // ] #ifndef __QAPI_ZB_ZCL_TIME_H__

/** @} */


