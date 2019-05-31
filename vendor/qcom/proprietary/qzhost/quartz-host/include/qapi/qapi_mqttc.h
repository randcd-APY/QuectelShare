/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_mqttc.h
 *
 * @addtogroup qapi_networking_mqttc
 * @{
 *
 * @details The MQTT (Message Queuing Telemetry Transport) Client service provides
 * a collection of APIs that allow the application to implement client functionalities,
 * such as CONNECT, SUBSCRIBE, UNSUBSCRIBE, PUBLISH, DISCONNECT, etc.
 *
 * @}
 */

#ifndef _QAPI_MQTTC_H_
#define _QAPI_MQTTC_H_

#include "qapi/qapi_types.h"
#include "qapi/qapi_net_status.h"
#include "qapi_ssl.h"

/** @addtogroup qapi_networking_mqttc
@{ */

/* Please see qapi_net_status.h for QAPI_NET_STATUS_MQTTC_xxx status codes */

/**
 * @brief Callback to return the client status.
 */
typedef void (* qapi_Net_MQTTc_Connect_CB_t)(
    int32_t handle,
    /**< Client handle returned from qapi_Net_MQTTc_New(). */

    void * arg,
    /**< User-specified argument. */

    qapi_Status_t status
    /**< status code: QAPI_NET_STATUS_MQTTC_xxx. */
    );

/* Reason codes for SUBSCRIBE callback */
#define QAPI_NET_MQTTC_SUBSCRIPTION_DENIED    0   /**< Subscription is denied by the broker. */
#define QAPI_NET_MQTTC_SUBSCRIPTION_GRANTED   1   /**< subscription is granted by the broker. */
#define QAPI_NET_MQTTC_SUBSCRIPTION_MSG       2   /**< Application message is received from the broker*/

/**
 * @brief Subscribe callback.
 */
typedef void (* qapi_Net_MQTTc_Subscribe_CB_t)(
    int32_t handle,
    /**< Client handle returned from qapi_Net_MQTTc_New(). */

    void * arg,
    /**< User-specified argument. */

    int32_t reason,
    /**< Reason code (QAPI_NET_MQTTC_SUBSCRIPTION_xxx). */

    const char * topic,
    /**< Topic filter. */

    uint16_t topic_Length,
    /**< Size (in bytes) of the topic filter. */

	const char * msg,
    /**< Application message published by another client. */

    uint32_t msg_Length,
    /**< Size (in bytes) of the application message. */

    uint32_t QOS
    /**< Granted QOS for this subscription when 'reason' is QAPI_NET_MQTTC_SUBSCRIPTION_GRANTED or
         QOS for 'msg' delivered by the broker when 'reason' is QAPI_NET_MQTTC_SUBSCRIPTION_MSG. */
    );

/**
 * @brief Initializes and starts MQTT client services.
 *
 * @details Must be called before calling any other MQTTc APIs.
 *
 * @param[in] calist    Define a file (on the local file system) containing
 *                      CA certificates, which are in SharkSSL format. This
 *                      is used for SSL communication, so it can be set to NULL
 *                      if no SSL communication is required. 
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t qapi_Net_MQTTc_Init(const char *calist);

/**
 * @brief Shuts down MQTT client services and frees resources.
 *
 * @return 0 is returned.
 */
int32_t qapi_Net_MQTTc_Shutdown(void);

/**
 * @brief Create an MQTT client instance.
 *
 * @param[in] client_ID     A NULL-terminated string that is used as the client ID
 *                          in the MQTT CONNECT packet.
 *                          If NULL, a random 8-byte client ID is generated.
 *
 * @param[in] clean_Session Set to TRUE to instruct the broker to clean all messages and
 *                          subscriptions on receving DISCONNECT, and FALSE to instruct it to
 *                          keep them. When client_id is NULL, the value is ignored and
 *                          the Clean Session flag in the CONNECT packet is set to 1.
 * @param[in,out] error     If not NULL, *error will contain QAPI_NET_STATUS_MQTTC_xxx
 *
 * @return On success, a non-zero handle is returned. On error, -1 is returned.
 */
int32_t qapi_Net_MQTTc_New(const char *client_ID, qbool_t clean_Session, qapi_Status_t *error);

