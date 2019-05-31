/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_zb_whitelist.h
   @brief QAPI for the ZigBee stack testing.

   @addtogroup qapi_zb_test
   @{

   This header provides definitions for whitelisting of devices. This
   functionality is intended for certification and other network topology
   testing.

   Once enabled (via qapi_ZB_Whitelist_Enable_CB()), packets will only be
   accepted from devices that have been added to the whitelist.

   @}
*/

#ifndef  __QAPI_ZB_WHITELIST_H__ // [
#define  __QAPI_ZB_WHITELIST_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_zb.h"

/** @addtogroup qapi_zb_test
@{ */

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

#define QAPI_ZB_WHITELIST_UNKOWN_EXTENDED_ADDRESS                       (0xFFFFFFFFFFFFFFFFULL)
#define QAPI_ZB_WHITELIST_UNKOWN_SHORT_ADDRESS                          (0xFFFF)

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief Enables whitelist filtering of received pacekts.

   The callback registered by this function is called when the persistent
   network data is to be backed up.

   @param[in] ZB_Handle Handle of the ZigBee instance as returned by a
                        successful call to qapi_ZB_Initialize().
   @param[in] Enable    Flag indicating whether whitelist filtering is to be
                        enabled (true) or disabled (false).

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Whitelist_Enable(qapi_ZB_Handle_t ZB_Handle, qbool_t Enable);

/**
   @brief Adds a device to the whitelist.

   Adds a device to the whitelist, either by its extended address or its short
   address.  If either address is specified as unknown, the stack will attempt
   to fill it in when the neighbor table entry is added for the device.  At
   least one of the addresses must be valid.

   @param[in] ZB_Handle       Handle of the ZigBee instance as returned
                              by a successful call to qapi_ZB_Initialize().
   @param[in] ExtendedAddress Extended address of the device. Set to
                              QAPI_ZB_WHITELIST_UNKOWN_EXTENDED_ADDRESS if
                              unknown.
   @param[in] ShortAddress    Short address of the device. Set to
                              QAPI_ZB_WHITELIST_UNKOWN_SHORT_ADDRESS if unknown.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Whitelist_Add(qapi_ZB_Handle_t ZB_Handle, uint64_t ExtendedAddress, uint16_t ShortAddress);

/**
   @brief Removes a device from the whitelist, either by its extended address or
          its short address.

   The removed entry will be removed from the whitelist if the extended and
   short addresses specfied match the whitelist entry. If either of the
   addresses is specified as unknown, it will be ignored when checking if an
   entry is a match. If both fields are specified as unknown, all whitelist
   entries will be removed.

   @param[in] ZB_Handle       Handle of the ZigBee instance as returned
                              by a successful call to qapi_ZB_Initialize().
   @param[in] ExtendedAddress Extended address of the device. Set to
                              QAPI_ZB_WHITELIST_UNKOWN_EXTENDED_ADDRESS if
                              unknown.
   @param[in] ShortAddress    Short address of the device. Set to
                              QAPI_ZB_WHITELIST_UNKOWN_SHORT_ADDRESS if unknown.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_ZB_Whitelist_Remove(qapi_ZB_Handle_t ZB_Handle, uint64_t ExtendedAddress, uint16_t ShortAddress);


#endif // ] #ifndef __QAPI_ZB_WHITELIST_H__

/** @} */

