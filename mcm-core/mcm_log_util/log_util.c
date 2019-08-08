/******************************************************************************
#@file    log_util.c
#@brief   Provides interface for clients for logging
#
#  ---------------------------------------------------------------------------
#
#  Copyright (c) 2016 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/
#include "log_util.h"
FILE *log_fp = NULL;
pthread_mutex_t log_lock_mutex;
static log_util_type_e log_status = LOG_UTIL_NONE;

static uint8_t log_util_open_file_to_capture();

log_util_type_e log_util_init()
{
    int log_type = LOG_UTIL_NONE;
    pthread_mutexattr_t mtx_atr;
    uint8_t status = FALSE;
    DIR *d = NULL;
    int dc = -1;
    FILE *file_desc = NULL;

    do
    {
        pthread_mutexattr_init(&mtx_atr);
        pthread_mutex_init(&log_lock_mutex, &mtx_atr);

        d = opendir(LOG_CONFIG_FILE_DIR);
        if(NULL == d)
        {
            dc = mkdir(LOG_CONFIG_FILE_DIR, 0666);
            if( -1 == dc)
            {
                log_status = LOG_UTIL_CONSOLE;
                LOG_MSG_INFO("mcm-core directory creation failed");
                break;
            }

            LOG_MSG_INFO("mcm-core directory not present, so created");
        }

        file_desc = fopen(LOG_CONFIG_FILE, "r");
        if (NULL == file_desc)
        {
            file_desc = fopen(LOG_CONFIG_FILE, "w+");
            if(NULL == file_desc)
            {
                log_status = LOG_UTIL_CONSOLE;
                LOG_MSG_INFO("log_config.txt file creation failed");
                break;
            }

            LOG_MSG_INFO("log_config.txt not present, so created and written log_status as LOG_UTIL_CONSOLE");
            log_type = LOG_UTIL_CONSOLE;
            fprintf(file_desc, "%d", log_type);
            log_status = LOG_UTIL_CONSOLE;
        }

        if(LOG_UTIL_CONSOLE != log_type)
        {
            fscanf (file_desc, "%d", &log_type);

            if(LOG_UTIL_DISABLE == log_type)
            {
                log_status = LOG_UTIL_DISABLE;
            }
            else if(LOG_UTIL_CONSOLE == log_type)
            {
                log_status = LOG_UTIL_CONSOLE;
            }
            else if(LOG_UTIL_FILE == log_type)
            {
                log_status = LOG_UTIL_FILE;
            }
            else
            {
                log_status = LOG_UTIL_CONSOLE;
            }
        }

        if(LOG_UTIL_FILE == log_status)
        {
            status = log_util_open_file_to_capture();
            if(FALSE == status)
            {
                LOG_MSG_INFO("Failed to create File to capture log");
                log_status = LOG_UTIL_CONSOLE;
            }
            else
            {
                LOG_MSG_INFO("File created to capture log");
            }
        }
    }while(FALSE);

    if(NULL != d)
    {
        closedir(d);
    }

    if(NULL != file_desc)
    {
        fclose(file_desc);
        file_desc = NULL;
    }

    return log_status;
}

uint8_t log_util_open_file_to_capture()
{
    DIR *d = NULL;
    struct dirent *dir = NULL;
    int dc = -1;
    char *actual_file_name_to_delete = NULL;
    char *actual_file_name_to_create = NULL;
    int count =0;
    struct timeval tv;
    struct tm* tm = NULL;
    uint8_t status = FALSE;
    char time_string[40];
    char *(file_available_time_string[3]) = {NULL, NULL, NULL};
    struct stat attr[3];
    double diff_time[3];
    int file_deletion_status = 0;
    int index = 0;
    char *file_name = LOG_FILE_NAME;

    do
    {
        d = opendir(LOG_FILE_CAPTURE_DIR);
        if(NULL == d)
        {
            dc = mkdir(LOG_FILE_CAPTURE_DIR, 0666);
            if( -1 == dc )
            {
                break;
            }

            d = opendir(LOG_FILE_CAPTURE_DIR);
            if (NULL == d)
            {
                break;
            }
        }

        while ((dir = readdir(d)) != NULL)
        {
            if(!strncmp(dir->d_name, file_name, strlen(file_name)))
            {
                count+=1;
                file_available_time_string[index] = (char*)malloc(sizeof(char)*(strlen(dir->d_name) + 1));
                if(NULL == file_available_time_string[index])
                {
                    index = -1;
                    break;
                }

                memset(file_available_time_string[index], 0, (sizeof(char)*(strlen(dir->d_name) + 1)));
                snprintf(file_available_time_string[index],(sizeof(char)*(strlen(dir->d_name) + 1)), "%s", dir->d_name);
                index+=1;
                if(MAX_LOG_FILE == count)
                {
                    break;
                }
            }
        }

        if(index == -1)
        {
            break;
        }

        if(MAX_LOG_FILE == count)
        {
            for(index=0;index < MAX_LOG_FILE;index++)
            {
                stat(file_available_time_string[index], &attr[index]);
            }

            diff_time[0] = difftime(attr[0].st_mtime , attr[1].st_mtime);
            if(diff_time[0] > 0)
            {
                diff_time[1] = difftime(attr[1].st_mtime , attr[2].st_mtime);
                if(diff_time[1] > 0)
                {
                    index = 2; //The oldest file
                }
                else
                {
                    index = 1; //The oldest file
                }
            }
            else
            {
                diff_time[2] = difftime(attr[0].st_mtime , attr[2].st_mtime);
                if(diff_time[2] > 0)
                {
                    index = 2; //The oldest file
                }
                else
                {
                    index = 0; //The oldest file
                }
            }
        }

        if((MAX_LOG_FILE == count) && (index < MAX_LOG_FILE))
        {
            actual_file_name_to_delete = (char*)malloc(sizeof(char)*(strlen(LOG_FILE_CAPTURE_DIR) + strlen(file_available_time_string[index])+ 1));
            if(NULL == actual_file_name_to_delete)
            {
                break;
            }

            memset(actual_file_name_to_delete, 0, (sizeof(char)*(strlen(LOG_FILE_CAPTURE_DIR) + strlen(file_available_time_string[index])+ 1)));
            snprintf(actual_file_name_to_delete, (sizeof(char)*(strlen(LOG_FILE_CAPTURE_DIR) + strlen(file_available_time_string[index])+ 1)),"%s%s",LOG_FILE_CAPTURE_DIR, file_available_time_string[index]);
            file_deletion_status = remove(actual_file_name_to_delete);
            if(file_deletion_status)
            {
                break;
            }
        }

        gettimeofday(&tv, NULL);
        tm = localtime (&tv.tv_sec);
        if (tm == NULL)
        {
            break;
        }

        memset(time_string, 0, 40);
        strftime (time_string, sizeof (time_string), "%Y-%m-%d_%H:%M:%S", tm);
        actual_file_name_to_create = (char*)malloc(sizeof(char)*(strlen(LOG_FILE_CAPTURE_DIR) + strlen(file_name) + strlen(time_string)+ strlen(LOG_FILE_EXTN) + 1));
        if(NULL == actual_file_name_to_create)
        {
            break;
        }

        memset(actual_file_name_to_create, 0, (sizeof(char)*(strlen(LOG_FILE_CAPTURE_DIR) + strlen(file_name) + strlen(time_string)+ strlen(LOG_FILE_EXTN) + 1)));
        snprintf(actual_file_name_to_create, (sizeof(char)*(strlen(LOG_FILE_CAPTURE_DIR) + strlen(file_name) + strlen(time_string)+ strlen(LOG_FILE_EXTN) + 1)),"%s%s%s%s", LOG_FILE_CAPTURE_DIR, file_name, time_string, LOG_FILE_EXTN);

        log_fp = fopen(actual_file_name_to_create, "w");
        if(NULL == log_fp)
        {
            break;
        }

        status = TRUE;
    }while(FALSE);

    if(NULL != d)
    {
        closedir(d);
    }

    for(index = 0; index < MAX_LOG_FILE; index++)
    {
        if(NULL != file_available_time_string[index])
        {
            free(file_available_time_string[index]);
        }
    }

    if(NULL != actual_file_name_to_delete)
    {
        free(actual_file_name_to_delete);
    }

    if(NULL != actual_file_name_to_create)
    {
        free(actual_file_name_to_create);
    }

    return status;
}

void log_util_format_msg
(
  char *buf_ptr,
  int buf_size,
  char *fmt,
  ...
)
{
  va_list ap;


  va_start( ap, fmt );

  if ( NULL != buf_ptr && buf_size > 0 )
  {
      vsnprintf( buf_ptr, buf_size, fmt, ap );
  }

  va_end( ap );
}

void log_util_close()
{
    acquire_lock();
    if(log_fp)
    {
        fclose(log_fp);
        log_fp = NULL;
    }
    release_lock();
}

void acquire_lock()
{
    pthread_mutex_lock(&log_lock_mutex);
}

void release_lock()
{
    pthread_mutex_unlock(&log_lock_mutex);
}

void write_log_to_file(char *buf)
{
    do
    {
        if(LOG_UTIL_FILE == log_status)
        {
            if(log_fp)
            {
                fprintf(log_fp, "%s\n", buf);
            }
            else
            {
                printf("%s\n", buf);
            }
        }
        else if(LOG_UTIL_CONSOLE == log_status)
        {
            printf("%s\n", buf);
        }
    }while(FALSE);
}
