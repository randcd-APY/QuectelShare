/*******************************************************************************

Copyright (c) 2008-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved.
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

(C) COPYRIGHT Cambridge Consultants Ltd 1999

*******************************************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_

#include "csr_synergy.h"

#ifdef BTCHIP
#include <stdarg.h>
#include "hci_debug.h"
#endif

#include "csr_bt_bluestack_types.h"
#include "bluetooth.h"
#include "csr_sched.h"
#include "csr_bt_tasks.h"
#include "csr_bt_util.h"

#ifndef BUILD_FOR_HOST
#include "bootmode.h"   /* For onchip_hci_client() */

/* Streams only used on chip */
#ifdef INSTALL_STREAM_MODULE
#include "stream.h"
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Timer IDs */
/* TODO, this conditionalisation should become unnecessary after B-62415 */
#if !defined(BUILD_FOR_HOST) || defined(BLUESTACK_HOST_IS_APPS)
typedef CsrUint32 CsrSchedTid;
#endif

/*! \brief Check for presence of HCI client. Always TRUE for BCHS. */
#ifdef BUILD_FOR_HOST
#ifdef BLUESTACK_HOST_IS_APPS
/* On the Hydra Apps subsystem, the HCI transport is never up when the Apps
 * initialises its message queues.  We wait until the BT full stack service is
 * started before starting Bluestack properly */
#define HciClientEnabled() FALSE
#else /* BLUESTACK_HOST_IS_APPS */
#define HciClientEnabled() TRUE
#endif /* BLUESTACK_HOST_IS_APPS */
#else /* BUILD_FOR_HOST */
#define HciClientEnabled() onchip_hci_client()
#endif /* BUILD_FOR_HOST */

/*! \brief Use of bitfields */
#ifndef BUILD_FOR_HOST
#define USE_BLUESTACK_BITFIELDS /*!< Always use them when on chip */
#endif

#ifdef USE_BLUESTACK_BITFIELDS
#define BITFIELD(type, name, size)  unsigned int name:size
#else
#define BITFIELD(type, name, size)  type name
#endif

/*! Timers. */
void timer_start(CsrSchedTid *p_id,
                 CsrTime delay,
                 void (*fn)(CsrUint16 mi, void *mv),
                 CsrUint16 fniarg,
                 void *fnvarg);
void timer_cancel(CsrSchedTid *p_id);
#define TIMER_EXPIRED(id) ((id) = (CsrSchedTid)0)

/*! \brief Special realloc may be needed for host builds

     On chip we have memory pools and can do clever prealloc magic
     whereas on-host we may need to know the old size for
     reallocations to work properly, hence the "wos" version ("with
     old size")
*/
#if defined (BUILD_FOR_HOST) && !defined(BLUESTACK_HOST_IS_APPS)
extern void* host_xprealloc(void *ptr, size_t oldsize, size_t newsize);
#define bpxprealloc(ptr, oldsize, newsize) \
    host_xprealloc((ptr), (oldsize), (newsize))
#else
#define bpxprealloc(ptr, oldsize, newsize) \
    xprealloc((ptr), (newsize))
#endif

/* Very basic minimum/maximum macros */
#if 0
#define CSRMIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#if 0
#define CSRMAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#if defined(INSTALL_STREAM_MODULE) && defined(INSTALL_RFCOMM_MODULE)
/* Bluestack stream destructor list stuff
*/
typedef enum
{
    RFCOMM_DESTRUCTOR,
    L2CAP_DESTRUCTOR,
    DM_DESTRUCTOR

} BS_DESTRUCTOR_TYPE;

typedef struct bs_destructor_tag
{
    struct bs_destructor_tag  *p_next;
    BS_DESTRUCTOR_TYPE  type;
    CsrUint16  stream_key;
    STREAMS   *stream;
    BD_ADDR_T bd_addr;

} BS_DESTRUCTOR_T;

extern void bs_add_to_destructor_list(BS_DESTRUCTOR_TYPE  type,
                                      CsrUint16  stream_key,
                                      STREAMS   *stream,
                                      BD_ADDR_T *bd_addr);
extern void bs_remove_from_distructor_list(BS_DESTRUCTOR_TYPE  type,
                                           CsrUint16  stream_key);
extern CsrBool bs_get_bd_addr(BS_DESTRUCTOR_TYPE  type,
                             CsrUint16  stream_key,
                             BD_ADDR_T *bd_addr);
#else
#define bs_add_to_destructor_list(type, key, stream, bd_addr) ((void)0)
#define bs_remove_from_distructor_list(type, key) ((void)0)
#define bs_get_bd_addr(type, key, bd_addr) (FALSE)
#endif

/* Read macros from a byte stream

   The standard macros are endian-safe and architecture-independent.
   Only define UINTR_OPTIMIZE_LITTLE_ENDIAN if you know that your architecture
   is little-endian and will support the macros this enables.*/

#ifdef UINTR_OPTIMIZE_LITTLE_ENDIAN

/* 8 bit numbers */
#define UINT8_R(ptr, offset) \
    (*((ptr) + (offset)))

