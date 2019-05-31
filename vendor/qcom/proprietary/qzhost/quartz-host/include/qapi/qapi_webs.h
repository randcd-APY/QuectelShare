/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_webs.h
 *
 * @addtogroup qapi_networking_webs
 * @{
 *
 * @details This file describes QAPIs for getting values from, and putting values into, WEB forms.
 *
 * @}
 */

#ifndef _QAPI_WEBS_H_
#define _QAPI_WEBS_H_

#include <stdint.h>
#include "qapi/qapi_status.h"   /* qapi_Status_t */

/** @addtogroup qapi_networking_webs
@{ */

/**
 * @brief Web form.
 */
typedef struct qapi_Net_Web_Form_s
{
    char        *request_Line;
    /**< Method token plus Request-URI in the HTTP request message, e.g., POST/index.html. */

    uint32_t    count;
    /**< Number of entries in the following name/value table. */

    struct
    {
        char *name;
        char *value;
    } name_Value[1];
    /**< First name/value entry. */

} qapi_Net_Web_Form_t;

/**
 * @brief Copies the HTTP message body to a buffer.
 *
 * @param[in] hp    WEB server handle.
 *
 * @param[in] buf   Buffer provided by the caller to contain the HTTP message body.
 *                  If NULL, the message body length is returned in *plen.
 *                  If not NULL, at most *plen bytes of message body is copied to buf and
 *                  the actual number of bytes copied is returned in *plen. 
 *
 * @param[in,out] plen  See the description of buf. 
 *                      Cannot be NULL.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
qapi_Status_t qapi_Net_Webs_Get_Message_Body(void *hp, char *buf, uint32_t *plen);

/**
 * @brief Searches a form for the name/value pair matching the name passed
 *        and copies a NULL-terminated string into the provided buffer.
 *
 * @details If the string is too long for the buffer, the string will
 *          be truncated with a NULL-terminator at the end. If no match
 *          is found, the contents of the buffer are not changed.
 *
 * @param[in] hp    WEB server handle.
 *
 * @param[in] name  Search key for a name/value pair in an HTML form.
 *
 * @param[in] buf   Buffer provided by the caller to contain the string value of the name/value pair.
 *
 * @param[in] buf_Len    Size in bytes of buf[].
 *
 * @return
 * On success, the number of bytes copied to buffer is returned; on error, -1 is returned.
 */
qapi_Status_t qapi_Net_Webs_Get_Form_String(void *hp, const char *name, char *buf, int32_t buf_Len);

/**
 * @brief Searches a form for the name/value pair matching the name passed
 *        and copies an integer value into the provided buffer.
 *
 * @param[in] hp    WEB server handle.
 *
 * @param[in] name  Search key for a name/value pair in an HTML form.
 *
 * @param[in] value Buffer provided by the caller to contain the integer value of the name/value pair.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
qapi_Status_t qapi_Net_Webs_Get_Form_Int(void *hp, const char *name, uint32_t *value);

/**
 * @brief Searches a form for the name/value pair matching the name passed
 *        and gets the TRUE/FALSE state of an INPUT type=CHECKBOX.
 *
 * @param[in] hp    WEB server handle.
 *
 * @param[in] name  Search key for a name/value pair in an HTML form.
 *
 * @param[in] value Buffer provided by the caller to contain the TRUE or FALSE value
 *                  as to whether the name is found.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
qapi_Status_t qapi_Net_Webs_Get_Form_Bool(void *hp, const char *name, uint32_t *value);

/**
 * @brief Extracts an IPv4 address from a form based on a name passed.
 *
 * @param[in] hp    WEB server handle.
 *
 * @param[in] name  Search key for a name/value pair in an HTML form.
 *
 * @param[in] ipv4_Addr Buffer provided by the caller to contain the IPv4 address in
 *                  network order.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
qapi_Status_t qapi_Net_Webs_Get_Form_Ip4addr(void *hp, const char *name, uint32_t *ipv4_Addr);

/**
 * @brief Extracts an IPv6 address from a form based on a name passed.
 *
 * @param[in] hp    WEB server handle.
 *
 * @param[in] name  Search key for a name/value pair in an HTML form.
 *
 * @param[in] ipv6_Addr Buffer provided by the caller to contain the 16-byte IPv6 address.
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
qapi_Status_t qapi_Net_Webs_Get_Form_Ip6addr(void *hp, const char *name, uint8_t *ipv6_Addr);

/**
 * @brief Sends raw data.
 *
 * @param[in] hp    WEB server handle.
 *
 * @param[in] data  Data buffer.
 *
 * @param[in] length  Number of data bytes to be sent.
 *
 * @return
 * The number of data bytes sent, or a negative error code.
 */
