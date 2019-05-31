/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATIONCALLBACK_H_
#define AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATIONCALLBACK_H_

#include <memory>

#include <AVSManager/Notification/Notification.h>

namespace alexaClientSDK {
namespace avsManager {

class NotificationCallback {
public:
    virtual ~NotificationCallback() = default;
    virtual void handleNotification(std::shared_ptr<Notification> notification) = 0;
};

}
}
#endif /* AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATIONCALLBACK_H_ */
