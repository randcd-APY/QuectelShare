#ifndef CSR_BT_COAL_UTIL_H__
#define CSR_BT_COAL_UTIL_H__
/****************************************************************************

Copyright (c) 2009-2014 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"
#include "csr_bt_types.h"
#include "csr_util.h"
#include "bluetooth.h"
#include "csr_bt_profiles.h"
#include "csr_mblk.h"
#include "csr_unicode.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EXCLUDE_CSR_BT_RFC_MODULE
#include "rfcomm_prim.h"
#endif

#if !defined(EXCLUDE_CSR_BT_BNEP_MODULE) || !defined(EXCLUDE_CSR_BT_BSL_MODULE)
#include "csr_bt_bnep_prim.h"
#endif

/*------------------------------------------------------------------*/

#define CSR_BT_CLAMP(a,min,max)        ((a) < (min) ? (min) : ((a) > (max) ? (max) : a))

/* Extended Inquery Response specific macros */
/*------------------------------------------------------------------*/
#define CSR_BT_EIR_CONV_UUID_SIZE_TO_TYPE(size, complete)     ((complete ? 1 : 0) + ((size) == CSR_BT_EIR_UUID128_SIZE ? CSR_BT_EIR_DATA_TYPE_MORE_128_BIT_UUID : (size)))
#define CSR_BT_EIR_CONV_UUID_TYPE_TO_SIZE(type)    (((type) >= CSR_BT_EIR_DATA_TYPE_MORE_128_BIT_UUID) ? CSR_BT_EIR_UUID128_SIZE : ((type) & 0xFE))
/* Macros for converting UUIDs to and from a (little endian) stream */
#define CSR_BT_EIR_GET_UINT16_FROM_PTR(ptr)        (((CsrUint16)((CsrUint8 *)(ptr))[0]) | ((CsrUint16)((CsrUint8 *)(ptr))[1]) << 8)
#define CSR_BT_EIR_GET_UINT32_FROM_PTR(ptr)        (((CsrUint32)((CsrUint8 *)(ptr))[0]) | ((CsrUint32)((CsrUint8 *)(ptr))[1]) << 8 | \
                                             ((CsrUint32)((CsrUint8 *)(ptr))[2]) << 16 | ((CsrUint32)((CsrUint8 *)(ptr))[3]) << 24)
#define CSR_BT_EIR_COPY_UINT16_TO_PTR(uint, ptr)    ((CsrUint8 *) (ptr))[0] = ((CsrUint8) ((uint) & 0x00FF)); \
                                             ((CsrUint8 *) (ptr))[1] = ((CsrUint8) ((uint) >> 8))
#define CSR_BT_EIR_COPY_UINT32_TO_PTR(uint, ptr)    ((CsrUint8 *) (ptr))[0] = ((CsrUint8) ((uint) & 0x000000FF)); \
                                             ((CsrUint8 *) (ptr))[1] = ((CsrUint8) (((uint) >> 8) & 0x000000FF)); \
                                             ((CsrUint8 *) (ptr))[2] = ((CsrUint8) (((uint) >> 16) & 0x000000FF)); \
                                             ((CsrUint8 *) (ptr))[3] = ((CsrUint8) (((uint) >> 24) & 0x000000FF))

/*------------------------------------------------------------------*/
/* Bluetooth address */
/*------------------------------------------------------------------*/
#ifdef CSR_CHIP_MANAGER_ENABLE
extern BD_ADDR_T *CsrBtBdAddrDup(const BD_ADDR_T *p_bd_addr_1);
#endif
extern void CsrBtBdAddrZero(BD_ADDR_T *pktBdAddr);
extern CsrBool CsrBtBdAddrEqZero(const BD_ADDR_T *pktBdAddr);
/* void CsrBtBdAddrCopy(BD_ADDR_T *p_bd_addr_dest, const BD_ADDR_T *p_bd_addr_src); 
   replaced by macro because it is faster and smaller - the code to call the function 
   is larger than inlining the function. */
#define CsrBtBdAddrCopy(addr_dest, addr_src) ((*addr_dest) = (*addr_src))

