/*
 * Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_ver.h
 * @addtogroup qapi_version 
 * @{
 * @brief QAPI version information
 *
 * @details This file provides the version information for the QAPI.
 * @}  
 */

#ifndef __QAPI_VER_H__ // [
#define __QAPI_VER_H__

#include "qapi_status.h"

/** @addtogroup qapi_build_info
@{ */

#define QAPI_VERSION_MAJOR                                     (2)
#define QAPI_VERSION_MINOR                                     (0)
#define QAPI_VERSION_NIT                                       (1)

#define __QAPI_VERSION_MAJOR_MASK                             (0xff000000)
#define __QAPI_VERSION_MINOR_MASK                             (0x00ff0000)
#define __QAPI_VERSION_NIT_MASK                               (0x0000ffff)

#define __QAPI_VERSION_MAJOR_SHIFT                             (24)
#define __QAPI_VERSION_MINOR_SHIFT                             (16)
#define __QAPI_VERSION_NIT_SHIFT                               (0)

#define __QAPI_ENCODE_VERSION(__major__, __minor__, __nit__)   (((__major__) << __QAPI_VERSION_MAJOR_SHIFT) | \
                                                                ((__minor__) << __QAPI_VERSION_MINOR_SHIFT) | \
                                                                ((__nit__)   << __QAPI_VERSION_NIT_SHIFT))

/*----------------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------------*/
/**
 * Data structure used by application to get build information.
 */
typedef struct {
    uint32_t qapi_Version_Number;
    /**< qapi version number */
    uint32_t crm_Build_Number;
    /**< CRM build number */
} qapi_FW_Info_t;

/*----------------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------------*/
/**
 * This API allows user to retrieve version information from system. \n
 *
 * @param[out]    info          Value retrieved from system.
 *
 * @return        QAPI_OK -- Requested parameter retrieved from the system. \n
 *                Non-Zero value -- Parameter retrieval failed.
 *  
 * @dependencies          None.
 */
qapi_Status_t qapi_Get_FW_Info(qapi_FW_Info_t *info);

//const uint32_t qapi_Version_Number = __QAPI_ENCODE_VERSION(QAPI_VERSION_MAJOR, QAPI_VERSION_MINOR, QAPI_VERSION_NIT);
/** @} */
#endif // ] #ifndef __QAPI_VER_H__

