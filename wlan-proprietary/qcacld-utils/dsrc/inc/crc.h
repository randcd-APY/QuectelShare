#ifndef __CRC_H__
#define __CRC_H__

/*===========================================================================

              C R C    S E R V I C E S    D E C L A R A T I O N S

DESCRIPTION
  The following declarations are for use with the CRC Services.

Copyright(c) 1991-2015 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
===========================================================================*/

/*!
** When crc_16_l_calc() is applied to the
** unchanged entire buffer, it returns CRC_16_L_OK.
*/
#define CRC_16_L_OK             0x0F47

/*! size of crc_16 in octets */
#define CRC_16_SIZE             2

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*!===========================================================================

@brief  This function calculates an LSB-first 16-bit CRC over a specified number
  of data bits.  It can be used to produce a CRC and to check a CRC.

@param  buf_ptr : Pointer to bytes containing the data to CRC. The bit stream
                  starts in the LS bit of the first byte.
@param len : Number of data bits to calculate the CRC over

@return
  Returns a unsigned short holding 16 bits which are the contents of the CRC
  register as calculated over the specified data bits.  If this
  function is being used to check a CRC, then the return value will be
  equal to CRC_16_L_OK (defined in crc.h) if the CRC checks correctly.

===========================================================================*/
extern unsigned short crc_16_l_calc (unsigned char *buf_ptr, unsigned short len);

#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */
