/* 
 * Copyright (c) 2016-2018  Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

#ifndef _QAPI_FIRMWARE_UPGRADE_EXT_H_
#define _QAPI_FIRMWARE_UPGRADE_EXT_H_

/*----------------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------------*/
#include "qapi_types.h"
#include "qapi_status.h"
 /*----------------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------------*/ 

 /** @addtogroup qapi_Fw_Upgrade
@{ */
 
/**
 *  Definition used by the qapi_Fw_Upgrade() and qapi_Fw_Upgrade_done() APIs as a flag bit.
 *  The Fw_Upgrade and Fw_Upgrade_Done APIs automatically reboot if this flag bit is set.
 */
#define QAPI_FW_UPGRADE_FLAG_AUTO_REBOOT            (1<<0)

/**
 *  Definition used by the qapi_Fw_Upgrade() API as a flag.
 *  Fw_Upgrade copies files from an active image file system to a trial image file system 
 *  if this flag is set
 */
#define QAPI_FW_UPGRADE_FLAG_DUPLICATE_ACTIVE_FS    (1<<1) 
 
/**
 *  Definition used by the qapi_Fw_Upgrade() API as a flag.
 *  When Fw_Upgrade copies files from an active image file system to a trial image file system,
 *  Fw_Upgrade overwrites files if the files exist at the trial image file system if this flag is not set.
 *  Fw_Upgrade does not copy files if the files exist at the trial image file system if this flag is set.
 */
#define QAPI_FW_UPGRADE_FLAG_DUPLICATE_KEEP_TRIAL_FS    (1<<2) 
 
/** @name FWD Bit Definition
 *  Definition used by the qapi_Fw_Upgrade_Get_Active_FWD() API as a return 
 *  to indicate the FWD bit type.
@{ */
#define QAPI_FW_UPGRADE_FWD_BIT_GOLDEN   (0)
#define QAPI_FW_UPGRADE_FWD_BIT_CURRENT  (1)
#define QAPI_FW_UPGRADE_FWD_BIT_TRIAL    (2)
/** @} */ 
/** @name FWD Boot Type Definition
 *  Definition used by the qapi_Fw_Upgrade_Get_Active_FWD() API as a return
 *  to indicate the FWD type for booting.
@{ */
#define QAPI_FW_UPGRADE_FWD_BOOT_TYPE_GOLDEN	(1<<QAPI_FW_UPGRADE_FWD_BIT_GOLDEN)
#define QAPI_FW_UPGRADE_FWD_BOOT_TYPE_CURRENT	(1<<QAPI_FW_UPGRADE_FWD_BIT_CURRENT)
#define QAPI_FW_UPGRADE_FWD_BOOT_TYPE_TRIAL	(1<<QAPI_FW_UPGRADE_FWD_BIT_TRIAL)
/** @} */
/*----------------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------------*/


/**
 *  Enumeration that represents the various states in Firmware Upgrade state machine.
 */ 
