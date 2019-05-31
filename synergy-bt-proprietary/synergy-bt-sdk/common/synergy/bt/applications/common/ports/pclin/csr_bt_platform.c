/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include "csr_unicode.h"
#include "csr_bt_platform.h"

/* ****************** Linux  ****************** */
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <asm/ioctls.h>

static struct termios orig_term;

void my_atexit(void)
{
    tcsetattr(0, TCSANOW, &orig_term);
}

int _kbhit(void)
{
    static const int STDIN = 0;
    static int initialized = 0;
    int bytesWaiting;

    if(!initialized)
    {
        struct termios term;

        /* Use termios to turn off line buffering */
        tcgetattr(STDIN, &orig_term);

        term = orig_term;
        term.c_lflag &= ~ICANON;
        term.c_lflag &= ~ECHO;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);

        atexit(my_atexit);
        initialized = 1;
    }

    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

int _getch(void)
{
    return(getchar());
}

void system_cls(void)
{
    system(CLS);
}
