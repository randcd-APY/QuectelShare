/*****************************************************************************
 * @copyright
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 * *******************************************************************************/

#include <sensor-imu/sensor_imu_api.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>

#define MMAP_SIZE 4096
#define DATA_LENGTH 24

#define IDLE 0
#define INIT 1
#define INIT_RAW 11
#define START 2
#define STOP 3

struct imu_pack{
   double angular_velocity_x;
   double angular_velocity_y;
   double angular_velocity_z;
   double acceloration_x;
   double acceloration_y;
   double acceloration_z;
   int16_t temperature;
   uint64_t time;
};

struct imu_raw{
   int16_t gyro_raw_x;
   int16_t gyro_raw_y;
   int16_t gyro_raw_z;
   int16_t acc_raw_x;
   int16_t acc_raw_y;
   int16_t acc_raw_z;
   int16_t temperature;
   uint64_t time;
};

#define PACK_SIZE (sizeof(struct imu_pack))
static sensor_handle icm20602_handle;
static int sockfd;
static int mmap_fd = 0;
static volatile char *map = NULL;
static char raw_buf[MMAP_SIZE] = {0};
static uint64_t last_time = 0;
static int isRawData = 0;  //default is to get converted data

void sensor_imu_api_set_data_type(int type)
{
   //type, 0 is converted data, !0 is raw data
   isRawData = type;
}

IMU_API int16_t sensor_imu_api_attitude_get_registered_imu_count( int16_t* imu_count )
{
   *imu_count = 1; //currently just 1
   return 0;
}

IMU_API char* sensor_imu_attitude_api_get_server_version()
{
   return "server-1.0";
}

/**
 * API to get the array of for different IMU's supported by the IMU server on dsp
 * @param imu_id_array
 *    This is an output array. The array of IMU's supported by the Server.  This is a structure defined by
 *    @sensor_imu_id.
 * @param array_size
 *    The max size of the array that the server can fill upto.  This should be atleast the
 *    number of the IMU's reported by the @sensor_imu_api_attitude_get_register_imu_count api.
 * @param returned_element_count
 *    Specifies the number of imu'id's returned.
 * @return
 *    0 = success
 *  otherwise = failure;
 */
IMU_API int16_t sensor_imu_attitude_api_get_imu_ids
(
  sensor_imu_id* imu_id_array,
  int16_t  array_size,
  int16_t* returned_elements_count
)
{   char *desc = "INV-ICM20602";
   size_t desc_len = strnlen(desc, SENSOR_IMU_MAX_DESCRIPTION_LEN);

   imu_id_array[0].imu_id = 1;
   strncmp((const char *)imu_id_array[0].imu_description, desc, desc_len);
   imu_id_array[0].imu_type = SENSOR_IMU_TYPE_CAMERA_VIDEO;
   imu_id_array[0].imu_max_buffer_size = 256;

   *returned_elements_count = 1;
   return 0;
}

/**
 * This returns the handle to the sensor_imu_attitude_api instance for the specified
 * imu_id. The imu_id should be less than the imu_count returned by @sensor_imu_attitude_api_get_num_imu_count
 * If the imu_id is not in the valid range the return handle is null/0.
 * Use this handle to invoke the api's.
 * @param imu_id
 *   the imu_id returned from get_imu_ids call.
 * @return sensor_handle*
 *  pointer to the api handle.
 *  0 = failure.
 *  anyother value = success.
 */
IMU_API sensor_handle* sensor_imu_attitude_api_get_imu_handle( int16_t imu_id )
{
   return &icm20602_handle;
}

/**
 * Initializes the sensor api. If the flight stack is running
 * on the ADSP, this initializes the communication channel between the apps process
 * and the adsp. The clock type is used to handle the sychronization of the ADSP and
 * the apps processor.
 * @param handle
 *   This handle should be obtained by calling the function @sensor_imu_attitude_api_get_imu_handle
 *   This represents the IMU instance to be initialized.
 * @param type
 *  The clock type of the apps process to use for the time synchronization between
 *  apps processor and the adsp.
 * @param
 * @return
 *    0 = success
 *    other = failure.
 */
IMU_API int16_t sensor_imu_attitude_api_initialize
(
    sensor_handle*         handle,
    sensor_clock_sync_type type
)
{
   if(SENSOR_CLOCK_SYNC_TYPE_REALTIME == type)
   {
      //create socket connection
      struct sockaddr_un address;
      int len;
      int result;

      if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
      {
          printf("create socket error\n");
            return -1;
      }

        address.sun_family = AF_UNIX;
      strcpy (address.sun_path, "/data/imud_socket");
      len = sizeof (address);

      result = connect (sockfd, (struct sockaddr *)&address, len);
      if (result == -1)
      {
          printf ("imu server is down\n");
          return -1;
      }
   }
   else
      return -1;

   return 0;
}

