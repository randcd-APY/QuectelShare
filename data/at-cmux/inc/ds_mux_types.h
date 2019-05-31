#ifndef _DS_MUX_TYPES_H_
#define _DS_MUX_TYPES_H_

/******************************************************************************

                           DS_MUX_TYPES.H

******************************************************************************/

/******************************************************************************

  @file    DS_MUX_TYPES.H

  DESCRIPTION
  Header file for all the data types used in the DS_MUX_MODULE

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  ---------------------------------------------------------------------------

******************************************************************************/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  when       who        what, where, why
  --------   ---        -------------------------------------------------------
  11/10/17   sm     Initial version

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/


#ifndef _UINT8_DEFINED
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
#define _UINT8_DEFINED
#endif

#ifndef _INT32_DEFINED
typedef  signed long int    int_32;       /* Signed 32 bit value */
#define _INT32_DEFINED
#endif

#ifndef _INT16_DEFINED
typedef  signed short       int16;       /* Signed 16 bit value */
#define _INT16_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint_32;
#define _UINT32_DEFINED
#endif

#ifndef _UINT16_DEFINED
typedef  unsigned short     uint16;
#define _UINT16_DEFINED
#endif

#ifndef _BOOLEAN_DEFINED
typedef  unsigned char      boolean;
#define _BOOLEAN_DEFINED
#endif

#ifndef _BYTE_DEFINED
typedef  unsigned char      byte;        /* byte type */
#define  _BYTE_DEFINED
#endif

#endif
