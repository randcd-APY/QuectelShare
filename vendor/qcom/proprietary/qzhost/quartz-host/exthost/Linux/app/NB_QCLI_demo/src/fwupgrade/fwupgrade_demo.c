/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include "malloc.h"
#include "string.h"

#include "qcli_api.h"
#include "qcli_util.h"
#include "qapi_firmware_upgrade_ext.h"
#include "qapi_timer.h"
#include "qsOSAL.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/

#define FW_UPGRADE_PRINTF_HANDLE  qcli_fw_upgrade_group

/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

QCLI_Group_Handle_t qcli_fw_upgrade_group;              /* Handle for our QCLI Command Group. */


/*-------------------------------------------------------------------------
 * Function Declarations
 *-----------------------------------------------------------------------*/
static QCLI_Command_Status_t Command_Display_FWD(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t Command_Fw_Upgrade_Info(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t Command_Done_Trial(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);
static QCLI_Command_Status_t Command_Fw_Upgrade_Fwupgrade(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List);

/* The following is the complete command list for the Firmware Upgrade demo. */
const QCLI_Command_t Fw_Upgrade_Command_List[] =
{
   /* cmd_function                     thread cmd_string usage_string           description */
   {Command_Display_FWD,               false, "fwd",    "",                     "Display FWD"},
   {Command_Fw_Upgrade_Info,           false, "info",   "",                     "state and status"},
   {Command_Done_Trial,                false, "trial",  "[0|1] [reboot flag]",  "Accept/Reject Trial FWD"},
   {Command_Fw_Upgrade_Fwupgrade,      false, "fwup",   "[file] [flasg]",       "start fw upgrade"},
};

const QCLI_Command_Group_t Fw_Upgrade_Command_Group =
{
    "FwUp",  /* Firmware Upgrade */
    sizeof(Fw_Upgrade_Command_List) / sizeof(QCLI_Command_t),
    Fw_Upgrade_Command_List,
};

/*-------------------------------------------------------------------------
 * Function Definitions
 *-----------------------------------------------------------------------*/

/* This function is used to register the Firmware Upgrade Command Group with QCLI   */
void Initialize_Firmware_Upgrade_Demo(void)
{
   /* Attempt to reqister the Command Groups with the qcli framework.*/
   FW_UPGRADE_PRINTF_HANDLE = QCLI_Register_Command_Group(NULL, &Fw_Upgrade_Command_Group);
   if(FW_UPGRADE_PRINTF_HANDLE)
   {
      QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "Firmware Upgrade Registered \n");
   }
}



/**
   @brief Un-Registers the HMI interface commands with QCLI.
*/
void Cleanup_Firmware_Upgrade_Demo(void)
{
   if(FW_UPGRADE_PRINTF_HANDLE)
   {
      /* Un-register the firmware upgrade Group.                        */
      /* * NOTE * This function will un-register all sub-groups.        */
      QCLI_Unregister_Command_Group(FW_UPGRADE_PRINTF_HANDLE);

      FW_UPGRADE_PRINTF_HANDLE = NULL;
   }
}
/*****************************************************************************************************************
*****************************************************************************************************************/

/**
   @brief This function processes the "FWD" command from the CLI.

*/
static QCLI_Command_Status_t Command_Display_FWD(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
   int32_t               Index;
   uint32_t              boot_type, fwd_present;

    //get active FWD
    Index = qapi_Fw_Upgrade_Get_Active_FWD(&boot_type, &fwd_present);
    QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "Active FWD: %s  index:%d, present:%d\n", (boot_type==QAPI_FW_UPGRADE_FWD_BOOT_TYPE_TRIAL)?"Trial":(boot_type==QAPI_FW_UPGRADE_FWD_BOOT_TYPE_CURRENT)?"Current":"Golden",Index, fwd_present);

    return(QCLI_STATUS_SUCCESS_E);
}
/**
   @brief This function processes the "Trial" command from the CLI.

*/
static QCLI_Command_Status_t Command_Done_Trial(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    if( (Parameter_Count != 2)  || (Parameter_List[0].Integer_Is_Valid == 0) || (Parameter_List[1].Integer_Is_Valid == 0) )
    {
        return QCLI_STATUS_USAGE_E;
    }

    if( (Parameter_List[0].Integer_Value > 1)  || (Parameter_List[1].Integer_Value > 1) )
    {
        QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "Invalid parameter\n");
        return QCLI_STATUS_USAGE_E;
    }

    if( Parameter_List[0].Integer_Value == 1 )
    {
        if( qapi_Fw_Upgrade_Done(1, Parameter_List[1].Integer_Value) != QAPI_FW_UPGRADE_OK_E )
        {
            QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "Fail to Accept Trial FWD\n");
            return QCLI_STATUS_ERROR_E;
        } else {
            QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "Success to Accept Trial FWD\n");
        }
    } else {
        if( qapi_Fw_Upgrade_Done(0, Parameter_List[1].Integer_Value) != QAPI_FW_UPGRADE_OK_E )
        {
            QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "Fail to Reject Trial FWD\n");
            return QCLI_STATUS_ERROR_E;
        } else {
            QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "Success to Reject Trial FWD\n");
        }
    }
    return QCLI_STATUS_SUCCESS_E;
}

