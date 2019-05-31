/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
* Qualcomm Technologies Proprietary and Confidential.
*
*/

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include<sys/socket.h>
#include <utils/Log.h>
#include "wifilogger_test.hpp"

FILE *gf_write = NULL;
FILE *gwrite_files[5];
char file_type[100];
int handler_set = 0, handler_count = 0;

char power_events_ring_name[] = "power_events_rb";
char connectivity_events_ring_name[] = "connectivity_events_rb";
char pkt_stats_ring_name[] = "pkt_stats_rb";
char driver_prints_ring_name[] = "driver_prints_rb";
char firmware_prints_ring_name[] = "firmware_prints_rb";

wifi_interface_handle wifi_get_iface_handle(wifi_handle handle, char *name);
namespace WIFI_LOGGER_TEST
{
    /* CLI cmd strings */
    const char *WifiLoggerTestSuite::WIFI_LOGGER_CMD = "wifi_logger";
    const char *WifiLoggerTestSuite::WIFI_LOGGER_GET_WIFI_INFO = "wifi_info";
    const char *WifiLoggerTestSuite::WIFI_LOGGER_START_LOGGING = "start_log";
    const char *WifiLoggerTestSuite::WIFI_LOGGER_GET_RING_BUFFERS_STATUS =
        "ring_status";
    const char *WifiLoggerTestSuite::WIFI_LOGGER_GET_SUPPORTED_FEATURE_SET =
        "supported_info";
    const char *WifiLoggerTestSuite::WIFI_LOGGER_GET_RING_DATA = "ring_data";
    const char *WifiLoggerTestSuite::WIFI_LOGGER_GET_MEMORY_DUMP = "memory_dump";

    /* Constructor */
    WifiLoggerTestSuite::WifiLoggerTestSuite(wifi_handle handle, wifi_request_id request_id)
        :wifiHandle_(handle)
    {
        fprintf(stderr, "WifiLoggerTestSuite::WifiLoggerTestSuite: Created a WifiLogger Test "
            "Suite with request_id:%d\n", request_id);
        id = request_id;
    }

    void WifiLoggerTestSuite::setRequestId(int reqId)
    {
        id = reqId;
    }

    int WifiLoggerTestSuite::getRequestId()
    {
        return id;
    }

    /* process the command line args */
    void WifiLoggerTestSuite::executeCmd(int argc,
                                      char **argv,
                                      int cmdIndex,
                                      cmdData data)
    {
        fprintf(stderr, "%s: Enter \n", __FUNCTION__);

        if(argc < 3)
        {
            fprintf(stderr, "%s: insufficient WifiLogger args\n", argv[0]);
            fprintf(stderr, "Usage : hal_proxy_daemon wifilogger interface_name");
            return;
        }

        ifaceHandle = wifi_get_iface_handle(wifiHandle_, argv[2]);
        if(!ifaceHandle)
        {
            fprintf(stderr, "Interface %s is not up, exiting.\n", argv[2]);
            fprintf(stderr, "Please restart hal_proxy_daemon with a valid"
                " initialized interface\n");
            return;
        }

        switch(cmdIndex) {
            case 1:
            case 2:
                wifiLoggerGetWifiInfo(argc, argv, data.versionType);
                break;
            case 3:
            case 4:
                data.mode = 1;
                wifiLoggerStartLogging(argc, argv, data);
                break;
            case 5:
                wifiLoggerGetRingBufferStatus(argc, argv);
                break;
            case 6:
                wifiLoggerGetSupportedFeatureSet(argc, argv);
                break;
            case 7:
                wifiLoggerGetRingData(argc, argv, data.ringId);
                break;
            case 8:
                wifiLoggerGetMemoryDump(argc, argv);
                break;
            default:
                fprintf(stderr, "%s: Unknown Cmd ID.\n", __FUNCTION__);
        }
    }

