/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/
#ifndef _CONNX_ARG_SEARCH_H_
#define _CONNX_ARG_SEARCH_H_

#include "connx_common_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************

    NAME
        ConnxArgSearchInit

    DESCRIPTION
        Prepare the ConnxArgSearch function by installing the command line
        arguments to search.

    PARAMETERS
        argc - Number of command line arguments.
        argv - Array of command line arguments.

*******************************************************************************/
void ConnxArgSearchInit(uint32_t argc, char *argv[]);


/*******************************************************************************

    NAME
        ConnxArgSearchValidate

    DESCRIPTION
        Validate that the installed command line arguments are well formed and
        appropriate for use with ConnxArgSearch. To be considered well formed,
        all values (i.e. non-parameters) in the command line arguments must be
        preceded by a parameter.

    RETURNS
        TRUE if the installed command line arguments are well formed, and
        FALSE otherwise.

*******************************************************************************/
bool ConnxArgSearchValidate(void);


/*******************************************************************************

    NAME
        ConnxArgSearch

    DESCRIPTION
        General purpose helper function for doing incremental search in command
        line arguments. ConnxArgSearchInit must be called first to install the
        arguments to search.

    PARAMETERS
        argi - The index in the array to start the search at. When a match
               occurs it will be updated to the next index in the array to
               facilitate incremental searching. If this is set to NULL, the
               search will start at index 1, and the first parameter that match
               the prefix exactly (content and length) will be returned.
        prefix - A string to match the parameter prefix. Must start with "--".
                 Only parameters that start with this prefix will be returned.
                 If argi is NULL only the first parameter that match this prefix
                 exactly (content and length) will be returned.
        parameter - The first parameter found will be returned in this variable.
                    If nothing was found, this variable nor the value variable
                    will be modified. Specifying NULL is allowed.
        value - if the argument following the found parameter does not start
                with "--", it will be considered a value, and returned in this
                variable. If the following argument is a parameter (start with
                "--"), NULL will be written to this variable. Specifying NULL
                is allowed.

    RETURNS
        If a parameter is not found, NULL will be returned. The parameter and
        value variables will not be modified.

        If a parameter is found and the parameter has a value, the value will
        be returned, and the parameter and value variables will be updated to
        point at the parameter and value respectively.

        If a parameter is found but no value is present for that parameter, the
        parameter itself is returned, and the parameter variable is updated to
        point at the parameter and the value variable is set to NULL.

    EXAMPLE USAGE
        The following example will search through all given arguments and print
        out only those parameters that are prefixed with "--prefix".

        static void exampleFunction(uint32_t argc, char *argv[])
        {
            uint32_t argi = 1;
            char *parameter;
            char *value;

            ConnxArgSearchInit(argc, argv);

            while (ConnxArgSearch(&argi, "--prefix", &parameter, &value) != NULL)
            {
                if (value != NULL)
                {
                    Print("%s = %s\n", parameter, value);
                }
                else
                {
                    Print("%s\n", parameter);
                }
            }
        }

        For an example argument string:
        'executable --prefix-a -123 --bbb --prefix-b --xxx --prefix-c abcd'

        The example will print out:
        --prefix-a = -123
        --prefix-b
        --prefix-c = abcd

*******************************************************************************/
char *ConnxArgSearch(uint32_t *argi, const char *prefix,
    char **parameter, char **value);

#ifdef __cplusplus
}
#endif

#endif  /* _CONNX_ARG_SEARCH_H_ */