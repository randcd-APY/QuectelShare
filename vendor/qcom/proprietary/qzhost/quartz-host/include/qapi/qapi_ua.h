/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$


/**
 * @file qapi_ua.h
 *
 * @addtogroup qapi_networking_user_account
 * @{
 *
 * @details The User Account service manages all access to networking services (e.g. HTTP server).
 * It provides a collection of APIs that allow the application to add/delete a user or change a
 * user's password.
 *
 * @}
 */

#ifndef _QAPI_UA_H_
#define _QAPI_UA_H_

#include <stdint.h>
#include "qapi/qapi_net_status.h"

/** @addtogroup qapi_networking_user_account
@{ */

/**
 * @brief Networking services which user with correct password is allowed to access.
 * @details Each service is defined as a bit in a 32-bit mask. If the bit is set in the user
 *          account, then the user is allowed to access that service.
 */
#define HTTP_USER       0x01
#define FTP_USER        0x02
#define SERVICE_ALL     0xFFFFFFFF  /**< Allowed to access all services. */

/**
 * @brief User account information for each user
 */
#define MAX_USERLENGTH  32
typedef struct
{
    char     username[MAX_USERLENGTH];  /**< NULL-terminated user name. */
    uint32_t service;                   /**< Each bit represents one allowed networking service. */
} qapi_Net_User_Info_t;

/**
 * @brief Initialize User Account service.
 *
 * @details This API should be called before any networking service can use user/password for authentication.
 *
 * @return On success, 0 is returned. On error, QAPI_NET_STATUS_USER_xxx defined in qapi_net_status.h is returned.
 */
qapi_Status_t qapi_Net_User_Init(void);

/**
 * @brief Add a networking service user.
 *
 * @param[in] username  A NULL-terminated textual name of a user. The name length
 *                      can not be over MAX_USERLENGTH - 1.
 *
 * @param[in] password  A NULL-terminated password string. The string length
 *                      can not be over MAX_USERLENGTH - 1.
 *
 * @param[in] service   Each bit represents a networking service. If the bit is set, the user is
 *                      allowed to access the indicated service. For example, if 'service' is HTTP_USER,
 *                      the user is allowed to access HTTP service. 
 *
 * @return On success, 0 is returned. On error, QAPI_NET_STATUS_USER_xxx defined in qapi_net_status.h is returned.
 */
qapi_Status_t qapi_Net_User_Add(const char *username, const char *password, uint32_t service);

/**
 * @brief Delete a networking service user.
 *
 * @param[in] username  A NULL-terminated textual name of a user. The name length
 *                      can not be over MAX_USERLENGTH - 1.
 *
 * @param[in] password  A NULL-terminated password string. The string length
 *                      can not be over MAX_USERLENGTH - 1.
 *
 * @return On success, 0 is returned. On error, QAPI_NET_STATUS_USER_xxx defined in qapi_net_status.h is returned.
 */
qapi_Status_t qapi_Net_User_Del(const char *username, const char *password);

/**
 * @brief Change a user's password.
 *
 * @param[in] username  A NULL-terminated textual name of a user. The name length
 *                      can not be over MAX_USERLENGTH - 1.
 *
 * @param[in] cur_Password  Current password is a NULL-terminated string. The string length
 *                          can not be over MAX_USERLENGTH - 1.
 *
 * @param[in] new_Password  New password is a NULL-terminated string. The string length
 *                          can not be over MAX_USERLENGTH - 1.
 *
 * @return On success, 0 is returned. On error, QAPI_NET_STATUS_USER_xxx defined in qapi_net_status.h is returned.
 */
qapi_Status_t qapi_Net_User_Change_Password(const char *username, const char *cur_Password, const char *new_Password);

/**
 * @brief Add a networking service to user account.
 *
 * @param[in] username  A NULL-terminated textual name of a user. The name length
 *                      can not be over MAX_USERLENGTH - 1.
 *
 * @param[in] password  A NULL-terminated string. The string length
 *                      can not be over MAX_USERLENGTH - 1.
 *
 * @param[in] service   Each bit represents a networking service. If the bit is set, the user is
 *                      allowed to access the indicated service. For example, if 'service' is HTTP_USER,
 *                      the user is allowed to access HTTP service. 
 *
 * @return On success, 0 is returned. On error, QAPI_NET_STATUS_USER_xxx defined in qapi_net_status.h is returned.
 */
qapi_Status_t qapi_Net_User_Add_Service(const char *username, const char *password, uint32_t service);

/**
 * @brief Delete a networking service from user account.
 *
 * @param[in] username  A NULL-terminated textual name of a user. The name length
 *                      can not be over MAX_USERLENGTH - 1.
 *
 * @param[in] password  A NULL-terminated string. The string length
 *                      can not be over MAX_USERLENGTH - 1.
 *
 * @param[in] service   Each bit represents a networking service. If the bit is set, the service is
 *                      deleted from the user account. For example, if 'service' is HTTP_USER,
 *                      the user's access to HTTP is denied. 
 *
 * @return On success, 0 is returned. On error, QAPI_NET_STATUS_USER_xxx defined in qapi_net_status.h is returned.
 */
qapi_Status_t qapi_Net_User_Del_Service(const char *username, const char *password, uint32_t service);

/**
 * @brief Get the list of users.
 *
 * @param[out] p_User_Info  Pointer to a buffer that contains the retrieved user info.
 *                          If NULL, the number of users is returned.
 *
 * @return On success, number of users is returned. On error, QAPI_NET_STATUS_USER_xxx defined in qapi_net_status.h is returned.
 *
 * @code {.c}
 *
 *      int32_t i, num;
 *      uint32_t size;
 *      char *buf;
 *      qapi_Net_User_Info_t *up;
 *
 *      num = qapi_Net_User_List(NULL);
 *      if (num >= 0)
 *      {
 *          printf("%d users:\n", num);
 *
 *          if (num > 0)
 *          {
 *              size = num * sizeof(qapi_Net_User_Info_t);
 *              buf = (qapi_Net_User_Info_t *)malloc(size);
 *              if (buf == NULL)
 *              {
 *                  printf("ERROR: no memory\n");
 *                  return -1;
 *              }
 *              memset(buf, 0, size);
 *
 *              up = (qapi_Net_User_Info_t *)buf;
 *              if (qapi_Net_User_List(up) < 0)
 *              {
 *                  free(buf);
 *                  printf("failed to get user list\n");
 *                  return -1;
 *              }
 *
 *              for (i = 0; i < num; ++i)
 *              {
 *                  printf("[%d]: %s  0x%02x\n", i+1, up->username, up->service);
 *                  ++up;
 *              }
 *              free(buf);
 *          }
 *      }
 *
 * @endcode
 */
int32_t qapi_Net_User_List(qapi_Net_User_Info_t *p_User_Info);

/** @} */

#endif /* _QAPI_UA_H_ */