    /* process the command line args */
    void WifiLoggerTestSuite::processCmd(int argc, char **argv, cmdData data)
    {
        if(argc <3)
        {
            fprintf(stderr, "%s: insufficient WifiLogger args\n", argv[0]);
            return;
        }
        ifaceHandle = wifi_get_iface_handle(wifiHandle_, argv[2]);
        if(!ifaceHandle)
        {
            fprintf(stderr, "Interface %s is not up, exiting.\n", argv[2]);
            fprintf(stderr, "Please restart hal_proxy_daemon with a valid"
                " initialized interface\n");
            return;
        }

        if(strcasecmp(argv[3], WIFI_LOGGER_GET_WIFI_INFO) == 0)
            return wifiLoggerGetWifiInfo(argc, argv, data.versionType);

        if(strcasecmp(argv[3], WIFI_LOGGER_START_LOGGING) == 0)
            return wifiLoggerStartLogging(argc, argv, data);

        if(strcasecmp(argv[3], WIFI_LOGGER_GET_RING_BUFFERS_STATUS) == 0)
            return wifiLoggerGetRingBufferStatus(argc, argv);

        if(strcasecmp(argv[3], WIFI_LOGGER_GET_SUPPORTED_FEATURE_SET) == 0)
            return wifiLoggerGetSupportedFeatureSet(argc, argv);

        if(strcasecmp(argv[3], WIFI_LOGGER_GET_RING_DATA) == 0)
            return wifiLoggerGetRingData(argc, argv, data.ringId);

        if(strcasecmp(argv[3], WIFI_LOGGER_GET_MEMORY_DUMP) == 0)
            return wifiLoggerGetMemoryDump(argc, argv);

        fprintf(stderr, "%s: unknown  arg %s\n", argv[0], argv[2]);
    }

    /* Helper routine to print usage */
    void WifiLoggerTestSuite::wifiLoggerPrintCmdUsage(char **argv, const char *cmd,
       const char *sub_cmd, const struct option long_options[], int size)
    {
        fprintf(stderr, "Usage: %s %s %s\n", argv[0], cmd, sub_cmd);
        for(int i = 1; i <= size-2; i++)
        {
            if(long_options[i].has_arg)
                fprintf(stderr, "\t[--%s arg]\n", long_options[i].name);
            else
                fprintf(stderr, "\t[--%s]\n", long_options[i].name);
        }
        return;
    }

    void WifiLoggerTestSuite::wifiLoggerGetWifiInfo(int argc, char **argv,
                                                    int version_type)
    {
        int ret = 0;
        char version[20];
        char buffer[256];
        int  buffer_size = 256;

        /* A string listing valid short options letters.  */
        const char* const short_options = "h:";
        /* An array describing valid long options.  */
        const struct option long_options[] = {
            { "help",         0,   NULL, 'h' },
            { "version",      1,   NULL, 'v' },
            { NULL,           0,   NULL,  0  }   /* Required at end of array.  */
            };

        /* Override with command line arguements */
        int long_index = 0, opt = 0;
        while ((opt = getopt_long(argc, argv, short_options,
                long_options, &long_index )) != -1)
        {
            switch (opt)
            {
                case 'v':
                    strlcpy(version, optarg, strlen(optarg) + 1);
                    break;
                case 'h':
                default:
                    wifiLoggerPrintCmdUsage(argv, WIFI_LOGGER_CMD, WIFI_LOGGER_GET_WIFI_INFO,
                        long_options,
                        sizeof(long_options)/sizeof(struct option));
                    return;
            }
        }
        fprintf(stderr, "%s: Sending Get Wifi Info Request. \n",
            __FUNCTION__);
        if ((strcasecmp(version, "driver") == 0) || (version_type == 1)) {
            ret = wifi_get_driver_version(ifaceHandle, buffer, buffer_size);
            strlcpy(version, "Driver", sizeof(version));
        } else if ((strcasecmp(version, "firmware") == 0) || (version_type == 2)) {
            ret = wifi_get_firmware_version(ifaceHandle, buffer, buffer_size);
            strlcpy(version, "Firmware", sizeof(version));
        } else {
            fprintf(stderr, "%s: Invalid argument passed \n", version);
        }

        if (!ret)
            fprintf(stderr, "%s: %s version: %s \n",
                    __FUNCTION__, version, buffer);
        else
            fprintf(stderr, "%s: Received Wifi logger with value:%d. \n",
                    __FUNCTION__, ret);
    }

    bool is_valid_ring_buffer(char *buffer_name)
    {
        if ((strcmp(buffer_name, power_events_ring_name) == 0) ||
            (strcmp(buffer_name, connectivity_events_ring_name) == 0) ||
            (strcmp(buffer_name, pkt_stats_ring_name) == 0))
            return true;
        else
            return false;
    }

