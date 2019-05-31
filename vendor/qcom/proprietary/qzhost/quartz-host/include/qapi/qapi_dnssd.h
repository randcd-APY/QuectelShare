/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_dnssd.h
 *
 * @addtogroup qapi_networking_dnssd
 * @{
 *
 * @details The DNS service discovery provides APIs that allow
 * users to discover all the available services of a specific type
 * on the local link.
 *
 * @}
 */

#ifndef _QAPI_DNSSD_H
#define _QAPI_DNSSD_H

/** @addtogroup qapi_networking_dnssd
@{ */

#include "qapi/qapi_status.h"
#include "qapi_addr.h"  /* ip6_addr */

#define QAPI_NET_DNSSD_MAX_BUF_ENTRIES 5 /**< Maximum number of entries in the data buffer. */
#define QAPI_NET_DNSSD_MAX_TIMEOUT 5000  /**< Timeout for a discovery request. @newpage */


struct qapi_Net_DNSSD_Discover_t;

/**
 * Callback declarations.
 *
 * When a discovery request is made by the user, the callback
 * of the following types, registered during init, is invoked upon
 * completion of the request.
 */
typedef void (*qapi_Net_DNSSD_CB_t)(struct qapi_Net_DNSSD_Discover_t *param);

/**
 * Commands supported by DNS-SD.
 *
 * List of commands to start, stop, initialize, perform
 * discovery, and retrieve information from DNS-SD.
 */
typedef enum {
	QAPI_NET_DNSSD_START_E,		/**< Start DNS-SD service. */
	QAPI_NET_DNSSD_STOP_E,		/**< Stop DNS-SD service. */
	QAPI_NET_DNSSD_DISCOVER_E,	/**< Starts a new service discovery request. */
	QAPI_NET_DNSSD_INIT_E,		/**< Initialize DNS-SD service. */
	QAPI_NET_DNSSD_TARGET_INFO_E,	/**< Retrieve information about a service instance. */
} qapi_Net_DNSSD_Command_t;

/**
 * State machine for DNS-SD.
 *
 * The following are the different states through
 * which a DNS-SD service transitions during its
 * life cycle.
 */
typedef enum {
	QAPI_NET_DNSSD_STATE_STARTED_PENDING_INIT=1,	/**< DNS-SD started but pending initialization. */
	QAPI_NET_DNSSD_STATE_INIT,			/**< DNS-SD initialization is complete. */
	QAPI_NET_DNSSD_STATE_DISCOVERY_IN_PROGRESS,	/**< Service discovery request is in progress. */
	QAPI_NET_DNSSD_STATE_READY,			/**< Ready for the next discovery request. */
	QAPI_NET_DNSSD_STATE_STOPPED,			/**< DNS-SD service stopped. */
} qapi_Net_DNSSD_State_t;

/**
 * Context information for DNS-SD.
 *
 * Contains context-specific information, such as
 * state, callback, maximum buffer entries, default timeout
 * for discovery request, etc.
 */
typedef struct {
	uint32_t timeout;			/**< Timeout for a discovery request. */
	qapi_Net_DNSSD_CB_t cb;			/**< Application callback to be invoked upon completion of a request. */
	uint32_t max_Entries;			/**< Maximum number of entries in the data buffer. */
} qapi_Net_DNSSD_Ctxt_t;


/**
 * Response types.
 *
 * List of response types possible for a request.
 */
typedef enum {
	QAPI_NET_DNSSD_IPV4_ADDR = 0x01,	/**< Response contains an IPV4 address. */
	QAPI_NET_DNSSD_PTR = 0x0c,		/**< Response contains a pointer. */
	QAPI_NET_DNSSD_TYPE_TXT_INFO = 0x10,	/**< Response contains text information. */
	QAPI_NET_DNSSD_IPV6_ADDR = 0x1c,	/**< Response contains an IPV6 address. */
	QAPI_NET_DNSSD_TYPE_TARGET_INFO = 0x21,	/**< Response contains server information. */
	QAPI_NET_DNSSD_SEC = 0x2f,		/**< Response contains security information. */
	QAPI_NET_DNSSD_ANY = 0xff,		/**< Used in specific scenarios. */
} qapi_Net_DNSSD_Response_Type_t;

/**
 * DNS-SD start command.
 *
 * Contains the IP version and interface name that are
 * necessary to start the DNS-SD service.
 */
typedef struct {
	uint32_t ip_Version;			/**< IP version: IPv4, IPv6, or both. */
	char *if_Name;					/**< Name of the interface. */
} qapi_Net_DNSSD_Start_t;

