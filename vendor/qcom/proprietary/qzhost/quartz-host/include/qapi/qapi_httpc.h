/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

 
/**
 * @file qapi_httpc.h
 *
 * @addtogroup qapi_networking_httpc
 * @{
 *
 * @details The HTTP client service provides a collection of API functions that
 * allow the application to enable and configure HTTP client services.
 * The HTTP client can be configured to support IPv4, IPv6, as well as
 * HTTP mode, HTTPS mode (secure), or both.
 *
 * @}
 */

#ifndef _QAPI_HTTPC_H_
#define _QAPI_HTTPC_H_

#include <stdint.h>
#include "qapi/qapi_status.h"   /* qapi_Status_t */
#include "qapi_ssl.h"           /* qapi_Net_SSL_Obj_Hdl_t */

/** @addtogroup qapi_networking_httpc
@{ */

/**
 * @brief For use with qapi_Net_HTTPc_Request().
 */
typedef enum {
	/*supported http client methods */
	QAPI_NET_HTTP_CLIENT_GET_E = 1,
	QAPI_NET_HTTP_CLIENT_HEAD_E,
	QAPI_NET_HTTP_CLIENT_POST_E,
	QAPI_NET_HTTP_CLIENT_PUT_E,
	QAPI_NET_HTTP_CLIENT_DELETE_E,
	QAPI_NET_HTTP_CLIENT_CONNECT_E,
	QAPI_NET_HTTP_CLIENT_PATCH_E
} qapi_Net_HTTPc_Method_e;

/**
 * @brief HTTP client callback state. For use with #qapi_HTTPc_CB_t.
 */
typedef enum
{
	QAPI_NET_HTTPC_RX_ERROR_SERVER_CLOSED = -8, 
    /**< Server closes the connection when the response has not been completely received yet. */

	QAPI_NET_HTTPC_RX_ERROR_RX_PROCESS = -7,
    /**< Size section of a chunk is longer than the RX buffer length. */

	QAPI_NET_HTTPC_RX_ERROR_RX_HTTP_HEADER = -6,
    /**< Header section is longer than the RX buffer length. */

	QAPI_NET_HTTPC_RX_ERROR_INVALID_RESPONSECODE = -5,
    /**< Status code is less than 100 or greater than 999. */

	QAPI_NET_HTTPC_RX_ERROR_CLIENT_TIMEOUT = -4,
    /**< Request times out. */

	QAPI_NET_HTTPC_RX_ERROR_NO_BUFFER = -3,
    /**< RESERVED. */

	QAPI_NET_HTTPC_RX_CONNECTION_CLOSED = -2,
    /**< RESERVED. */

	QAPI_NET_HTTPC_RX_ERROR_CONNECTION_CLOSED = -1,
    /**< Connection is closed due to error on socket read. */

	QAPI_NET_HTTPC_RX_FINISHED = 0,
    /**< Response is completely received. */

	QAPI_NET_HTTPC_RX_MORE_DATA = 1,
    /**< Response is partially received. */

	QAPI_NET_HTTPC_RX_TUNNEL_ESTABLISHED = 2,
    /**< Tunnel to the origin server is established. */

	QAPI_NET_HTTPC_RX_DATA_FROM_TUNNEL = 3,
    /**< Receiving data from origin server. */

	QAPI_NET_HTTPC_RX_TUNNEL_CLOSED = 4, 
    /**< Server closes the tunnel. */

} qapi_Net_HTTPc_CB_State_e;

/**
 * @brief HTTP client response. For use with #qapi_HTTPc_CB_t.
 */
typedef struct {
    uint32_t    length;
    /**< Length of the data. */

    uint32_t    resp_Code;
    /**< Response code. */

    const uint8_t *    data;
    /**< Data associated with the response if not NULL. */
} qapi_Net_HTTPc_Response_t;

/**
 * @brief User registered callback for returning response message.
 */
typedef void (*qapi_HTTPc_CB_t)(
        void* arg,
        /**< Argument passed in qapi_Net_HTTPc_New_sess(). */

        int32_t state,
        /**< State in qapi_Net_HTTPc_CB_State_e. */

        void* value
        /**< Pointer to qapi_Net_HTTPc_Response_t. */
        );

/**
 * @brief HTTP client session handle.
 */
typedef void* qapi_Net_HTTPc_handle_t;