/* 16 bit numbers */
#define UINT16_R(ptr, offset) \
    (*(CsrUint16 *)((ptr) + (offset)))

/* 32 bit numbers */
#define UINT32_R(ptr, offset) \
    (*(CsrUint32 *)((ptr) + (offset)))

#else
/* Note, for the XAP, non byte packed byte stream assumed as in c6066-TM-046a */

/* 8 bit numbers */
#define UINT8_R(ptr, offset) \
    ((CsrUint8)(ptr)[(offset)])

/* 16 bit numbers */
#define UINT16_R(ptr, offset)                          \
    ((CsrUint16)((CsrUint16)(ptr)[0 + (offset)]       |  \
                (CsrUint16)(ptr)[1 + (offset)] << 8))

/* 32 bit numbers */
#define UINT32_R(ptr, offset)                          \
    ((CsrUint32)((CsrUint32)(ptr)[0 + (offset)]       |  \
                (CsrUint32)(ptr)[1 + (offset)] << 8  |  \
                (CsrUint32)(ptr)[2 + (offset)] << 16 |  \
                (CsrUint32)(ptr)[3 + (offset)] << 24))
#endif

/* Writing uints of various sizes into a CsrUint8 buffer. */
#define URW_TYPE_BIT_OFFSET     3
#define URW_TOTAL_BIT_OFFSET    5

#define URW_SIZE_MASK           ((1 << URW_TYPE_BIT_OFFSET) - 1)
#define URW_TYPE_MASK           \
                ((1 << URW_TOTAL_BIT_OFFSET) - URW_SIZE_MASK - 1)

#define URW_TYPE_CsrUint8        (0 << URW_TYPE_BIT_OFFSET)
#define URW_TYPE_CsrUint16       (1 << URW_TYPE_BIT_OFFSET)
#define URW_TYPE_UNDEFINED      (2 << URW_TYPE_BIT_OFFSET)
#define URW_TYPE_CsrUint32       (3 << URW_TYPE_BIT_OFFSET)

#define URW_FORMAT_INDIVIDUAL(index, type, quantity) \
    (((quantity) | (URW_TYPE_ ## type)) << ((index)*URW_TOTAL_BIT_OFFSET))

#define URW_FORMAT(type1, quantity1, type2, quantity2, type3, quantity3) \
     (URW_FORMAT_INDIVIDUAL(0, type1, (quantity1)) \
    | URW_FORMAT_INDIVIDUAL(1, type2, (quantity2)) \
    | URW_FORMAT_INDIVIDUAL(2, type3, (quantity3)))

/* We write to the place pointed to by *buf and increment the pointer */
void write_uint8(CsrUint8 **buf, CsrUint8 val);
void write_uint16(CsrUint8 **buf, CsrUint16 val);
void write_uint32(CsrUint8 **buf, CsrUint32 *p_val); /* NOTE: Pointer to CsrUint32! */

/* Read CsrUint8/CsrUint16/CsrUint32 from buffer and increment the pointer */
CsrUint8 read_uint8(const CsrUint8 **buf);
CsrUint16 read_uint16(const CsrUint8 **buf);
CsrUint32 read_uint32(const CsrUint8 **buf);

/* Read or write various numbers of various sizes of uint_t into buffer.
   Use the URW_FORMAT macro for the format parameter. The pointer will
   be incremented automatically.

   While writing, the additional arguments should be CsrUint8, CsrUint16 or
   CsrUint32*, as specified by the format. Note the pointer to CsrUint32, rather
   than uin32 itself. While reading, the additional arguments should be
   CsrUint8*, CsrUint16*, CsrUint32*.
*/
/*lint -e1916 Ellipsis */
void read_uints(const CsrUint8 **buf, unsigned int format, ...);
/*lint -e1916 Ellipsis */
void write_uints(CsrUint8 **buf, unsigned int format, ...);

#define bd_addr_copy(a, b) CsrBtBdAddrCopy(a,b)
#define bd_addr_zero(p_bd_addr) CsrBtBdAddrZero(p_bd_addr)
#define bd_addr_eq(a, b) CsrBtBdAddrEq(a, b)

/* Bluestack CsrPmemFree(). Safe to use even on vm_const. */
#ifdef BUILD_FOR_HOST
#define bpfree(d) CsrPmemFree(d)
#else
void bpfree(void *ptr);
#endif

/* BCHS doesn't need vm_const_fetch() */
#ifdef BUILD_FOR_HOST
#define vm_const_fetch(ptr) (*(ptr))
#endif

/* Rationalise the various flavours of "UNUSED" */
#ifdef BLUESTACK_HOST_IS_APPS
#define BLUESTACK_UNUSED(x) UNUSED(x)
#else
#define BLUESTACK_UNUSED(x) CSR_UNUSED(x)
#endif
#ifndef PARAM_UNUSED
#define PARAM_UNUSED(x) BLUESTACK_UNUSED(x)
#endif

#ifdef __cplusplus
}
#endif


#endif

