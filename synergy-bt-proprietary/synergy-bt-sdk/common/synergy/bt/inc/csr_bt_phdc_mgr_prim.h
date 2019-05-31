#ifndef CSR_BT_PHDC_MGR_PRIM_H__
#define CSR_BT_PHDC_MGR_PRIM_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_types.h"
#include "csr_bt_result.h"
#include "csr_bt_profiles.h"
#include "l2cap_prim.h"
#include "csr_bt_hdp_prim.h"


#ifdef __cplusplus
extern "C" {
#endif

/* search_string="CsrBtPhdcMgrPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim   CsrBtPhdcMgrPrim;

/* Masks to be used during device search */
#define CSR_BT_PHDC_MGR_GET_DEVICE_MASK_BLOOD_PRESSURE      (1)
#define CSR_BT_PHDC_MGR_GET_DEVICE_MASK_WEIGHT_SCALE        (2)
#define CSR_BT_PHDC_MGR_GET_DEVICE_MASK_PEDOMETER           (4)
#define CSR_BT_PHDC_MGR_GET_DEVICE_MASK_COMPOSITION         (8)
#define CSR_BT_PHDC_MGR_GET_DEVICE_MASK_BODY_TEMP           (16)

/* Place holder for 20601 MDC_ATTR_UNIT_CODE */
#define CSR_BT_PHDC_MGR_DIM_UNIT_CODE_KILO_G                (1731)
#define CSR_BT_PHDC_MGR_DIM_UNIT_CODE_LB                    (1760)
#define CSR_BT_PHDC_MGR_DIM_UNIT_CODE_CENTI_M               (1297)
#define CSR_BT_PHDC_MGR_DIM_UNIT_CODE_INCH                  (1376)
#define CSR_BT_PHDC_MGR_DIM_UNIT_CODE_KG_PER_M_SQ           (1952)
#define CSR_BT_PHDC_MGR_DIM_UNIT_CODE_BEAT_PER_MIN          (2720
#define CSR_BT_PHDC_MGR_DIM_UNIT_CODE_KILO_PASCAL           (3843)
#define CSR_BT_PHDC_MGR_DIM_UNIT_CODE_MMHG                  (3872)

/* Place holder for 20601 Object Infrastructure (MDC_PART_OBJ) 
   Used to define a template for optmised byte transfer in 
   Attribute Value MAP.*/
#define CSR_BT_PHDC_MGR_ATTR_NU_VAL_OBS_SIMP                (2646)
#define CSR_BT_PHDC_MFR_ATTR_TIME_STAMP_ABS                 (2448)

typedef CsrUint16 CsrBtPhdcScanReportType;
/* Place holder for 20601 scan report type */
#define CSR_BT_PHDC_MGR_SCAN_REPORT_TYPE_FIXED              (3357)  /* 0x0d1d */
#define CSR_BT_PHDC_MGR_SCAN_REPORT_TYPE_VAR                (3358)  /* 0x0d1e */
#define CSR_BT_PHDC_MGR_SCAN_REPORT_TYPE_MP_FIXED           (3359)  /* 0x0d1f */
#define CSR_BT_PHDC_MGR_SCAN_REPORT_TYPE_MP_VAR             (3360)  /* 0x0d20 */

/*********************************************************************************
* From Medical supervisory control and data acquisition (MDC_PART_SCADA)
**********************************************************************************/
/* objectType */
#define CSR_BT_PHDC_MGR_BP_OBJ_PULS_RATE                         (18474) /* MDC_PULS_RATE_NON_INV */
#define CSR_BT_PHDC_MGR_BP_OBJ_PRESS_BLD                         (18948) /* MDC_PRESS_BLD_NONINV*/
#define CSR_BT_PHDC_MGR_WS_OBJ_LEN_BODY_ACTUAL                   (57668) /* MDC_LEN_BODY_ACTUAL*/
#define CSR_BT_PHDC_MGR_WS_OBJ_BMI                               (57680) /* MDC_RATIO_MASS_BODY_LEN_SQ*/
#define CSR_BT_PHDC_MGR_WS_OBJ_MASS_BODY_ACTUAL                  (57664) /* MDC_MASS_BODY_ACTUAL*/
/* objectType specific */
#define CSR_BT_PHDC_MGR_BP_OBJ_PRESS_BLD_SYS                     (18949) /*MDC_PRESS_BLD_NONINV_SYS*/
#define CSR_BT_PHDC_MGR_BP_OBJ_PRESS_BLD_DIA                     (18950) /*MDC_PRESS_BLD_NONINV_DIA*/
#define CSR_BT_PHDC_MGR_BP_OBJ_PRESS_BLD_MEAN                    (18951) /*MDC_PRESS_BLD_NONINV_MEAN*/

#define CSR_BT_PHDC_MGR_PERSON_ID_NA                            (0xFFFF)

/* Result Supplier 
  *  PHDC       - result code points for protocol and local errors
  */
typedef CsrUint16 CsrBtPhdcMgrResult;

#define CSR_BT_PHDC_MGR_RESULT_CODE_SUCCESS                                   (0)

/* PHDC_AARE error codes for the CSR_BT_SUPPLIER_PHDC supplier */
#define CSR_BT_PHDC_MGR_RESULT_CODE_AARE_RESULT_ACCEPTED                      (0)
#define CSR_BT_PHDC_MGR_RESULT_CODE_AARE_REJECTED_PERMANENT                   (1)
#define CSR_BT_PHDC_MGR_RESULT_CODE_AARE_REJECTED_TRANSIENT                   (2)
#define CSR_BT_PHDC_MGR_RESULT_CODE_AARE_ACCEPTED_UNKNOWKN_CONFIG             (3)
#define CSR_BT_PHDC_MGR_RESULT_CODE_AARE_REJECTED_NO_COMMON_PROTOCOL          (4)
#define CSR_BT_PHDC_MGR_RESULT_CODE_AARE_REJECTED_NO_COMMON_PARAMETER         (5)
#define CSR_BT_PHDC_MGR_RESULT_CODE_AARE_REJECTED_UNKNOWN                     (6)
#define CSR_BT_PHDC_MGR_RESULT_CODE_AARE_REJECTED_UNAUTHORIZED                (7)
#define CSR_BT_PHDC_MGR_RESULT_CODE_AARE_REJECTED_UNSUPPORTED_ASSOC_VERSION   (8)
#define CSR_BT_PHDC_MGR_RESULT_CODE_TO_ASSOC_EXPIRY                           (9)

/* PHDC_CONFIG error codes for the CSR_BT_SUPPLIER_PHDC supplier */
#define CSR_BT_PHDC_MGR_RESULT_CODE_CONFIG_ACCEPTED_CONFIG                    (10)
#define CSR_BT_PHDC_MGR_RESULT_CODE_CONFIG_UNSUPPORTED_CONFIG                 (11)
#define CSR_BT_PHDC_MGR_RESULT_CODE_CONFIG_STANDARD_CONFIG_UNKNOWN            (12)

/* PHDC_ABORT error codes for the CSR_BT_SUPPLIER_PHDC supplier */
#define CSR_BT_PHDC_MGR_RESULT_CODE_ABORT_REASON_UNDEFINED                    (13)
#define CSR_BT_PHDC_MGR_RESULT_CODE_ABORT_REASON_BUFFER_OVERFLOW              (14)
#define CSR_BT_PHDC_MGR_RESULT_CODE_ABORT_REASON_RESPONSE_TMEOUT              (15)
#define CSR_BT_PHDC_MGR_RESULT_CODE_ABORT_REASON_CONFIGURATION_TMEOUT         (16)

/* PHDC_RLRQ error codes for the CSR_BT_SUPPLIER_PHDC supplier */
#define CSR_BT_PHDC_MGR_RLRQ_REASON_NO_MORE_CONFIGURATIONS                    (17)
#define CSR_BT_PHDC_MGR_RLRQ_REASON_CONFIGURATION_CHANGED                     (18)
#define CSR_BT_PHDC_MGR_RLRQ_REASON_NORMAL                                    (19)

/* PHDC error codes for the CSR_BT_SUPPLIER_PHDC supplier */

#define CSR_BT_PHDC_MGR_RESULT_CODE_INVALID_DEVICE_CONFIG_ID                  (20)
#define CSR_BT_PHDC_MGR_RESULT_CODE_BD_ADDR_INVALID                           (21)
#define CSR_BT_PHDC_MGR_RESULT_CODE_INVALID_DATA_CH                           (22)

#define CSR_BT_PHDC_MGR_RESULT_CODE_ALREADY_CONFIGURED                        (24)
#define CSR_BT_PHDC_MGR_RESULT_CODE_NOT_REGISTERED                            (25)
#define CSR_BT_PHDC_MGR_RESULT_CODE_BUSY                                      (26)
#define CSR_BT_PHDC_MGR_RESULT_CODE_INVALID_OP                                (27)
#define CSR_BT_PHDC_MGR_RESULT_CODE_NO_RSP_FROM_PEER                          (28)
#define CSR_BT_PHDC_MGR_RESULT_CODE_RCVD_ABRT                                 (29)
#define CSR_BT_PHDC_MGR_RESULT_CODE_RESUME_FAILED                             (30)
#define CSR_BT_PHDC_MGR_RESULT_CODE_UNKNOWN                                   (31)

#define CSR_BT_PHDC_MGR_EXT_CONFIG_OBJ_OPCODE_NEW                             (0)
#define CSR_BT_PHDC_MGR_EXT_CONFIG_OBJ_OPCODE_UPDATE                          (1)


typedef struct CsrBtPhdcMgrAbsoluteTim 
{ 
    CsrUint8    century; 
    CsrUint8    year; 
    CsrUint8    month; 
    CsrUint8    day; 
    CsrUint8    hour; 
    CsrUint8    minute; 
    CsrUint8    second; 
    CsrUint8    secondFractions; 
} CsrBtPhdcMgrAbsoluteTim;

typedef struct
{
    CsrUint16   objHandle;
    CsrUint16   personId;    
    CsrBtPhdcMgrAbsoluteTim time;
    CsrUint16    dataLength;
    CsrUint16   *data;    
}CsrBtPhdcMgrDataNode;

#define CSR_BT_PHDC_MGR_DEVICE_CONFIG_ID_BPM    (0x02BC)
#define CSR_BT_PHDC_MGR_DEVICE_CONFIG_ID_WS     (0x05DC)

#define CSR_BT_PHDC_MGR_INVALID_VAL             (0xFF)

#define CSR_BT_PHDC_MGR_U32_NAN              (0x007FFFFF)
#define CSR_BT_PHDC_MGR_U32_RES              (0x00800000)
#define CSR_BT_PHDC_MGR_PLUS_INFINITY        (0x007FFFFE)
#define CSR_BT_PHDC_MGR_U32_NEG_INFINITY     (0x00800002)
#define CSR_BT_PHDC_MGR_U32_RFU              (0x00800001)

#define CSR_BT_PHDC_MGR_U16_NAN              (0x7FFF)
#define CSR_BT_PHDC_MGR_U16_RES              (0x0800)
#define CSR_BT_PHDC_MGR_U16_NEG_INFINITY     (0x0802)
#define CSR_BT_PHDC_MGR_U16_RFU              (0x0801)

/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/

#define CSR_BT_PHDC_MGR_PRIM_DOWNSTREAM_LOWEST      (0x0000)

#define CSR_BT_PHDC_MGR_CONFIGURE_ENDPOINT_REQ      ((CsrBtPhdcMgrPrim) (0x0000 + CSR_BT_PHDC_MGR_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_CONFIGURE_ENDPOINT_RES      ((CsrBtPhdcMgrPrim) (0x0001 + CSR_BT_PHDC_MGR_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_ACCEPT_BT_LINK_RES          ((CsrBtPhdcMgrPrim) (0x0002 + CSR_BT_PHDC_MGR_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_ACCEPT_ASSOCIATE_RES        ((CsrBtPhdcMgrPrim) (0x0003 + CSR_BT_PHDC_MGR_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_DISASSOCIATE_REQ            ((CsrBtPhdcMgrPrim) (0x0004 + CSR_BT_PHDC_MGR_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_GET_DEVICE_CAPAB_REQ        ((CsrBtPhdcMgrPrim) (0x0005 + CSR_BT_PHDC_MGR_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_SETUP_BT_LINK_REQ           ((CsrBtPhdcMgrPrim) (0x0006 + CSR_BT_PHDC_MGR_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_UNREGISTER_REQ              ((CsrBtPhdcMgrPrim) (0x0008 + CSR_BT_PHDC_MGR_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_PHDC_MGR_PRIM_DOWNSTREAM_HIGHEST     ((CsrUint16) (0x0008 + CSR_BT_PHDC_MGR_PRIM_DOWNSTREAM_LOWEST))


#define CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST        (0x0100 + CSR_PRIM_UPSTREAM)

#define CSR_BT_PHDC_MGR_CONFIGURE_ENDPOINT_CFM      ((CsrBtPhdcMgrPrim) (0x0000 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_CONFIGURE_ENDPOINT_IND      ((CsrBtPhdcMgrPrim) (0x0002 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_ACCEPT_BT_LINK_IND          ((CsrBtPhdcMgrPrim) (0x0003 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_ASSOCIATE_COMPLETE_IND      ((CsrBtPhdcMgrPrim) (0x0004 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_ACCEPT_ASSOCIATE_IND        ((CsrBtPhdcMgrPrim) (0x0005 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_EXT_CONFIG_OBJ_IND          ((CsrBtPhdcMgrPrim) (0x0006 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_DISASSOCIATE_CFM            ((CsrBtPhdcMgrPrim) (0x0007 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_DISASSOCIATE_IND            ((CsrBtPhdcMgrPrim) (0x0008 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_DS_IND                      ((CsrBtPhdcMgrPrim) (0x000B + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_BPM_BP_IND                  ((CsrBtPhdcMgrPrim) (0x000C + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_BPM_PULSE_IND               ((CsrBtPhdcMgrPrim) (0x000D + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_WS_WEIGHT_IND               ((CsrBtPhdcMgrPrim) (0x000E + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_WS_HEIGHT_IND               ((CsrBtPhdcMgrPrim) (0x000F + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_WS_BMI_IND                  ((CsrBtPhdcMgrPrim) (0x0010 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_GET_DEVICE_CAPAB_IND        ((CsrBtPhdcMgrPrim) (0x0011 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_GET_DEVICE_CAPAB_CFM        ((CsrBtPhdcMgrPrim) (0x0012 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_SETUP_BT_LINK_CFM           ((CsrBtPhdcMgrPrim) (0x0013 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_UNREGISTER_CFM              ((CsrBtPhdcMgrPrim) (0x0016 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_MGR_BT_LINK_REL_IND             ((CsrBtPhdcMgrPrim) (0x0017 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_PHDC_MGR_PRIM_UPSTREAM_HIGHEST       ((CsrUint16) (0x0017 + CSR_BT_PHDC_MGR_PRIM_UPSTREAM_LOWEST))

/*******************************************************************************
 * Primitive signal type definitions
 *******************************************************************************/

typedef struct /* CsrBtHdpRegisterReq */
{
    CsrBtPhdcMgrPrim    type;                           /* primitive type */
    CsrSchedQid         qId;                            /* handle to the application application */     
    dm_security_level_t secLevel;                       /* security level of the device being activated */  
    CsrCharString       *serviceName;                   /* name of the service ...upto 200 characters */
    CsrCharString       *serviceDescription;            /* brief description of the service upto 200 characters */
    CsrCharString       *providerName;
    CsrTime             sniffTimeOut;                   /* Time (in ms) the MCL must be idle before requesting sniff */
    CsrUint8            numOfMdep;                      /* number of End Points */
    CsrUint8            supportedProcedures;            /* byte mask - MCAP procedures supported by HDP */    
    CsrUint8            numOfActiveDeviceConnections;   /* Number of devices can be connected simultaneously */
}CsrBtPhdcMgrConfigureEndpointReq;

typedef struct 
{
    CsrBtPhdcMgrPrim    type;   
    CsrBtMdepId         mdepId;  
}CsrBtPhdcMgrConfigureEndpointInd;

typedef struct 
{
    CsrBtPhdcMgrPrim    type;   
    CsrBtMdepId         mdepId;
    CsrBtMdepDataType   datatype;
    CsrBtMdepRole       role;
    CsrUtf8String       *description;
    CsrBool             reuseMdepId;    /* Indicates if the device data specialisation should be multiplexed on the mdepid */
}CsrBtPhdcMgrConfigureEndpointRes;


typedef struct 
{
    CsrBtPhdcMgrPrim    type;   
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
}CsrBtPhdcMgrConfigureEndpointCfm;

typedef struct 
{
    CsrBtPhdcMgrPrim    type;
    CsrBtDeviceAddr     deviceAddr;
    CsrUint8            mdepDataTypeMask;    /*Bit Mask of the Device Specializations */    
}CsrBtPhdcMgrGetDeviceCapabReq;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrBtDeviceAddr     deviceAddr;
    CsrUint32           psmIdentifier;
    CsrCharString       *serviceName;
    CsrCharString       *providerName;
    CsrUint8            supportedFeatureListLength;    
    CsrBtHdpMdep        *supportedFeatureList;
}CsrBtPhdcMgrGetDeviceCapabInd;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrBtDeviceAddr     deviceAddr;   
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
}CsrBtPhdcMgrGetDeviceCapabCfm;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrUint32           psmIdentifier;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtMdepId         mdepId;
    CsrUint16           mdepDataType;
    CsrUint16           maxPacketLength;
    
} CsrBtPhdcMgrSetupBtLinkReq;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrUint32           psmIdentifier;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtMdepId         mdepId;
    CsrUint16           maxPacketLength;
    CsrUint32           dataChannelId;  
    CsrBtResultCode     resultCode;  
    CsrBtSupplier       resultSupplier;     

} CsrBtPhdcMgrSetupBtLinkCfm;


typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrBtDeviceAddr     deviceAddr;
} CsrBtPhdcMgrAcceptBtLinkInd;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrBtDeviceAddr     deviceAddr;
    CsrBool             accept;
    CsrUint16           maxPacketLength; 
} CsrBtPhdcMgrAcceptBtLinkRes;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrUint16           devConfigId;
    CsrUint8            systemId[8];
    CsrBtDeviceAddr     deviceAddr;
}CsrBtPhdcMgrAcceptAssociateInd;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrBtDeviceAddr     deviceAddr;
    CsrUint8            systemId[8];
    CsrBool             accept;
} CsrBtPhdcMgrAcceptAssociateRes;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrUint32           psmIdentifier;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtMdepId         mdepId;           
    CsrUint16           mdepDataType;
    CsrUint16           maxPacketLength;
    CsrUint32           dataChannelId;  
    CsrBtPhdcMgrResult  resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtPhdcMgrAssociateCompleteInd;


typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrUint16           devConfigId;
    CsrBtDeviceAddr     deviceAddr;
    CsrUint16           objHandle;
    CsrUint16           opCode;    
    CsrUint16           dataLength;
    CsrUint8            *data; 
}CsrBtPhdcMgrExtConfigObjInd;


typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrUint32           dataChannelId;
    CsrUint8            rlrqReason;
} CsrBtPhdcMgrDisassociateReq;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrUint32           dataChannelId;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtPhdcMgrResult  resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtPhdcMgrDisassociateCfm;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrUint32           dataChannelId;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtPhdcMgrResult  resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtPhdcMgrDisassociateInd;

