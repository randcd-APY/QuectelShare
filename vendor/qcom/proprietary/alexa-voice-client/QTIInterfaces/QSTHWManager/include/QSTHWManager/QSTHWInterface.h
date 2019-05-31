/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TRIGGER_INCLUDE_TRIGGER_QTISTHALINTERFACE_H_
#define TRIGGER_INCLUDE_TRIGGER_QTISTHALINTERFACE_H_

namespace alexaClientSDK {
namespace QSTHWManager {

class QSTHWInterface {
public:
    virtual ~QSTHWInterface() = default;
    virtual void startRecognition() = 0;
    virtual void stopRecognition() = 0;
    virtual void stopRecording() = 0;
    virtual int initialize() = 0;
    virtual void shutdown() = 0;
    virtual int getDOADirection() = 0;
    virtual int getChannelIndex() = 0;
};

}
}

#endif /* TRIGGER_INCLUDE_TRIGGER_QTISTHALINTERFACE_H_ */
