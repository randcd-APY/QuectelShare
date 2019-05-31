/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_mdnss.h
 *
 * @addtogroup qapi_networking_mdnss
 * @{
 *
 * @details The Multicast DNS service provides APIs that allow users
 * to register available services and respond to mDNS queries
 * from other clients on a local link. It enables clients to perform DNS-like
 * operations on the local link in the absence of any conventionals Unicast DNS server.
 *
 * @}
 */

#ifndef _QAPI_mDNS_H
#define _QAPI_mDNS_H

/** @addtogroup qapi_networking_mdnss
@{ */

/**
 * @brief Commands supported by the mDNS service.
 *
 * @details
 * List of commands to start, stop, or disable an
 * mDNS responder, and to register/unregister
 * services with mDNS. 
 */
typedef enum {
	QAPI_NET_MDNS_START_E, 		/**< Start mDNS server. */
	QAPI_NET_MDNS_STOP_E, 		/**< Stop mDNS server. */
	QAPI_NET_MDNS_DISABLE_E, 	/**< Disable mDNS server. */
	QAPI_NET_MDNS_ADDSVC_E, 	/**< Add/register a service with mDNS. */
	QAPI_NET_MDNS_RMSVC_E, 		/**< Remove a service already registered with mDNS. */
	QAPI_NET_MDNS_SET_HOSTNAME_E, /**< Set the hostname for the mDNS server. The hostname should be set after starting mDNS
	 	 	 	 	 	 	 	 	 	and before registering a new service. If sethostname returns a hostname conflict
	 	 	 	 	 	 	 	 	 	error, retry sethostname until it succeeds. */
	QAPI_NET_MDNS_UPDATE_TXT_RECORD_E, /**< Update an existing text record or add a new text record for an already registered service. */
} qapi_Net_mDNS_Command_t;


/*
 * @brief Structure for updating or adding a new text record.
 *
 * @details
 * This struct contains the text record in the format key=value and a service name
 * that is already registered. If the key in the text record already exists for
 * the given service name, the value is updated with the given value, otherwise
 * the text record is added as a new text record.
 */
typedef struct {
	const char *svc_Name;	/**< Registered service name whose text record is to be updated. */
	const char *txt;		/**< Text record to be added or updated. */
} qapi_Net_mDNS_TXT_t;

/*
 * @brief Structure for registration status information.
 *
 * @details
 * This struct contains the name of the service/hostname and status code to denote
 * if the registration is successful or to report if there is a
 * subsequent conflict of hostname/servicename. An application callback will receive this
 * asynchronously in the following cases:
 * - Subsequent service/hostname conflict, after a service or hostname was successfully registered.
 * - If Nonblocking mode is used for an operation.
 */
typedef struct {
	const char *svc_Name;			/**< Service name or hostname whose status is reported. */
	qapi_Status_t status;			/**< Status code. */
} qapi_Net_mDNS_Svc_Reg_Status_t;


#define QAPI_NET_MDNS_MAX_TXT_RECORDS   10 /**< Maximum text records in a service information structure. */

/**
 * @brief mDNS service information structure.
 *
 */
typedef struct {
	char *instance;	/**< Name of the service instance to be registered. */
	char *type;		/**< Type of service instance to be registered. */
	uint16_t port;	/**< Port on which the service is listening (e.g., 80 for HTTP). */
	char *txt[QAPI_NET_MDNS_MAX_TXT_RECORDS]; /**< Array of strings containing information about the service. Last entry must be NULL */
} qapi_Net_mDNS_Svc_Info_t;

/**
 * @brief mDNS start command structure.
 *
 */
typedef struct {
	int ip_Version;   /**< IP version: IPV4, IPV6, or both. */
	char *if_Name;    /**< Name of the interface to be used by the mDNS service for outgoing/incoming mDNS packets. */
} qapi_Net_mDNS_Start_t;


/**
 * @brief mDNS application callback.
 *
 * Callback to be registered when the mDNS service is started.
 * The callback will be invoked for all mDNS commands (Nonblocking mode)
 * and for all asynchronous responses from mDNS.
 */
