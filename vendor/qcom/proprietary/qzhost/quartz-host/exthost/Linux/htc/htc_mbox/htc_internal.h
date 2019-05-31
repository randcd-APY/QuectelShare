/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Copyright (c) 2004-2006 Atheros Communications Inc.
 * All rights reserved.
 */
/* Host-Target Communication API */

#ifndef _HTC_H_
#define _HTC_H_

#include <assert.h>

#ifdef USER_SPACE

#define		SPI_THREAD_STACKSIZE		16536

#ifndef     true
#define		true	1
#endif

#ifndef     false
#define		false   0
#endif

typedef	uint32_t	bool;

#define		BUG_ON(a)	assert(!(a))
#define		printk(a,...)

#endif

/*
 * TBD:
 * Need thorough doxygen documentation for each API.
 * Describe expectations for buffers (space for headers, 
 * space for block padding), asynchronous nature, ordering
 * of calls, startup, etc.
 *
 * Also describe compatibility (or not) with existing HTC
 * protocol versions.
 *
 * Are API names OK or should we be more "QAPI-esque"?
 */

#if defined(CONFIG_QCA402X_DEBUG)
#define DEBUG
#endif

/*
 * Maximum size of an HTC header.
 *
 * Callers of HTC must leave this many bytes before
 * the start of a buffer passed to any HTC API for
 * use by HTC itself.
 *
 * FUTURE: Investigate ways to remove this requirement.
 * Doesn't seem that hard to do....just tack on the
 * length in a separate buffer and send buffer pairs
 * to HIF. When extracting, first pull header then
 * pull payload into paired buffers.
 */
#define HTC_HEADER_LEN_MAX 8

/*
 * The HTC API is independent of the underlying interconnect and
 * independent of the protocols used across that interconnect.
 */

typedef int HTC_STATUS;
#define HTC_OK                0 /* Success */
#define HTC_ERROR             1 /* generic error */
#define HTC_EINVAL            2 /* Invalid parameter */
#define HTC_ECANCELED         3 /* Operation canceled */
#define HTC_EPROTO            4 /* Protocol error */
#define HTC_ENOMEM            5 /* Memory exhausted */

/*
 * Note: An Endpoint ID is always Interconnect-relative. So we
 * are likely to see the same Endpoint ID with different Targets
 * on a multi-Target system.
 */
typedef uint8_t HTC_ENDPOINT_ID;
#define HTC_EP_UNUSED (0xff)

typedef uint8_t HTC_EVENT_ID;
#define HTC_EVENT_UNUSED                   0

/* Start global events */
#define HTC_EVENT_GLOBAL_START             1
#define HTC_EVENT_TARGET_AVAILABLE         1
#define HTC_EVENT_TARGET_UNAVAILABLE       2
#define HTC_EVENT_GLOBAL_END               2
#define HTC_EVENT_GLOBAL_COUNT             (HTC_EVENT_GLOBAL_END - HTC_EVENT_GLOBAL_START + 1)
/* End global events */

/* Start endpoint-specific events */
#define HTC_EVENT_EP_START                 3
#define HTC_EVENT_BUFFER_RECEIVED          3
#define HTC_EVENT_BUFFER_SENT              4
#define HTC_EVENT_DATA_AVAILABLE           5
#define HTC_EVENT_EP_END                   5
#define HTC_EVENT_EP_COUNT                 (HTC_EVENT_EP_END - HTC_EVENT_EP_START + 1)
/* End endpoint-specific events */

#define HTC_IS_GLOBAL_EVENT(n) (((n) >= HTC_EVENT_GLOBAL_START) && ((n) <= HTC_EVENT_GLOBAL_END))
#define HTC_IS_EP_EVENT(n) (((n) >= HTC_EVENT_EP_START) && ((n) <= HTC_EVENT_EP_END))

struct htc_target;              /* opaque */
typedef struct htc_target HTC_TARGET;

typedef struct htc_event_info {
    uint8_t *buffer;
    void *cookie;
    uint32_t bufferLength;
    uint32_t actualLength;
    HTC_STATUS status;
} HTC_EVENT_INFO;

typedef void (*HTC_EVENT_HANDLER) (HTC_TARGET * target,
                                   HTC_ENDPOINT_ID ep, HTC_EVENT_ID eventId, HTC_EVENT_INFO * eventInfo, void *context);

HTC_STATUS HTCInit(void *pInfo);

void HTCShutDown(void);

HTC_STATUS HTCStart(HTC_TARGET * target);

void HTCStop(HTC_TARGET * target);

HTC_STATUS HTCEventReg(HTC_TARGET * target,
                       HTC_ENDPOINT_ID endPointId, HTC_EVENT_ID eventId, HTC_EVENT_HANDLER eventHandler, void *context);

HTC_STATUS HTCBufferReceive(HTC_TARGET * target,
                            HTC_ENDPOINT_ID endPointId, uint8_t * buffer, uint32_t length, void *cookie);

HTC_STATUS HTCBufferSend(HTC_TARGET * target,
                         HTC_ENDPOINT_ID endPointId, uint8_t * buffer, uint32_t length, void *cookie);

void signal_htc(void* pCxt);

#endif                          /* _HTC_H_ */