extern CsrBool CsrBtBdAddrEq( const BD_ADDR_T *p_bd_addr_1, const BD_ADDR_T *p_bd_addr_2 );

#define CSR_BT_BD_ADDR_EQ(addr1, addr2)           CsrBtBdAddrEq(&(addr1), &(addr2))
#define CSR_BT_BD_ADDR_COPY(addr_dest, addr_src)  CsrBtBdAddrCopy(&(addr_dest), &(addr_src))
#define CSR_BT_BD_ADDR_ZERO(addr)                 CsrBtBdAddrZero(&(addr))


#if !defined(EXCLUDE_CSR_BT_BNEP_MODULE) || !defined(EXCLUDE_CSR_BT_BSL_MODULE)
extern ETHER_ADDR CsrBtBdAddrToEtherAddr (const BD_ADDR_T *bdAddr);
extern BD_ADDR_T CsrBtBdAddrFromEtherAddr (const ETHER_ADDR *etherAddr);
extern ETHER_ADDR CsrBtConvBdAddr2EtherBig (BD_ADDR_T *bdAddr);
extern ETHER_ADDR CsrBtEtherAddrWordSwap(ETHER_ADDR *addr);
#if !defined(EXCLUDE_CSR_BT_BNEP_MODULE)
CsrUint16 CsrBtReadUint16Val(CsrUint8 *buf, CsrInt32 offset);
void CsrBtWriteUint8Val(CsrUint8 **buf, CsrUint8 val);
void CsrBtWriteUint16Val(CsrUint8 **buf, CsrUint16 val);
void CsrBtWriteUint32Val(CsrUint8 **buf, CsrUint32 val);
void CsrBtWriteBnepAddr(CsrUint8 **buf, const ETHER_ADDR *addr);
#endif /* !defined(EXCLUDE_CSR_BT_BNEP_MODULE) */
#endif /* !defined(EXCLUDE_CSR_BT_BNEP_MODULE) || !defined(EXCLUDE_CSR_BT_BSL_MODULE) */

#define CsrBtUtf8StrTruncate(source, size) CsrUtf8StrTruncate(source, size)

#ifdef CSR_BT_INSTALL_OPTIONAL_UTIL
extern char *CsrBtStrDupLen(const char *string, CsrUint32 *len);
#endif

#define CSR_BT_MODEM_SEND_CTRL_DCE_DEFAULT (RFC_MSC_RTR_BIT | RFC_MSC_RTC_BIT)
#define CSR_BT_MODEM_SEND_CTRL_DTE_DEFAULT (RFC_MSC_RTC_BIT | RFC_MSC_RTR_BIT | RFC_MSC_DV_BIT)
extern CsrUint8 CsrBtMapSendingControlSignal(CsrUint8 theModemStatus, CsrUint8 theDataRole);
extern CsrUint8 CsrBtMapReceivedControlSignal(CsrUint8 theModemStatus, CsrUint8 theDataRole);
extern void CsrBtPortParDefault(RFC_PORTNEG_VALUES_T *theportPar);
#ifdef CSR_BT_INSTALL_OPTIONAL_UTIL
extern char* CsrBtGetBaseName(char *file);
#endif
/* Special MBLK helpers */
extern CsrUint8 *CsrBtMblkConsumeToMemory (CsrMblk **pp_mblk);
extern CsrBool CsrBtMblkReadHead8(CsrMblk **mblk, CsrUint8 *value);
extern CsrBool CsrBtMblkReadHead16(CsrMblk **mblk, CsrUint16 *value);
extern CsrBool CsrBtMblkReadHead8s(CsrMblk **mblk, unsigned int num, ...);

extern void CsrBtArrayReverse(void *array, CsrUint16 length);
extern void CsrBtMemCpyPack(CsrUint16 *dest, const CsrUint8 *source, CsrUint16 length);
extern void CsrBtMemCpyUnpack(CsrUint8 *dest, const CsrUint16 *source, CsrUint16 length);

#ifdef __cplusplus
}
#endif

#endif

