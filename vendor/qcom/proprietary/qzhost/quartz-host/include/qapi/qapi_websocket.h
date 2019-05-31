/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_websocket.h
 *
 * @addtogroup qapi_networking_websocket
 * @{
 *
 * @details The Websocket client provides methods to establish a Websocket
 * client connection, send and receive message, to send pings or one-way
 * pongs, or receive pongs.  Due to limited memory large messages are
 * received in chunks to the registered event callback.  The size of the
 * chunk is controlled by the max_Recv_Chunk_Size configuration parameter.
 * When the end of the message is reached, the end_Of_Message callback
 * parameter is set to TRUE.
 *
 * Note that the Websocket client does not validate text data as valid
 * UTF-8 for either send or receive.  The application must valid this if
 * required.
 *
 * @}
 */

#ifndef _QAPI_WEBSOCKET_H_
#define _QAPI_WEBSOCKET_H_

#include "qapi/qapi_types.h"
#include "qapi/qapi_net_status.h"
#include "qapi_ssl.h"           /* qapi_Net_SSL_Obj_Hdl_t */

/**
 * @brief The state of the websocket.
 */
typedef enum {
	QAPI_NET_WEBSOCKET_STATE_INITIAL_E = 0, /**< Websocket handshake has not started. */
	QAPI_NET_WEBSOCKET_STATE_CONNECTING_E = 1, /**< Websocket handshake in progress. */
	QAPI_NET_WEBSOCKET_STATE_OPEN_E = 2, /**< Websocket handshake complete. */
	QAPI_NET_WEBSOCKET_STATE_CLOSING_E = 3, /**< Closing handshake started. */
	QAPI_NET_WEBSOCKET_STATE_CLOSED_E = 4, /**< Websocket closed. */
} qapi_Net_Websocket_State_t;

/**
 * @brief Websocket options that can be read using qapi_Net_Websocket_Get_Opt.
 */
typedef enum {
	QAPI_NET_WEBSOCKET_OPTION_STATE = 1, /**< Current state of websocket, e.g. OPEN, CLOSED. */
	QAPI_NET_WEBSOCKET_OPTION_ERRNO = 2, /**< Last RX/TX or handshake error. */
	QAPI_NET_WEBSOCKET_OPTION_SOCKET_ERRNO = 3, /**< Last socket error (if errno is QAPI_NET_WEBSOCKET_STATUS_SOCKET_ERROR). */
	QAPI_NET_WEBSOCKET_OPTION_SSL_ERRNO = 4, /**< Last ssl error (if errno is QAPI_NET_WEBSOCKET_STATUS_SSL_ERROR). */
	QAPI_NET_WEBSOCKET_OPTION_NUM_RX_BYTES = 5, /**< Total number of bytes received after connection established. */
	QAPI_NET_WEBSOCKET_OPTION_NUM_TX_BYTES = 6, /**< Total number of bytes sent after connection established. */
	QAPI_NET_WEBSOCKET_OPTION_NUM_RX_PINGS = 7, /**< Total number of pings received. */
	QAPI_NET_WEBSOCKET_OPTION_NUM_TX_PINGS = 8, /**< Total number of pings sent. */
	QAPI_NET_WEBSOCKET_OPTION_NUM_RX_PONGS = 9, /**< Total number of pongs received. */
	QAPI_NET_WEBSOCKET_OPTION_NUM_TX_PONGS = 10 /**< Total number of pongs sent. */
} qapi_Net_Websocket_Option_t;

/**
 * @brief Client or server.
 */
typedef enum {
	QAPI_NET_WEBSOCKET_MODE_CLIENT_E = 0, /**< Websocket client. */
	QAPI_NET_WEBSOCKET_MODE_SERVER_E = 1 /**< Websocket server. */
} qapi_Net_Websocket_Mode_t;

/**
 * @brief Type of data sent or received on websocket.
 */
typedef enum {
	QAPI_NET_WEBSOCKET_DATA_TYPE_TEXT_E = 1, /**< UTF-8 text data. */
	QAPI_NET_WEBSOCKET_DATA_TYPE_BINARY_E = 2 /**< Binary data. */
} QAPI_Net_Websocket_Data_Type_t;

/**
 * @brief Websocket event type.  Provided to qapi_Net_Event_CB_t callback.
 */
