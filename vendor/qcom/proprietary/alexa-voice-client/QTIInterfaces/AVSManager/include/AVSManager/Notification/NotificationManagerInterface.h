/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATIONMANAGERINTERFACE_H_
#define AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATIONMANAGERINTERFACE_H_

#include <memory>

#include <AVSManager/Notification/NotificationCallback.h>
#include <AVSManager/Notification/NotificationCallbackTypeEvent.h>

namespace alexaClientSDK {
namespace avsManager {

class NotificationManagerInterface {
public:
    virtual ~NotificationManagerInterface() = default;
    virtual void registerNotificationCallback(NotificationCallbackTypeEvent event, std::shared_ptr<NotificationCallback> notificationCallBack) = 0;
    virtual void handlelibECNSEvents(const std::string libEcnsEventJson) = 0;
    virtual void handleMediaPlayerEvents(const std::string mediaPlayerEventJson) = 0;
};

}
}

#endif /* AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATIONMANAGERINTERFACE_H_ */
