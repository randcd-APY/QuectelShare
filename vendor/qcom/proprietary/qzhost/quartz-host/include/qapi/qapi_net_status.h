/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/** @file qapi_net_status.h
 *
 * @addtogroup qapi_networking_status
 * @{
 *
 * @details This section contains QAPI error codes that may be returned as a
 * result of an error in one of the networking services.
 *
 * @}
 */

#ifndef _QAPI_NET_STATUS_H_
#define _QAPI_NET_STATUS_H_

#include "qapi/qapi_status.h"

/** @addtogroup qapi_networking_status
@{ */


/*
 * SSL QAPI return values. See qapi_status.h for common error codes. The
 * ones below are SSL module specific.
 */
/** Error in own certificate. */
#define QAPI_ERR_SSL_CERT __QAPI_ERROR(QAPI_MOD_NETWORKING, 1)

/** Error with SSL connection. */
#define QAPI_ERR_SSL_CONN __QAPI_ERROR(QAPI_MOD_NETWORKING, 2)

/** Handshake must be completed before the operation can be attempted. */
#define QAPI_ERR_SSL_HS_NOT_DONE __QAPI_ERROR(QAPI_MOD_NETWORKING, 3)

/** Received SSL warning alert message. */
#define QAPI_ERR_SSL_ALERT_RECV __QAPI_ERROR(QAPI_MOD_NETWORKING, 4)

/** Received SSL fatal alert message. */
#define QAPI_ERR_SSL_ALERT_FATAL __QAPI_ERROR(QAPI_MOD_NETWORKING, 5)

/** Handshake in progress. */
#define QAPI_SSL_HS_IN_PROGRESS __QAPI_ERROR(QAPI_MOD_NETWORKING, 6)

/** Handshake successful. */
#define QAPI_SSL_OK_HS __QAPI_ERROR(QAPI_MOD_NETWORKING, 7)

/** The peer's SSL certificate is trusted, CN matches the host name, and the time has expired. */
#define QAPI_ERR_SSL_CERT_CN __QAPI_ERROR(QAPI_MOD_NETWORKING, 8)

/** The peer's SSL certificate is trusted, CN does not match the host name, and the time is valid. */
#define QAPI_ERR_SSL_CERT_TIME __QAPI_ERROR(QAPI_MOD_NETWORKING, 9)

/** The peer's SSL certificate is not trusted */
#define QAPI_ERR_SSL_CERT_NONE __QAPI_ERROR(QAPI_MOD_NETWORKING, 10)

/** Connection drop when out of network buffers. This is usually a
 * resource configuration error. */
#define QAPI_ERR_SSL_NETBUF __QAPI_ERROR(QAPI_MOD_NETWORKING, 11)

/** Socket error. Use qapi_errno to check for the reason code. */
#define QAPI_ERR_SSL_SOCK __QAPI_ERROR(QAPI_MOD_NETWORKING, 12)

/** No DTLS server connection matching a peer address was found. */
#define QAPI_ERR_SSL_CONN_NOT_FOUND __QAPI_ERROR(QAPI_MOD_NETWORKING, 13)

/*
 * Generic error codes
 */
/** IP address is invalid. */
#define QAPI_NET_ERR_INVALID_IPADDR         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  21)))

/** Failed to get the scope ID. */
#define QAPI_NET_ERR_CANNOT_GET_SCOPEID     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  22)))

/** Socket command timed out. */
#define QAPI_NET_ERR_SOCKET_CMD_TIME_OUT    ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  23)))

/** Socket failure. */
#define QAPI_NET_ERR_SOCKET_FAILURE         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  24)))

/** Operation failed. */
#define QAPI_NET_ERR_OPERATION_FAILED       ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  25)))

/*
 * MQTTC status codes
 */
/** Operation was successful. */
#define QAPI_NET_STATUS_MQTTC_OK                            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  30)))

/** Connection was successful. */
#define QAPI_NET_STATUS_MQTTC_CONNECT_SUCCEEDED             (QAPI_NET_STATUS_MQTTC_OK)

/** Unacceptable protocol version. */
#define QAPI_NET_STATUS_MQTTC_INVALID_PROTOCOL_VERSION      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  31)))

/** Client ID is rejected by the broker. */
#define QAPI_NET_STATUS_MQTTC_CLIENT_ID_REJECTED            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  32)))

