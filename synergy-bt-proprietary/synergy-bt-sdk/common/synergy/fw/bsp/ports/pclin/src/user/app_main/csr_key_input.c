/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/
#include "csr_synergy.h"

#include <stdio.h>
#include <pthread.h>
#include <termios.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "csr_types.h"
#include "csr_macro.h"
#include "csr_pmem.h"
#include "csr_ui_keycode.h"
#include "csr_ui_lower.h"

#include "csr_sched_init.h"
#include "csr_arg_search.h"


#define retry_condition(err) ((err) == EAGAIN || (err) == EWOULDBLOCK)

static CsrBool initialised = FALSE;
static CsrBool useBlockMode = FALSE;

static int threadTerminatePipe[2];
static pthread_t thread;


/* [QTI] Add the function to check whether keyinput is in block or non-block mode. */
static CsrBool GetKeyInputBlockMode(void)
{
    CsrCharString *parameter, *value;

    if (CsrArgSearch(NULL, "--platform", &parameter, &value) && (value != NULL))
    {
        /* MTP9X45 is based on Linux kernel v3.10, which limit that keyinput can only run in non-block mode. */
        if (!CsrStrCmp(value, "MDM9640"))
            return FALSE;
    }

    return TRUE;
}

CsrUint16 handleAsciiKeyCodes(char key)
{
    switch (key)
    {
#if defined(USE_MDM_PLATFORM) || defined(USE_MSM_PLATFORM) || defined(USE_IMX_PLATFORM)
        case 10:    /* LF */
#endif
        case 13:    /* Enter (CR) */
            return CSR_UI_KEY_SK1;
        case 8:     /* Backspace (^H, BS) */
        case 127:   /* Backspace (^?, DEL) */
            return CSR_UI_KEY_BACK;
        default:
        {
            if ((key >= 0x20) && (key <= 0x7E))
            {
                return CSR_UI_KEY_ASCII(key);
            }
            break;
        }
    }

    return 0;
}

