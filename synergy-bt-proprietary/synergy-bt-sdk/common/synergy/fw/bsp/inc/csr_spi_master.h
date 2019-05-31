#ifndef CSR_SPI_MASTER_H__
#define CSR_SPI_MASTER_H__

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
 #define CSR_SPI_MASTER_RESULT_NOT_RESET    ((CsrResult) 1)

/* Features (for use in features member of CsrSpiMasterDevice) */
#define CSR_SPI_MASTER_FEATURE_DMA_CAPABLE_MEM_REQUIRED 0x00000001

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpiMasterDevice
 *
 *  DESCRIPTION
 *      This structure represents a single device on a SPI bus.
 *
 *  MEMBERS
 *      deviceIndex - The index of the device on the bus.
 *      busIndex - The index of the bus on which the device resides.
 *      features - Bit mask with any of CSR_SPI_MASTER_FEATURE_* set.
 *      driverData - For use by the upper layer (CSR SDIO).
 *      priv - For use by the lower layer (CSR SPI Master).
 *
 *----------------------------------------------------------------------------*/
typedef struct
{
    CsrUint8  deviceIndex;
    CsrUint8  busIndex;
    CsrUint32 features;
    void     *driverData;
    void     *priv;
} CsrSpiMasterDevice;

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpiMasterCallback, CsrSpiMasterDsrCallback
 *
 *  DESCRIPTION
 *      These types are used for all callbacks to signal operation completion
 *      as well as interrupts. The CsrSpiMasterCallback is typically called
 *      in interrupt context, that allows very limited interaction with the
 *      platform operating system. If this function returns TRUE a
 *      CsrSpiMasterDsrCallback shall be triggered, which is called in a
 *      context that allows setting an operating system event to wake a thread.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device for which the event
 *               is signalled.
 *
 *----------------------------------------------------------------------------*/
typedef void (*CsrSpiMasterDsrCallback)(CsrSpiMasterDevice *device);
typedef CsrBool (*CsrSpiMasterCallback)(CsrSpiMasterDevice *device);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpiMasterInitialise, CsrSpiMasterDeinitialise
 *
 *  DESCRIPTION
 *      Initialise/deinitialise the SPI Master driver. CsrSpiMasterInitialise
 *      shall be called exactly once before using any other part of the API,
 *      and CsrSpiMasterDeinitialise shall be called exactly once to allow
 *      cleanup of any allocated resources.
 *
 *      The CsrSpiMasterInitialise call will return an array of
 *      CsrSpiMasterDevice structs. On return of this call the deviceIndex,
 *      busIndex and features bits must be initialised to valid values.
 *
 *  PARAMETERS
 *      devicesCount - Pointer to variable that will be updated with the number
 *                     of devices available on the platform.
 *      devices - Pointer to an array of CsrSpiMasterDevice structs. The
 *                length of the array is determined by devicesCount.
 *
 *----------------------------------------------------------------------------*/
void CsrSpiMasterInitialise(CsrUint8 *devicesCount, CsrSpiMasterDevice **devices);
void CsrSpiMasterDeinitialise(void);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpiMasterCallbackRegister
 *
 *  DESCRIPTION
 *      Register callback function pointers. If a callback is specified as NULL
 *      the registration for that particular callback shall be removed.
 *
 *  PARAMETERS
 *      device - Pointer to CsrSpiMasterDevice struct representing the device.
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
void CsrSpiMasterCallbackRegister(CsrSpiMasterDevice *device, CsrSpiMasterCallback interruptCallback, CsrSpiMasterDsrCallback interruptDsrCallback, CsrSpiMasterDsrCallback operationDsrCallback);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpiMasterInterruptAcknowledge
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
void CsrSpiMasterInterruptAcknowledge(CsrSpiMasterDevice *device);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpiMasterCallbackInhibitEnter, CsrSpiMasterCallbackInhibitLeave
 *
 *  DESCRIPTION
 *      Calling CsrSpiMasterCallbackInhibitEnter shall inbibit all callbacks
 *      for the corresponding device until CsrSpiMasterCallbackInhibitLeave
 *      is called. These calls can be nested, and the callback inhibit shall
 *      not be lifted until the final CsrSpiMasterCallbackInhibitLeave has
 *      been called.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device for which to inhibit
 *               callbacks.
 *
 *----------------------------------------------------------------------------*/
