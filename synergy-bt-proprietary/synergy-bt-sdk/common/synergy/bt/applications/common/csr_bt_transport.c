/****************************************************************************

Copyright (c) 2009-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #2 $
****************************************************************************/

#include "csr_synergy.h"
#include "platform/csr_serial_init.h"
#include "csr_bt_app.h"

void CsrBtAppTransportSetDefault(CsrUint16 transportType)
{
    if (CsrStrLen(casira_ComPort) == 0)
    {
        switch(transportType)
        {
            case TRANSPORT_TYPE_H5_THREE_WIRE: /* fall through */
            case TRANSPORT_TYPE_H4_UART:       /* fall through */
            case TRANSPORT_TYPE_H4_DS:         /* fall through */
            case TRANSPORT_TYPE_H4_IBS:        /* fall through */
            case TRANSPORT_TYPE_BCSP:
                {
                    CsrStrLCpy(casira_ComPort, DEFAULT_COMPORT, sizeof(casira_ComPort));

                    if (CsrStrLen(baudRate) == 0)
                    {
                        CsrStrLCpy(baudRate, DEFAULT_BAUDRATE, sizeof(baudRate));
                    }
                    break;
                }
        }
    }
}

void CsrBtAppTransportSetTask(CsrUint16 transportType)
{
    switch(transportType)
    {
        case TRANSPORT_TYPE_BCSP:
            {
                CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_BCSP_INIT;
                break;
            }

#ifdef CSR_H4DS_TRANSPORT_ENABLE
        case TRANSPORT_TYPE_H4_DS:
            {
                CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_H4DS_INIT;
                break;
            }
#endif

#ifdef CSR_H4_TRANSPORT_ENABLE
        case TRANSPORT_TYPE_H4_UART:
            {
                CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_H4_UART_INIT;
                break;
            }
#endif

#ifdef CSR_H4IBS_TRANSPORT_ENABLE
            case TRANSPORT_TYPE_H4_IBS:
            {
                CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_H4IBS_INIT;
                break;
            }
#endif

#ifdef CSR_HCI_SOCKET_TRANSPORT
        case TRANSPORT_TYPE_BLUEZ:
            {
                CSR_TM_BLUECORE_TRANSPORT_INIT = CSR_TM_BLUECORE_HCI_SOCKET_INIT;
                break;
            }
#endif

        default:
            printf("*** Unhandled transport type: %d\n", transportType);
            exit(1);
            break;
    }
}

void *CsrBtAppTransportInit(void)
{
    void *uartHdl = NULL;

    switch(transportType)
    {
        case TRANSPORT_TYPE_H5_THREE_WIRE: /* fall through */
#ifdef CSR_H4DS_TRANSPORT_ENABLE
        case TRANSPORT_TYPE_H4_DS:      /* fall through */
#endif
            {
                printf("H4/H4DS/H5: Using COM port: %s and baud rate: %s\n",
                       casira_ComPort,baudRate);

                uartHdl = CsrUartDrvInitialise(casira_ComPort,
                                               CsrStrToInt(baudRate),
                                               romBaud,
                                               CSR_UART_DRV_DATA_BITS_8,
                                               CSR_UART_DRV_PARITY_NONE,
                                               CSR_UART_DRV_STOP_BITS_1,
                                               TRUE, NULL);
                if (uartHdl == NULL)
                {
                    printf("Unable to initialise UART driver\n");
                    exit(1);
                }
                else
                {
                    CsrTmBlueCoreRegisterUartHandleH4ds(uartHdl);
                }
            }
            break;

#ifdef CSR_H4_TRANSPORT_ENABLE
        case TRANSPORT_TYPE_H4_UART:
            {
                printf("H4: Using COM port: %s and baud rate: %s\n",
                       casira_ComPort,baudRate);

                uartHdl = CsrUartDrvInitialise(casira_ComPort,
                                               CsrStrToInt(baudRate),
                                               romBaud,
                                               CSR_UART_DRV_DATA_BITS_8,
                                               CSR_UART_DRV_PARITY_NONE,
                                               CSR_UART_DRV_STOP_BITS_1,
                                               TRUE, NULL);
                if (uartHdl == NULL)
                {
                    printf("Unable to initialise UART driver\n");
                    exit(1);
                }
                else
                {
                    CsrTmBlueCoreRegisterUartHandleH4(uartHdl);
                }
            }
            break;
#endif

#ifdef CSR_H4IBS_TRANSPORT_ENABLE
        case TRANSPORT_TYPE_H4_IBS:
            {
                printf("H4IBS: Using COM port: %s and baud rate: %s\n",
                       casira_ComPort,baudRate);

                uartHdl = CsrUartDrvInitialise(casira_ComPort,
                                               CsrStrToInt(baudRate),
                                               romBaud,
                                               CSR_UART_DRV_DATA_BITS_8,
                                               CSR_UART_DRV_PARITY_NONE,
                                               CSR_UART_DRV_STOP_BITS_1,
                                               TRUE, NULL);
                if (uartHdl == NULL)
                {
                    printf("Unable to initialise UART driver\n");
                    exit(1);
                }
                else
                {
                    CsrTmBlueCoreRegisterUartHandleH4ibs(uartHdl);
                }
            }
            break;
#endif

        case TRANSPORT_TYPE_BCSP:
            {
                CsrUint8 token = 0xC0;

                printf("BCSP: Using COM port: %s and baud rate: %s\n",
                       casira_ComPort,baudRate);

                uartHdl = CsrUartDrvInitialise(casira_ComPort,
                                               CsrStrToInt(baudRate),
                                               romBaud,
                                               CSR_UART_DRV_DATA_BITS_8,
                                               CSR_UART_DRV_PARITY_EVEN,
                                               CSR_UART_DRV_STOP_BITS_1,
                                               FALSE, &token);
                if (uartHdl == NULL)
                {
                    printf("Unable to initialise UART driver\n");
                    exit(1);
                }
                else
                {
                    CsrTmBlueCoreRegisterUartHandleBcsp(uartHdl);
                }
                break;
            }
#ifdef CSR_IBFP_H4I_SUPPORT
            case TRANSPORT_TYPE_H4_I:
            {
                printf("H4I: Using COM port: %s and baud rate: %s\n",
                       casira_ComPort,baudRate);

                uartHdl = CsrUartDrvInitialise(casira_ComPort,
                                               CsrStrToInt(baudRate),
                                               romBaud,
                                               CSR_UART_DRV_DATA_BITS_8,
                                               CSR_UART_DRV_PARITY_EVEN,
                                               CSR_UART_DRV_STOP_BITS_1,
                                               FALSE, NULL);


                CsrTmBlueCoreRegisterUartHandleH4i(&uartHdl);
                
                break;
            }
#endif

#ifdef CSR_HCI_SOCKET_TRANSPORT
        case TRANSPORT_TYPE_BLUEZ:
            {
                printf("BlueZ transport: Using device: %s \n", casira_ComPort);

                CsrTransportHciSocketConfigure(casira_ComPort);
            }
            break;
#endif

        default:
            printf(" *** Unhandled transport type ***\n");
            exit(1);
            break;
    }

    return(uartHdl);
}

void CsrBtAppTransportDeinit(void *uartHdl)
{
    if (uartHdl != NULL)
    {
        CsrUartDrvDeinitialise(uartHdl);
    }
    else
    {
        /* Do nothing */
    }
}