/** MQTT service is not available. */
#define QAPI_NET_STATUS_MQTTC_MQTT_SERVICE_UNAVAILABLE      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  33)))

/** Malformed user name or password. */
#define QAPI_NET_STATUS_MQTTC_INVALID_USERNAME_PASSWORD     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  34)))

/** Client is not authorized. */
#define QAPI_NET_STATUS_MQTTC_NOT_AUTHORIZED                ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  35)))

/** Socket write error. */
#define QAPI_NET_STATUS_MQTTC_SOCKET_WRITE_ERROR            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  36)))

/** Socket read error. */
#define QAPI_NET_STATUS_MQTTC_SOCKET_READ_ERROR             ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  37)))

/** Malformed packet. */
#define QAPI_NET_STATUS_MQTTC_MALFORMED_PACKET              ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  38)))

/** CONNACK was not received from the broker. */
#define QAPI_NET_STATUS_MQTTC_CONNACK_NOT_RECEIVED          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  39)))

/** TCP connection failed. */
#define QAPI_NET_STATUS_MQTTC_TCP_CONNECT_FAILED            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  40)))

/** SSL handshake failed. */
#define QAPI_NET_STATUS_MQTTC_SSL_HANDSHAKE_FAILED          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  41)))

/** MQTT client was not started. */
#define QAPI_NET_STATUS_MQTTC_CLIENT_NOT_STARTED            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  42)))

/** Invalid handle. */
#define QAPI_NET_STATUS_MQTTC_INVALID_HANDLE                ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  43)))

/** Unknown host. */
#define QAPI_NET_STATUS_MQTTC_UNKNOWN_HOST                  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  44)))

/** Socket creation failed. */
#define QAPI_NET_STATUS_MQTTC_SOCKET_CREATION_FAILED        ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  45)))

/** Socket fatal error. */
#define QAPI_NET_STATUS_MQTTC_SOCKET_FATAL_ERROR            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  46)))

/** TCP bind failed. */
#define QAPI_NET_STATUS_MQTTC_TCP_BIND_FAILED               ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  47)))

/** SSL creation failed. */
#define QAPI_NET_STATUS_MQTTC_SSL_CREATION_FAILED           ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  48)))

/** Memory allocation error. */
#define QAPI_NET_STATUS_MQTTC_MEMORY_ALLOCATION_FAILED      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  49)))

/** Invalid UTF8 string. */
#define QAPI_NET_STATUS_MQTTC_INVALID_UTF8_STRING           ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  50)))

/** Bad parameters. */
#define QAPI_NET_STATUS_MQTTC_BAD_PARAM                     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  51)))

/** Bad state. Must not issue commands at the current state. */
#define QAPI_NET_STATUS_MQTTC_BAD_STATE                     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  52)))

/** Message serialization failed. */
#define QAPI_NET_STATUS_MQTTC_MSG_SERIALIZATION_FAILURE     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  53)))

/** One or more subscriptions failed. */
#define QAPI_NET_STATUS_MQTTC_PARTIAL_SUBSCRIPTION_FAILURE  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  54)))

/** Unknown topic. */
#define QAPI_NET_STATUS_MQTTC_UNKNOWN_TOPIC                 ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  55)))

/** TCP connection closed. */
#define QAPI_NET_STATUS_MQTTC_TCP_CONNECTION_CLOSED         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  56)))

/** MQTT client existed already. */
#define QAPI_NET_STATUS_MQTTC_CLIENT_EXISTED                ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  57)))

/** MQTT DISCONNECT failed. */
#define QAPI_NET_STATUS_MQTTC_DISCONNECT_FAILED             ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  58)))

/** MQTT client end. */
#define QAPI_NET_STATUS_MQTTC_END                           ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  69)))

/*
 * MDNSD status codes
 */
/** Registration in progrss. */
#define QAPI_NET_STATUS_MDNSD_SVC_REGISTRATION_IN_PROGRESS  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  70)))

/** Registration failure. */
#define QAPI_NET_STATUS_MDNSD_SVC_REGISTRATION_FAILURE      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  71)))

/** Hostname conflict. */
#define QAPI_NET_STATUS_MDNSD_HOSTNAME_CONFLICT             ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  72)))

/** Hostname and service conflict. */
#define QAPI_NET_STATUS_MDNSD_HOSTNAME_AND_SVC_CONFLICT     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  73)))