typedef enum {
	QAPI_NET_WEBSOCKET_EVENT_CONNECT_E = 1, /**< websocket connection established. */
	QAPI_NET_WEBSOCKET_EVENT_MESSAGE_E = 2, /**< message or partial message received. */
	QAPI_NET_WEBSOCKET_EVENT_PONG_E = 3, /**< pong received. */
	QAPI_NET_WEBSOCKET_EVENT_CLOSE_E = 4 /**< websocket connection closed or failed to connect. */
} QAPI_Net_Websocket_Event_t;

/**
 * @brief Connection close status.
 */
typedef enum {
	QAPI_NET_WEBSOCKET_CLOSE_STATUS_NORMAL = 1000, /**< normal closure. */
	QAPI_NET_WEBSOCKET_CLOSE_STATUS_GOING_AWAY = 1001, /**< end point is going away. */
	QAPI_NET_WEBSOCKET_CLOSE_STATUS_PROTOCOL_ERROR = 1002, /**< protocol error. */
	QAPI_NET_WEBSOCKET_CLOSE_STATUS_UNSUPPORTED_MESSAGE_TYPE = 1003, /**< unsupported message type. */
	QAPI_NET_WEBSOCKET_CLOSE_STATUS_NO_STATUS_RECEIVED = 1005, /**< No close status specified. */
	QAPI_NET_WEBSOCKET_CLOSE_STATUS_ABNORMAL_CLOSURE = 1006, /**< Peer closed connection without sending close frame. */
	QAPI_NET_WEBSOCKET_CLOSE_STATUS_DATA_DOES_NOT_MATCH_MESSAGE_TYPE = 1007, /**< data does not match message type (e.g., non-UTF-8 in text message). */
	QAPI_NET_WEBSOCKET_CLOSE_STATUS_POLICY_VIOLATION = 1008, /**< policy violation. */
	QAPI_NET_WEBSOCKET_CLOSE_STATUS_MESSAGE_TOO_BIG = 1009, /**< message too big to process. */
	QAPI_NET_WEBSOCKET_CLOSE_STATUS_EXPECTED_EXTENSION = 1010, /**< expected server to negotiate extension. */
	QAPI_NET_WEBSOCKET_CLOSE_STATUS_UNEXPECTED_CONDITION = 1011 /**< unexpected condition. */
} qapi_Net_Websocket_Close_Status_t;

/**
 * @brief Handle to websocket context.
 */
typedef uint32_t qapi_Net_Websocket_Hdl_t;

/**
 * @brief Websocket event. Received by qapi_Net_Event_CB_t callback
 * registered with qapi_Net_Websocket_Register_Event_Callback.
 */
typedef struct __qapi_Net_Websocket_Event_Info_t
{
	QAPI_Net_Websocket_Data_Type_t data_Type;
	/**< Type of event see definition of QAPI_Net_Websocket_Data_Type_t. */

	const void *data;
	/**< For MESSAGE events contains the message or part of the message.
	 *  For CLOSE events contains the close reason if specified by the peer.
	 *  For PONG events contains optional pong data. Assume only valid
	 *  during callback. */

	size_t data_Length;
	/**< Length of data */

	qbool_t end_Of_Message;
	/**< Applies to MESSAGE event only.  Indicates if end of the current
	 *   message was reach or if there is more data to be read. Data is
	 *   read in chunks of at most max_Recv_Chunk_Size
	 *   (see qapi_Net_Websocket_Client_Config_t). */

	qapi_Net_Websocket_Close_Status_t close_Status_Code;
	/**< Applies to CLOSE event only. */

	const char *subprotocol;
	/**< Negotiated sub-protocol.  Applies to CONNECT event only.  Assume only valid during callback.*/

} qapi_Net_Websocket_Event_Info_t;

/**
 * @brief Callback to receive new websocket messages.
 */
typedef void (* qapi_Net_Event_CB_t)(
	qapi_Net_Websocket_Hdl_t handle,
	/**< Websocket handle. */

    void * arg,
    /**< User-specified argument. */

	QAPI_Net_Websocket_Event_t event,
	/**< Websocket event type. */

	qapi_Net_Websocket_Event_Info_t *event_Info
	/**< Contains data associated with event. */
    );