typedef enum qapi_Fw_Upgrade_State {
    QAPI_FW_UPGRADE_STATE_NOT_START_E = 0,
    /**< Firmware upgrade operation is not started. */
    QAPI_FW_UPGRADE_STATE_GET_TRIAL_INFO_E,
    /**< Get trial image information at flash. */
    QAPI_FW_UPGRADE_STATE_ERASE_FWD_E,
    /**< Erase FWD. */
    QAPI_FW_UPGRADE_STATE_ERASE_FLASH_E,
    /**< Erase the partition. */
    QAPI_FW_UPGRADE_STATE_ERASE_SECOND_FS_E,
    /**< Erase the second file system. */
    QAPI_FW_UPGRADE_STATE_PREPARE_FS_E,
    /**< Prepare the file system. */
    QAPI_FW_UPGRADE_STATE_ERASE_IMAGE_E,
    /**< Erase the subimage. */
    QAPI_FW_UPGRADE_STATE_PREPARE_CONNECT_E,
    /**< Prepare to connect to a remote firmware upgrade server. */
    QAPI_FW_UPGRADE_STATE_CONNECT_SERVER_E,
    /**< Connect to a remote firmware upgrade server. */
    QAPI_FW_UPGRADE_STATE_RESUME_SERVICE_E,
    /**< Resume the firmware upgrade service. */
    QAPI_FW_UPGRADE_STATE_RESUME_SERVER_E,
    /**< Resume connecting to the firmware upgrade server. */
    QAPI_FW_UPGRADE_STATE_RECEIVE_DATA_E,
    /**< Receive data from the remote firmware upgrade server. */
    QAPI_FW_UPGRADE_STATE_DISCONNECT_SERVER_E,
    /**< Disconnected from a remote firmware upgrade server. */
    QAPI_FW_UPGRADE_STATE_PROCESS_CONFIG_FILE_E,
    /**< Process firmware upgrade configuration file. */
    QAPI_FW_UPGRADE_STATE_PROCESS_IMAGE_E,
    /**< Process the image. */
    QAPI_FW_UPGRADE_STATE_DUPLICATE_IMAGES_E,
    /**< Duplicate the images from the current FWD. */
    QAPI_FW_UPGRADE_STATE_DUPLICATE_FS_E,
    /**< Duplicate the file system. */
	QAPI_FW_UPGRADE_STATE_FINISH_E,
    /**< Firmware upgrade is done. */
} /** @cond */ qapi_Fw_Upgrade_State_t /** @endcond */; 

/**
 *  Enumeration that represents the valid error codes that can be returned
 *            by the FW Upgrade APIs.  
 */
