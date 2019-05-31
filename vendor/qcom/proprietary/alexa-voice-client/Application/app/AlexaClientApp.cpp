/*
 *Copyright (c) 2017 Qualcomm Technologies, Inc.
 *All Rights Reserved.
 *Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <string>
#include <AVSManager/QTIAVSApp.h>
#include <signal.h>

using namespace alexaClientSDK::avsManager;

void handle_signal(int signal) {
    if(signal== SIGINT)
    printf("Caught SIGINT, exiting now\n");
}

int main(int argc, char **argv) {
    struct sigaction sa;
    // Setup the sighub handler
    sa.sa_handler = &handle_signal;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
       perror("Error: cannot handle SIGINT");
    }

    auto qtiAVSApp = QTIAVSApp::create();

    //initialize AVS SDK and QTI Interfaces
    qtiAVSApp->initialize();

    //run application
    qtiAVSApp->run();

    return 0;
}