static int init_mmap(void)
{
    mmap_fd = open("/data/imu_map", O_RDWR, 00644);
    int ret = 0;

    if(mmap_fd > 0)
    {
        map = (char *)mmap(0, MMAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mmap_fd, 0);
        if(map)
        {
            printf("c : init_mmap : map addr = %p\n", map);
        }
        else
        {
            printf("c : init_mmap : map failed\n");
            ret = -1;
        }
    }
    else
    {
        printf("c : init_mmap : open failed\n");
        ret = -1;
    }

    return ret;
}

/**
 * Method to wait on driver initializtion if needed.
 * @Note: Call this method before calling the sensor_imu_attitude_api_get_imu_raw()
 * to make sure the driver is initialized.
 * @return int32_t
 * 0 = sucess
 * otherwise = failure
 */
IMU_API int16_t sensor_imu_attitude_api_wait_on_driver_init( sensor_handle* handle )
{
   int a = INIT;
   int bytes;

   if(isRawData)
      a = INIT_RAW;  //get raw data via cmd = 11

   //write cmd to init imud
   if ((bytes = write (sockfd, &a, sizeof(int))) == -1)
   {
      printf("write socket cmd failed\n");
      return -1;
   }

   if(init_mmap())
   {
      printf("mmap failed\n");
      return -1;
   }

   //write cmd to notify client init done
   a = START;
   if ((bytes = write (sockfd, &a, sizeof(int))) == -1)
   {
      printf("write socket cmd failed\n");
      return -1;
   }
   sleep(1);  //wait imu server ready
   return 0;
}

/**
 * This returns Mpu Driver settings.  If the driver is not initialized,
 * the sample rates will be set to -1.
 * @param settings
 *  The setting to be returned.
 * @return int32_t
 *  0 = on success;
 *  otherwise = failure;
 */
IMU_API int16_t sensor_imu_attitude_api_get_mpu_driver_settings
(
    sensor_handle*              handle,
    sensor_mpu_driver_settings* settings
)
{
   //hard code first
    settings->sample_rate_in_hz = 200;
    settings->compass_sample_rate_in_hz = 0;  //not support
    settings->accel_lpf_in_hz = 92;
    settings->gyro_lpf_in_hz = 99;

    settings->is_initialized = 1;
   return 0;
}

/**
 * Gets the raw imu data.  For the ADSP implementation
 * this is a blocking call, if there this no data.
 * On Adsp the numbers of samples bufferred is 100.  If the
 * buffer is full the oldest data is overwritten.
 * @param dataArray
 *    Pointer pre-allocated sensor_imu array to get the imu data.
 * @param max_count
 *    The maximum size of the pre-allocated array
 * @param returned_sample_count
 *     the number of imu's samples returned.
 * @return
 *     0 = success
 *     other = failure;
 * @Note: The ImuData is in  the raw IMU Frame of the eagle board.
 */
IMU_API int16_t sensor_imu_attitude_api_get_imu_raw
(
    sensor_handle* handle,
    sensor_imu*    dataArray,
    int32_t        max_count,
    int32_t*       returned_sample_count
)
{
   static uint32_t sequence_number = 0;
   struct imu_pack *tmp;
   int i;
   int startIndex = -1;
   int availableNum;
   
   //this API is not available if request IMU sensor raw data
   if(isRawData)
   {
      *returned_sample_count = 0;
      return -1;
   }

   memcpy((void *)raw_buf, (void *)map, MMAP_SIZE);

   if(last_time == 0)  //only get the latest data at beginning
   {
      tmp = (struct imu_pack *)(raw_buf + PACK_SIZE*63);
      last_time = tmp->time;
      dataArray[0].angular_velocity[0] = tmp->angular_velocity_x;
      dataArray[0].angular_velocity[1] = tmp->angular_velocity_y;
      dataArray[0].angular_velocity[2] = tmp->angular_velocity_z;

      dataArray[0].linear_acceleration[0] = tmp->acceloration_x;
      dataArray[0].linear_acceleration[1] = tmp->acceloration_y;
      dataArray[0].linear_acceleration[2] = tmp->acceloration_z;

      dataArray[0].sequence_number = sequence_number++;
      dataArray[0].timestamp_in_us = tmp->time/1000;
      dataArray[0].temperature = (float)tmp->temperature;

      *returned_sample_count = 1;
   }
   else
   {
      *returned_sample_count = 0;

      tmp = (struct imu_pack *)raw_buf;
      if(tmp->time > last_time)
         startIndex = 0;

      if(startIndex < 0) 
      {
         i = 0;
         do{
            if(tmp->time == last_time)
               break;

            tmp++;
            i++;
         }while(i < 64);

         //find index done
         if(i < 64)
         {
            tmp++;
            i++;
            startIndex = i;
         }
         else
         {
            //last_time = 0;
            printf("imu data error\n");
            return -1;
         }
      }

      //availableNum = 64 - startIndex;
      tmp = (struct imu_pack *)(raw_buf + PACK_SIZE*startIndex);
      int j = 0;
      for(i = startIndex; i < 64; i++, tmp++)
      {
         if(tmp->time < last_time)
            continue;

         last_time = tmp->time;
         //printf( "imuClient %lld\n", last_time );

         dataArray[j].angular_velocity[0] = tmp->angular_velocity_x;
         dataArray[j].angular_velocity[1] = tmp->angular_velocity_y;
         dataArray[j].angular_velocity[2] = tmp->angular_velocity_z;

         dataArray[j].linear_acceleration[0] = tmp->acceloration_x;
         dataArray[j].linear_acceleration[1] = tmp->acceloration_y;
         dataArray[j].linear_acceleration[2] = tmp->acceloration_z;

         dataArray[j].sequence_number = sequence_number++;
         dataArray[j].timestamp_in_us = tmp->time/1000;

         dataArray[j].temperature = (float)tmp->temperature;
         j++;

         if(j == max_count)
            break;
      }

      *returned_sample_count = j;
   }

   return 0;
}