    void on_data_ring_buffer_handler(char *ring_name,
                                     char *buffer,
                                     int buffer_size,
                                     wifi_ring_buffer_status *status)
    {
        int szWritten = 0;
        unsigned int ring_id;

        //fprintf(stderr, "%s: RingName %s BufferSize 0x%x \n",
        //          __FUNCTION__, ring_name, buffer_size);

        if (ring_name == NULL) {
            fprintf(stderr, "no ring_name received in %s\n", __FUNCTION__);
            return;
        }

        if (strcmp(ring_name, power_events_ring_name) == 0) {
            ring_id = 0;
        } else if (strcmp(ring_name, connectivity_events_ring_name) == 0) {
            ring_id = 1;
        } else if (strcmp(ring_name, pkt_stats_ring_name) == 0) {
            ring_id = 2;
        } else if (strcmp(ring_name, driver_prints_ring_name) == 0) {
            ring_id = 3;
        } else if (strcmp(ring_name, firmware_prints_ring_name) == 0) {
            ring_id = 4;
        } else {
            fprintf(stderr, "Ring id not defined for %s yet\n", ring_name);
            return;
        }

        if (!gwrite_files[ring_id]) {
            snprintf(file_type, 100, "/data/misc/wifi/%s.bin", ring_name);
            gwrite_files[ring_id] = fopen(file_type, "wb+");
        }
        gf_write = gwrite_files[ring_id];

        if(gf_write) {
            szWritten = (int) fwrite(buffer, 1, buffer_size, gf_write);
            fflush(gf_write);
            //fprintf(stderr, "%s: Size written to file: 0x%x \n",
            //          __FUNCTION__, szWritten);
        } else {
            fprintf(stderr, "%s: Not able to open %s \n",
                    __FUNCTION__, file_type);
            return;
        }
    }

    void WifiLoggerTestSuite::wifiLoggerStartLogging(int argc, char **argv,
                                                     cmdData data)
    {
        int ret = 0;
        unsigned int verbose_level = 0, flags = 0, max_interval_sec = 0;
        unsigned int min_data_size = 0;
        char ring_name[32];
        wifi_ring_buffer_data_handler handler;

        handler.on_ring_buffer_data = on_data_ring_buffer_handler;
       /* A string listing valid short options letters.  */
        const char* const short_options = "h:";
        /* An array describing valid long options.  */
        const struct option long_options[] = {
            { "help",              0,   NULL, 'h' },
            { "verbose_level",     1,   NULL, 'v' },
            { "max_interval_sec",  1,   NULL, 'm' },
            { "min_data_size",     1,   NULL, 'n' },
            { "ring_name",         1,   NULL, 'b' },
            { NULL,                0,   NULL,  0  }   /* Required at end of array.  */
            };

        /* Override with command line arguements */
        int long_index = 0, opt = 0;
        while ((opt = getopt_long(argc, argv, short_options,
                long_options, &long_index )) != -1)
        {
            switch (opt)
            {
                case 'v':
                    verbose_level = atoi(optarg);
                    break;
                case 'm':
                    max_interval_sec = atoi(optarg);
                    break;
                case 'n':
                    min_data_size = atoi(optarg);
                    break;
                case 'b':
                    strlcpy(ring_name, optarg, strlen(optarg) + 1);
                    break;
                case 'h':
                default:
                    wifiLoggerPrintCmdUsage(argv, WIFI_LOGGER_CMD, WIFI_LOGGER_START_LOGGING,
                        long_options,
                        sizeof(long_options)/sizeof(struct option));
                    return;
            }
        }
        /* Use this for automation */
        if (data.mode != 1) {
            if  (argc < 6) {
                fprintf(stderr, "Invalid command,"
                        "require the ring_name and verbose level \n");
                wifiLoggerPrintCmdUsage(argv, WIFI_LOGGER_CMD,
                                        WIFI_LOGGER_START_LOGGING,
                                        long_options,
                        sizeof(long_options)/sizeof(struct option));
                return;
            }

            if (is_valid_ring_buffer(ring_name) == false) {
                fprintf(stderr, "Supported ring names 1) %s 2)%s 3)%s \n",
                        power_events_ring_name, connectivity_events_ring_name,
                        pkt_stats_ring_name);
                return;
            }

        } else {
            if (data.ringId == 1)
                memcpy(ring_name, power_events_ring_name,
                       strlen(power_events_ring_name) + 1);
            else if (data.ringId == 2)
                memcpy(ring_name, connectivity_events_ring_name,
                       strlen(connectivity_events_ring_name) + 1);
            else if (data.ringId == 3)
                memcpy(ring_name, pkt_stats_ring_name,
                       strlen(pkt_stats_ring_name) + 1);
            else if (data.ringId == 4)
                memcpy(ring_name, driver_prints_ring_name,
                       strlen(driver_prints_ring_name) + 1);
            else if (data.ringId == 5)
                memcpy(ring_name, firmware_prints_ring_name,
                       strlen(firmware_prints_ring_name) + 1);
            verbose_level = data.verboseLevel;
            max_interval_sec = data.maxInterval;
            min_data_size = data.minDataSize;
        }
        if (verbose_level >= 1) {
            fprintf(stderr, "Start Logging for Ring Name %s. \n",
                    ring_name);
            handler_count++;
            if (handler_set != true)
                wifi_set_log_handler(id, ifaceHandle, handler);
        } else {
            fprintf(stderr, "Stop Logging for Ring Name %s. \n",
                    ring_name);
            handler_count--;
            if ((handler_set == true) && (handler_count == 0))
               wifi_reset_log_handler(id, ifaceHandle);
        }

        fprintf(stderr, "verbose_level %d flags %d max_int %d "
                "min_dat %d ring_name %s \n", verbose_level, flags,
                max_interval_sec, min_data_size, ring_name);
        ret = wifi_start_logging(ifaceHandle, verbose_level, flags,
                                 max_interval_sec, min_data_size,
                                 ring_name);

        fprintf(stderr, "%s: Received Wifi logger with value:%d. \n",
            __FUNCTION__, ret);
    }

