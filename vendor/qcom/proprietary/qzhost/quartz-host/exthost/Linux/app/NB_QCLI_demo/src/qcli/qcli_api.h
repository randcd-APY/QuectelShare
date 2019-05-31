/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __QCLI_API_H__  // [
#define __QCLI_API_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_types.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   This enumeration represents the valid error codes that can be returned
   by the command functions.  They represent a success, a general error or
   a usage error.
*/
typedef enum
{
   QCLI_STATUS_SUCCESS_E,
   QCLI_STATUS_ERROR_E,
   QCLI_STATUS_USAGE_E
} QCLI_Command_Status_t;

/**
   This type represents a group handle.
*/
typedef void *QCLI_Group_Handle_t;

/**
   This structure contains the information for a single parameter entered
   into the command line.  It contains the string value (as entered), the
   integer value (if the string could be successfully converted) and a
   boolean flag which indicates if the integer value is valid.
*/
typedef struct QCLI_Parameter_s
{
   char    *String_Value;
   int32_t  Integer_Value;
   qbool_t  Integer_Is_Valid;
} QCLI_Parameter_t;

/**
   @brief Type which represents the format of a function which processes
          commands from the CLI.

   @param Parameter_Count indicates the number of parameters that were
          specified to the CLI for the function.
   @param Parameter_List is the list of parameters specified to the CLI
          for the function.

   @return
    - QCLI_STATUS_SUCCESS_E if the command executed successfully.
    - QCLI_STATUS_ERROR_E if the command encounted a general error. Note
      that the CLI currently doesn't take any action for this error.
    - QCLI_STATUS_USAGE_E indicates that the parameters passed to the CLI
      were not correct for the command.  When this error code is returned,
      the CLI will display the usage message for the command.
*/
typedef QCLI_Command_Status_t (*QCLI_Command_Function_t)(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

/**
   This structure represents the information for a single command in a
   command list.
*/
typedef struct QCLI_Command_s
{
   QCLI_Command_Function_t  Command_Function; /** The function that will be called when the command is executed from the CLI. */
   qbool_t                  Start_Thread;     /** A flag which indicates if the command should start on its own thread.       */
   const char              *Command_String;   /** The string representation of the function.                                  */
   const char              *Usage_String;     /** The usage string for the command.                                           */
   const char              *Description;      /** The description string for the commmand.                                    */
} QCLI_Command_t;

/**
   The following structure represents a command group that can be
   registered with the CLI.
*/
typedef struct QCLI_Command_Group_s
{
   const char           *Group_String;   /** The string representation of the group. */
   uint32_t              Command_Count; /** The number of commands in the group.    */
   const QCLI_Command_t *Command_List;   /** The list of commands for the group.     */
} QCLI_Command_Group_t;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function is used to register a command group with the CLI.

   @param Parent_Group is the group which this group should be registerd
          under as a subgroup.  If this parameter is NULL, then the group
          will be registered at the top level.
   @param Command_Group is the command group to be registered.  Note that
          this function assumes the command group information will be
          constant and simply stores a pointer to the data.  If the command
          group and its associated information is not constant, its memory
          MUST be retained until the command is unregistered.

   @return
    - THe handle for the group that was added.
    - NULL if there was an error registering the group.
*/
QCLI_Group_Handle_t QCLI_Register_Command_Group(QCLI_Group_Handle_t Parent_Group, const QCLI_Command_Group_t *Command_Group);

/**
   @brief This function is used to usregister a command group from the CLI.

   @param Group_Handle is the handle for the group to be unregistered.
          This will be the value returned form
          QCLI_Register_Command_Group() when the function was registered.
          Note that if the specified group has subgroups, they will be
          unregistred as well.
*/
void QCLI_Unregister_Command_Group(QCLI_Group_Handle_t Group_Handle);

/**
   @brief This function prints the prompt to the console.

   This provides a means to re-display the prompt after printing data to
   the console from an asynchronous function such as a callback or seperate
   command thread.
*/
void QCLI_Display_Prompt(void);

/**
   @brief This function prints a formated string to the CLI.

   Note that this function will also replace newline uint8_tacters ('\n') with
   the string specified by PAL_OUTPUT_END_OF_LINE_STRING.

   @param Group_Handle is the handle for the group associated with the
          message being printed.  This will be the value returned when the
          group was registered using QCLI_Regsiter_Command_Group().
   @param Format is the formatted string to be printed.
   @param ... is the variatic parameter for the format string.
*/
void QCLI_Printf(QCLI_Group_Handle_t Group_Handle, const char *format, ...);

#endif   // ] #ifndef __QCLI_API_H__