/**
 * @brief Client configuration parameters.  For use with qapi_Net_Websocket_Client_New.
 */
typedef struct __qapi_Net_Websocket_Client_Config_t
{
	const char *origin;
	/**< Client origin.  NULL if no origin specified. */

	const char **subprotocol_List;
	/**< List of string containing subprotocols. NULL if no subprotocol specified. */

	size_t subprotocol_List_Length;
	/**< Length of subprotocol list. */

	qapi_Net_SSL_Obj_Hdl_t ssl_Object_Handle;
	/**< SSL context for websocket over SSL.  NULL for no SSL. */

	size_t max_Recv_Chunk_Size;
	/**< Messages longer than this value will be passed to recv callback in chunks of max_Recv_Chunk_Size. */

	uint32_t handshake_Timeout_Millseconds;
	/**< Handshake (qapi_Net_Websocket_Client_Connect) timeout in milliseconds. */

	uint32_t closing_Timeout_Millseconds;
	/**< Closing timeout in milliseconds. If the peer does not respond after we have initiated a close
	 * within the timeout, the Websocket is forcibly shutdown. */

} qapi_Net_Websocket_Client_Config_t;

/**
 * @brief Create a client websocket context.
 *
 * @param[in] config  Client config parameters.
 *
 * @return On success, client handle return. NULL returned on failure.
 */
qapi_Net_Websocket_Hdl_t qapi_Net_Websocket_Client_New(qapi_Net_Websocket_Client_Config_t *config);

/**
 * @brief Sets SSL configuration parameters on a secure (i.e., HTTPS) session.
 *
 * @param[in] handle    Client handle returned from qapi_Net_Websocket_Client_New.
 *
 * @param[in] ssl_Cfg   SSL connection parameters.
 *
 * @return On success, 0 is returned. On error, nonzero error code is returned.
 */
qapi_Status_t qapi_Net_Websocket_Configure_SSL(qapi_Net_Websocket_Hdl_t handle, qapi_Net_SSL_Config_t *ssl_Cfg);

/**
 * @brief Free a client websocket context.
 *
 * @param[in] handle  Client handle returned from qapi_Net_Websocket_Client_New.
 *
 * @return On success, returns QAPI_OK.  Else returns error code.
 */
qapi_Status_t qapi_Net_Websocket_Client_Free(qapi_Net_Websocket_Hdl_t handle);

/**
 * @brief Adds a HTTP header field to the websocket client handshake HTTP request.
 *
 * @param[in] handle   Client handle returned from qapi_Net_Websocket_Client_New.
 *
 * @param[in] type     Pointer to the header field name.
 *
 * @param[in] value    Pointer to the header field value.
 *
 * @return
 * On success, QAPI_OK is returned; other on error code.
 */
qapi_Status_t qapi_Net_Websocket_Client_Add_Handshake_HTTP_Header_Field(qapi_Net_Websocket_Hdl_t handle, const char *type, const char *value);

/**
 * @brief Clear HTTP header fields added with qapi_Net_Websocket_Client_Add_Handshake_HTTP_Header_Field.
 *
 * @param[in] handle   Client handle returned from qapi_Net_Websocket_Client_New.
 *
 * @return
 * On success, QAPI_OK is returned; other on error code.
 */
qapi_Status_t qapi_Net_Websocket_Client_Clear_Handshake_HTTP_Headers(qapi_Net_Websocket_Hdl_t handle);

/**
 * @brief Register a callback that handles messages received from the peer.
 *
 * Note that the callback might only get part of a message if the message is
 * larger than the available memory for buffering the data.  In that case
 * the end of message flag will be set to 0.
 *
 * @param[in] handle  Client handle returned from qapi_Net_Websocket_Client_New.
 * @param[in] callback Event callback.  Since the callback returns in a thread context
 *            shared by other websockets and HTTP clients, the callback is expected to
 *            return promptly.
 * @param[in] arg Caller-specified pointer that will be passed as an argument to callback.
 *
 * @return On success, returns QAPI_OK.  Else returns error code.
 */
qapi_Status_t qapi_Net_Websocket_Register_Event_Callback(qapi_Net_Websocket_Hdl_t handle, qapi_Net_Event_CB_t callback, void *arg);