/**
   @brief This function processes the "fwup" command from the CLI.

*/
static QCLI_Command_Status_t Command_Fw_Upgrade_Fwupgrade(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
#define BUF_SIZE 892  //note: need check qcli buf size (1024 now)
    qapi_Status_t status = QAPI_OK;
    qapi_Fw_Upgrade_Status_Code_t fwup_status;
    qapi_Fw_Upgrade_State_t fwup_state;
    char *file_name;    
    uint8_t *data = NULL;
    int fd = -1, data_size, len;
    uint32_t flags, stage, bytes_written;

    stage = 0;
    // check parameters
    if(Parameter_Count < 1)
    {
        QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "Invalid number of parameters\n");
        status = QAPI_ERR_INVALID_PARAM;
        goto fw_start_on_error;
    }

    /* set file name */
    file_name = (char *) Parameter_List[0].String_Value;
    
    /* set flags */
    if ( (Parameter_Count == 2) && (!Parameter_List[1].Integer_Is_Valid) ) 
    {
        QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "flag is not a valid integer\n");
        status = QAPI_ERR_INVALID_PARAM;
        goto fw_start_on_error;
    }
    flags = Parameter_List[1].Integer_Value;

    /* open image file */
    if( (fd = open(file_name, O_RDONLY )) == -1) 
    {
        QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "fail to open %s\n", file_name);
        goto fw_start_on_error;
    }

    /* allocate memory */
    data_size = BUF_SIZE;
    if( (data = (uint8_t *) malloc(data_size)) == NULL ) 
    {
        QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "ERROR: Memory allocation failed\n");
        goto fw_start_on_error;
    }

    /* Init firmware upgrade session */
    if( (fwup_status = qapi_Fw_Upgrade_Host_Init(flags)) != QAPI_FW_UPGRADE_OK_E )
    {
        QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "fail to init firmware upgrade: %d\n", fwup_status);
        goto fw_start_on_error;
    }
    
    stage = 1;
    bytes_written = 0;
    QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE,"\nstart");
    while( (len=read(fd, data, data_size)) != 0 )   /* read data from image file */
    {
        bytes_written += len;
        QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE,".");
        /* send image data to quartz */
        if( (fwup_status = qapi_Fw_Upgrade_Host_Write(data, len)) != QAPI_FW_UPGRADE_OK_E) 
        {
            QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "\nfail to write: %d\n", fwup_status);
            goto fw_start_on_error;
        }
    }

    QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE,"\nsend %s with %d bytes\n", file_name, (int)bytes_written);

    /* check firmware upgrade status */
    while( (fwup_status = qapi_Fw_Upgrade_Get_Status()) == QAPI_FW_UPGRADE_OK_E ) 
    {
        /* check firmware upgrade state */
        if( (fwup_state = qapi_Fw_Upgrade_Get_State()) == QAPI_FW_UPGRADE_STATE_FINISH_E) 
        {
            QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE,"Firmware Upgrade is done.\n");
            goto fw_start_on_error;
        }
        sleep(1);
    }
    QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE,"fail to do firmware upgrade: (state:%d, status:%d)\n", fwup_state, fwup_status );

fw_start_on_error:
    if( stage != 0 ) 
    {
        qapi_Fw_Upgrade_Host_Deinit();
    }
  
    if( data != NULL ) 
    {
        free(data);
    }

    if( fd >= 0 ) 
    {
        close(fd);
    }
   
    if( QAPI_OK != status ) 
    {
        return QCLI_STATUS_USAGE_E;
    }

    return QCLI_STATUS_SUCCESS_E;
}

/**
   @brief This function processes the "Info" command from the CLI.

*/
static QCLI_Command_Status_t Command_Fw_Upgrade_Info(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    QCLI_Printf(FW_UPGRADE_PRINTF_HANDLE, "state: %d, status:%d\n", qapi_Fw_Upgrade_Get_State(), qapi_Fw_Upgrade_Get_Status());
    return QCLI_STATUS_SUCCESS_E;
}
