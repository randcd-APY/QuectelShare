/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <AVSManager/Notification/Notification.h>

namespace alexaClientSDK {
namespace avsManager {

std::shared_ptr<Notification> Notification::create(std::string payloadJson) {
    return std::shared_ptr < Notification > (new Notification(payloadJson));
}

Notification::Notification(std::string payloadJson)
        : m_payloadJson(payloadJson) {

}

const std::string Notification::getPayloadJson() const {
    return m_payloadJson;
}

}
}