typedef struct
{
    CsrBtPhdcMgrPrim        type;
    CsrUint32               dataChannelId;
    CsrUint16               personId;    
    CsrUint16               unitCode;    
    CsrBtPhdcMgrAbsoluteTim time;
    CsrUint16               systolicPressure;
    CsrUint16               diastolicPressure;
    CsrUint16               meanArterialPressure;

}CsrBtPhdcMgrBpmBpInd;

typedef struct
{
    CsrBtPhdcMgrPrim        type;
    CsrUint32               dataChannelId;
    CsrUint16               personId;
    CsrUint16               unitCode;    
    CsrBtPhdcMgrAbsoluteTim time;
    CsrUint16               pulse;
}CsrBtPhdcMgrBpmPulseInd;

typedef struct
{
    CsrBtPhdcMgrPrim        type;
    CsrUint32               dataChannelId;
    CsrUint16               personId;
    CsrUint16               unitCode;    
    CsrBtPhdcMgrAbsoluteTim time;
    CsrUint32               weight;
}CsrBtPhdcMgrWsWeightInd;

typedef struct
{
    CsrBtPhdcMgrPrim        type;
    CsrUint32               dataChannelId;
    CsrUint16               personId;
    CsrUint16               unitCode;    
    CsrBtPhdcMgrAbsoluteTim time;
    CsrUint16               height;
}CsrBtPhdcMgrWsHeightInd;

typedef struct
{
    CsrBtPhdcMgrPrim        type;
    CsrUint32               dataChannelId;
    CsrUint16               personId;
    CsrUint16               unitCode;    
    CsrBtPhdcMgrAbsoluteTim time;
    CsrUint16               bmi;
}CsrBtPhdcMgrWsBmiInd;

typedef struct
{   
    CsrBtPhdcMgrPrim        type;
    CsrUint32               dataChannelId;
    CsrUint16               objHandle;
    CsrUint16               personId;  
    CsrBtPhdcScanReportType scanReportType;   
    CsrUint16               dataLength;
    CsrUint8                *data;
}CsrBtPhdcMgrDsInd;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrUint32           dataChannelId;
}CsrBtPhdcMgrBtLinkRelInd;


typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrSchedQid         qId;  
} CsrBtPhdcMgrUnregisterReq;

typedef struct
{
    CsrBtPhdcMgrPrim    type;
    CsrSchedQid         qId;      
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtPhdcMgrUnregisterCfm;


#ifdef __cplusplus
}
#endif

#endif