/**
 * @brief Destroys an MQTT client instance.
 *
 * @details     If the client is connected to the broker, a DISCONNECT packet is sent. 
 *              All resources associated with the client are freed, including the handle.
 *
 * @param[in] handle    Client handle returned from qapi_Net_MQTTc_New().
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t qapi_Net_MQTTc_Destroy(int32_t handle);

/**
 * @brief Specifies a callback function for the client status.
 *
 * @details This must be called before calling qapi_Net_MQTTc_Connect().
 *
 * @param[in] handle    Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] callback  Since this function is called under the Timer thread,
 *                      it is expected to return promptly and cannot be blocked. 
 * @param[in] arg       Caller-specified pointer that will be passed as an argument to callback.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t qapi_Net_MQTTc_Register_Connect_Callback(int32_t handle, qapi_Net_MQTTc_Connect_CB_t callback, void *arg);

/**
 * @brief Specifies a subscribe callback function.
 *
 * @details This must be called before calling qapi_Net_MQTTc_Connect().
 *          The callback is invoked at two cases:
 *          - When SUBACK is received from the broker
 *          - When PUBLISH is received from the broker
 *
 * @param[in] handle    Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] callback  Since this function is called under the Timer thread, it is expected to return
 *                      promptly and cannot be blocked.
 * @param[in] arg      Caller-specified pointer that will be passed as an argument to callback.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t qapi_Net_MQTTc_Register_Subscribe_Callback(int32_t handle, qapi_Net_MQTTc_Subscribe_CB_t callback, void *arg);

/**
 * @brief Sets the Keep Alive value.
 *
 * @param[in] handle        Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] keepalive_Sec Keep Alive value in seconds. Setting this to 0 turns off
 *                          the keep alive mechanism.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t qapi_Net_MQTTc_Set_Keep_Alive(int32_t handle, uint16_t keepalive_Sec);

/**
 * @brief Sets the time interval for waiting for CONNACK. 
 *
 * @param[in] handle                Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] wait_For_Connack_Sec  After sending a connect packet to the broker, if the
 *                                  client does not receive a CONNACK packet from the broker
 *                                  within this time (in seconds), the client will close
 *                                  the network connection.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t qapi_Net_MQTTc_Set_Connack_Wait_Time(int32_t handle, uint16_t wait_For_Connack_Sec);

/**
 * @brief Configures will information for a client instance.
 *
 * @details By default, clients do not have a will. This must be called before calling qapi_Net_MQTTc_Connect().
 *
 * @param[in] handle        Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] topic         Topic on which to publish the will.
 * @param[in] topic_Len     Cize (in bytes) of topic.
 * @param[in] message       Application message to send. 
 * @param[in] message_Len   Size (in bytes) of message.
 * @param[in] QOS           Integer value 0, 1, or 2, indicating the Quality of Service to be used for the will.
 * @param[in] retain        Set to TRUE to make the will a retained message.
 * @param[in,out] error     If not NULL, *error will contain QAPI_NET_STATUS_MQTTC_xxx.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t
qapi_Net_MQTTc_Set_Will(int32_t      handle,
                        const char * topic,
                        uint16_t     topic_Len,
                        const char * message,
                        uint16_t     message_Len,
                        uint32_t     QOS,
                        qbool_t      retain,
                        qapi_Status_t * error);

/**
 * @brief Remove a previously configured will.
 *
 * @details This must be called before calling qapi_Net_MQTTc_Connect().
 *
 * @param[in] handle    Client handle returned from qapi_Net_MQTTc_New().
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t qapi_Net_MQTTc_Clear_Will(int32_t handle);

/**
 * @brief Configures the username and password for a client instance.
 *
 * @details By default, no username or password will be sent.
 *          If username is NULL, the password argument is ignored.
 *          This must be called before calling qapi_Net_MQTTc_Connect().
 *
 * @param[in] handle        Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] username      Username to send as a string, or NULL to disable authentication.
 * @param[in] username_Len  Size (in bytes) of username.
 * @param[in] password      Password to send. Set to NULL when username is valid in order to
 *                          send just a username.
 * @param[in] password_Len  Size (in bytes) of password.
 * @param[in,out] error     If not NULL, *error will contain QAPI_NET_STATUS_MQTTC_xxx.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t
qapi_Net_MQTTc_Set_Username_Password(
                int32_t         handle,
                const char *    username,
                uint16_t        username_Len,
                const char *    password,
                uint16_t        password_Len,
                qapi_Status_t * error);

/**
 * @brief Configures an SSL connection.
 *
 * @details This must be called before calling qapi_Net_MQTTc_Connect()
 *          if the configuration is required.
 *
 * @param[in] handle        Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] cfg           Pointer to a block of SSL configuration parameters.
 * @param[in,out] error     If not NULL, *error will contain QAPI_NET_STATUS_MQTTC_xxx.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t
qapi_Net_MQTTc_Set_SSL_Config(int32_t handle, qapi_Net_SSL_Config_t *cfg, int32_t *error);

/**
 * @brief Connects to an MQTT broker.
 *
 * @param[in] handle        Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] host          Hostname or IP address of the broker to which to connect.
 * @param[in] secure_Session  Set to TRUE if running on SSL is intended, and the client
 *                            will connect to the broker's TCP port 8883. \n
 *                            Set to FALSE if no encryption/decryption on the message
 *                            is intended, and the client will connect to the broker's
 *                            TCP port 1883.
 * @param[in] nonblocking_Connect   Set to TRUE to cause the function to return immediately.
 *                                  When CONNACK is received, the user-specified callback
 *                                  (qapi_Net_MQTTc_Connect_CB_t) will be invoked to indicate
 *                                  the completion status. \n
 *                                  Set to FALSE to cause the function to wait for CONNACK. If
 *                                  the function returns 0, the client is connected to the broker.
 *                                  If the function returns -1, the error code is reported in *error.
 * @param[in] bind_Ifname   Interface name of the local network interface to which to bind. If not NULL,
 *                          the communication to the broker is restricted over this interface.
 * @param[in,out] error     If not NULL, *error will contain QAPI_NET_STATUS_MQTTC_xxx.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t
qapi_Net_MQTTc_Connect(
                int32_t     handle,
                const char *host,
                qbool_t     secure_Session,
                qbool_t     nonblocking_Connect,
                const char *bind_Ifname,
                qapi_Status_t * error);

/**
 * @brief Disconnects from an MQTT broker.
 *
 * @details     A disconnect packet will be sent to the broker. 
 *
 * @param[in] handle        Client handle returned from qapi_Net_MQTTc_New().
 * @param[in,out] error     If not NULL, *error will contain QAPI_NET_STATUS_MQTTC_xxx.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t qapi_Net_MQTTc_Disconnect(int32_t handle, qapi_Status_t *error);

/**
 * @brief Subscribes to multiple topics.
 *
 * @param[in] handle        Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] topics        Array of subscription patterns.
 * @param[in] topics_Len    Array of subscription pattern sizes.
 * @param[in] topics_QOS    Array of requested QOS for subscriptions.
 * @param[in] num_Topics    Number of subscription patterns. Must be >= 1 and <= 32.
 * @param[in,out] error     If not NULL, *error will contain QAPI_NET_STATUS_MQTTC_xxx.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t
qapi_Net_MQTTc_Subscribe_Multiple(
                            int32_t        handle,
                            const char *   topics[],
                            uint32_t       topics_Len[],
                            uint32_t       topics_QOS[],
                            uint32_t       num_Topics,
                            qapi_Status_t * error);

/**
 * @brief Subscribes to one topic.
 *
 * @param[in] handle        Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] topic         NULL-terminated subscription pattern.
 * @param[in] topic_QOS     Requested QOS for this subscription.
 * @param[in,out] error     If not NULL, *error will contain QAPI_NET_STATUS_MQTTC_xxx.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t
qapi_Net_MQTTc_Subscribe(int32_t        handle,
                         const char *   topic,
                         uint32_t       topic_QOS,
                         qapi_Status_t * error);

/**
 * @brief Unsubscribes from multiple topics.
 *
 * @param[in] handle        Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] topics        Array of topics.
 * @param[in] topics_Len    Array of topic sizes.
 * @param[in] num_Topics    Number of topics.
 * @param[in,out] error     If not NULL, *error will contain QAPI_NET_STATUS_MQTTC_xxx.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t
qapi_Net_MQTTc_Unsubscribe_Multiple(
                            int32_t      handle,
                            const char * topics[],
                            uint32_t     topics_Len[],
                            uint32_t     num_Topics,
                            qapi_Status_t * error);

/**
 * @brief Unsubscribes from a topic.
 *
 * @param[in] handle        Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] topic         Unsubscription pattern.
 * @param[in,out] error     If not NULL, *error will contain QAPI_NET_STATUS_MQTTC_xxx.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t
qapi_Net_MQTTc_Unsubscribe(
                            int32_t      handle,
                            const char * topic,
                            qapi_Status_t * error);

/**
 * @brief Publishes a message on a given topic.
 *
 * @param[in] handle        Client handle returned from qapi_Net_MQTTc_New().
 * @param[in] topic         Topic to which msg is published.
 * @param[in] topic_Len     Size of topic.
 * @param[in] msg           Pointer to the application message to send.
 * @param[in] msg_Len       Size of msg in bytes. 
 *                          Valid values are between 0 and 268,435,455.
 * @param[in] QOS           Integer value 0, 1, or 2 indicating the QOS to be used for msg.
 * @param[in] retained      Set to TRUE to make the message retained.
 * @param[in] dup           Set to TRUE to redeliver a publish packet.
 * @param[in,out] error     If not NULL, *error will contain QAPI_NET_STATUS_MQTTC_xxx.
 *
 * @return On success, 0 is returned. On error, -1 is returned.
 */
int32_t
qapi_Net_MQTTc_Publish(int32_t      handle,
                       const char * topic,
                       uint32_t     topic_Len,
                       const char * msg,
                       uint32_t     msg_Len,
                       uint32_t     QOS,
                       qbool_t      retained,
                       qbool_t      dup,
                       qapi_Status_t * error);

/** @} */

#endif /* _QAPI_MQTTC_H_ */