typedef int (*qapi_Net_mDNS_CB_t)(void *arg);

/**
 * @brief Handle for mDNS commands from the user.
 *
 * @param[in] cmd Command to be handled.
 *
 * @param[in] input Input parameter.
 *
 * @param[in] blocking Flag to indicate if the mDNS functions is in Blocking or Nonblocking mode.
 *             - If the flag = 1, all mDNS commands will block until complete (success or failure).
 *             - If the flag = 0, all mDNS commands will be nonblocking and will invoke the app callback
 *             to return the result of the operation. This flag is used only for the QAPI_NET_MDNS_START_E
 *             and QAPI_NET_MDNS_ADDSVC_E commands.
 *
 * @param[in] app_CB Application callback. The callback is invoked for nonblocking operations and
 *                   to return asynchronous responses, e.g., hostname conflicts in the local network.
 *                   Used only by the QAPI_NET_MDNS_START_E command.
 *
 * @return
 * Status of the command execution.
 */
qapi_Status_t qapi_Net_mDNS_Command(qapi_Net_mDNS_Command_t cmd, void *input, uint8_t blocking, qapi_Net_mDNS_CB_t app_CB);

/**
 *  @brief Starts the mDNS service.
 *
 *  @param[in] start Parameters required to start mDNS, such as ip_Version, if_Name, etc.
 *
 *  @param[in] app_CB Application callback. The callback is invoked for nonblocking operations and
 *                   to return asynchronous responses, e.g., hostname conflicts in the local network.
 *                   Used only by the QAPI_NET_MDNS_START_E command.
 *
 *  @param[in] blocking Flag to indicate if the mDNS function is in Blocking or Nonblocking mode.
 *             - If the flag = 1, all mDNS commands will block until complete (success or failure).
 *             - If the flag = 0, all mDNS commands will be nonblocking and will invoke the app callback
 *             to return the result of the operation. This flag is used only by the QAPI_NET_MDNS_START_E
 *             and QAPI_NET_MDNS_ADDSVC_E commands.
 *
 *  @return
 *  Status of the command execution.
 */
qapi_Status_t qapi_Net_mDNS_Start(qapi_Net_mDNS_Start_t *start, qapi_Net_mDNS_CB_t app_CB, uint8_t blocking);

/**
 * @brief Stops the mDNS service.
 *
 * @return
 * Status of the command execution.
 */
qapi_Status_t qapi_Net_mDNS_Stop(void);

/**
 * @brief Registers a new service with mDNS.
 *
 * @param[in] svc_Info Service information, such as name, type, port, txt records, etc.
 *
 * @param[in] blocking Flag to indicate if the service registration is blocking or nonblocking.
 *            - If the flag = 1, service registration will block until complete (success or failure).
 *            - If the flag = 0, service registration will be nonblocking and will invoke the app callback
 *            to return the result of the operation.
 *
 * @return
 * Status of the command execution.
 */
qapi_Status_t qapi_Net_mDNS_Register_Service(qapi_Net_mDNS_Svc_Info_t *svc_Info, uint8_t blocking);

/**
 * @brief Unregisters a service.
 *
 * @param[in] svc_Name Name of the service to be unregistered.
 *
 * @return
 * Status of the command execution.
 */
qapi_Status_t qapi_Net_mDNS_Unregister_Service(const char *svc_Name);

/**
 * @brief Sets the hostname for mDNS.
 *
 * param[in] host_Name Hostname to be set for mDNS. This is the hostname that will be used by queriers
 *                     to query for an IP address or a record.
 *
 * @return
 * Status of the command execution.
 */
qapi_Status_t qapi_Net_mDNS_Set_Hostname(const char *host_Name);

/**
 * @brief Updates or adds a new text record to an existing service.
 *
 * @param[in] txt_Update Contains the service name and the text record to be updated. If the text record
 *                       does not exist, a new text record will be created. The text record should be of the form key=value.
 *
 * @return
 * Status of the command execution.
 */
qapi_Status_t qapi_Net_mDNS_Update_TXT(qapi_Net_mDNS_TXT_t *txt_Update);
/** @} */

#endif
