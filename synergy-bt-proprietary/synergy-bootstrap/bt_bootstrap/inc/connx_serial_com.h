/******************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _CONNX_SERIAL_COM_H_
#define _CONNX_SERIAL_COM_H_

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif


#define INVALID_BGINT_HANDLE ((uint16_t) 0xFFFF)

typedef uint8_t ConnxUartDrvParity;
#define CONNX_UART_DRV_PARITY_NONE    1
#define CONNX_UART_DRV_PARITY_EVEN    0

typedef uint8_t ConnxUartDrvDataBits;
#define CONNX_UART_DRV_DATA_BITS_7    1
#define CONNX_UART_DRV_DATA_BITS_8    0

typedef uint8_t ConnxUartDrvStopBits;
#define CONNX_UART_DRV_STOP_BITS_1    1
#define CONNX_UART_DRV_STOP_BITS_2    0


void *ConnxUartDrvInitialise(const char *device,
    uint32_t baud, uint32_t resetBaud,
    ConnxUartDrvDataBits dataBits,
    ConnxUartDrvParity parity,
    ConnxUartDrvStopBits stopBits,
    bool flowControl,
    const uint8_t *token);

void ConnxUartDrvDeinitialise(void *handle);

uint32_t ConnxUartDrvBaudrateActualGet(uint32_t wantedBaudrate);


/*****************************************************************************

    NAME
        ConnxUartDrvDataRx

    DESCRIPTION
        This callback type is used for the callback that is registerd in the
        ConnxUartDrvRegister function and called by the ConnxUartDrvRx function.
        Please refer to the description of these functions for further
        information.

    PARAMETERS
        data - pointer to the data.
        dataLength - the number of contiguous data bytes available.

    RETURNS
        The number of data bytes consumed from the data pointer.

*****************************************************************************/
typedef uint32_t (*ConnxUartDrvDataRx)(const uint8_t *data, uint32_t dataLength);


/*****************************************************************************

    NAME
        ConnxUartDrvOpen

    DESCRIPTION
        This function is called to open the UART device and retrieve a handle
        to it, which is subsequently used in functions that accept a UART
        device handle.

    PARAMETERS
        device - a (platform dependent) string that identifies the UART device
            to open.
        baud - a pointer to a variable containing the desired baud rate to use.
            If the value specified is not supported by the UART device, the
            UART driver may, if possible, adjust the value to the nearest
            suitable rate (updating the baud variable accordingly), or, if
            possible, reject the request by returning NULL.
        dataBits - number of databit (valid values: 7 or 8).
        parity - parity bit: 0=None, 1=Odd, 2=Even.
        stopBits - number of stopbit (valid values: 1 or 2).
        flowControl - whether to enable hardware flow control (RTS/CTS).
        token - If this token is given, the UART driver shall only deliver
            data to the upper layer when this token is encountered in the RX
            datastream. This increases the efficieny by only triggering the
            upper layer processing when an entire data unit has been
            received. Pass NULL to disable this functionality.

    RETURNS
        A UART device handle or NULL if unable to open the specified device or
        if the arguments are invalid or unsupported. NULL is considered an
        invalid handle and shall not be used in any subsequent calls.

*****************************************************************************/
void *ConnxUartDrvOpen(const char *device,
    uint32_t *baud,
    uint8_t dataBits,
    uint8_t parity,
    uint8_t stopBits,
    bool flowControl,
    const uint8_t *token);


/*****************************************************************************

    NAME
        ConnxUartDrvClose

    DESCRIPTION
        This function is called to close the UART device that was previously
        opened with ConnxUartDrvOpen. When the call returns, the handle is no
        longer valid and shall not be used in any subsequent calls.

        ConnxUartDrvStop must be called before calling this function, if the
        UART driver is in the running state (i.e. ConnxUartDrvStart has been
        called).

    PARAMETERS
        handle - handle of the UART device.

*****************************************************************************/
void ConnxUartDrvClose(void *handle);


