#ifndef CSR_SDIO_MASTER_H__
#define CSR_SDIO_MASTER_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"
#include "csr_result.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Result Codes */
#define CSR_SDIO_MASTER_RESULT_TIMEOUT      ((CsrResult) 1)
#define CSR_SDIO_MASTER_RESULT_CRC          ((CsrResult) 2)
#define CSR_SDIO_MASTER_RESULT_NOT_RESET    ((CsrResult) 3)

/* Features (for use in features member of CsrSdioMasterDevice) */
#define CSR_SDIO_MASTER_FEATURE_DMA_CAPABLE_MEM_REQUIRED 0x00000001

/* Maximum number of blocks that can be transfered in a single Block Mode Read/Write Operation */
#define CSR_SDIO_MASTER_BLOCK_MODE_TRANSFER_LIMIT 511

/* Maximum number of byte that can be transfered in a single Byte Mode Read/Write Operation */
#define CSR_SDIO_MASTER_BYTE_MODE_TRANSFER_LIMIT 512

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterDevice
 *
 *  DESCRIPTION
 *      This structure represents a single device on an SDIO bus.
 *
 *  MEMBERS
 *      deviceIndex - The index of the device on the bus.
 *      busIndex - The index of the bus on which the device resides.
 *      features - Bit mask with any of CSR_SDIO_MASTER_FEATURE_* set.
 *      functions - Number of functions on the device (not including
 *                  function 0).
 *      blockSize - The actual configured block size of the 8 possible device
 *                  functions.
 *      driverData - For use by the upper layer (CSR SDIO).
 *      priv - For use by the lower layer (CSR SDIO Master).
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    CsrUint8  deviceIndex;
    CsrUint8  busIndex;
    CsrUint32 features;
    CsrUint8  functions;
    CsrUint16 blockSize[8];
    void     *driverData;
    void     *priv;
} CsrSdioMasterDevice;

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterCallback, CsrSdioMasterDsrCallback
 *
 *  DESCRIPTION
 *      These types are used for all callbacks to signal operation completion
 *      as well as interrupts. The CsrSdioMasterCallback is typically called
 *      in interrupt context, that allows very limited interaction with the
 *      platform operating system. If this function returns TRUE a
 *      CsrSdioMasterDsrCallback shall be triggered, which is called in a
 *      context that allows setting an operating system event to wake a thread.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device for which the event
 *               is signalled.
 *      result - The result of the operation. Always CSR_RESULT_SUCCESS, when
 *               signalling an interrupt.
 *
 *----------------------------------------------------------------------------*/
typedef void (*CsrSdioMasterDsrCallback)(CsrSdioMasterDevice *device);
typedef CsrBool (*CsrSdioMasterCallback)(CsrSdioMasterDevice *device, CsrResult result);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterInitialise, CsrSdioMasterDeinitialise
 *
 *  DESCRIPTION
 *      Initialise/deinitialise the SDIO Master driver. CsrSdioMasterInitialise
 *      shall be called exactly once before using any other part of the API,
 *      and CsrSdioMasterDeinitialise shall be called exactly once to allow
 *      cleanup of any allocated resources.
 *
 *      The CsrSdioMasterInitialise call will return an array of
 *      CsrSdioMasterDevice structs. On return of this call, at least the
 *      deviceIndex, busIndex and features bits must be initialised to valid
 *      values.
 *
 *  PARAMETERS
 *      devicesCount - Pointer to variable that will be updated with the number
 *                     of devices available on the platform.
 *      devices - Pointer to an array of CsrSdioMasterDevice structs. The
 *                length of the array is determined by devicesCount.
 *
 *----------------------------------------------------------------------------*/
