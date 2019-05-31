/**  
  @file
  ql_usb.h

  @brief
  This file provides the definitions for usb otg, and declares the 
  API functions.

*/
/*============================================================================
  Copyright (c) 2017 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
 =============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


WHEN        WHO            WHAT, WHERE, WHY
----------  ------------   ----------------------------------------------------
27/06/2018  Quectel        Changed the parameter of ql_adc_show() from char-type
                           to Enum_QADC.
=============================================================================*/

#ifndef _QL_USB_H_
#define _QL_USB_H_

typedef enum{
	 host = 0,	
	 device,
}enum_otg;

/*========================================================================
  FUNCTION:  Ql_USB_Otg
=========================================================================*/
/** @brief 
    This function retrieves the usb otg mode

    @param[in] device_enable, one value of enum_otg.

    @return
    0 on success
    on failure, the return value is -1;
*/
/*=======================================================================*/
int Ql_USB_Otg (enum_otg device_enable);

#endif
