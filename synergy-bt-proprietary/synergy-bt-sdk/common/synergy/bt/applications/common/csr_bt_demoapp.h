#ifndef CSR_BT_DEMOAPP_H__
#define CSR_BT_DEMOAPP_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2012-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_bt_sd_lib.h"

#include "csr_bt_file.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Global demo settings */
extern BD_ADDR_T defGlobalBdAddr;
extern CsrUint16 romCrystalFreq;
extern CsrUint32 romBaud;
extern BD_ADDR_T romBdAddress;
extern CsrUint32 logTaskLevel;
extern CsrUint32 logEnvLevel;
extern CsrUint32 logTextLevel;
extern schedEntryFunction_t CSR_TM_BLUECORE_TRANSPORT_INIT;
extern CsrUint16 transportType;
extern char casira_ComPort[128];

/* Application functions that needs to be implemented in order to
 * augment command line parsing */

/* Usage parameters */
void applicationUsage(char *col, char *des, char *opt);

/* Command line parser injection
 * In case of errors call usage(argv[0], char *error) */
char *applicationCmdLineParse(int optionChar, char *optionArgument);

/* Fetches getopt style parameters, "" means no parameters */
char *applicationExtraOptions(void);

/* Application helper functions for Device Identification */
extern void CmnDiPrintServiceRecordV13(CsrBtSdDiServiceRecordV13Struct *v13);
extern void CmnPrintDataArrayInHex(CsrUint8  *data_p, CsrUint16 dataLen);

/* Application helper functions for CoD decoding */
extern char *CodDecodeDeviceClass(CsrUint32 cod);

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_DEMOAPP_H__ */