void CsrSdioMasterInitialise(CsrUint8 *devicesCount, CsrSdioMasterDevice **devices);
void CsrSdioMasterDeinitialise(void);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterCallbackRegister
 *
 *  DESCRIPTION
 *      Register callback function pointers. If a callback is specified as NULL
 *      the registration for that particular callback shall be removed.
 *
 *  PARAMETERS
 *      device - Pointer to CsrSdioMasterDevice struct representing the device.
 *      interruptCallback - Function pointer to callback that shall be called
 *                          when a device interrupt is received.
 *      interruptDsrCallback - Function pointer to callback that shall be
 *                             called when the interruptCallback function
 *                             returns TRUE.
 *      operationDsrCallback - Function pointer to callback that shall be
 *                             called when the operationCallback function
 *                             returns TRUE. Note that the operationCallback
 *                             is given as a parameter on each operation that
 *                             is started.
 *
 *----------------------------------------------------------------------------*/
void CsrSdioMasterCallbackRegister(CsrSdioMasterDevice *device, CsrSdioMasterCallback interruptCallback, CsrSdioMasterDsrCallback interruptDsrCallback, CsrSdioMasterDsrCallback operationDsrCallback);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterDeviceInitialise
 *
 *  DESCRIPTION
 *      Initialise the device represented by the device parameter. This
 *      includes the SDIO initialisation as described in the SDIO
 *      Specification, as well as initialising the functions and blockSize
 *      members of the CsrSdioMasterDevice struct.
 *
 *  PARAMETERS
 *      device - Pointer to CsrSdioMasterDevice struct representing the device.
 *
 *  RETURNS
 *      CSR_RESULT_SUCCESS - The device was successfully initialised and is
 *                           ready for communication.
 *      CSR_RESULT_FAILURE - Unspecified/unknown error.
 *      CSR_SDIO_MASTER_RESULT_CRC - A CRC error occured. The state of the
 *                                   device is undefined.
 *      CSR_SDIO_MASTER_RESULT_TIMEOUT - No response from the device.
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrSdioMasterDeviceInitialise(CsrSdioMasterDevice *device);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterInterruptAcknowledge
 *
 *  DESCRIPTION
 *      Acknowledge that a signalled interrupt has been handled. Shall only
 *      be called once, and exactly once for each signalled interrupt to the
 *      corresponding device.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device to which the
 *               interrupt was signalled.
 *
 *----------------------------------------------------------------------------*/
void CsrSdioMasterInterruptAcknowledge(CsrSdioMasterDevice *device);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterCallbackInhibitEnter, CsrSdioMasterCallbackInhibitLeave
 *
 *  DESCRIPTION
 *      Calling CsrSdioMasterCallbackInhibitEnter shall inbibit all callbacks
 *      for the corresponding device until CsrSdioMasterCallbackInhibitLeave
 *      is called. These calls can be nested, and the callback inhibit shall
 *      not be lifted until the final CsrSdioMasterCallbackInhibitLeave has
 *      been called.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device for which to inhibit
 *               callbacks.
 *
 *----------------------------------------------------------------------------*/