/*****************************************************************************

    NAME
        ConnxUartDrvReconfigure

    DESCRIPTION
        This function is called to change the baud rate and the hardware flow
        control state.

        This function can be called at any time with a valid UART device
        handle including when the UART driver is in the running state, i.e.
        after ConnxUartDrvStart has been called and before ConnxUartDrvStop has
        been called.

        If hardware flow control is disabled and ConnxUartDrvReconfigure is
        called without enabling hardware flow control, the state of the RTS pin
        shall be preserved as set by ConnxUartDrvRtsSet. In all other cases the
        state of the RTS pin is unknown on return from this function, and
        ConnxUartDrvRtsSet should be called to set a specific state if needed.

    PARAMETERS
        handle - handle of the UART device.
        baud - a pointer to a variable containing the desired baud rate to use.
            If the value specified is not supported by the UART device, the
            UART driver may, if possible, adjust the value to the nearest
            suitable rate (updating the baud variable accordingly), or, if
            possible, reject the request by returning FALSE.
        flowControl - whether to enable hardware flow control (RTS/CTS).

    RETURNS
        TRUE if the UART device was successfully reconfigured and FALSE
        otherwise.

*****************************************************************************/
bool ConnxUartDrvReconfigure(void *handle, uint32_t *baud, bool flowControl);


/*****************************************************************************

    NAME
        ConnxUartDrvStart

    DESCRIPTION
        This function is called to start the UART driver.

        If hardware flow control is disabled, the state of the RTS pin is
        unknown on return, and should be set by calling ConnxUartDrvRtsSet if
        needed.

    PARAMETERS
        handle - handle of the UART device.
        reset - number of previous resets. This can be used by the UART driver
            to determine the baud rate to use. However, if ConnxUartDrvOpen
            and/or ConnxUartDrvReconfigure have been called, the baud rate set
            by these shall take effect regardless of this value.

    RETURNS
        TRUE if the UART driver was successfully started and FALSE otherwise.

*****************************************************************************/
bool ConnxUartDrvStart(void *handle, uint8_t reset);


/*****************************************************************************

    NAME
        ConnxUartDrvStop

    DESCRIPTION
        This function is called to stop (close) the UART driver. It undoes all
        actions performed by ConnxUartDrvStart, freeing any allocated resources.

        This function must be called before calling ConnxUartDrvClose, if the
        UART driver is in the running state (i.e. ConnxUartDrvStart has been
        called)

    PARAMETERS
        handle - handle of the UART device.

    RETURNS
        TRUE if UART driver was successfully stopped and FALSE otherwise.

*****************************************************************************/
bool ConnxUartDrvStop(void *handle);


/*****************************************************************************

    NAME
        ConnxUartDrvRtsSet

    DESCRIPTION
        This function is called to manually set the output level of the RTS
        signal when hardware flow control is disabled.

        It can only be called when the UART driver is in the running state,
        i.e. after ConnxUartDrvStart has been called and before ConnxUartDrvStop
        has been called. Behaviour is undefined if this function is called
        on a UART device where hardware flow control is enabled.

        When hardware flow control is disabled, the state of the RTS pin is
        unknown when ConnxUartDrvStart returns. This function should be called
        to set a specific state if needed.

        If hardware flow control is disabled and ConnxUartDrvReconfigure is
        called without enabling hardware flow control, the state of the RTS pin
        shall be preserved as set by ConnxUartDrvRtsSet. In all other cases the
        state of the RTS pin is unknown on return from ConnxUartDrvReconfigure,
        and this function should be called to set a specific state if needed.

    PARAMETERS
        handle - handle of the UART device.
        state - determines whether the pin should be asserted (TRUE) or
            negated (FALSE). Specifically:

                +-------+--------+-----+-------------+
                | state | Logic  | TTL | RS232       |
                +-------+--------+-----+-------------+
                | TRUE  | 1/HIGH | 5V  | -3V to -25V |
                | FALSE | 0/LOW  | 0V  | +3V to +25V |
                +-------+--------+-----+-------------+

*****************************************************************************/
void ConnxUartDrvRtsSet(void *handle, bool state);


/*****************************************************************************

    NAME
        ConnxUartDrvGetTxSpace

    DESCRIPTION
        Obtain amount of available free space in the UART driver transmit
        buffer. Should be able to buffer at least this number of bytes in a
        call to ConnxUartDrvTx.

    PARAMETERS
        handle - handle of the UART device.

    RETURNS
        The number of bytes that can be written to the UART driver transmit
        buffer.

*****************************************************************************/
uint32_t ConnxUartDrvGetTxSpace(void *handle);