/** Service conflict. */
#define QAPI_NET_STATUS_MDNSD_SVC_CONFLICT                  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  76)))

#define QAPI_NET_STATUS_MDNSD_STATE_INVALID                 ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  77)))

/*
 * User Account Management status codes
 */
/** Internal error. */
#define QAPI_NET_STATUS_USER_INTERNAL_ERROR                 ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  80)))

/** Invalid username or password. */
#define QAPI_NET_STATUS_USER_INVALID_PARAMETER              ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  81)))

/** User table is full. */
#define QAPI_NET_STATUS_USER_TABLE_FULL                     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  82)))

/** User is not found in User Account table. */
#define QAPI_NET_STATUS_USER_NOT_FOUND                      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  83)))

/** User already exists in User Account table. */
#define QAPI_NET_STATUS_USER_ALREADY_EXIST                  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  84)))

/** Fail to open User Account table. */
#define QAPI_NET_STATUS_USER_OPEN_ERROR                     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  85)))

/** Fail to write User Account table. */
#define QAPI_NET_STATUS_USER_WRITE_ERROR                    ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  86)))

/** Fail to read User Account table. */
#define QAPI_NET_STATUS_USER_READ_ERROR                     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  87)))

/** Incorrect password (permission denied). */
#define QAPI_NET_STATUS_USER_INCORRECT_PASSWORD             ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  88)))

/** User account management end. */
#define QAPI_NET_STATUS_USER_END                            ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  89)))

/*
 * Websocket status codes
 */

/* Handshake message from peer is not valid */
#define QAPI_NET_WEBSOCKET_STATUS_HANDSHAKE_INVALID_RESPONSE  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  90)))

/*  Need more data to process message or control frame. */
#define QAPI_NET_WEBSOCKET_STATUS_RX_NEED_MORE_DATA           ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  91)))

/*  Attempted to use websocket while already in use. */
#define QAPI_NET_WEBSOCKET_STATUS_ALREADY_IN_USE              ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  92)))

/*  Error during HTTP handshake. */
#define QAPI_NET_WEBSOCKET_STATUS_MALFORMED_HTTP_RESPONSE     ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  93)))

/** Socket error. */
#define QAPI_NET_WEBSOCKET_STATUS_SOCKET_ERROR                ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  94)))

/** SSL error. */
#define QAPI_NET_WEBSOCKET_STATUS_SSL_ERROR                   ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  95)))

/** Fatal TX error.  Peer sent frame larger than the maximum that can be handled. */
#define QAPI_NET_WEBSOCKET_STATUS_MESSAGE_TOO_BIG             ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  96)))

/** Fatal TX error.  Malformed websocket frame received. */
#define QAPI_NET_WEBSOCKET_STATUS_MALFORMED_FRAME             ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  97)))

/** Non-fatal TX error.  Not enough buffers to service send request. */
#define QAPI_NET_WEBSOCKET_STATUS_NO_BUFFERS                  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  98)))

/** Fatal TX error.  Will result in socket being closed */
#define QAPI_NET_WEBSOCKET_STATUS_CANNOT_COMPLETE_TX          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  99)))

/** Cannot service send request because previous send has not completed */
#define QAPI_NET_WEBSOCKET_STATUS_NEED_TO_FINISH_PENDING_SEND ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  100)))

/** Cannot obtain lock on websocket connection context */
#define QAPI_NET_WEBSOCKET_STATUS_CANNOT_OBTAIN_LOCK          ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  101)))

/** Peer selected invalid sub-protocol */
#define QAPI_NET_WEBSOCKET_STATUS_HANDSHAKE_INVALID_SUBPROTOCOL ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  102)))

/** Error with socket connect */
#define QAPI_NET_WEBSOCKET_STATUS_SOCKET_CONNECT_FAILURE      ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  103)))

/** SSL handshake failure */
#define QAPI_NET_WEBSOCKET_STATUS_SSL_HANDSHAKE_FAILURE       ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  104)))

/** Attempting to send or receive data on websocket that is not open */
#define QAPI_NET_WEBSOCKET_STATUS_ERROR_NOT_OPEN              ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  105)))

#define QAPI_NET_WEBSOCKET_STATUS_END                         ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_NETWORKING,  109)))

#endif /* _QAPI_NET_STATUS_H_ */
