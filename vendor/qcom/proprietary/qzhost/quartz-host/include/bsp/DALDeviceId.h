#ifndef DALIDEVICEID_H
#define DALIDEVICEID_H

/*
 * Copyright (c) 2015,2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*==================================================================================

                             EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file. Notice that
changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/api/dal/DALDeviceId.h#11 $
=============================================================================*/




/*=============================================================================
  SHARED DEVICES 
=============================================================================*/
#define DALDEVICEID_SYSTEM              0x02000001
#define DALDEVICEID_PWM                 0x02000002
#define DALDEVICEID_ADC                 0x02000003
#define DALDEVICEID_TSENS               0x02000004
#define DALDEVICEID_URAD_DALTF          0x02000005
#define DALDEVICEID_PLATFORM            0x02000006
#define DALDEVICEID_PLATFORM_INTERNAL   0x02000007

#define DALDEVICEID_UART_DEVICE_1       0x0200000e
#define DALDEVICEID_UART_DEVICE_2       0x0200000f
#define DALDEVICEID_SPI_DEVICE_1        0x02000010
#define DALDEVICEID_SPI_DEVICE_2        0x02000011
#define DALDEVICEID_SPI_DEVICE_3        0x02000012
#define DALDEVICEID_SPI_DEVICE_4        0x02000013
#define DALDEVICEID_DIAG                0x02000014
#define DALDEVICEID_GPIOINT             0x02000015
#define DALDEVICEID_INTERRUPTCONTROLLER 0x02000016
#define DALDEVICEID_IPCINT              0x02000017
#define DALDEVICEID_SLEEP               0x02000018
#define DALDEVICEID_DOG                 0x02000019
#define DALDEVICEID_TIMETICK            0x0200001a
#define DALDEVICEID_QUADSPI_DEVICE_1    0x0200001b
#define DALDEVICEID_TLMM                0x0200001c
#define DALDEVICEID_SDIO_SLAVE          0x0200001d

/*last device-id */
#define DALDEVICEID_TEST_DEVICE         0x020000FF

/*=============================================================================
  FOM ONLY
=============================================================================*/
#define DALDEVICEID_IOE_NET_SERVICES    0x03000001
#define DALDEVICEID_IOE_CRYPTO          0x03000002
#define DALDEVICEID_IOE_WLAN_0          0x03000003
#define DALDEVICEID_IOE_WLAN_1          0x03000004
#define DALDEVICEID_IOE_WLAN_2          0x03000005
#define DALDEVICEID_SDCC_DEVICE_1       0x03000006
#define DALDEVICEID_SDCC_DEVICE_2       0x03000007
#define DALDEVICEID_CONSS_PLAT          0x0300000a
#define DALDEVICEID_BLE                 0x0300000b
#define DALDEVICEID_I15P4               0x0300000c
#define DALDEVICEID_COEX                0x0300000d
#define DALDEVICEID_QIPC                0x0300000e
#define DALDEVICEID_I2S_DEVICE_1        0x0300000f
#define DALDEVICEID_I2S_DEVICE_2        0x03000010
#define DALDEVICEID_I2S_DEVICE_3        0x03000011
#define DALDEVICEID_I2S_DEVICE_4        0x03000012
#define DALDEVICEID_FS                  0x03000013
#define DALDEVICEID_RFS_CLIENT          0x03000014
#define DALDEVICEID_RFS_SERVER          0x03000015
#define DALDEVICEID_FW_UPGRADE_SCHEME   0x03000016
#define DALDEVICEID_ZIGBEE              0x03000017
#define DALDEVICEID_THREAD              0x03000018
#define DALDEVICEID_EXHOST              0x03000019

/*last device-id */
#define DALDEVICEID_FOM_TEST_DEVICE     0x030000FF

#endif //DALIDEVICEID_H