    void WifiLoggerTestSuite::wifiLoggerGetRingBufferStatus(int argc, char **argv)
    {
        int ret = 0;
        unsigned int num_rings = 10, i = 0;
        wifi_ring_buffer_status status[10];
        /* A string listing valid short options letters.  */
        const char* const short_options = "h:";
        /* An array describing valid long options.  */
        const struct option long_options[] = {
            { "help",         0,   NULL, 'h' },
            { NULL,           0,   NULL,  0  }   /* Required at end of array.  */
            };

        /* Override with command line arguements */
        int long_index = 0, opt = 0;
        while ((opt = getopt_long(argc, argv, short_options,
                long_options, &long_index )) != -1)
        {
            switch (opt)
            {
                case 'h':
                default:
                    wifiLoggerPrintCmdUsage(argv, WIFI_LOGGER_CMD, WIFI_LOGGER_GET_RING_BUFFERS_STATUS,
                        long_options,
                        sizeof(long_options)/sizeof(struct option));
                    return;
            }
        }
        fprintf(stderr, "%s: Sending Get Ring Buffer Status. \n",
            __FUNCTION__);

        ret = wifi_get_ring_buffers_status(ifaceHandle, &num_rings, status);
        fprintf(stderr, "%s: Received Wifi logger with value:%d. \n",
            __FUNCTION__, ret);
        for (i = 0; i < num_rings; i++) {
                printf("\n################ Ring Info ########################\n");
                printf("Ring Name :           %s\n", status[i].name);
                printf("flags :               %d\n", status[i].flags);
                printf("Ring Id :             %d\n", status[i].ring_id);
                printf("Ring Buffer Size :    %d\n", status[i].ring_buffer_byte_size);
                printf("Verbose Level :       %d\n", status[i].verbose_level);
                printf("Written Bytes :       %d\n", status[i].written_bytes);
                printf("Read Bytes :          %d\n", status[i].read_bytes);
                printf("Written Records :     %d\n", status[i].written_records);
        }

        if (ret)
            return;
    }

    void WifiLoggerTestSuite::wifiLoggerGetSupportedFeatureSet(int argc, char **argv)
    {
        int ret = 0;
        unsigned int support;

        /* A string listing valid short options letters.  */
        const char* const short_options = "h:";
        /* An array describing valid long options.  */
        const struct option long_options[] = {
            { "help",         0,   NULL, 'h' },
            { NULL,           0,   NULL,  0  }   /* Required at end of array.  */
            };

        /* Override with command line arguements */
        int long_index = 0, opt = 0;
        while ((opt = getopt_long(argc, argv, short_options,
                long_options, &long_index )) != -1)
        {
            switch (opt)
            {
                case 'h':
                default:
                    wifiLoggerPrintCmdUsage(argv, WIFI_LOGGER_CMD, WIFI_LOGGER_GET_WIFI_INFO,
                        long_options,
                        sizeof(long_options)/sizeof(struct option));
                    return;
            }
        }
        fprintf(stderr, "%s: Sending Get Supported Feature Set. \n",
            __FUNCTION__);
        ret = wifi_get_logger_supported_feature_set(ifaceHandle, &support);

        fprintf(stderr, "%s: Received Wifi logger with value:%d. \n",
            __FUNCTION__, ret);

        if (ret)
            return;
        fprintf(stderr, "Supported Feature Set: 0x%x. \n", support);
    }

