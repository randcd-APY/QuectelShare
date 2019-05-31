/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "connx_arg_search.h"

static uint32_t localArgc = 0;
static char **localArgv = NULL;


void ConnxArgSearchInit(uint32_t argc, char *argv[])
{
    localArgc = argc;
    localArgv = argv;
}

bool ConnxArgSearchValidate(void)
{
    bool param = false;
    uint32_t argi;

    for (argi = 1; argi < localArgc; ++argi)
    {
        if (strncmp("--", localArgv[argi], 2) == 0)
        {
            param = true;
        }
        else
        {
            if (param)
            {
                param = false;
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}

char *ConnxArgSearch(uint32_t *argi, const char *prefix, char **parameter, char **value)
{
    bool exact = false;
    uint32_t index = 1;
    size_t prefixLen = strlen(prefix);

    if (argi == NULL)
    {
        exact = true;
        argi = &index;
    }

    for ( ; *argi < localArgc; ++*argi)
    {
        if ((exact && (strcmp(prefix, localArgv[*argi]) == 0)) ||
            (!exact && (strncmp(prefix, localArgv[*argi], prefixLen) == 0)))
        {
            if (parameter != NULL)
            {
                *parameter = localArgv[*argi];
            }

            ++*argi;

            if ((*argi < localArgc) && (strncmp("--", localArgv[*argi], 2) != 0))
            {
                if (value != NULL)
                {
                    *value = localArgv[*argi];
                }

                ++*argi;
                return localArgv[*argi - 1];
            }

            if (value != NULL)
            {
                *value = NULL;
            }

            return localArgv[*argi - 1];
        }
    }

    return NULL;
}