/**
 * @brief (Re)starts the HTTP client module.
 *
 * @details Normally, this is called to start or restart the client after it was
 *          stopped via a call to qapi_Net_HTTPc_Stop().
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Start(void);

/**
 * @brief Stops the HTTP client module.
 *
 * @return On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Stop(void);

/**
 * @brief Starts a new a HTTP client session. 
 *
 * @param[in] timeout  Timeout (in ms) on an HTTP request in this session.
 *
 * @param[in] ssl_Object_Handle  SSL context for HTTPs connect; 0 for insecure session.
 *
 * @param[in] callback  Pointer to the user callback function (see qapi_HTTPc_CB_t)
 *
 * @param[in] arg    Argument for the callback function.
 *
 * @param[in] httpc_Max_Body_Length  Size in bytes of message-body buffer for HTTP request.
 *
 * @param[in] httpc_Max_Header_Length   Size in bytes of header buffer for HTTP request.
 *
 * @param[in] httpc_Rx_Buffer_Size   Size in bytes of RX buffer for HTTP response.
 *                                   If size is less than 512, system will use 512.
 * @return
 * On success, a non-NULL handle is returned; on error, NULL is returned.
 */
qapi_Net_HTTPc_handle_t qapi_Net_HTTPc_New_sess2(
        uint32_t                timeout,
        qapi_Net_SSL_Obj_Hdl_t  ssl_Object_Handle,
        qapi_HTTPc_CB_t         callback,
        void*                   arg,
        uint16_t                httpc_Max_Body_Length,
        uint16_t                httpc_Max_Header_Length,
        uint16_t                httpc_Rx_Buffer_Size);

/**
 * @brief Starts a new a HTTP client session. 
 *
 * @param[in] timeout  Timeout (in ms) on an HTTP request in this session.
 *
 * @param[in] ssl_Object_Handle  SSL context for HTTPs connect; 0 for insecure session.
 *
 * @param[in] callback  Pointer to the user callback function (see qapi_HTTPc_CB_t)
 *
 * @param[in] arg    Argument for the callback function.
 *
 * @param[in] httpc_Max_Body_Length  Size in bytes of message-body buffer for HTTP request.
 *
 * @param[in] httpc_Max_Header_Length   Size in bytes of header buffer for HTTP request.
 *
 * @note    Internally, the system allocates 1,750-byte RX buffer for caller.
 *
 * @return
 * On success, a non-NULL handle is returned; on error, NULL is returned.
 */
qapi_Net_HTTPc_handle_t qapi_Net_HTTPc_New_sess(
        uint32_t                timeout,
        qapi_Net_SSL_Obj_Hdl_t  ssl_Object_Handle,
        qapi_HTTPc_CB_t         callback,
        void*                   arg,
        uint16_t                httpc_Max_Body_Length,
        uint16_t                httpc_Max_Header_Length);