/**
 * @brief Establishes a websocket connection to the server.
 *
 * If the websocket was previously used for a connection that was closed,
 * the websocket can be reused to establish a new connection by calling
 * connect again.
 *
 * @param[in] handle  Client handle returned from qapi_Net_Websocket_Client_New.
 * @param[in] server  Server hostname or IP address.
 * @param[in] port Server port.
 * @param[in] timeout_Millseconds Connect timeout in milliseconds.
 *
 * @return On success, returns QAPI_OK.  Else returns error code.
 */
qapi_Status_t qapi_Net_Websocket_Client_Connect(qapi_Net_Websocket_Hdl_t handle, const char *server, uint16_t port, const char *resource_Path);

/**
 * @brief Close the websocket connection and notify the sender.  This also closes the underlying
 * TCP socket and SSL connection.
 *
 * @param[in] handle  Client handle returned from qapi_Net_Websocket_Client_New.
 * @param[in] close_Status_Code Code indicating why the connection was closed. See
 * 			  qapi_Net_Websocket_Close_Status_t for status codes.
 * @param[in] close_Reason Optional string containing the reason the connection was closed.
 *            Can be NULL if no reason is provided.
 *
 * @return On success, returns QAPI_OK.  Else returns error code.
 */
int qapi_Net_Websocket_Close(qapi_Net_Websocket_Hdl_t handle, qapi_Net_Websocket_Close_Status_t close_Status_Code, const char *close_Reason);

/**
 * @brief Get properties of the websocket.  For example, this can be used to get
 * the negociated subprotocol.
 *
 * @param[in] handle  Client handle returned from qapi_Net_Websocket_Client_New.
 * @param[in] opt The option to get.  See qapi_Net_Websocket_Option_t.
 * @param[out] opt_Value The buffer that will hold the option value.
 * @param[in/out] opt_Length Should be set by the caller to the length of opt_Value.
 *                If the call is sucessful, this will be set to the actual length.
 *
 * @return On success, returns QAPI_OK.  Else returns error code.
 */
qapi_Status_t qapi_Net_Websocket_Get_Opt(qapi_Net_Websocket_Hdl_t handle, qapi_Net_Websocket_Option_t opt, void *opt_Value, size_t *opt_Length);

/**
 * @brief Send a websocket ping.  The resulting pong will be receive by the
 * registered pong callback.
 *
 * @param[in] handle  Client handle returned from qapi_Net_Websocket_Client_New.
 * @param[in] data Optional data to include in the ping.
 * @param[in] data_Length Length of data.
 *
 * @return On success, returns QAPI_OK.  Else returns error code.
 */
qapi_Status_t qapi_Net_Websocket_Ping(qapi_Net_Websocket_Hdl_t handle, const void *data, size_t data_Length);

/**
 * @brief Send a websocket pong.  The application may choose to send a pong
 * unidirectionally without a ping for keep alive.
 *
 * @param[in] handle  Client handle returned from qapi_Net_Websocket_Client_New.
 * @param[in] data Optional data to include in the pong.
 * @param[in] data_Length Length of data.
 *
 * @return On success, returns QAPI_OK.  Else returns error code.
 */
qapi_Status_t qapi_Net_Websocket_Pong(qapi_Net_Websocket_Hdl_t handle, const void *data, size_t data_Length);

/**
 * @brief Send application data.
 *
 * @param[in] handle  Client handle returned from qapi_Net_Websocket_Client_New.
 * @param[in] type Type indicating if the data is text or binary.
 * @param[in] data Data to send.
 * @param[in] data_Length Length of data.
 * @param[in] end_Of_Message If data contains an entire message, this should be set to 1.
 *            Messages can be fragmented. If end_Of_Message is 0, this sends the data as
 *            a fragment of a message.  The last fragment of the message should be sent
 *            with end_Of_Message set to 1.
 *
 * @return On success, returns number of bytes sent.  Else returns error code ( return will be less than 0 ).
 */
int qapi_Net_Websocket_Send(qapi_Net_Websocket_Hdl_t handle, QAPI_Net_Websocket_Data_Type_t type, const void *data, uint64_t data_Length, qbool_t end_Of_Message);

#endif // _QAPI_WEBSOCKET_H_