    void WifiLoggerTestSuite::wifiLoggerGetRingData(int argc, char **argv, int ring_id)
    {
        int ret = 0;
        char ring_name[100];

        /* A string listing valid short options letters.  */
        const char* const short_options = "h:";
        /* An array describing valid long options.  */
        const struct option long_options[] = {
            { "ring_name",    0,   NULL, 'r' },
            { "help",         0,   NULL, 'h' },
            { NULL,           0,   NULL,  0  }   /* Required at end of array.  */
            };

        /* Override with command line arguements */
        int long_index = 0, opt = 0;
        while ((opt = getopt_long(argc, argv, short_options,
                long_options, &long_index )) != -1)
        {
            switch (opt)
            {
                case 'v':
                    strlcpy(ring_name, optarg, strlen(optarg) + 1);
                    break;
                case 'h':
                default:
                    wifiLoggerPrintCmdUsage(argv, WIFI_LOGGER_CMD, WIFI_LOGGER_GET_RING_DATA,
                        long_options,
                        sizeof(long_options)/sizeof(struct option));
                    return;
            }
        }
        if (ring_id == 1)
            memcpy(ring_name, power_events_ring_name,
                   strlen(power_events_ring_name) + 1);
        else if (ring_id == 2)
            memcpy(ring_name, connectivity_events_ring_name,
                   strlen(connectivity_events_ring_name) + 1);
        else if (ring_id == 3)
            memcpy(ring_name, pkt_stats_ring_name,
                   strlen(pkt_stats_ring_name) + 1);
        else if (ring_id == 4)
            memcpy(ring_name, driver_prints_ring_name,
                   strlen(driver_prints_ring_name) + 1);
        else if (ring_id == 5)
            memcpy(ring_name, firmware_prints_ring_name,
                   strlen(firmware_prints_ring_name) + 1);

        fprintf(stderr, "%s: Sending Get Ring Data. \n",
            __FUNCTION__);
        ret = wifi_get_ring_data(ifaceHandle, ring_name);

        fprintf(stderr, "%s: Received Wifi logger with value:%d. \n",
            __FUNCTION__, ret);
        if (ret)
            return;
    }

    void firmware_memory_dump_handler(char *buffer, int buffer_size)
    {
        int szWritten = 0;
        fprintf(stderr, "%s: memBufferSize:%d\n",
                __FUNCTION__, buffer_size);
        /* Send the file */
        FILE *f_write = fopen("/data/misc/wifi/firmware_memory_dump.bin",
                              "wb+");
        if (f_write) {
            szWritten = (int)fwrite(buffer, 1, buffer_size, f_write);
            fprintf(stderr, "%s: Size written to file:%d\n",
                    __FUNCTION__, szWritten);
        } else {
            fprintf(stderr,"%s: Not able to open "
                    "/data/misc/wifi/firmware_memory_dump.bin.\n",
                    __FUNCTION__);
            return;
        }
        fclose(f_write);
        return;
    }

    void WifiLoggerTestSuite::wifiLoggerGetMemoryDump(int argc, char **argv)
    {
        int ret = 0, buf_len;
        wifi_firmware_memory_dump_handler handler;
        handler.on_firmware_memory_dump = firmware_memory_dump_handler;

        /* A string listing valid short options letters.  */
        const char* const short_options = "h:";
        /* An array describing valid long options.  */
        const struct option long_options[] = {
            { "help",         0,   NULL, 'h' },
            { NULL,           0,   NULL,  0  }   /* Required at end of array.  */
            };

        /* Override with command line arguements */
        int long_index = 0, opt = 0;
        while ((opt = getopt_long(argc, argv, short_options,
                long_options, &long_index )) != -1)
        {
            switch (opt)
            {
                case 'h':
                default:
                    wifiLoggerPrintCmdUsage(argv, WIFI_LOGGER_CMD, WIFI_LOGGER_START_LOGGING,
                        long_options,
                        sizeof(long_options)/sizeof(struct option));
                    return;
            }
        }
        fprintf(stderr, "%s: Sending Get Memory Dump Request. \n",
            __FUNCTION__);

        ret = wifi_get_firmware_memory_dump(ifaceHandle, handler);
        fprintf(stderr, "%s: Received Wifi logger with value:%d. \n",
            __FUNCTION__, ret);
        if (ret)
            return;
    }
}