static void *threadFunction(void *nothing)
{
    static struct pollfd handles[2];
    CsrUint8 parseState = 0;
    CsrUint16 csrUiKey = 0;
    CsrUint8 isSkipEnter = 0;

    CSR_UNUSED(nothing);

    handles[0].fd = fileno(stdin);
    handles[0].events = POLLIN;
    handles[1].fd = threadTerminatePipe[0];
    handles[1].events = POLLIN;

    while (TRUE)
    {
        if ((poll(handles, 2, -1) < 0) || (handles[1].revents & POLLIN))
        {
            return NULL;
        }

        if (handles[0].revents & POLLIN)
        {
            char c = 0;

            /* [QTI] Poll stdin in non-block mode. */
            if (!useBlockMode)
            {
                ssize_t count = 0;
                useconds_t usec = 500000;

                count = read(fileno(stdin), &c, 1);

                if (count == 1)
                {
                    /* Debug only */
                    putchar(c);
                }
                else if (count < 0 && !retry_condition(errno))
                {
                    return NULL;
                }
                else
                {
                    /* Workaound busy-waiting for I/O. */
                    usleep(usec);
                    continue;
                }

                if (c == 'q' || c == 'Q')
                {
                    exit(0);
                }
            }
            else
            {
                if (read(fileno(stdin), &c, 1) < 0)
                {
                    return NULL;
                }

                if (c == 'q' || c == 'Q')
                {
                    exit(0);
                }
            }

            switch(c)
            {
                case 0x62:  /* fall through (b) */
                    csrUiKey = CSR_UI_KEY_BACK;
                    parseState = 0;
                    isSkipEnter = 1;
                    break;
                case 0x68:  /* stands for home key (h) */
                    csrUiKey = CSR_UI_KEY_SK1;
                    parseState = 0;
                    isSkipEnter = 1;
                    break;
                case 0x75:  /* stands for up key (u) */
                    csrUiKey = CSR_UI_KEY_UP;
                    parseState = 0;
                    isSkipEnter = 1;
                    break;
                case 0x64:  /* stands for down key(d) */
                    csrUiKey = CSR_UI_KEY_DOWN;
                    parseState = 0;
                    isSkipEnter = 1;
                    break;
                case 0x6C:  /* stands for left key (l) */
                    csrUiKey = CSR_UI_KEY_SK1;
                    parseState = 0;
                    isSkipEnter = 1;
                    break;
                case 0x72:  /* stands for right key (r) */
                    csrUiKey = CSR_UI_KEY_SK2;
                    parseState = 0;
                    isSkipEnter = 1;
                    break;
                case 0x0a: /* LF */
                case 0x0d: /* CR */
                    if (isSkipEnter == 1)
                    {
                        isSkipEnter = 0;
                        continue;
                    }
                /* fall through in case of LF/CR */
                default:
                    switch (parseState)
                    {
                        case 0:
                        {
                            if (c == 0x1B) /* ESC */
                            {
                                parseState = 1;
                            }
                            else if (c == 0x03) /* CTRL-C */
                            {
#ifdef ENABLE_SHUTDOWN
                                CsrSchedStop();
                                return NULL;
#else
                                exit(0);
#endif
                            }
                            else
                            {
                                csrUiKey = handleAsciiKeyCodes(c);
                            }
                            break;
                        }
                        case 1:
                        {
                            if (c == '[')
                            {
                                parseState = 2;
                            }
                            else if (c == 0x4F)
                            {
                                parseState = 4;
                            }
                            else
                            {
                                csrUiKey = 0;
                                parseState = 0;
                            }
                            break;
                        }
                        case 2:
                        {
                            if (c == 0x41)
                            {
                                csrUiKey = CSR_UI_KEY_UP;
                                parseState = 0;
                            }
                            else if (c == 0x42)
                            {
                                csrUiKey = CSR_UI_KEY_DOWN;
                                parseState = 0;
                            }
                            else if (c == 0x43)
                            {
                                csrUiKey = CSR_UI_KEY_RIGHT;

                                if (!useBlockMode)
                                {
                                    parseState = 5;
                                }
                                else
                                {
                                    if (csrUiKey == CSR_UI_KEY_LEFT)
                                    {
                                        csrUiKey = CSR_UI_KEY_BACK;
                                    }
                                    else
                                    {
                                        csrUiKey = CSR_UI_KEY_SK1;
                                    }
                                    parseState = 0;
                                }
                            }
                            else if (c == 0x44)
                            {
                                csrUiKey = CSR_UI_KEY_LEFT;

                                if (!useBlockMode)
                                {
                                    parseState = 5;
                                }
                                else
                                {

                                    if (csrUiKey == CSR_UI_KEY_LEFT)
                                    {
                                        csrUiKey = CSR_UI_KEY_BACK;
                                    }
                                    else
                                    {
                                        csrUiKey = CSR_UI_KEY_SK1;
                                    }
                                    parseState = 0;
                                }
                            }
                            else if (c == 0x31) /* Home */
                            {
                                csrUiKey = CSR_UI_KEY_SK1;
                                parseState = 3;
                            }
                            else if (c == 0x33) /* Delete */
                            {
                                csrUiKey = CSR_UI_KEY_DEL;
                                parseState = 3;
                            }
                            else if (c == 0x34) /* End */
                            {
                                csrUiKey = CSR_UI_KEY_BACK;
                                parseState = 3;
                            }
                            else if (c == 0x35) /* PgUp */
                            {
                                csrUiKey = CSR_UI_KEY_SK2;
                                parseState = 3;
                            }
                            else if (c == 0x36) /* PgDn */
                            {
                                csrUiKey = CSR_UI_KEY_DEL;
                                parseState = 3;
                            }
                            else
                            {
                                csrUiKey = 0;
                                parseState = 0;
                            }
                            break;
                        }
                        case 3:
                        {
                            if (c == '~')
                            {
                                parseState = 0;
                            }
                            else
                            {
                                csrUiKey = 0;
                                parseState = 0;
                            }
                            break;
                        }
                        case 4:
                        {
                            if (c == 0x48) /* Home */
                            {
                                csrUiKey = CSR_UI_KEY_SK1;
                                parseState = 0;
                            }
                            else if (c == 0x46) /* End */
                            {
                                csrUiKey = CSR_UI_KEY_BACK;
                                parseState = 0;
                            }
                            else
                            {
                                csrUiKey = 0;
                                parseState = 0;
                            }
                            break;
                        }
                        case 5: //This state is valid only when kernel version is lower than 3.18.0
                        {
                            if (c == 0x0a)  /* LF */
                            {
                                if (csrUiKey == CSR_UI_KEY_LEFT)
                                {
                                    csrUiKey = CSR_UI_KEY_BACK;
                                }
                                else
                                {
                                    csrUiKey = CSR_UI_KEY_SK1;
                                }
                            }

                            parseState = 0;
                            break;
                        }
                        default:
                            break;
                    }
            }

            if ((csrUiKey != 0) && (parseState == 0))
            {
                CsrUiKeyEvent(csrUiKey);
            }
        }
    }
}

static struct termios orig_term;

void CsrKeyInputActivate(void)
{
    useBlockMode = GetKeyInputBlockMode();

    if (!initialised)
    {
        if (!useBlockMode)
        {
            /* [QTI] Puzzled that synergy app is blocked infinitely in "tcsetattr".
               Or "tcsetattr" return the errno  25 (Inappropriate ioctl for device).
               So the workaround is used to set stdin as NONBLOCK. */
            int fd = fileno(stdin);
            int flags = 0;

            flags = fcntl(fd, F_GETFL);

            if (flags == -1)
            {
                return;
            }

            flags |= O_NONBLOCK;

            if (fcntl(fd, F_SETFL, flags))
            {
                return;
            }
        }
        else
        {
            struct termios raw_term;

            /* save terminal settings */
            tcgetattr(fileno(stdin), &orig_term);
            raw_term = orig_term;
            raw_term.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
            raw_term.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
            raw_term.c_cflag &= ~(CSIZE | PARENB);
            raw_term.c_cflag |= CS8;
            raw_term.c_cc[VMIN] = 0;
            raw_term.c_cc[VTIME] = 0;
            tcsetattr(fileno(stdin), TCSANOW, &raw_term);
        }

        if (pipe(threadTerminatePipe) == 0)
        {
            pthread_create(&thread, NULL, threadFunction, NULL);
            initialised = TRUE;
        }
        else
        {
            tcsetattr(fileno(stdin), TCSANOW, &orig_term);
        }
    }
}

void CsrKeyInputDeactivate(void)
{
    if (initialised)
    {
        tcsetattr(fileno(stdin), TCSANOW, &orig_term);
        if (write(threadTerminatePipe[1], "1", 1) > 0)
        {
            pthread_join(thread, NULL);
            close(threadTerminatePipe[0]);
            close(threadTerminatePipe[1]);
            initialised = FALSE;
        }
    }
}
