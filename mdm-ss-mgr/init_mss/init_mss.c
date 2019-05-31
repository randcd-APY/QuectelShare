/******************************************************************************
Copyright (c) 2014, 2016, 2018 The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above
          copyright notice, this list of conditions and the following
          disclaimer in the documentation and/or other materials provided
          with the distribution.
        * Neither the name of The Linux Foundation nor the names of its
          contributors may be used to endorse or promote products derived
          from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define LOGI(...) fprintf(stderr, "I:" __VA_ARGS__)
#define MSS_NODE "/dev/subsys_modem"
#define WCNSS_NODE "/dev/subsys_wcnss"

int main() {

    LOGI("INITIALISING SUB SYSTEMS\n");

    while(1) {

#ifdef SOC_HAS_MODEM
        /* open mss node */
        LOGI("opening mss node\n");
        if(-1 == open(MSS_NODE, O_RDONLY)) {
            LOGI("open failed: %s\n", strerror(errno));
            return 1;
        }
#endif

#ifdef SOC_HAS_WCNSS
        /* open wcnss node */
        LOGI("opening wcnss node\n");
        if(-1 == open(WCNSS_NODE, O_RDONLY)) {
            LOGI("open failed: %s\n", strerror(errno));
            return 1;
        }
#endif

        break;
    }

#ifdef SLEEP_INDEFINITE
    /* no timeout */
    int ret = select(0, NULL, NULL, NULL, NULL);
    if (ret < 0) {
        LOGI("Exit from select w/ errno %s", strerror(errno));
    }
#else
    sleep(100);
#endif

    return 1;
}
