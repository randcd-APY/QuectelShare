/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef __QAPI_BLE_BTTYPES_H__
#define __QAPI_BLE_BTTYPES_H__

   /* Miscellaneous defined type declarations.                          */

   /* Definitions for compilers that required structure to be explicitly*/
   /* declared as packed.                                               */

   /* Check for ARM Real View.                                          */
#ifdef __ARMCC_VERSION
   #define __QAPI_BLE_PACKED_STRUCT_BEGIN__   __packed
#else
   #define __QAPI_BLE_PACKED_STRUCT_BEGIN__
#endif

   /* Check for GCC.                                                    */
#ifdef __GNUC__
   #define __QAPI_BLE_PACKED_STRUCT_END__     __attribute__ ((packed))
#else
   #define __QAPI_BLE_PACKED_STRUCT_END__
#endif

#define __QAPI_BLE_BTTYPESH_INC__

   /* Force ALL Structure Declarations to be Byte Packed (noting the    */
   /* current Structure Packing).                                       */

#include "qapi_ble_basetypes.h"
#include "qapi_ble_btbtypes.h"

   /* Restore Structure Packing.                                        */

#undef __QAPI_BLE_BTTYPESH_INC__


#endif
