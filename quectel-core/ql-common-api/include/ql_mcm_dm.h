#ifndef __QL_MCM_DM_H__
#define __QL_MCM_DM_H__


#define QL_MAX_DMS_IMEI_LEN 32      /**  Max IMEI limit */
#define QL_MAX_DMS_MEID_LEN 32      /**  Max MEIID limit */
#define QL_MAX_DMS_HARDWARE_REV_LEN 256

typedef uint32 dm_client_handle_type;


typedef enum 
{
    E_QL_MCM_DM_AIRPLANE_MODE_UNKNOWN   = 0,    /**< Radio online. */
    E_QL_MCM_DM_AIRPLANE_MODE_ON        = 1,    /**< Radio power off or unknown. Airplane ON. */
    E_QL_MCM_DM_AIRPLANE_MODE_OFF       = 2,    /**< Radio online. Airplane OFF. */
    E_QL_MCM_DM_AIRPLANE_MODE_NA        = 3     /**< Radio Unvailable. */
}E_QL_MCM_DM_AIRPLANE_MODE_TYPE_T;

typedef struct 
 {
    uint8_t imei_valid; 
    char imei[QL_MAX_DMS_IMEI_LEN + 1];    /**<   imei.*/
    uint8_t meid_valid; 
    char meid[QL_MAX_DMS_MEID_LEN + 1];    /**<   meid.*/
}ql_dm_device_serial_numbers_t;

typedef struct 
 {
    char device_rev_id[QL_MAX_DMS_HARDWARE_REV_LEN + 1];    /**<   meid.*/
}ql_dm_device_rev_id_t;

typedef enum 
{
    E_QL_MCM_DM_RADIO_MODE_CHANGED_EVENT = 0,        /**<  pv_data = &E_QL_MCM_DM_AIRPLANE_MODE_TYPE_T  */
}E_QL_MCM_DM_NFY_MSG_ID_T;

typedef void (*QL_MCM_DM_RxIndMsgHandlerFunc_t)   
(    
    dm_client_handle_type       h_dm,
    E_QL_MCM_DM_NFY_MSG_ID_T    e_msg_id,
    void                        *pv_data,
    void                        *contextPtr    
);

E_QL_ERROR_CODE_T QL_MCM_DM_Client_Init(dm_client_handle_type  *ph_dm);

E_QL_ERROR_CODE_T QL_MCM_DM_AddRxIndMsgHandler(QL_MCM_DM_RxIndMsgHandlerFunc_t handlerPtr, void* contextPtr);

/* Stop getting coordinates */
E_QL_ERROR_CODE_T QL_MCM_DM_Client_Deinit(dm_client_handle_type h_dm);

/* Get airplane mode*/
E_QL_ERROR_CODE_T QL_MCM_DM_GetAirplaneMode
(
    dm_client_handle_type                   h_dm,
    E_QL_MCM_DM_AIRPLANE_MODE_TYPE_T        *pe_airplane_mode   ///< [OUT] Radio mode
);

/* Set airplane mode*/
E_QL_ERROR_CODE_T QL_MCM_DM_SetAirplaneMode
(
    dm_client_handle_type                   h_dm,
    E_QL_MCM_DM_AIRPLANE_MODE_TYPE_T        e_airplane_mode     ///< [IN] Airplane mode
);

/* Set indication on off when airplane mode changed */
E_QL_ERROR_CODE_T QL_MCM_DM_SetAirplaneModeChgInd
(
    dm_client_handle_type       h_dm,
    uint32_t                    ind_onoff  ///< [IN] 0: indication off, 1: on
);

/* Get serial numbers (IMEI MEID)*/
E_QL_ERROR_CODE_T QL_MCM_DM_GetSerialNumbers
(
    dm_client_handle_type       h_dm,
    ql_dm_device_serial_numbers_t  *serial_numbers
);

/* Get Firmware revisition identification */
E_QL_ERROR_CODE_T QL_MCM_DM_GetFirmware
(
    dm_client_handle_type       h_dm,
    ql_dm_device_rev_id_t      *hardware_rev
);


#endif // __QL_MCM_DM_H__