void CsrSpiMasterCallbackInhibitEnter(CsrSpiMasterDevice *device);
void CsrSpiMasterCallbackInhibitLeave(CsrSpiMasterDevice *device);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpiMasterBusClockFrequencySet
 *
 *  DESCRIPTION
 *      Set the clock frequency to use on the bus associated with the specified
 *      device.
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
 *----------------------------------------------------------------------------*/
void CsrSpiMasterBusClockFrequencySet(CsrSpiMasterDevice *device, CsrUint32 *frequency);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpiMasterChipSelectAssert, CsrSpiMasterChipSelectNegate
 *
 *  DESCRIPTION
 *      Assert or negate the chip select for the specified device on the bus.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device.
 *
 *----------------------------------------------------------------------------*/
void CsrSpiMasterChipSelectAssert(CsrSpiMasterDevice *device);
void CsrSpiMasterChipSelectNegate(CsrSpiMasterDevice *device);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpiMasterPowerOn, CsrSdiMasterPowerOff
 *
 *  DESCRIPTION
 *      Power on/off the device. CsrSpiMasterPowerOn shall only be called if
 *      CsrSpiMasterPowerOff has previously been called. If this feature is not
 *      available CsrSpiMasterPowerOff shall do nothing, and
 *      CsrSpiMasterPowerOn shall do nothing and return
 *      CSR_SPI_MASTER_RESULT_NOT_RESET.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device to power on/off.
 *
 *  RETURNS (only CsrSpiMasterPowerOn)
 *      CSR_RESULT_SUCCESS - Power was succesfully reapplied. The device is not
 *                           initialised.
 *      CSR_RESULT_FAILURE - Unspecified/unknown error.
 *      CSR_SPI_MASTER_RESULT_NOT_RESET - The power was not removed by the
 *                                        CsrSpiMasterPowerOff call. The state
 *                                        of the device is unchanged.
 *
 *----------------------------------------------------------------------------*/
CsrResult CsrSpiMasterPowerOn(CsrSpiMasterDevice *device);
void CsrSpiMasterPowerOff(CsrSpiMasterDevice *device);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrSpiMasterRead, CsrSpiMasterWrite, CsrSpiMasterReadAsync,
 *      CsrSpiMasterWriteAsync
 *
 *  DESCRIPTION
 *      Read/write a specified number of byte from/to the specified device. It
 *      is the responsibility of the upper layer to control the chip select by
 *      calling CsrSpiMasterChipSelectAssert/CsrSpiMasterChipSelectNegate as
 *      required by the higher level protocol.
 *
 *  PARAMETERS
 *      device - Pointer to struct representing the device.
 *      data - The data to read/write.
 *      length - The number of byte to read/write.
 *      callback - The function to call on operation completion.
 *
 *      NOTE: The CsrSpiMasterReadAsync and CsrSpiMasterWriteAsync functions
 *            return immediately, and the supplied callback function is called
 *            when the operation is complete.
 *
 *----------------------------------------------------------------------------*/
void CsrSpiMasterRead(CsrSpiMasterDevice *device, CsrUint8 *data, CsrUint32 length);
void CsrSpiMasterWrite(CsrSpiMasterDevice *device, const CsrUint8 *data, CsrUint32 length);
void CsrSpiMasterReadAsync(CsrSpiMasterDevice *device, CsrUint8 *data, CsrUint32 length, CsrSpiMasterCallback operationCallback);
void CsrSpiMasterWriteAsync(CsrSpiMasterDevice *device, const CsrUint8 *data, CsrUint32 length, CsrSpiMasterCallback operationCallback);

#ifdef __cplusplus
}
#endif

#endif