int16_t sensor_imu_api_get_sensor_raw
(
    sensor_handle* handle,
	sensor_imu_raw* dataArray,
    int32_t        max_count,
    int32_t*       returned_sample_count
)
{
   struct imu_raw *tmp;  //the raw data packed in the same shape as (struct imu_pack)
   int i;
   int startIndex = -1;
   int availableNum;

   //this API is not available if request IMU calculated data
   if(!isRawData)
   {
      *returned_sample_count = 0;
      return -1;
   }

   memcpy((void *)raw_buf, (void *)map, MMAP_SIZE);

#if 0  //debug IMU
   tmp = (struct imu_raw *)(raw_buf + PACK_SIZE*60);
   printf("acc1.x raw %d\n", tmp->acc_raw_x);
   tmp = (struct imu_raw *)(raw_buf + PACK_SIZE*61);
   printf("acc2.x raw %d\n", tmp->acc_raw_x);
   tmp = (struct imu_raw *)(raw_buf + PACK_SIZE*62);
   printf("acc3.x raw %d\n", tmp->acc_raw_x);
   tmp = (struct imu_raw *)(raw_buf + PACK_SIZE*63);
   printf("acc4.x raw %d\n", tmp->acc_raw_x);
#endif

   if(last_time == 0)  //only get the latest data at beginning
   {
      tmp = (struct imu_raw *)(raw_buf + PACK_SIZE*63);
      last_time = tmp->time;
      dataArray[0].acc[0] = tmp->acc_raw_x;
      dataArray[0].acc[1] = tmp->acc_raw_y;
      dataArray[0].acc[2] = tmp->acc_raw_z;

      dataArray[0].gyro[0] = tmp->gyro_raw_x;
      dataArray[0].gyro[1] = tmp->gyro_raw_y;
      dataArray[0].gyro[2] = tmp->gyro_raw_z;

      dataArray[0].timestamp_in_us = tmp->time/1000;
      dataArray[0].temp = tmp->temperature;

      *returned_sample_count = 1;
   }
   else
   {
      *returned_sample_count = 0;

      tmp = (struct imu_raw *)raw_buf;
      if(tmp->time > last_time)
         startIndex = 0;

      if(startIndex < 0)
      {
         i = 0;
         do{
            if(tmp->time == last_time)
               break;

            i++;
            tmp = (struct imu_raw *)(raw_buf + PACK_SIZE*i);
         }while(i < 64);

         //find index done
         if(i < 64)
         {
        	tmp = (struct imu_raw *)(raw_buf + PACK_SIZE*i);
            i++;
            startIndex = i;
         }
         else
         {
            last_time = 0;
            printf("imu data error\n");
            return -1;
         }
      }

      availableNum = 64 - startIndex;
      if(availableNum > max_count)
         *returned_sample_count = max_count;
      else
         *returned_sample_count = availableNum;

      for(i = 0; i <*returned_sample_count; i++)
      {
         tmp = (struct imu_raw *)(raw_buf + PACK_SIZE*(startIndex+i));
         last_time = tmp->time;

         dataArray[i].acc[0] = tmp->acc_raw_x;
         dataArray[i].acc[1] = tmp->acc_raw_y;
         dataArray[i].acc[2] = tmp->acc_raw_z;

         dataArray[i].gyro[0] = tmp->gyro_raw_x;
         dataArray[i].gyro[1] = tmp->gyro_raw_y;
         dataArray[i].gyro[2] = tmp->gyro_raw_z;

         dataArray[i].timestamp_in_us = tmp->time/1000;

         dataArray[i].temp = (float)tmp->temperature;
      }
   }

   return 0;
}

/**
 * Api to allow for cleanup of the API.  This should be called at shutdown of the
 * client application.
 * @return
 *   0 = success
 *   other = failure
 */
IMU_API int16_t sensor_imu_attitude_api_terminate( sensor_handle* handle )
{
    int a = STOP;
    int bytes;
    int retry = 0;

    last_time = 0;
   while ((bytes = write (sockfd, &a, sizeof(int))) == -1)
   {
      printf("write socket cmd failed\n");
      retry++;
      if(retry > 10)
         break;
   }
   close(sockfd);

   munmap((void *)map, MMAP_SIZE);
   close(mmap_fd);

   if(retry > 10)
      return -1;
   else
      return 0;
}