void CsrSdioMasterCallbackInhibitEnter(CsrSdioMasterDevice *device);
void CsrSdioMasterCallbackInhibitLeave(CsrSdioMasterDevice *device);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterBlockSizeSet
 *
 *  DESCRIPTION
 *      Set the block size to use for the function. The SDIO Master
 *      implementation may round the block size to suit the host controller,
 *      if necessary. When this function returns, the actual block size
 *      configured shall be available in the blockSize member of the
 *      CsrSdioMasterDevice struct. If block mode is not supported, the
 *      implementation shall not write to the block size register on the device,
 *      and the blockSize member shall remain 0.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device.
 *      function - The function number to set the block size for.
 *      blockSize - Requested block size to use for the function. It is the
 *                  responsibility of the upper layer to ensure that maximum
 *                  block size supported by the function is not exceeded.
 *
 *  RETURNS
 *      CSR_RESULT_SUCCESS - The block size register on the chip was updated.
 *      CSR_RESULT_FAILURE - Unspecified/unknown error.
 *      CSR_SDIO_MASTER_RESULT_CRC - A CRC error occured. The configured block
 *                                   size is undefined.
 *      CSR_SDIO_MASTER_RESULT_TIMEOUT - No response from the device.
 *
 *      NOTE: Setting the block size requires two individual operations. The
 *            implementation shall ignore the OUT_OF_RANGE bit of the SDIO R5
 *            response for the first operation, as the partially configured
 *            block size may be out of range, even if the final block size
 *            (after the second operation) is in the valid range.
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrSdioMasterBlockSizeSet(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint16 blockSize);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterBusClockFrequencySet
 *
 *  DESCRIPTION
 *      Set the clock frequency to use on the bus associated with the specified
 *      device.
 *
 *      If the clock frequency exceeds 25MHz, it is the responsibility of the
 *      SDIO Master implementation to enable high speed mode on all devices on
 *      the bus, using the standard defined procedure, before increasing the
 *      frequency beyond the limit.
 *
 *      Note that the clock frequency configured affects all devices on the
 *      same bus.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device.
 *      frequency - The clock frequency for the device in Hertz. It is the
 *                  responsibility of the upper layer to ensure that the maximum
 *                  frequency of the device is not exceeded.
 *
 *  RETURNS
 *      CSR_RESULT_SUCCESS - The maximum clock frequency was succesfully
 *                                set for the function.
 *      CSR_RESULT_FAILURE - Unspecified/unknown error.
 *      CSR_SDIO_MASTER_RESULT_CRC - A CRC error occured.
 *      CSR_SDIO_MASTER_RESULT_TIMEOUT - No response from the device.
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrSdioMasterBusClockFrequencySet(CsrSdioMasterDevice *device, CsrUint32 *frequency);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterPowerOn, CsrSdioMasterPowerOff
 *
 *  DESCRIPTION
 *      Power on/off the device. CsrSdioMasterPowerOn shall only be called if
 *      CsrSdioMasterPowerOff has previously been called. If this feature is not
 *      available CsrSdioMasterPowerOff shall do nothing, and
 *      CsrSdioMasterPowerOn shall do nothing and return
 *      CSR_SDIO_MASTER_RESULT_NOT_RESET.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device to power on/off.
 *
 *  RETURNS (only CsrSdioMasterPowerOn)
 *      CSR_RESULT_SUCCESS - Power was succesfully reapplied. The device is not
 *                           initialised and CsrSdioMasterDeviceInitialise must
 *                           be called before communicating with the device.
 *      CSR_RESULT_FAILURE - Unspecified/unknown error.
 *      CSR_SDIO_MASTER_RESULT_NOT_RESET - The power was not removed by the
 *                                         CsrSdioMasterPowerOff call. The state
 *                                         of the device is unchanged.
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrSdioMasterPowerOn(CsrSdioMasterDevice *device);
void CsrSdioMasterPowerOff(CsrSdioMasterDevice *device);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterRead8, CsrSdioMasterWrite8, CsrSdioMasterRead8Async,
 *      CsrSdioMasterWrite8Async
 *
 *  DESCRIPTION
 *      Read/write an 8bit value from/to the specified register address using
 *      CMD52.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device.
 *      function - The function number.
 *      address - Register address within the function.
 *      data - The data to read/write.
 *      callback - The function to call on operation completion.
 *
 *  RETURNS
 *      CSR_RESULT_SUCCESS - The data was successfully read/written.
 *      CSR_RESULT_FAILURE - Unspecified/unknown error.
 *      CSR_SDIO_MASTER_RESULT_CRC - A CRC error occured. No data read/written.
 *      CSR_SDIO_MASTER_RESULT_TIMEOUT - No response from the device.
 *
 *      NOTE: The CsrSdioMasterRead8Async and CsrSdioMasterWrite8Async functions
 *            return immediately, and the supplied callback function is called
 *            when the operation is complete. The result value is given as an
 *            argument to the callback function.
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrSdioMasterRead8(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, CsrUint8 *data);
CsrResult CsrSdioMasterWrite8(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, CsrUint8 data);
void CsrSdioMasterRead8Async(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, CsrUint8 *data, CsrSdioMasterCallback operationCallback);
void CsrSdioMasterWrite8Async(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, CsrUint8 data, CsrSdioMasterCallback operationCallback);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterByteModeRead, CsrSdioMasterByteModeWrite,
 *      CsrSdioMasterByteModeReadAsync, CsrSdioMasterByteModeWriteAsync
 *
 *  DESCRIPTION
 *      Read/write a specified number of bytes from/to the specified register
 *      address, using CMD53 in byte mode.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device.
 *      function - The function number.
 *      address - Register address within the function.
 *      data - The data to read/write.
 *      length - Number of bytes to read/write.
 *      callback - The function to call on operation completion.
 *
 *  RETURNS
 *      CSR_RESULT_SUCCESS - The data was successfully read/written.
 *      CSR_RESULT_FAILURE - Unspecified/unknown error.
 *      CSR_SDIO_MASTER_RESULT_CRC - A CRC error occured. Data may have been
 *                                   partially read/written.
 *      CSR_SDIO_MASTER_RESULT_TIMEOUT - No response from the device.
 *
 *      NOTE: The CsrSdioMasterByteModeReadAsync and
 *            CsrSdioMasterByteModeWriteAsync functions return immediately, and
 *            the supplied callback function is called when the operation is
 *            complete. The result value is given as an argument to the callback
 *            function.
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrSdioMasterByteModeRead(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, CsrUint8 *data, CsrUint16 length);
CsrResult CsrSdioMasterByteModeWrite(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, const CsrUint8 *data, CsrUint16 length);
void CsrSdioMasterByteModeReadAsync(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, CsrUint8 *data, CsrUint16 length, CsrSdioMasterCallback operationCallback);
void CsrSdioMasterByteModeWriteAsync(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, const CsrUint8 *data, CsrUint16 length, CsrSdioMasterCallback operationCallback);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSdioMasterBlockModeRead, CsrSdioMasterBlockModeWrite,
 *      CsrSdioMasterBlockModeReadAsync, CsrSdioMasterBlockModeWriteAsync
 *
 *  DESCRIPTION
 *      Read/write a specified number of bytes from/to the specified register
 *      address, using CMD53 in block mode.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device.
 *      function - The function number.
 *      address - Register address within the function.
 *      data - The data to read/write.
 *      blocks - Number of blocks to read/write.
 *      callback - The function to call on operation completion.
 *
 *  RETURNS
 *      CSR_RESULT_SUCCESS - The data was successfully read/written.
 *      CSR_RESULT_FAILURE - Unspecified/unknown error.
 *      CSR_SDIO_MASTER_RESULT_CRC - A CRC error occured. Data may have been
 *                                   partially read/written.
 *      CSR_SDIO_MASTER_RESULT_TIMEOUT - No response from the device.
 *
 *      NOTE: The CsrSdioMasterBlockModeReadAsync and
 *            CsrSdioMasterBlockModeWriteAsync functions return immediately, and
 *            the supplied callback function is called when the operation is
 *            complete. The result value is given as an argument to the callback
 *            function.
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrSdioMasterBlockModeRead(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, CsrUint8 *data, CsrUint16 blocks);
CsrResult CsrSdioMasterBlockModeWrite(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, const CsrUint8 *data, CsrUint16 blocks);
void CsrSdioMasterBlockModeReadAsync(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, CsrUint8 *data, CsrUint16 blocks, CsrSdioMasterCallback operationCallback);
void CsrSdioMasterBlockModeWriteAsync(CsrSdioMasterDevice *device, CsrUint8 function, CsrUint32 address, const CsrUint8 *data, CsrUint16 blocks, CsrSdioMasterCallback operationCallback);

#ifdef __cplusplus
}
#endif

#endif
