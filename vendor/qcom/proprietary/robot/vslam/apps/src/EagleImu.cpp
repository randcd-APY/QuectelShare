/*****************************************************************************
 * @copyright
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/
#include "EagleImu.h"
#include "Visualization.h"
#include "SystemTime.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define BUF_SIZE 100

#ifdef WIN32
#include <windows.h>
#define VSLAM_MASTER_SLEEP(x)  Sleep(x)
#else
#include <unistd.h>
#define VSLAM_MASTER_SLEEP(x)  usleep(x*1000)
#endif //WIN32

extern class Visualiser * visualiser;

float NORM_G = 9.80665f;

extern EagleImu *  eagleImu;
extern bool THREAD_RUNNING;

void imuProc( )
{

   if(!eagleImu )
   {
       printf( "imu instance not initialized or not use imu \n" );
       return;
   }
   sensor_mpu_driver_settings imuSettings;
   sensor_imu_attitude_api_get_mpu_driver_settings( eagleImu->sensorHandlePtr, &imuSettings );
   if( imuSettings.is_initialized != 1 )
   {
       printf( "imu driver not initialized imuSettings.is_initialized=%d \n", imuSettings.is_initialized );
       return;
   }
   
   printf("imu settings: \n");
   printf( "sample_rate_in_hz:  %d\n", imuSettings.sample_rate_in_hz );
   printf( "compass_sample_rate_in_hz:  %d\n", imuSettings.compass_sample_rate_in_hz );
   printf( "accel_lpf_in_hz:  %d\n", imuSettings.accel_lpf_in_hz );
   printf( "gyro_lpf_in_hz:  %d\n", imuSettings.gyro_lpf_in_hz );
   
   int64_t realClock = (int64_t)getRealTime();
   int64_t monotonicClock = getMonotonicTime();
   
   printf( "creating thread \n" );

    if( !eagleImu )
    {
        printf( "here with null context returning!\n" ); 
        //pthread_exit( NULL ); jiangtao
    }

    int numSamples = 0;
    float accVal[3], gyrVal[3];
    float delta = 0.f;

    sensor_imu * sensorDataPtr = new sensor_imu[BUF_SIZE]; 
    int64_t lastTimeStamp = 0;
    const int32_t * axleSign = eagleImu->getAxleSign();
    while( THREAD_RUNNING )
    { 
        eagleImu->getData( sensorDataPtr, BUF_SIZE, &numSamples ); 

        for( int j = 0; j < numSamples; ++j )
        {
            sensor_imu * curData = (sensor_imu *)sensorDataPtr + j;
            //int64_t curTimeStampNs = (int64_t)curData->timestamp_in_us * 1000 + myImuPtr->getOffset();
            int64_t curTimeStampNs = (int64_t)curData->timestamp_in_us * 1000;
            //printf( "time %" PRIu64 "\n", curData->timestamp_in_us );
            if( lastTimeStamp != 0 )
            {
                delta = (curTimeStampNs - lastTimeStamp)*1e-6f;
                if( delta > 50.0 )
                    printf( "SensorInterarrival > 50ms :%fms \n", delta );
            }
            lastTimeStamp = curTimeStampNs;
            //Assume that we have both accel and gyro every sample.
            accVal[0] = curData->linear_acceleration[0] * NORM_G;
            accVal[1] = curData->linear_acceleration[1] * NORM_G;
            accVal[2] = curData->linear_acceleration[2] * NORM_G;
            gyrVal[0] = curData->angular_velocity[0];
            gyrVal[1] = curData->angular_velocity[1];
            gyrVal[2] = curData->angular_velocity[2];

            eagleImu->invokeIMUCallback( accVal[0]*axleSign[0], accVal[1] * axleSign[1], accVal[2] * axleSign[2],
                                         gyrVal[0] * axleSign[0], gyrVal[1] * axleSign[1], gyrVal[2] * axleSign[2], curTimeStampNs/1000 );

            visualiser->PublishRawIMUdata( curTimeStampNs, gyrVal[0], gyrVal[1], gyrVal[2],
                                           accVal[0], accVal[1], accVal[2] );
        }

        // Sleep to avoid the crash caused by destroying the mutex while its busy
        // Would not a good idea and just a workaround
        VSLAM_MASTER_SLEEP( 20 );
    }
    delete[]( sensor_imu * )sensorDataPtr;
    /* Last thing that main() should do */
    //pthread_exit( NULL ); jiangtao

    if( eagleImu->sensorHandlePtr )
        sensor_imu_attitude_api_terminate( eagleImu->sensorHandlePtr );
}

EagleImu::EagleImu(int32_t sign[3]) 
{
    //gyroCallback = NULL;
    //accelCallback = NULL;
    imuDataCallback = NULL; 
    sensorHandlePtr = NULL;
    axleSign[0] = sign[0];
    axleSign[1] = sign[1];
    axleSign[2] = sign[2];
}


EagleImu::~EagleImu()
{
}

bool EagleImu::init()
{
    int retVal = 0;
    printf( "initializing sensor imu\n" );
    int16_t ret;

    int16_t imuNums = 0;
    int  assignedIMUID = 1;
    
    ret = sensor_imu_api_attitude_get_registered_imu_count( &imuNums );
    const char* imuSevVer = sensor_imu_attitude_api_get_server_version();
    printf( " Get imu number , result %d,imu server version %s \n", imuNums, imuSevVer );
    sensor_imu_id  imuList[5]; // our max imu list is 3. so 5 is large enough.
    ret = sensor_imu_attitude_api_get_imu_ids( imuList, 5, &imuNums );
    printf( "GET IMU ID INFO , total id %d, expeced to use %d  \n", imuNums, assignedIMUID );
    for( int16_t i = 0; i<imuNums; i++ )
    {
       printf( " IMU id %d, desc %s\n", i, imuList[i].imu_description );
    }
    sensorHandlePtr = sensor_imu_attitude_api_get_imu_handle( (int16_t)assignedIMUID );

    ret = sensor_imu_attitude_api_initialize( sensorHandlePtr, SENSOR_CLOCK_SYNC_TYPE_REALTIME );
    printf( "initialize the imu server api result : %d \n", ret );
    if(ret)
       return false;

    ret = sensor_imu_attitude_api_wait_on_driver_init( sensorHandlePtr );
    printf( "Wait imu server drier init result : %d \n", ret );
    if(ret)
       return false;

    //int rmatsz = 9;
    //ret = sensor_imu_attitude_api_imu_frame_to_body_frame_rotation_matrix( pImuServer, rmat, rmatsz );
    //printf( " imu to body matrix: return %d (%f %f %f)( %f %f %f)( %f %f %f) \n", ret, rmat[0], rmat[1], rmat[2], rmat[3], rmat[4], rmat[5], rmat[6], rmat[7], rmat[8] );

    return true;
}

//void EagleImu::addAccelCallback(ImuCallback _callback)
//{
//    accelCallback = _callback;
//}
//
//void EagleImu::addGyroCallback(ImuCallback _callback)
//{
//    gyroCallback = _callback;
//}
 
void EagleImu::addIMUCallback( ImuDataCallback _callback )
{
   imuDataCallback = _callback;
}

