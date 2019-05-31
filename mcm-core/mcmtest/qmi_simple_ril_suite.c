/******************************************************************************
  @file    qmi_simple_ril_suite.c
  @brief   QMI simple RIL test suite, main file

  DESCRIPTION
  QMI simple RIL test suite, main
  Initialization and shutdown
  ---------------------------------------------------------------------------

  Copyright (c) 2010 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "qmi_client_utils.h"
#include "qmi_test_console.h"
#include "qmi_simple_ril_core.h"

/*qmi message library handle*/
//static int qmi_handle = QMI_INVALID_CLIENT_HANDLE;

static FILE* collector_input_handle;
static FILE* distributor_output_handle;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

FILE* distributor_data_handle;

static int qmi_simple_ril_suite_downlink_msg_collector (char* downlink_msg);
static void qmi_simple_ril_suite_uplink_msg_distributor(int nof_strings, char** uplinklink_msg);
static void qmi_simple_ril_suite_shutdown_handler (int cause);

#define MODEM_KEY "modem="
#define MODEM_PORT_KEY "modem_port="
#define INPUT_KEY "input="
#define OUTPUT_KEY "output="
#define DATA_KEY "data="

int main(int argc, char *argv[])
    {
    int i;
    int modemset = 0;

    char * input = NULL;
    char * output = NULL;
    char * data = NULL;

    printf("*** Simple RIL / QMI test console v2.0 ***\n");

    for(i = 1; i < argc; i++)
    {
        if (0 == strcmp(argv[i], "debug"))
        {
            qmi_util_log_enable(1);
        }
        else if (0 == strncmp(argv[i], MODEM_PORT_KEY, strlen(MODEM_PORT_KEY)))
        {
            qmi_util_set_modem_port(argv[i] + strlen(MODEM_PORT_KEY));
            modemset = 1;
        }
        else if (0 == strncmp(argv[i], INPUT_KEY, strlen(INPUT_KEY)))
        {
            input = argv[i] + strlen(INPUT_KEY);
        }
        else if (0 == strncmp(argv[i], OUTPUT_KEY, strlen(OUTPUT_KEY)))
        {
            output = argv[i] + strlen(OUTPUT_KEY);
        }
        else if (0 == strncmp(argv[i], DATA_KEY, strlen(DATA_KEY)))
        {
            data = argv[i] + strlen(DATA_KEY);
        }
    }


    qmi_util_logln2s("sril input output", input, output);

    if ( NULL == input )
    {
        collector_input_handle = stdin;
    }
    else
    {
        collector_input_handle = fopen( input, "r" );
        if ( NULL == collector_input_handle )
        { // fallback
            qmi_util_logln0( "fallback input to stdin " );
            collector_input_handle = stdin;
        }
    }
    if ( NULL == output )
    {
        distributor_output_handle = stdout;
    }
    else
    {
        distributor_output_handle = fopen( output, "w" );
        if ( NULL == distributor_output_handle )
        {
            qmi_util_logln0( "fallback output to stdout " );
            distributor_output_handle = stdout;
        }
    }
    if ( NULL == data )
    {
        distributor_data_handle = stdout;
    }
    else
    {
        distributor_data_handle = fopen( data, "w" );
        if ( NULL == distributor_data_handle )
        {
            qmi_util_logln0( "fallback data to stdout " );
            distributor_data_handle = stdout;
        }
    }

    fprintf(distributor_output_handle, "*** starting console ***\n");
    qmi_test_console_initialize(qmi_simple_ril_suite_downlink_msg_collector,
                                qmi_simple_ril_suite_uplink_msg_distributor,
                                qmi_simple_ril_suite_shutdown_handler);
    fprintf(distributor_output_handle, "Type in simple RIL commands and observe feedback \n");
    fprintf(distributor_output_handle, "Use 'quit' command to terminate simple RIL \n");
    fflush(distributor_output_handle);
    qmi_test_console_run(TRUE);
    return 0;
    }

int qmi_simple_ril_suite_downlink_msg_collector (char* downlink_msg)
    {
    int c;
    int nof_collected = 0;
    char *cur = downlink_msg;
    int res;
    while ( (c = fgetc(collector_input_handle)) && (c != '\n') && (c!= EOF) )
    {
        *cur = c;
        cur++;
        nof_collected++;
    }
    *cur = 0;
    if ( 0 == nof_collected && EOF == c )
    {
        res = QMI_SIMPLE_RIL_NO_DATA;
    }
    else
    {
        res = QMI_SIMPLE_RIL_ERR_NONE;
    }
    return res;
    }

FILE * qmi_simple_ril_suite_output_handle()
{
    return distributor_output_handle;
}

void qmi_simple_ril_suite_uplink_msg_distributor(int nof_strings, char** uplinklink_msg)
{
    int idx;
    FILE * file = qmi_simple_ril_suite_output_handle();
    pthread_mutex_lock( &log_mutex );
    for (idx = 0; idx < nof_strings; idx++)
    {
        // this is where writing to output file/stdout happens
        fprintf(file, "%s\n", uplinklink_msg[idx]);
        if(stdout != file)  {
            fprintf(stdout, "%s\n", uplinklink_msg[idx]);
        }
    }
    fflush (file);
    pthread_mutex_unlock( &log_mutex );
}

void qmi_simple_ril_suite_shutdown_handler (int cause)
    {
    qmi_util_logln1("qmi_simple_ril_suite_shutdown_handler", cause);
    }