/**
 * DNS-SD initialization command.
 *
 * Contains information needed to initialize the
 * DNS-SD service, such as context, timeout, maximum number
 * of buffer entries, callback information, etc.
 */
typedef struct {
	qapi_Net_DNSSD_Ctxt_t **ctxt;		/**< Context information. */
	uint32_t timeout;			/**< Request timeout. */
	uint32_t max_Entries;				/**< Maximum number of entries in the data buffer. */
	qapi_Net_DNSSD_CB_t cb;			/**< QAPI callback to be invoked upon request completion. */
} qapi_Net_DNSSD_Init_t;

/**
 * @brief DNS-SD server information.
 *
 * @details
 * Contains information about a server
 * hosting the requested type of service.
 */
typedef struct {
	uint16_t priority;			/**< Priority information. */
	uint16_t weight;			/**< Weight. */
	uint16_t port;				/**< Port number on which the service is running. */
	uint8_t *server_Name;		/**< Name of the server. */
} qapi_Net_DNSSD_Target_Info_t;

/**
 * @brief DNS-SD data.
 *
 * @details
 * Data for a specific instance of the requested
 * service.
 */
typedef struct {
	qapi_Net_DNSSD_Response_Type_t type;		/**< Type of response. Can be PTR, IPV4, etc. */
	char *name;					/**< Service instance name. */
	uint32_t ttl;					/**< Time for which the record is valid. */
	union {
		char *svr_Name;				/**< Server information received in response of type QAPI_NET_DNSSD_PTR. */
		qapi_Net_DNSSD_Target_Info_t *target;	/**< Server information. */
		uint32_t ipv4_Addr;			/**< Holds the IPv4 address of the server. */
		ip6_addr *ipv6_Addr;			/**< Holds the IPv6 address of the server. */
		char *txt[5];				/**< Text information. */
	} data;						/**< Data entry containing the actual response for the query. */
} qapi_Net_DNSSD_Data_t;

/**
 * @brief DNS-SD data entry.
 *
 * @details
 * Data buffer and the count of the
 * number of available entries.
 */
typedef struct {
	qapi_Net_DNSSD_Data_t **data;		/**< Array of pointers to the data buffer. */
	uint32_t data_Count;			/**< Count of the number of available data entries. */
} qapi_Net_DNSSD_Discover_Entry_t;

/**
 * @brief DNS-SD discover data buffer.
 *
 * @details
 * Data buffer used to hold the responses
 * corresponding to a discovery request.
 */
typedef struct qapi_Net_DNSSD_Discover_t {
    qapi_Net_DNSSD_Ctxt_t *ctxt;
    /**< Context information. */
    qapi_Net_DNSSD_Discover_Entry_t **entries;
    /**< Array of pointers to the service instances
         that in turn point to a data buffer for each service instance. */
    uint32_t entry_Count;
    /**< Count of the number of service instances.*/
} qapi_Net_DNSSD_Discover_t;


/**
 * DNS-SD service start.
 *
 * Kick-starts the DNS-SD service.
 *
 * @param[in] start Start command arguments (IP version and interface name).
 *
 * @return
 * Status of the command execution.
 */
qapi_Status_t qapi_Net_DNSSD_Start(qapi_Net_DNSSD_Start_t *start);

/**
 * Stops the DNS-SD service.
 *
 * @param[in] ctxt Context of the DNS-SD to be stoped.
 *
 * @return
 * Status of the command execution.
 */
qapi_Status_t qapi_Net_DNSSD_Stop(qapi_Net_DNSSD_Ctxt_t *ctxt);

/**
 * Initializes some of the necessary parameters for service discovery,
 * such as timeout, maximum entries, callback, etc.
 *
 * @param[in] init Specifies the timeout, maximum entries, and callback.
 *
 * @return
 * Status of the command execution.
 */
qapi_Status_t qapi_Net_DNSSD_Init(qapi_Net_DNSSD_Init_t *init);

/**
 * Performs service discovery for the given service instance
 * on the local link.
 *
 * @param[in] svcName Name of the service instance to be discovered.
 *
 * @return
 * Status of the command execution.
 */
qapi_Status_t qapi_Net_DNSSD_Discover(const char *svcName);

/**
 * Gets the server information for the given service instance.
 *
 * @param[in] svcName Name of the service instance.
 *
 * @return
 * Status of the command execution.
 */
qapi_Status_t qapi_Net_DNSSD_Get_Target_Info(const char *svcName);
/** @} */
#endif
