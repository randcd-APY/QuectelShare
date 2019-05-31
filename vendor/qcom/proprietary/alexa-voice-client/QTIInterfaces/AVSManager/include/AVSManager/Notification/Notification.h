/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATION_H_
#define AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATION_H_

#include <memory>
#include <string>

namespace alexaClientSDK {
namespace avsManager {

class Notification {
public:
    static std::shared_ptr<Notification> create(std::string payloadJson);

    const std::string getPayloadJson() const;

private:
    Notification(std::string payloadJson);

    std::string m_payloadJson;
};

}
}

#endif /* AVSMANAGER_INCLUDE_AVSMANAGER_NOTIFICATION_NOTIFICATION_H_ */
