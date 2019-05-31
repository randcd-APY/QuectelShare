/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
   Copyright (c) 2013 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifdef USE_GLIB
#include <glib.h>
#define strlcpy g_strlcpy
#endif
#include "smart_meter_app.h"

void config_file_read_intialize_variables_smart_meter(char * config_filename,smart_meter_config_struct *smart_meter_config)
{
    printf ("Configuration File to be read = %s\n",config_filename);

    FILE *fp;    /*file pointer variable */
    fp=fopen(config_filename,"r");    /* open a file for reading */

    if (fp == NULL)
    {
        fprintf(stderr, "Can't open file %s!\n", config_filename);
        exit(1);
    }
    else
    {
        char line [128];
        char *search = "=";
        char *key,*value,*last;
        while (fgets(line,sizeof line,fp) != NULL)    /* read a line */
        {
            key = strtok_r(line, search,&last);    /*Key will point to the part before the = */
            value=strtok_r(NULL,"\n",&last);    /*Value will point to the part after = */

            if(key!=0 && value!=0)
            {
                if (strcmp(key,"Emergency")== 0)
                    strlcpy(smart_meter_config->emergency,value,MCM_MAX_PHONE_NUMBER_V01 + 1);
                else if (strcmp(key,"Server")== 0)
                    strlcpy(smart_meter_config->server,value,MCM_MAX_PHONE_NUMBER_V01 + 1);
                else if (strcmp(key,"Interval")== 0)
                    smart_meter_config->time_interval=atoi(value);
            }
        }
    }
    fclose(fp);    /* close the file */
}


void file_read_static_values_smart_meter(char * text_filename,smart_meter_values_struct *smart_meter_values)
{
    FILE *fp;    /*file pointer variable */
    fp=fopen(text_filename,"r");     /* open a file for reading */

    if (fp == NULL)
    {
        fprintf(stderr, "Can't open file %s!\n", text_filename);
        exit(1);
    }
    else
    {
        char line [10];
        int i;
        for (i=0;i<ARRAY_SIZE_VALUES_SMART_METER;i++)
        {
            if(fgets(line,sizeof line,fp)!=NULL)    /* read a line*/
            {
                smart_meter_values->meter_reading_static_values_arr[i]=atoi(line);
            }
        }
    }
    fclose(fp);    /* close the file */
}