/*****************************************************************************

    NAME
        ConnxUartDrvTx

    DESCRIPTION
        Buffer the supplied data for transmission. If the free space in the
        transmit buffer is less than length of the specified data, the
        implementation shall either attempt to buffer as many bytes as
        possible and return the actual number of bytes buffered in the numSent
        variable, or simply set numSent to 0 and in both cases return FALSE.

    PARAMETERS
        handle - handle of the UART device.
        data - pointer to data.
        dataLength - number of bytes to buffer for transmission.
        numSent - the actual number of bytes buffered is returned in this
            variable.

    RETURNS
        Returns TRUE if all the supplied data has been buffered (numSent is
        equal to dataLength), else return FALSE.

*****************************************************************************/
bool ConnxUartDrvTx(void *handle, const uint8_t *data, uint32_t dataLength, uint32_t *numSent);


/*****************************************************************************

    NAME
        ConnxUartDrvGetRxAvailable

    DESCRIPTION
        Obtain amount of data available in the UART driver receive buffer.
        Should be able to receive at least this number of bytes in a call to
        the ConnxUartDrvRx or ConnxUartDrvLowLevelTransportRx function.

    PARAMETERS
        handle - handle of the UART device.

    RETURNS
        The number of bytes that can be read from the UART driver receive
        buffer.

*****************************************************************************/
uint32_t ConnxUartDrvGetRxAvailable(void *handle);


/*****************************************************************************

    NAME
        ConnxUartDrvRx

    DESCRIPTION
        Calling this function will in turn cause the ConnxUartDrvDataRx callback
        function registered in the call to ConnxUartDrvRegister to be called.
        The ConnxUartDrvDataRx callback function passes a pointer (and length)
        to the upper layer. The upper layer may either process this data
        in-place or copy it to a local buffer for later processing. The return
        value from the ConnxUartDrvDataRx function indicates the number of bytes
        that was consumed (regardles of whether it was processed or copied) by
        the upper layer.

    PARAMETERS
        handle - handle of the UART device.

*****************************************************************************/
void ConnxUartDrvRx(void *handle);


/*****************************************************************************

    NAME
        ConnxUartDrvLowLevelTransportRx

    DESCRIPTION
        Copies up to the specified number of bytes from the receive buffer to
        the supplied data buffer.

    PARAMETERS
        handle - handle of the UART device.
        dataLength - the number of byte to retrieve.
        data - buffer to copy the data into.

    RETURNS
        The number of bytes actually copied. Note that this may be less than
        the length of the supplied buffer.

*****************************************************************************/
uint32_t ConnxUartDrvLowLevelTransportRx(void *handle, uint32_t dataLength, uint8_t *data);


/*****************************************************************************

    NAME
        ConnxUartDrvLowLevelTransportTxBufLevel

    DESCRIPTION
        Retrieve the number of bytes in the transmit buffer waiting to be
        transmitted.

    PARAMETERS
        handle - handle of the UART device.

    RETURNS
        The number of bytes in the transmit buffer waiting to be transmitted.

*****************************************************************************/
uint32_t ConnxUartDrvLowLevelTransportTxBufLevel(void *handle);


/*****************************************************************************

    NAME
        ConnxUartDrvGetBaudrate

    DESCRIPTION
        Retrieve the currently configured baud rate.

    PARAMETERS
        handle - handle of the UART device.

    RETURNS
        The currently configured baud rate.

*****************************************************************************/
uint32_t ConnxUartDrvGetBaudrate(void *handle);


/*****************************************************************************

    NAME
        ConnxUartDrvRegister

    DESCRIPTION
        Register a callback function and background interrupt handle to
        provide the UART driver a means for indicating reception of data to
        the upper layer.

    PARAMETERS
        handle - handle of the UART device.
        rxDataFn - this function is called when the upper layer calls the
            ConnxUartDrvRx function to receive data.
        rxBgintHandle - this background interrupt is triggered when data is
            available in the receive buffer.

*****************************************************************************/
void ConnxUartDrvRegister(void *handle, ConnxUartDrvDataRx rxDataFn, uint16_t rxBgintHandle);


#ifdef __cplusplus
}
#endif

#endif  /*_CONNX_SERIAL_COM_H_ */