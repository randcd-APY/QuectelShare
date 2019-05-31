/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <AVSManager/Notification/NotificationManager.h>

namespace alexaClientSDK {
namespace avsManager {

std::shared_ptr<NotificationManager> NotificationManager::create() {
    return std::shared_ptr < NotificationManager > (new NotificationManager());
}

void NotificationManager::registerNotificationCallback(
        NotificationCallbackTypeEvent event,
        std::shared_ptr<NotificationCallback> notificationCallBack) {
    std::string eventStr = stateToString(event);
    auto callbackItr = m_callbacks.find(eventStr);
    if (m_callbacks.end() == callbackItr) {
        m_callbacks[eventStr] = notificationCallBack;
    }
    else {
        callbackItr->second = notificationCallBack;
    }
}

void NotificationManager::handlelibECNSEvents(const std::string libEcnsEventJson) {
    m_executor.submit([this, libEcnsEventJson] () {
        executeCallbackEvent(NotificationCallbackTypeEvent::ECNS, libEcnsEventJson);
    });
}

void NotificationManager::handleMediaPlayerEvents(const std::string mediaPlayerEventJson) {
    m_executor.submit([this, mediaPlayerEventJson] () {
        executeCallbackEvent(NotificationCallbackTypeEvent::QTI_AUDIO_PLAYER, mediaPlayerEventJson);
    });

}

void NotificationManager::executeCallbackEvent(const NotificationCallbackTypeEvent event, const std::string & eventPayloadJson) {
    std::string eventStr = stateToString(event);
    auto m_notificationItr = m_callbacks.find(eventStr);
    if (m_callbacks.end() != m_notificationItr) {
        std::shared_ptr < Notification > notification = avsManager::Notification::create(eventPayloadJson);
        m_notificationItr->second->handleNotification(notification);
    }
}

std::string NotificationManager::stateToString(NotificationCallbackTypeEvent event) {
    switch (event) {
    case NotificationCallbackTypeEvent::QTI_AUDIO_PLAYER:
        return "QTI_AUDIO_PLAYER";
    case NotificationCallbackTypeEvent::ECNS:
        return "ECNS";
    default:
        return "";
    }
}

}
}
