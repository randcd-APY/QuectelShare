/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef  __QAPI_HERH_H__
#define  __QAPI_HERH_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include <stdint.h>

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/
typedef struct HERH_Handle_s *HERH_Handle_t;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Funtion used to handle the data from the host side.

   This function will receive the data from host side and set the data arriving
   event. So a callback funtion blocking on this event will handle the data
   accordingly.

   @param[in] HERH_Handle Handle returned by HERH_Init_Response.
   @param[in] DataLength  Length of the data for the response.
   @param[in] Data        Data for the response.

   @return
     - QAPI_OK if ZigBee was initialized successfully.
     - A negative value if there was an error.
*/
void qapi_HEHR_Host_Data(HERH_Handle_t HERH_Handle, uint32_t DataLength, const uint8_t *Data);

#endif // ] #ifndef __QAPI_HERH_H__

/** @} */


