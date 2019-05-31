#ifndef CSR_BT_RESAMPLE_LIB_H__
#define CSR_BT_RESAMPLE_LIB_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/



#ifdef __cplusplus
extern "C" {
#endif

#include "csr_types.h"

typedef void* CsrBtResamplingHandler;

/*--------------------------------------------------------------------------*
 *  NAME
 *      CsrBtResamplingInit
 *
 *  DESCRIPTION
 *      Configure filters
 *
 *  PARAMETERS
 *      fs0:        Input sampling frequency
 *      chan0:      Input channel type: 0 for Mono and 1 for Stereo
 *      blockSize0: Size of input data
 *      fs1:        Output sampling frequency
 *      chan1:      Output channel type: 0 for Mono and 1 for Stereo
 *      blockSize1: Size of output data
 *--------------------------------------------------------------------------*/
CsrBtResamplingHandler CsrBtResamplingInit(CsrInt32 fs0, CsrUint8 chan0, CsrUint32 blockSize0, CsrInt32 fs1, CsrUint8 chan1, CsrUint32 blockSize1, CsrUint32 bufferSize);

/*--------------------------------------------------------------------------*
 *  NAME
 *      CsrBtResamplingDeInit
 *
 *  DESCRIPTION
 *      Free instance data
 *
 *  PARAMETERS
 *      inst:       Instance data
 *--------------------------------------------------------------------------*/
void CsrBtResamplingDeInit(CsrBtResamplingHandler inst);

/*--------------------------------------------------------------------------*
 *  NAME
 *      CsrBtResamplingStereo
 *
 *  DESCRIPTION
 *      Resampling function
 *
 *  PARAMETERS
 *      inst:       Instance data
 *      dataChan0:  Pointer to input data form channel zero
 *      dataChan1:  Pointer to input data form channel one
 *      blockSize:  Size of the input data
 *--------------------------------------------------------------------------*/
void CsrBtResamplingStereo(CsrBtResamplingHandler inst, CsrInt16 *dataChan0, CsrInt16 *dataChan1, CsrInt32 blockSize);

/*--------------------------------------------------------------------------*
 *  NAME
 *      CsrBtResamplingMono
 *
 *  DESCRIPTION
 *      Resampling function
 *
 *  PARAMETERS
 *      inst:       Instance data
 *      dataChan0:  Pointer to input data
 *      blockSize:  Size of the input data
 *--------------------------------------------------------------------------*/
void CsrBtResamplingMono(CsrBtResamplingHandler inst, CsrInt16 *dataChan0, CsrInt32 blockSise);

/*--------------------------------------------------------------------------*
 *  NAME
 *      ResamplingGetData
 *
 *  DESCRIPTION
 *      The resampling function puts the resamplede data into a buffer,
 *      to get the data out of the buffer then use getData
 *
 *  PARAMETERS
 *      inst:       Instance data
 *      dataChan0:  Pointer to output data from channel 0
 *      dataChan1:  Pointer to output data from channel 1
 *      CsrUint8:    Status. 1 for data and  for no data
 *--------------------------------------------------------------------------*/
CsrUint8 CsrBtResamplingGetDataStereo(CsrBtResamplingHandler inst, CsrInt16 *dataChan0, CsrInt16 *dataChan1);

/*--------------------------------------------------------------------------*
 *  NAME
 *      CsrBtResamplingGetDataMono
 *
 *  DESCRIPTION
 *      The resampling function puts the resamplede data into a buffer,
 *      to get the data out of the buffer then use getData
 *
 *  PARAMETERS
 *      inst:       Instance data
 *      dataChan0:  Pointer to output data
 *      CsrUint8:    Status. 1 for data and  for no data
 *--------------------------------------------------------------------------*/
CsrUint8 CsrBtResamplingGetDataMono(CsrBtResamplingHandler inst, CsrInt16 *dataChan0);

#ifdef __cplusplus
}
#endif

#endif  /* RESAMPLE_LIB */