/**
 * @brief Frees an HTTP client session.
 *
 * @details Disconnects from the server and frees the memory.
 *
 * @param[in] handle    HTTP client session handle.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Free_sess(qapi_Net_HTTPc_handle_t handle);

/**
 * @brief Connects to an HTTP server in Blocking mode.
 *
 * @param[in] handle  HTTP client session handle.
 *
 * @param[in] server  Pointer to server or proxy, e.g. "192.168.2.100" or "www.example.com"
 *
 * @param[in] port  Port of the server.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Connect(qapi_Net_HTTPc_handle_t handle, const char *server, uint16_t port);

/**
 * @brief Disconnects an HTTP client session from the server.
 *
 * @param[in] handle  HTTP client session handle.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Disconnect(qapi_Net_HTTPc_handle_t handle);

/**
 * @brief Send an HTTP request to an HTTP server or proxy.
 *
 * @param[in] handle  HTTP client session handle.
 *
 * @param[in] cmd   HTTP request (see qapi_Net_HTTPc_Method_e)
 *
 * @param[in] URL   Pointer to the request URL, e.g.
 *                  "index.html" or "/cgi/mycgi.pl" if cmd is not QAPI_NET_HTTP_CLIENT_CONNECT_E,
 *                  "www.example.com:22" if cmd is QAPI_NET_HTTP_CLIENT_CONNECT_E.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Request(qapi_Net_HTTPc_handle_t handle, qapi_Net_HTTPc_Method_e cmd, const char *URL);

/**
 * @brief Send an HTTP CONNECT request to a proxy for establishing a connection to an HTTPS origin server. 
 *
 * @param[in] handle  HTTP client session handle.
 *
 * @param[in] calist  A file (on the local file system) containing
 *                    CA certificates, which are in SharkSSL format. This
 *                    is used for authenticating the origin HTTPS server.
 *                    It can be set to NULL.
 *
 * @param[in] URL   Pointer to the request URL, e.g. "www.example.com:22"
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Tunnel_To_HTTPS(qapi_Net_HTTPc_handle_t handle, const char *calist, const char *URL);

/**
 * @brief Sets the body on an HTTP client session.
 *
 * @param[in] handle  HTTP client session handle.
 *
 * @param[in] body   Pointer to the body.
 *
 * @param[in] body_Length  Length of the body.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Set_Body(qapi_Net_HTTPc_handle_t handle, const char *body, uint32_t body_Length);

/**
 * @brief Forms a URL-encoded string on an HTTP client session.
 *
 * @param[in] handle    HTTP client session handle.
 *
 * @param[in] key   Pointer to the key.
 *
 * @param[in] value    Pointer to the value.
 *
 * @code
 *        // The following calls generate a URL-encoded string which will be
 *        // the message body for POST request or
 *        // the query string for GET request.
 *        qapi_Net_HTTPc_Set_Param(handle, "name", "Lucy");
 *        qapi_Net_HTTPc_Set_Param(handle, "neighbors", "Fred & Ethel");
 *
 *        // For example, if user calls
 *        // qapi_Net_HTTPc_Request(handle, QAPI_NET_HTTP_CLIENT_GET_E, "index.html");
 *        // the start line, "GET /index.html?name=Lucy&neighbors=Fred+%26+Ethel HTTP/1.1\r\n",
 *        // is generated.
 * @endcode \n
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Set_Param(qapi_Net_HTTPc_handle_t handle, const char *key, const char *value);

/**
 * @brief Sets the header field for an HTTP client session.
 *
 * @param[in] handle   HTTP client session handle.
 *
 * @param[in] type     Pointer to the type.
 *
 * @param[in] value    Pointer to the value.
 *
 * @note    If this API is not called, the system will send the following headers for user:
 *          - Host: <hostname>:<port>\r\n
 *          - Accept: text/html, <asterisk>/<asterisk>\r\n
 *          - User-Agent: IOE Client\r\n
 *          - Connection: keep-alive\r\n
 *          - Cache-control: no-cache\r\n
 *          Additionally, for POST, PUT and PATCH requests if message body has data:
 *          - Content-length: <nn>\r\n
 *          - Content-Type: application/x-www-form-urlencoded\r\n (for POST request)
 *
 *          If this API is called, the system will send the following headers for user:
 *          - Host: <hostname>:<port>\r\n
 *          - Connection: keep-alive\r\n
 *          - User's own headers added by calling this API 
 *          Additionally, for POST, PUT and PATCH requests if message body has data:
 *          - Content-length: <nn>\r\n 
 *
 * @code
 *        // The following calls will generate request headers in an HTTP GET request:
 *        // "User-Agent: My Own Browser 1.0\r\n"
 *        // "Connection: keep-alive\r\n"
 *        qapi_Net_HTTPc_Add_Header_Field(handle, "User-Agent", "My Own Browser 1.0");
 *        qapi_Net_HTTPc_Add_Header_Field(handle, "Connection", "keep-alive");
 *        qapi_Net_HTTPc_Request(handle, QAPI_NET_HTTP_CLIENT_GET_E, "index.html");
 * @endcode \n
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Add_Header_Field(qapi_Net_HTTPc_handle_t handle, const char *type, const char *value);

/**
 * @brief Clears the header field for an HTTP client session.
 *
 * @param[in] handle    HTTP client session handle.
 *
 * @return
 * On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_Clear_Header(qapi_Net_HTTPc_handle_t handle);

/**
 * @brief Sets SSL configuration parameters on a secure (i.e., HTTPS) session.
 *
 * @param[in] handle    HTTP client session handle.
 *
 * @param[in] ssl_Cfg   SSL connection parameters.
 *
 * @return On success, 0 is returned. On error, nonzero error code is returned.
 */
qapi_Status_t qapi_Net_HTTPc_Configure_SSL(qapi_Net_HTTPc_handle_t handle, qapi_Net_SSL_Config_t *ssl_Cfg);

/**
 * @brief Enables/disables the addition of an HTTP head in a session callback.
 *
 * @details By default, the system returns the message body of response via user registered callback (see qapi_HTTPc_CB_t).
 *          Message headers are not returned. To enable the system to also return messages headers, this API should be called
 *          with 'enable'= 1.
 *
 * @param[in] handle   HTTP client session handle.
 *
 * @param[in] enable   1 -- enable; 0 -- disable.
 *
 * @return On success, 0 is returned; other on error code.
 */
qapi_Status_t qapi_Net_HTTPc_CB_Enable_Adding_Header(qapi_Net_HTTPc_handle_t handle, uint16_t enable);

/**
 * @brief Send raw data when an HTTP tunnel is established.
 *
 * @param[in] handle  HTTP client session handle.
 *
 * @param[in] buf   Pointer to data
 *
 * @param[in] length  Length of data
 *
 * @return
 * When all data is sent, 0 is returned; on error, non-zero is returned. 
 */
qapi_Status_t qapi_Net_HTTPc_Send_Data(qapi_Net_HTTPc_handle_t handle, const char *buf, uint32_t length);

 /** @} */ /* end_addtogroup qapi_networking_httpc */

#endif /* _QAPI_HTTPC_H_ */
