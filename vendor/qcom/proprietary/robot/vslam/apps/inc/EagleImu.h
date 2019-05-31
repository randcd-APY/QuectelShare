/*****************************************************************************
 * @copyright
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/
#ifndef _EAGLE_IMU_H_
#define _EAGLE_IMU_H_

#include <thread> 
#include <mutex>

#include <sensor-imu/sensor_imu_api.h>
#include <sensor-imu/sensor_datatypes.h>

/// for using direct imu callbacks
class EagleImu
{
public:
    //typedef void( *ImuCallback )(float, float, float, int64_t);
    typedef void( *ImuDataCallback )(float, float, float, float, float, float, int64_t);
    EagleImu(int32_t sign[3]);
    ~EagleImu();
    
    bool init();

    void addIMUCallback( ImuDataCallback callback );
    //void addAccelCallback(ImuCallback callback);
    //void addGyroCallback(ImuCallback callback);

    void invokeIMUCallback( float accelX, float accelY, float accelZ, 
                            float gyroX,  float gyroY, float gyroZ, int64_t timeStamp )
    {
       if( !imuDataCallback )
       {
          return;
       }
       imuDataCallback( accelX, accelY, accelZ, gyroX, gyroY, gyroZ, timeStamp );
    }

    //void invokeAccelCallback( float valX, float valY, float valZ, int64_t timeStamp)
    //{
    //    if( !accelCallback )
    //    {
    //        return;
    //    }
    //    accelCallback( valX, valY, valZ, timeStamp );
    //}
    //void invokeGyroCallback( float valX, float valY, float valZ, int64_t timeStamp )
    //{
    //    if( !gyroCallback )
    //    {
    //        return;
    //    }
    //    gyroCallback( valX, valY, valZ, timeStamp );
    //}

    int16_t getData( sensor_imu* dataArray, int32_t max_count, int32_t* available_imu_data )
    {
        if( !sensorHandlePtr )
        {
            return -1;
        }
        int16_t result = sensor_imu_attitude_api_get_imu_raw( sensorHandlePtr, dataArray, max_count, available_imu_data );
        return result;
    }



    const int32_t * getAxleSign()
    {
       return axleSign;
    }

    sensor_handle* sensorHandlePtr;

protected:
    
    //ImuCallback gyroCallback, accelCallback;
    ImuDataCallback imuDataCallback;
    void* gyroUserData;
    void* accelUserData;

    std::thread imuPollThread;

    int32_t axleSign[3];
};

#endif
