/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_CALLBACK_H_
#define _CONNX_CALLBACK_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CONNX_CALLBACK(P_CB, P_CBACK, ...)        { \
                                                      if (P_CB && P_CB->P_CBACK) \
                                                      { \
                                                          P_CB->P_CBACK(__VA_ARGS__); \
                                                      } \
                                                  }


#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_CALLBACK_H_ */