typedef enum qapi_Fw_Upgrade_Status{     
    QAPI_FW_UPGRADE_OK_E = 0,
    /**< Operation performed successfully. */
    QAPI_FW_UPGRADE_ERROR_E = 1,
    /**< Operation failed. */
    QAPI_FW_UPGRADE_ERR_INCOMPLETE_E = 1000,
    /**< Operation is incomplete. */
	QAPI_FW_UPGRADE_ERR_SESSION_IN_PROGRESS_E,
    QAPI_FW_UPGRADE_ERR_SESSION_NOT_START_E,
    /**< Firmware upgrade session is not started. */
    QAPI_FW_UPGRADE_ERR_SESSION_NOT_READY_FOR_SUSPEND_E,
    /**< Firmware upgrade session is not ready to enter the Suspend state. */
    QAPI_FW_UPGRADE_ERR_SESSION_NOT_SUSPEND_E,
    /**< Firmware upgrade session is not in the Suspend state. */
    QAPI_FW_UPGRADE_ERR_SESSION_RESUME_NOT_SUPPORT_E,
    /**< Firmware upgrade session resume is not supported by the plugin. */
    QAPI_FW_UPGRADE_ERR_SESSION_CANCELLED_E,
    /**< Firmware upgrade session was cancelled. */
    QAPI_FW_UPGRADE_ERR_SESSION_SUSPEND_E,
    /**< Firmware upgrade session was suspended. */
    QAPI_FW_UPGRADE_ERR_INTERFACE_NAME_TOO_LONG_E,
    /**< Interface name is too long. */
    QAPI_FW_UPGRADE_ERR_URL_TOO_LONG_E,
    /**< URL is too long. */
    QAPI_FW_UPGRADE_ERR_FLASH_NOT_SUPPORT_FW_UPGRADE_E,
	/**< Not supported firmware upgrade. */
    QAPI_FW_UPGRADE_ERR_FLASH_INIT_TIMEOUT_E,
    /**< Flash initialization timeout. */
    QAPI_FW_UPGRADE_ERR_FLASH_READ_FAIL_E,
    /**< Flash read failure. */
    QAPI_FW_UPGRADE_ERR_FLASH_WRITE_FAIL_E,
    /**< Flash write failure. */
    QAPI_FW_UPGRADE_ERR_FLASH_ERASE_FAIL_E,
    /**< Flash erase failure. */
    QAPI_FW_UPGRADE_ERR_FLASH_NOT_ENOUGH_SPACE_E,
    /**< Not enough free space in flash. */    
    QAPI_FW_UPGRADE_ERR_FLASH_CREATE_PARTITION_E,
    /**< Partition creation failure. */
    QAPI_FW_UPGRADE_ERR_FLASH_IMAGE_NOT_FOUND_E,
    /**< Partition image was not found. */
    QAPI_FW_UPGRADE_ERR_FLASH_ERASE_PARTITION_E,
    /**< Partition erase failure. */
    QAPI_FW_UPGRADE_ERR_FLASH_WRITE_PARTITION_E,
    /**< Partition write failure. */
    QAPI_FW_UPGRADE_ERR_IMAGE_NOT_FOUND_E, 
    /**< Image not found failure. */
    QAPI_FW_UPGRADE_ERR_IMAGE_DOWNLOAD_FAIL_E, 
    /**< Image download failure. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_IMAGE_CHECKSUM_E,
    /**< Incorrect image checksum failure. */    
    QAPI_FW_UPGRADE_ERR_SERVER_RSP_TIMEOUT_E,
    /**< Server communication timeout. */
    QAPI_FW_UPGRADE_ERR_INVALID_FILENAME_E,
    /**< Image file name is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_IMAGE_HDR_E,
    /**< Firmware upgrade image header is invalid. */
    QAPI_FW_UPGRADE_ERR_INSUFFICIENT_MEMORY_E,
    /**< Not enough memory. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_SIGNATURE_E,
    /**< Firmware upgrade image signature is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRCT_VERSION_E,
    /**< Firmware upgrade image version is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_NUM_IMAGES_E,
    /**< Firmware upgrade image number of images is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_IMAGE_LENGTH_E,
    /**< Firmware upgrade image length is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_HASH_TYPE_E,
    /**< Firmware upgrade image hash type is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_IMAGE_ID_E, 
    /**< Firmware upgrade image ID is invalid. */
    QAPI_FW_UPGRADE_ERR_BATTERY_LEVEL_TOO_LOW_E,
    /**< Battery level is too low. */
    QAPI_FW_UPGRADE_ERR_CRYPTO_FAIL_E,
    /**< Crypto check failure. */
    QAPI_FW_UPGRADE_ERR_PLUGIN_ENTRY_EMPTY_E,  
    /**< Firmware upgrade plugin callback is empty. */
    QAPI_FW_UPGRADE_ERR_TRIAL_IS_RUNNING_E,
    /**< Trial image is running */
    QAPI_FW_UPGRADE_ERR_FILE_NOT_FOUND_E,
    /**< File was not found. */    
    QAPI_FW_UPGRADE_ERR_FILE_OPEN_ERROR_E,
    /**< Open file failure. */    
    QAPI_FW_UPGRADE_ERR_FILE_NAME_TOO_LONG_E,
    /**< File name is too long. */    
    QAPI_FW_UPGRADE_ERR_FILE_WRITE_ERROR_E,
    /**< Write file failure. */    
    QAPI_FW_UPGRADE_ERR_MOUNT_FILE_SYSTEM_ERROR_E,
    /**< Mount file system failure. */
    QAPI_FW_UPGRADE_ERR_CREATE_THREAD_ERROR_E,
    /**< Firmware upgrade create thread failure */
    QAPI_FW_UPGRADE_ERR_PRESERVE_LAST_FAILED_E,
} /** @cond */ qapi_Fw_Upgrade_Status_Code_t /** @endcond */;

/** @} */ /* end_addtogroup qapi_Fw_Upgrade */ 

/** @addtogroup qapi_build_info
@{ */
/**
 * Data structure used by application to get build information.
 */
