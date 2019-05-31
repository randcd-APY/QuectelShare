/*
 * Copyright (c) 2011-2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

/*
 *  Created on: May 17, 2015
 *      Author: boyil
 *
 *  Updated on: July 01, 2016
 *      Author: QTIL
 */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include "global.h"
#include "qca_h4_transport.h"


/* BT TRANSPORT DEVICE NAME */
#define BTTRANSPORT_DEVICE_DIR  "/dev/"
#define PATH_MAX                300
#define FOLW_CONTROL            1

#ifdef PASA
#define BTTRANSPORT_DEVICE_DEFAULT_NAME "serbt8"
#else
#define BTTRANSPORT_DEVICE_DEFAULT_NAME "serbt1"
#endif

/* Using QCMBR module */
struct sockaddr_in server, client;
char ipAddress[255] = "localhost";
int clientaddr_size, read_size;
char message[1000] , server_reply[2000];
int portNumber = 2390; /* default: 2390 */
char IOTypeString[10] = "";
char baudrate[10] = "115200"; /* default: 115200 */
char debugLevel[20] = "";
bool isPatchandNvmRequired = 0;


enum
{
    IPADDRESS  = 1,
    PORT       = 2,
    IOTYPE     = 3,
    DEVICE     = 4,
    BAUDRATE   = 5,
};

struct arg_struct
{
    int desc;
    unsigned char *buf;
    int size;
};

ConnectionType connectionTypeOptions;
char Device[PATH_MAX] = "";
int sock, client_sock, descriptor = 0;

static int udt_exec();


static const BYTE DataToSendasLogPacketHeaderFor1366[] = { 0x10, 00, 0x12, 00, 0x12, 00, 0x66, 0x13, 0xE5, 0x62, 0x2A, 0xC5, 0xFF, 01, 00 };

static unsigned long dbgmsk = 0;


static void SendMsgToPc(const uint8_t *buf, uint32_t len);

extern void DiagPrintToStream(BYTE response[], int Length, bool check);
extern int processDiagPacket(BYTE inputDIAGMsg[]);


unsigned long qca_bt_debug_level(unsigned long level)
{
    if (level > 0)
    {
        dbgmsk = level;
        printf("Debug level is 0x%lx\n", dbgmsk);
    }

    return dbgmsk;
}

int input_qcmbr(int argc, char **argv)
{
    char *pPosition = NULL;
    char tempStr[20] = "";

    /* get argument values and check if minimum agruments are provided */
    if (argc < 5)
    {
        printf("\nbtdiag usage example:\n");
        printf("btdiag ip=192.168.0.2 port=2390 io=SERIAL dev=/dev/ttyUSB0 baud=115200\n");
        return EINVAL;
    }

    /* Check UDT */
    if (!strstr(argv[1], "ip"))
    {
        printf("\nMissing QPST server IP\n");
        return EINVAL;
    }
    else
    {
        pPosition = strchr(argv[1], '=');
        memcpy(ipAddress, pPosition + 1, strlen(pPosition + 1));
    }


    /* UDT mode: user defined transport mode */
    /* PORT */
    if (!strstr(argv[2], "port"))
    {
        printf("\nMissing QPST server port number\n");
        return EINVAL;
    }
    else
    {
        pPosition = strchr(argv[2], '=');
        memcpy(tempStr, pPosition + 1, strlen(pPosition + 1));
        portNumber = atoi(tempStr);
    }

    /* IOType */
    if (!strstr(argv[3], "io"))
    {
        printf("\nMissing IOType option, valid options are SERIAL, USB\n");
        return EINVAL;
    }
    else
    {
        pPosition = strchr(argv[3], '=');
        memcpy(IOTypeString, pPosition + 1, strlen(pPosition + 1));
    }

    /* get connection type */
    if (!strcmp(IOTypeString, "SERIAL"))
    {
        connectionTypeOptions = SERIAL;
    }
    else if (!strcmp(IOTypeString, "USB"))
    {
        connectionTypeOptions = USB;
    }
    else
    {
        printf("Invalid entry, valid options are : SERIAL or USB\n");
        return EINVAL;
    }

    /* DEVICE */
    if (!strstr(argv[4], "dev"))
    {
        printf("\nSpecifies the serial device to attach is not valid\n");
        return EINVAL;
    }
    else
    {
        pPosition = strchr(argv[4], '=');
        memcpy(Device, pPosition + 1, strlen(pPosition + 1));
    }

    if (connectionTypeOptions == SERIAL)
    {
        /* BAUDRATE */
        if (argc > 5)
        {
            if (NULL == strstr(argv[5], "baud"))
            {
                printf("\nMissing baudrate option\n");
                return EINVAL;
            }
            else
            {
                pPosition = strchr(argv[5], '=');
                memcpy(baudrate, pPosition + 1, strlen(pPosition + 1));
            }
        }
        else
        {
            printf("\nMissing baudrate option\n");
            return EINVAL;
        }

        /* require to download patch and NVM files */
        isPatchandNvmRequired = 1;

        if (argc > 6)
        {
            if (NULL == strstr(argv[6], "debug"))
            {
                printf("\nMissing debug option\n");
                return EINVAL;

            }
            else
            {
                pPosition = strchr(argv[6], '=');

                memcpy(debugLevel, pPosition + 1, strlen(pPosition + 1));

                qca_bt_debug_level(strtoul(debugLevel , NULL, 16));
            }
        }
    }

    return 0;
}