qapi_Status_t qapi_Net_Webs_Send_Data(void *hp, const char *data, uint32_t length);

/**
 * @brief Sends a NULL-terminated string.
 *
 * @param[in] hp    WEB server handle.
 *
 * @param[in] string    NULL-terminated string to be sent.
 *
 * @return
 * The number of data bytes sent, or a negative error code.
 */
qapi_Status_t qapi_Net_Webs_Send_String(void *hp, char *string);

/**
 * @brief Sends a 4-byte integer.
 *
 * @param[in] hp    WEB server handle.
 *
 * @param[in] value 4-byte integer to be sent.
 *
 * @return
 * The number of data bytes sent, or a negative error code.
 */
qapi_Status_t qapi_Net_Webs_Send_Long(void *hp, uint32_t value);

/**
 * @brief Sends a 2-byte integer.
 *
 * @param[in] hp    WEB server handle.
 *
 * @param[in] value 2-byte integer to be sent.
 *
 * @return
 * The number of data bytes sent, or a negative error code.
 */
qapi_Status_t qapi_Net_Webs_Send_Short(void *hp, uint16_t value);

/**
 * @brief Sends an IPv4 address.
 *
 * @param[in] hp    WEB server handle.
 *
 * @param[in] ipv4_Addr    IPv4 address in network order.
 *
 * @return
 * The number of data bytes sent, or a negative error code.
 */
qapi_Status_t qapi_Net_Webs_Send_Ip4addr(void *hp, uint32_t ipv4_Addr);

/**
 * @brief Sends an IPv6 address.
 *
 * @param[in] hp     WEB server handle
 *
 * @param[in] ipv6_Addr     IPv6 address to be sent.
 *
 * @return
 * The number of data bytes sent, or a negative error code.
 */
qapi_Status_t qapi_Net_Webs_Send_Ip6addr(void *hp, uint8_t *ipv6_Addr );

/**
 * @brief Builds then sends the HTTP status line and headers.
 *
* @note1hang  The built message contains:
 *          - status-line: HTTP/1.1 \<status_code\> \<status_text\>
 *          - Server header, e.g., Server: Qualcomm Technologies WebServer 1.0
 *          - Date header if the system time can be obtained, e.g., Sun, 18 Oct 2017 10:36:20 GMT
 *          - Content-type header if the content_type is not NULL, e.g., Content-type: application/demo
 *          - Connection header, e.g., Connection: Keep-Alive
 *          - Content-length header, e.g., Content-length: \<content_length\>
 *          - User_headers
 *
 * @param[in] hp                WEB server handle.
 * @param[in] content_type      Content type of the message body, e.g., text/html.
 *                              If NULL, the Content-Type header will not be sent.
 * @param[in] content_length    Size (in bytes) of the message body.
 * @param[in] status_code       HTTP status code, e.g., 200 for OK, 401 for Bad Request.
 * @param[in] status_text       Optional text associated with the status_code, e.g., OK or Bad Request.
 * @param[in] user_headers      Optional headers built by the caller. 
 *
 * @return
 * On success, 0 is returned; on error, -1 is returned.
 */
qapi_Status_t qapi_Net_Webs_Send_HTTP_headers(void *hp,
                                              const char *content_type,
                                              int content_length,
                                              int status_code,
                                              const char *status_text,
                                              const char *user_headers);
/** @} */

#endif /* _QAPI_WEBS_H_ */