typedef struct {
    uint32_t qapi_Version_Number;
    /**< qapi version number */
    uint32_t crm_Build_Number;
    /**< CRM build number */
} qapi_FW_Ver_t;
/** @} */

/*----------------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------------*/
 
/* Miscellaneous Firmware Upgrade APIs */
/** @addtogroup qapi_Fw_Upgrade
@{ */
 
/**
 * @brief Gets the FWD index number that is current running.
 *
 * @details This is for the FWD that was selected by the bootloaders and is currently in use.
 *
 * @param[out] fwd_boot_type    Type of FWD used for booting.
 *
 * @param[out] valid_fwd        Information about which FWDs are present (1 bit per FWD).
 *  
 *
 * @return
 * The active FWD number.
 */
uint8_t qapi_Fw_Upgrade_Get_Active_FWD(uint32_t *fwd_boot_type, uint32_t *valid_fwd);


/**
 * @brief Initiates a device reboot
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */

qapi_Status_t qapi_Fw_Upgrade_Reboot_System(void);

/**
 * Activates or invalidates the trial image. The application calls this API after it has verified that
 * the image is valid or invalid. The criteria for image validity is defined by the application.
 *
 * @param[in] result        Result: \n
                            1: The image is valid; set trial image to active \n
 *                          0: The image is invalid; invalidate trial image  
 * @param[in] flags         Flags (bit0): \n
                            1: The device reboots after activating or invalidating the trial image \n
 *                          0: The device does not reboot after activating or invalidating the trial image \n
 *                          @note If the reboot_flag is set, the device will reboot and there is no return.
 *
 * @return
 * On success, QAPI_FW_UPGRADE_OK_E is returned. \n 
 * On error, QAPI_FW_UPGRADE_ERROR_E is returned.
 */
qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Done(uint32_t result, uint32_t flags);

/**
 * Starts a firmware upgrade session from host.
 *
 * @param[in] flags         Flags with bits defined for a firmware upgrade. See the qapi_Fw_Upgrade flag for a definition.
 *
 * @return
 *  On success, QAPI_FW_UPGRADE_OK_E is returned. \n 
 *  On error, error code defined by enum #qapi_Fw_Upgrade_Status is returned.
 */
qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Host_Init(uint32_t flags);

/**
 * Stop a firmware upgrade session which triggered by host.
 *
 * @return
 *  On success, QAPI_FW_UPGRADE_OK_E is returned. \n 
 *  On error, error code defined by enum #qapi_Fw_Upgrade_Status is returned.
 */
qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Host_Deinit(void);


/**
 * Pass buffer with len to firmware upgrade session which triggered by host.
 *
 * @return
 *  On success, QAPI_FW_UPGRADE_OK_E is returned. \n 
 *  On error, error code defined by enum #qapi_Fw_Upgrade_Status is returned.
 */
qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Host_Write(char *buffer, int32_t len);

/**
 * Get firmware upgrade session state.
 *
 * @return
 *   state defined by #qapi_Fw_Upgrade_State is returned.
 */
qapi_Fw_Upgrade_State_t qapi_Fw_Upgrade_Get_State(void);

/**
 * Get firmware upgrade session status.
 *
 * @return
 *  status code defined by #qapi_Fw_Upgrade_Status is returned.
 */
qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Get_Status(void);

/** @} */ /* end_addtogroup qapi_Fw_Upgrade */ 

/** @addtogroup qapi_build_info
@{ */
/**
 * This API allows user to retrieve version information from system. \n
 *
 * @param[out]    ver          Value retrieved from system.
 *
 * @return        Zero -- Requested parameter retrieved from the system. \n
 *                Non-Zero value -- Parameter retrieval failed.
 *  
 * @dependencies          None.
 */
uint32_t qapi_Get_FW_Ver(qapi_FW_Ver_t *ver);
/** @} */

#endif /* _QAPI_FIRMWARE_UPGRADE_EXT_H_ */
