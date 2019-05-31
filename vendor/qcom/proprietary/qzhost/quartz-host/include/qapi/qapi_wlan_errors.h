/*
 * Copyright (c) 2011-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
@file qapi_wlan_errors.h

This section provides the unique error codes for Wi-Fi modules.
*/

#ifndef __QAPI_WLAN_ERRORS_H__
#define __QAPI_WLAN_ERRORS_H__

#include "qapi_status.h"

//  This macro is used to get unique qapi error codes for WIFI modules.
// +2 we are doing it here because the driver API's which uses return type A_STATUS has error code -1
// With this error code we will get a negative value that will fall in the error base of other modules.
// To put all the A_STATUS error codes to fall into our Wifi error base, +2 is added.

/** @addtogroup qapi_wlan_errors
@{ */


/** Used to get unique API error codes for Wi-Fi modules. */
#define QAPI_STATUS(x)          x ? __QAPI_ERROR(QAPI_MOD_WIFI, ((x)+2)): QAPI_OK


/* ****************** List of qapi WLAN error codes *************************
*
*  NOTE:- DON'T MODIFY EXISTING codes. New error codes add at the end only
*
* ****************************************************************************/ 

/** Used to indicate API success. */
#define QAPI_WLAN_OK                                 QAPI_OK
/** Generic error. */
#define QAPI_WLAN_ERROR                              ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  1)))
// Intentionally skipping error code '2' to exactly map error codes A_STATUS to qapi_Status_t, Don't add it
/** API was not able to find a PCI device. */
#define QAPI_WLAN_ERR_DEVICE_NOT_FOUND               ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  3)))
/** API was not able to allocate memory dynamically. */
#define QAPI_WLAN_ERR_NO_MEMORY                      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  4)))
/** Memory region is not free for mapping. */
#define QAPI_WLAN_ERR_MEMORY_NOT_AVAIL               ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  5)))
/** Free descriptors are not available. */
#define QAPI_WLAN_ERR_NO_FREE_DESC                   ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  6)))
/** Address does not match the descriptor. */
#define QAPI_WLAN_ERR_BAD_ADDRESS                    ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  7)))
/** Used in NT_HW version if a problem occures at initialization. */
#define QAPI_WLAN_ERR_WIN_DRIVER_ERROR               ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  8)))
/** Registers are not correctly mapped. */
#define QAPI_WLAN_ERR_REGS_NOT_MAPPED                ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  9)))
/** Operation is not permitted. */
#define QAPI_WLAN_ERR_EPERM                          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  10)))
/** Access is denied. */
#define QAPI_WLAN_ERR_EACCES                         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  11)))
/** No such entry; search failed. */
#define QAPI_WLAN_ERR_ENOENT                         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  12)))
/** The object already exists. */
#define QAPI_WLAN_ERR_EEXIST                         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  13)))
/** Bad address error. */
#define QAPI_WLAN_ERR_EFAULT                         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  14)))
/** Object is busy. */
#define QAPI_WLAN_ERR_EBUSY                          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  15)))
/** Invalid parameter. */
#define QAPI_WLAN_ERR_EINVAL                         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  16)))
/** Inappropriate message buffer length. */
#define QAPI_WLAN_ERR_EMSGSIZE                       ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  17)))
/** Operation was canceled. */
#define QAPI_WLAN_ERR_ECANCELED                      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  18)))
/** Operation is not supported. */
#define QAPI_WLAN_ERR_ENOTSUP                        ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  19)))
/** Communication error on send. */
#define QAPI_WLAN_ERR_ECOMM                          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  20)))
/** Protocol error. */
#define QAPI_WLAN_ERR_EPROTO                         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  21)))
/** Incorrect device. */
#define QAPI_WLAN_ERR_ENODEV                         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  22)))
/** Device is not UP. */
#define QAPI_WLAN_ERR_EDEVNOTUP                      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  23)))
/** No resources available for the requested operation. */
#define QAPI_WLAN_ERR_NO_RESOURCE                    ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  24)))
/** Hardware failure */
#define QAPI_WLAN_ERR_HARDWARE                       ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  25)))
/** Asynchronous routine. Firmware will send results later, typically in callback. */
#define QAPI_WLAN_ERR_PENDING                        ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  26)))
/** The specified channel cannot be used. */
#define QAPI_WLAN_ERR_EBADCHANNEL                    ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  27)))
/** Decryption error. */
#define QAPI_WLAN_ERR_DECRYPT_ERROR                  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  28)))
/** Receiver error at the physical layer. */
#define QAPI_WLAN_ERR_PHY_ERROR                      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  29)))
/** Object was consumed. */
#define QAPI_WLAN_ERR_CONSUMED                       ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  30)))
/** The buffer is cloned. */
#define QAPI_WLAN_ERR_CLONE                          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  31)))
/** Error in a GPIO operation. */
#define QAPI_WLAN_ERR_HW_CONFIG_ERROR                ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  32)))
/** Socket context was not found. */
#define QAPI_WLAN_ERR_SOCKCXT_NOT_FOUND              ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  33)))
/** Unknown socket command. */
#define QAPI_WLAN_ERR_UNKNOWN_CMD                    ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  34)))
/** Socket limit was reached. */
#define QAPI_WLAN_ERR_SOCK_UNAVAILABLE               ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  35)))
/** Error while freeing a resource. */
#define QAPI_WLAN_ERR_MEMFREE_ERROR                  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  36)))

/** Invalid task ID. */
#define QAPI_WLAN_ERR_QOSAL_INVALID_TASK_ID          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  37)))
/** Invalid parameter. */
#define QAPI_WLAN_ERR_QOSAL_INVALID_PARAMETER        ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  38)))
/** Invalid pointer. */
#define QAPI_WLAN_ERR_QOSAL_INVALID_POINTER          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  39)))
/** Object already exists. */
#define QAPI_WLAN_ERR_QOSAL_ALREADY_EXISTS           ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  40)))
/** Invalid event. */
#define QAPI_WLAN_ERR_QOSAL_INVALID_EVENT            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  41)))
/** Event timed out. */
#define QAPI_WLAN_ERR_QOSAL_EVENT_TIMEOUT            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  42)))
/** Incorrect mutex. */
#define QAPI_WLAN_ERR_QOSAL_INVALID_MUTEX            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  43)))
/** Specified task is locked by another thread. */
#define QAPI_WLAN_ERR_QOSAL_TASK_ALREADY_LOCKED      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  44)))
/** Specified mutex is locked by another thread. */
#define QAPI_WLAN_ERR_QOSAL_MUTEX_ALREADY_LOCKED     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  45)))  
/** No memory is available for the operation. */
#define QAPI_WLAN_ERR_QOSAL_OUT_OF_MEMORY            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_WIFI,  46)))

/** @} */ /* end_addtogroup qapi_wlan_errors */  

#endif /* _QAPI_WLAN_ERRORS_H_ */



