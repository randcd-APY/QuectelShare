/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATIONMANAGER_H_
#define AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATIONMANAGER_H_

#include <unordered_map>

#include <AVSCommon/Utils/Threading/Executor.h>
#include <AVSCommon/Utils/Timing/Timer.h>

#include <AVSManager/Notification/NotificationManagerInterface.h>
#include <AVSManager/Notification/NotificationCallbackTypeEvent.h>

namespace alexaClientSDK {
namespace avsManager {

class NotificationManager: public NotificationManagerInterface {

public:
    static std::shared_ptr<NotificationManager> create();
    void registerNotificationCallback(NotificationCallbackTypeEvent event, std::shared_ptr<NotificationCallback> notificationCallBack) override final;
    void handlelibECNSEvents(const std::string libEcnsEventJson) override final;
    void handleMediaPlayerEvents(const std::string mediaPlayerEventJson) override final;

private:
    static std::string stateToString(NotificationCallbackTypeEvent event);
    void executeCallbackEvent(const NotificationCallbackTypeEvent event, const std::string & eventPayloadJson);
    std::unordered_map<std::string, std::shared_ptr<NotificationCallback>> m_callbacks;
    avsCommon::utils::threading::Executor m_executor;
};

}
}

#endif /* AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATIONMANAGER_H_ */
