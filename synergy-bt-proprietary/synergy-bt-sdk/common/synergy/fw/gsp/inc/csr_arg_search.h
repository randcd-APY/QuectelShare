#ifndef CSR_ARG_SEARCH_H__
#define CSR_ARG_SEARCH_H__
/*****************************************************************************

Copyright (c) 2010-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************

    NAME
        CsrArgSearchInit

    DESCRIPTION
        Prepare the CsrArgSearch function by installing the command line
        arguments to search.

    PARAMETERS
        argc - Number of command line arguments.
        argv - Array of command line arguments.

*******************************************************************************/
void CsrArgSearchInit(CsrUint32 argc, CsrCharString *argv[]);


/*******************************************************************************

    NAME
        CsrArgSearchValidate

    DESCRIPTION
        Validate that the installed command line arguments are well formed and
        appropriate for use with CsrArgSearch. To be considered well formed,
        all values (i.e. non-parameters) in the command line arguments must be
        preceded by a parameter.

    RETURNS
        TRUE if the installed command line arguments are well formed, and
        FALSE otherwise.

*******************************************************************************/
CsrBool CsrArgSearchValidate(void);


/*******************************************************************************

    NAME
        CsrArgSearch

    DESCRIPTION
        General purpose helper function for doing incremental search in command
        line arguments. CsrArgSearchInit must be called first to install the
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

        static void exampleFunction(CsrUint32 argc, CsrCharString *argv[])
        {
            CsrUint32 argi = 1;
            CsrCharString *parameter;
            CsrCharString *value;

            CsrArgSearchInit(argc, argv);

            while (CsrArgSearch(&argi, "--prefix", &parameter, &value) != NULL)
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
CsrCharString *CsrArgSearch(CsrUint32 *argi, const CsrCharString *prefix,
    CsrCharString **parameter, CsrCharString **value);

#ifdef __cplusplus
}
#endif

#endif
