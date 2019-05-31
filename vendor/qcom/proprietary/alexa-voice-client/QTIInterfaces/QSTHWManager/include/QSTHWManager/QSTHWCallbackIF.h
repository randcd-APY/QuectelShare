/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef ALEXA_CLIENT_SDK_TRIGGER_CALLBACK_IF_H_
#define ALEXA_CLIENT_SDK_TRIGGER_CALLBACK_IF_H_

#include <AVSCommon/AVS/AudioInputStream.h>

namespace alexaClientSDK {
namespace QSTHWManager {

using namespace alexaClientSDK::avsCommon::avs;

class QSTHWCallbackIF {

public:

    virtual ~QSTHWCallbackIF() = default;
    virtual void keywordRecognized(std::shared_ptr<AudioInputStream> stream, std::string keyword) = 0;
    virtual void keywordRecognizedWithIndices(std::shared_ptr<AudioInputStream> stream, std::string keyword, uint32_t startIndex, uint32_t endIndex) = 0;
    virtual void recognitionStateChanged(bool state) = 0;
    virtual void reinitializeSTHal() = 0;
};
}
}

#endif