static int udt_exec()
{
    printf("udt_exec_running\n");
    /* Create thread to recive HCI event */
    {
        /* Check: move it to subfunction. */
        /* Create a thread for receiving HCI event from DUT. */

        int err = 0;

        pthread_t thread;
        struct arg_struct args;
        unsigned char databuf[HCI_MAX_EVENT_SIZE];

        args.desc = descriptor;
        args.buf = databuf;
        args.size = HCI_MAX_EVENT_SIZE;


        switch (connectionTypeOptions)
        {
            case SERIAL:
                /* Not to create thread to handle raw packet from UART. */
                break;

            case USB:
                /* NOT support USB transport. */
                break;
        }

        if (err != 0)
            printf("\nCan't create thread :[%s]", strerror(err));
        else
            printf("\nThread created successfully\n");
    }

    /* start TCP server */
    /* Create socket */
    sock = socket(AF_INET, SOCK_STREAM , 0);

    if (sock == -1)
    {
        printf("Could not create socket");
    }

    puts("Socket created");

    server.sin_family = AF_INET;
    server.sin_port = htons( portNumber );

    server.sin_addr.s_addr = inet_addr(ipAddress);
    memset(server.sin_zero, '\0', sizeof server.sin_zero);

    /* For fixing "Address already in use" issue.
      (When client keeps connection to server, terminate server and 
      reopen it will make "Address already in use" issue occur.) */
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Bind */
    if ( bind(sock, (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        /* TODO: check */
        close(sock);
        return 1;
    }

    puts("bind done");

    /* Listen */
    listen(sock , 3);

    /* Accept and incoming connection */
    puts("Waiting for incoming connections...");
    clientaddr_size  = sizeof(struct sockaddr_in);

    /* accept connection from an incoming client */
    client_sock = accept(sock, (struct sockaddr *)&client, (socklen_t *)&clientaddr_size);

    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }

    puts("Connected!");

    /* Buffer for data from stream */
    BYTE incomingDataBytes[TCP_BUF_LENGTH];
    memset(incomingDataBytes, 0, TCP_BUF_LENGTH);

    /* Loop to receive all the data sent by the QMSL/QRCT(via QPST), as long as it is greater than 0 */
    int lengthOfStream;

    while ((lengthOfStream = recv(client_sock, incomingDataBytes, TCP_BUF_LENGTH, 0)) != 0)
    {
        printf("receiving %s\n", incomingDataBytes);
        processDiagPacket(incomingDataBytes);

        /* initiate incomingDataBytes buf */
        memset(incomingDataBytes, 0, TCP_BUF_LENGTH);
    }

    if (lengthOfStream == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if (lengthOfStream == -1)
    {
        perror("recv failed");
    }

    return 0;
}

static void SendMsgToPc(const uint8_t *buf, uint32_t len)
{
    int DataToSendasLogPacketFor1366_Length = 0;

    BYTE DataToSendasLogPacketFor1366[15 + len];
    DataToSendasLogPacketFor1366_Length = 15 + len;
    memset(DataToSendasLogPacketFor1366, 0, DataToSendasLogPacketFor1366_Length);

    /* for 0x1366 */
    memcpy(DataToSendasLogPacketFor1366, DataToSendasLogPacketHeaderFor1366, 15);

    /* adding BT HCI event to the DIAG log packet */
    memcpy(DataToSendasLogPacketFor1366 + 15, buf, len);

    /* fixing the exact length in the DIAG log packet for 0x1366 */
    int lengthOfDiagPacketFor1366 = DataToSendasLogPacketFor1366_Length - 4;
    DataToSendasLogPacketFor1366[2] = (BYTE)lengthOfDiagPacketFor1366;
    DataToSendasLogPacketFor1366[4] = (BYTE)lengthOfDiagPacketFor1366;

    DiagPrintToStream(DataToSendasLogPacketFor1366, DataToSendasLogPacketFor1366_Length, true);
}

int main(int argc, char **argv)
{
    int status = 0;

    if (EXIT_SUCCESS != status)
    {
        printf("status: %d.\n", status);

        return status;
    }

    if (!input_qcmbr(argc, argv))
    {
        if (QCA_OpenH4Transport(Device, 3000000, SendMsgToPc))
        {
            udt_exec();
        }
        else
        {
            printf("Open uart device: %s failure.\n", Device);
        }
    }
    else
    {
        printf("Invalid arguments.\n");
    }

    QCA_CloseH4Transport();

    fflush(stdout);
    return EXIT_SUCCESS;
}
