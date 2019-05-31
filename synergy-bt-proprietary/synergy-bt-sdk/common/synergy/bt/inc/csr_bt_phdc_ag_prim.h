#ifndef CSR_BT_PHDC_AG_PRIM_H__
#define CSR_BT_PHDC_AG_PRIM_H__

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

/* search_string="CsrBtPhdcAgPrim" */
/* conversion_rule="UPPERCASE_START_AND_REMOVE_UNDERSCORES" */

typedef CsrPrim   CsrBtPhdcAgPrim;

/* Masks to be used during device search */
#define CSR_BT_PHDC_AG_GET_DEVICE_MASK_BLOOD_PRESSURE      (1)
#define CSR_BT_PHDC_AG_GET_DEVICE_MASK_WEIGHT_SCALE        (2)
#define CSR_BT_PHDC_AG_GET_DEVICE_MASK_PEDOMETER           (4)
#define CSR_BT_PHDC_AG_GET_DEVICE_MASK_COMPOSITION         (8)
#define CSR_BT_PHDC_AG_GET_DEVICE_MASK_BODY_TEMP           (16)

#define CSR_BT_PHDC_AG_STD_DEVICE_CONFIG_WS                (0x05DC)
#define CSR_BT_PHDC_AG_STD_DEVICE_CONFIG_BP                (0x02BC)

/* Place holder for 20601 MDC_ATTR_UNIT_CODE */
#define CSR_BT_PHDC_AG_DIM_UNIT_CODE_KILO_G                (1731)
#define CSR_BT_PHDC_AG_DIM_UNIT_CODE_LB                    (1760)
#define CSR_BT_PHDC_AG_DIM_UNIT_CODE_CENTI_M               (1297)
#define CSR_BT_PHDC_AG_DIM_UNIT_CODE_INCH                  (1376)
#define CSR_BT_PHDC_AG_DIM_UNIT_CODE_KG_PER_M_SQ           (1952)
#define CSR_BT_PHDC_AG_DIM_UNIT_CODE_BEAT_PER_MIN          (2720
#define CSR_BT_PHDC_AG_DIM_UNIT_CODE_KILO_PASCAL           (3843)
#define CSR_BT_PHDC_AG_DIM_UNIT_CODE_MMHG                  (3872)

/* Place holder for 20601 Object Infrastructure (MDC_PART_OBJ)
   Used to define a template for optmised byte transfer in
   Attribute Value MAP.*/
#define CSR_BT_PHDC_AG_ATTR_NU_VAL_OBS_SIMP                (2646)
#define CSR_BT_PHDC_AG_ATTR_TIME_STAMP_ABS                 (2448)

typedef CsrUint16 CsrBtPhdcAgScanReportType;
/* Place holder for 20601 scan report type */
#define CSR_BT_PHDC_AG_SCAN_REPORT_TYPE_FIXED              (3357)
#define CSR_BT_PHDC_AG_SCAN_REPORT_TYPE_VAR                (3358)
#define CSR_BT_PHDC_AG_SCAN_REPORT_TYPE_MP_FIXED           (3359)
#define CSR_BT_PHDC_AG_SCAN_REPORT_TYPE_MP_VAR             (3360)

/*********************************************************************************
* From Medical supervisory control and data acquisition (MDC_PART_SCADA)
**********************************************************************************/
/* objectType */
#define CSR_BT_BP_OBJ_PULS_RATE                             (18474) /* 0x482A MDC_PULS_RATE_NON_INV */
#define CSR_BT_BP_OBJ_PRESS_BLD                             (18948) /* 0x4A04 MDC_PRESS_BLD_NONINV*/
#define CSR_BT_WS_OBJ_LEN_BODY_ACTUAL                       (57668) /* 0xE144 MDC_LEN_BODY_ACTUAL*/
#define CSR_BT_WS_OBJ_BMI                                   (57680) /* 0xE150 MDC_RATIO_MASS_BODY_LEN_SQ*/
#define CSR_BT_WS_OBJ_MASS_BODY_ACTUAL                      (57664) /* 0xE140 MDC_MASS_BODY_ACTUAL*/
/* objectType specific */
#define CSR_BT_BP_OBJ_PRESS_BLD_SYS                         (18949) /* 0x4A05 MDC_PRESS_BLD_NONINV_SYS*/
#define CSR_BT_BP_OBJ_PRESS_BLD_DIA                         (18950) /* 0x4A06 MDC_PRESS_BLD_NONINV_DIA*/
#define CSR_BT_BP_OBJ_PRESS_BLD_MEAN                        (18951) /* 0x4A07 MDC_PRESS_BLD_NONINV_MEAN*/

#define CSR_BT_PHDC_AG_PERSON_ID_NA                         (0xFFFF)

/* Result Supplier
  *  PHDC_AG       - result code points for protocol and local errors
  */
typedef CsrUint16 CsrBtPhdcAgResult;

/* rlrqReason codes used in DisassociateReq */
#define CSR_BT_RLRQ_REASON_NO_MORE_CONFIGURATIONS                           (17)
#define CSR_BT_RLRQ_REASON_CONFIGURATION_CHANGED                            (18)
#define CSR_BT_RLRQ_REASON_NORMAL                                           (19)


/* result codes for CSR_BT_SUPPLIER_PHDC_AG supplier */
#define CSR_BT_PHDC_AG_RESULT_CODE_SUCCESS                                  (0)

/* PHDC_AG_AARE result codes for the CSR_BT_SUPPLIER_PHDC_AG supplier */
#define CSR_BT_PHDC_AG_RESULT_CODE_AARE_RESULT_ACCEPTED                     (0)
#define CSR_BT_PHDC_AG_RESULT_CODE_AARE_REJECTED_PERMANENT                  (1)
#define CSR_BT_PHDC_AG_RESULT_CODE_AARE_REJECTED_TRANSIENT                  (2)
#define CSR_BT_PHDC_AG_RESULT_CODE_AARE_ACCEPTED_UNKNOWKN_CONFIG            (3)
#define CSR_BT_PHDC_AG_RESULT_CODE_AARE_REJECTED_NO_COMMON_PROTOCOL         (4)
#define CSR_BT_PHDC_AG_RESULT_CODE_AARE_REJECTED_NO_COMMON_PARAMETER        (5)
#define CSR_BT_PHDC_AG_RESULT_CODE_AARE_REJECTED_UNKNOWN                    (6)
#define CSR_BT_PHDC_AG_RESULT_CODE_AARE_REJECTED_UNAUTHORIZED               (7)
#define CSR_BT_PHDC_AG_RESULT_CODE_AARE_REJECTED_UNSUPPORTED_ASSOC_VERSION  (8)
#define CSR_BT_PHDC_AG_RESULT_CODE_TO_ASSOC_EXPIRY                          (9)

/* PHDC_AG_CONFIG result codes for the CSR_BT_SUPPLIER_PHDC_AG supplier */
#define CSR_BT_PHDC_AG_RESULT_CODE_CONFIG_ACCEPTED_CONFIG                   (10)
#define CSR_BT_PHDC_AG_RESULT_CODE_CONFIG_UNSUPPORTED_CONFIG                (11)
#define CSR_BT_PHDC_AG_RESULT_CODE_CONFIG_STANDARD_CONFIG_UNKNOWN           (12)

/* PHDC_AG_ABORT result codes for the CSR_BT_SUPPLIER_PHDC_AG supplier */
#define CSR_BT_PHDC_AG_RESULT_CODE_ABORT_REASON_UNDEFINED                   (13)
#define CSR_BT_PHDC_AG_RESULT_CODE_ABORT_REASON_BUFFER_OVERFLOW             (14)
#define CSR_BT_PHDC_AG_RESULT_CODE_ABORT_REASON_RESPONSE_TMEOUT             (15)
#define CSR_BT_PHDC_AG_RESULT_CODE_ABORT_REASON_CONFIGURATION_TMEOUT        (16)

/* PHDC_AG result codes for the CSR_BT_SUPPLIER_PHDC_AG supplier */
#define CSR_BT_PHDC_AG_RESULT_CODE_INVALID_DEVICE_CONFIG_ID                 (20)
#define CSR_BT_PHDC_AG_RESULT_CODE_BD_ADDR_INVALID                          (21)
#define CSR_BT_PHDC_AG_RESULT_CODE_INVALID_DATA_CH                          (22)

#define CSR_BT_PHDC_AG_RESULT_CODE_ALREADY_CONFIGURED                       (24)
#define CSR_BT_PHDC_AG_RESULT_CODE_SEND_NEXT_MEASUREMENT                    (25)
#define CSR_BT_PHDC_AG_RESULT_CODE_FAILURE                                  (26)

#define CSR_BT_PHDC_AG_RESULT_CODE_FAILURE_NO_MORE_CONFIG                   (27) /* Result code CSR_BT_PHDC_AG_RESULT_CODE_FAILURE_NO_MORE_CONFIG  is used when PHDC manager sends a
                                                                                    Release Request (RLRQ) apdu with reason "no-more-configurations"
                                                                                    Applicaitions supporting text based user interface, upon receiving this error code should display message
                                                                                    as "Thank you for choosing Continua certified personal health products. Even though this device has been
                                                                                    Continua certified, this model or the data is not intended for use in this solution. Please see your user
                                                                                    manual for more detail".
                                                                                    Applications not supporting text based user interface, through alternative methods (Ex : Visual Indication)
                                                                                    message should be conveyed
                                                                                    */
#define CSR_BT_PHDC_AG_RESULT_CODE_BUSY                                     (28) /* Result code CSR_BT_PHDC_AG_RESULT_CODE_BUSY  is used when PHDC agent has not received confirmation for
                                                                                    previous measurement sent and Application has sent the CSR_BT_PHDC_DATA_REQ  for next measurement */
#define CSR_BT_PHDC_AG_RESULT_CODE_INVALID_OBJECT_HANDLE                    (29)

/* */
#define CSR_BT_PHDC_AG_ATTR_DATA_FORMAT_NU_VAL_OBS_BASIC        (0x0100)
#define CSR_BT_PHDC_AG_ATTR_DATA_FORMAT_NU_CMPD_VAL_OBS_BASIC   (0x0200)
#define CSR_BT_PHDC_AG_ATTR_DATA_FORMAT_NU_VAL_OBS_SIMP         (0x0400)
#define CSR_BT_PHDC_AG_ATTR_DATA_FORMAT_NU_CMPD_VAL_OBS_SIMP    (0x0800)
#define CSR_BT_PHDC_AG_ATTR_DATA_FORMAT_NU_VAL_OBS              (0x1000)
#define CSR_BT_PHDC_AG_ATTR_DATA_FORMAT_NU_CMPD_VAL_OBS         (0x2000)
#define CSR_BT_PHDC_AG_ATTR_DATA_FORMAT_ABSOLUTE_TIME           (0x0001)

typedef struct CsrBtPhdcAgAbsoluteTime
{
    CsrUint8    century;
    CsrUint8    year;
    CsrUint8    month;
    CsrUint8    day;
    CsrUint8    hour;
    CsrUint8    minute;
    CsrUint8    second;
    CsrUint8    secondFractions;
} CsrBtPhdcAgAbsoluteTime;

typedef struct
{
    /* objHandle should be set based on measurements of objects being sent.
       For weighing scale standard configuration
         - To report measurements of "weight" objHandle should be set to 1.
       For Blood pressure monitor standard configuration,
        - To report measurements of "systolic, diastolic, MAP" objHandle should be set to 1
        - To report measurements of "pulse rate" objHandle should be set to 2
   */
    CsrUint16               objHandle;
    CsrUint16               personId;
    CsrBtPhdcAgAbsoluteTime time;
    CsrUint16               dataLength;
    CsrUint16               *data;
}CsrBtPhdcAgDataNode;

#define CSR_BT_PHDC_AG_INVALID_VAL         (0xFF)

#define CSR_BT_PHDC_AG_MAX_TX_MTU_SIZE     (896)

typedef CsrUint16 CsrBtPhdcAgTransmitFlagType;

#define CSR_BT_PHDC_TRANSMIT_FLAG_START         (0)
#define CSR_BT_PHDC_TRANSMIT_FLAG_CONTINUE      (1)
#define CSR_BT_PHDC_TRANSMIT_FLAG_FINAL         (2)


/*******************************************************************************
 * Primitive definitions
 *******************************************************************************/

#define CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST      (0x0000)

#define CSR_BT_PHDC_AG_CONFIGURE_ENDPOINT_REQ      ((CsrBtPhdcAgPrim) (0x0000 + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_CONFIGURE_ENDPOINT_RES      ((CsrBtPhdcAgPrim) (0x0001 + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_CONFIGURE_MDS_REQ           ((CsrBtPhdcAgPrim) (0x0002 + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_DIM_CONFIGURE_REQ           ((CsrBtPhdcAgPrim) (0x0003 + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_DIM_OBJECT_RES              ((CsrBtPhdcAgPrim) (0x0004 + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_DIM_ATTRIB_RES              ((CsrBtPhdcAgPrim) (0x0005 + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_GET_DEVICE_CAPAB_REQ        ((CsrBtPhdcAgPrim) (0x0006 + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_ASSOCIATE_REQ               ((CsrBtPhdcAgPrim) (0x0007 + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_ACCEPT_BT_LINK_RES          ((CsrBtPhdcAgPrim) (0x0008 + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_DISASSOCIATE_REQ            ((CsrBtPhdcAgPrim) (0x0009 + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_SUSPEND_REQ                 ((CsrBtPhdcAgPrim) (0x000A + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_RESUME_REQ                  ((CsrBtPhdcAgPrim) (0x000B + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_CHANGE_DIM_UNIT_CODE_REQ    ((CsrBtPhdcAgPrim) (0x000C + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_DATA_REQ                    ((CsrBtPhdcAgPrim) (0x000D + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_UNREGISTER_REQ              ((CsrBtPhdcAgPrim) (0x000E + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_HIGHEST     ((CsrUint16) (0x000E + CSR_BT_PHDC_AG_PRIM_DOWNSTREAM_LOWEST))

#define CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST        (0x0100 + CSR_PRIM_UPSTREAM)

#define CSR_BT_PHDC_AG_CONFIGURE_ENDPOINT_CFM      ((CsrBtPhdcAgPrim) (0x0000 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_CONFIGURE_ENDPOINT_IND      ((CsrBtPhdcAgPrim) (0x0001 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_DIM_CONFIGURE_CFM           ((CsrBtPhdcAgPrim) (0x0002 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_DIM_OBJECT_IND              ((CsrBtPhdcAgPrim) (0x0003 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_DIM_ATTRIB_IND              ((CsrBtPhdcAgPrim) (0x0004 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_GET_DEVICE_CAPAB_IND        ((CsrBtPhdcAgPrim) (0x0005 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_GET_DEVICE_CAPAB_CFM        ((CsrBtPhdcAgPrim) (0x0006 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_ACCEPT_BT_LINK_IND          ((CsrBtPhdcAgPrim) (0x0007 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_ASSOCIATE_CFM               ((CsrBtPhdcAgPrim) (0x0008 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_DISASSOCIATE_CFM            ((CsrBtPhdcAgPrim) (0x0009 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_DISASSOCIATE_IND            ((CsrBtPhdcAgPrim) (0x000A + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_SUSPEND_CFM                 ((CsrBtPhdcAgPrim) (0x000B + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_SUSPEND_IND                 ((CsrBtPhdcAgPrim) (0x000C + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_CHANGE_DIM_UNIT_CODE_CFM    ((CsrBtPhdcAgPrim) (0x000D + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_DATA_CFM                    ((CsrBtPhdcAgPrim) (0x000E + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_RESUME_CFM                  ((CsrBtPhdcAgPrim) (0x000F + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_RESUME_IND                  ((CsrBtPhdcAgPrim) (0x0010 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_SETUP_BT_LINK_IND           ((CsrBtPhdcAgPrim) (0x0011 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_UNREGISTER_CFM              ((CsrBtPhdcAgPrim) (0x0012 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))
#define CSR_BT_PHDC_AG_BT_LINK_REL_IND             ((CsrBtPhdcAgPrim) (0x0013 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))

#define CSR_BT_PHDC_AG_PRIM_UPSTREAM_HIGHEST       ((CsrUint16) (0x0013 + CSR_BT_PHDC_AG_PRIM_UPSTREAM_LOWEST))

/*******************************************************************************
 * Primitive signal type definitions
 *******************************************************************************/

typedef struct
{
    CsrBtPhdcAgPrim     type;                           /* primitive type */
    CsrSchedQid         qId;                            /* handle to the application application */
    dm_security_level_t secLevel;                       /* security level of the device being activated */
    CsrCharString       *serviceName;                   /* name of the service ...upto 200 characters */
    CsrCharString       *serviceDescription;            /* brief description of the service upto 200 characters */
    CsrCharString       *providerName;
    CsrTime             sniffTimeOut;                   /* Time (in ms) the MCL must be idle before requesting sniff */
    CsrUint8            numOfMdep;                      /* number of End Points */
    CsrUint8            supportedProcedures;            /* byte mask - MCAP procedures supported by HDP */
}CsrBtPhdcAgConfigureEndpointReq;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrBtMdepId         mdepId;
}CsrBtPhdcAgConfigureEndpointInd;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrBtMdepId         mdepId;
    CsrBtMdepDataType   datatype;
    CsrBtMdepRole       role;
    CsrUtf8String       *description;
    CsrBool             reuseMdepId;    /* Indicates if the device data specialisation should be multiplexed on the mdepid */
}CsrBtPhdcAgConfigureEndpointRes;


typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
}CsrBtPhdcAgConfigureEndpointCfm;


typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrSchedQid         qId;
} CsrBtPhdcAgUnregisterReq;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrSchedQid         qId;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtPhdcAgUnregisterCfm;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    /*Bitmap indicating the supported device specializations
      Bit 8 Bit 7   Bit 6   Bit 5   Bit 4   Bit 3   Bit 2   Bit 1
                                                    BPM     Weighing Scale
      Setting of particular bit indicates support of corresponding device specialization.
      Unused bits shall be set unset.
      At least one bit should be set indicating support of one device specialization*/
    CsrUint8            supportedDeviceSpecializations; /* MDC_PART_INFRA */
    CsrCharString       *manufacturer;
    CsrCharString       *modelNumber;
    CsrUint8            systemId[8];
    CsrUint16           devConfigId;
    CsrCharString       *serialNumber;
    CsrCharString       *fwVersion;
    CsrBtPhdcAgAbsoluteTime absolutetime;
}CsrBtPhdcAgConfigureMdsReq;
/* NOTE : All strings fields mentioned in the structure CsrBtPhdcAgMdsConfigureReq should have even lenght */


typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint16           devConfigId;
    CsrUint16           objCount;
}CsrBtPhdcAgDimConfigureReq;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint16           objIndex;
}CsrBtPhdcAgDimObjectInd;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint16           objIndex;
    CsrUint16           objClass;           /* Object class - refer to IEEE 11073-20601 for details of object class */
    CsrUint16           objHandle;          /* Object Handle as given by the application to uniquely identify the objects.
                                               Handle value '0' is reserved for MDS object. Refer to IEEE 11073-20601 for details */
    CsrUint16           dataValueLength;    /* The number of elements in dataValue array given below. Basically, this number is
                                               same as the number of elements in the structure which the data format is representing.*/
    CsrUint16           *dataValue;         /* 'data_value' array takes basic data format values given below for each
                                               element in the structure that it represents. */
                                            /*The value for supported basic data types are */
                                            /* #define ATTR_DATA_FORMAT_NU_VAL_OBS_BASIC 0x0100 (for uint16) */
                                            /* #define ATTR_DATA_FORMAT_NU_CMPD_VAL_OBS_BASIC 0x0200 (for array of uint16),Lower byte
                                            * indicates number of array elements.
                                            * ex : 0x0202, indicates array of two elements of size uint16*/
                                            /* #define ATTR_DATA_FORMAT_NU_VAL_OBS_SIMP 0x0400 (for uint32) */
                                            /* #define ATTR_DATA_FORMAT_NU_CMPD_VAL_OBS_SIMP 0x0800 (for array of uint32),Lower byte
                                            * indicates number of array elements.
                                            * ex : 0x0802, indicates array of two elements of size uint32*/
                                            /* #define ATTR_DATA_FORMAT_NU_VAL_OBS 0x1000 (for NuObsValue) */
                                            /* #define ATTR_DATA_FORMAT_NU_CMPD_VAL_OBS 0x2000 (for array of NuObsValue),Lower byte
                                            * indicates number of array elements.
                                            * ex : 0x2002, indicates array of two elements of size NuObsValue*/
                                            /* #define ATTR_DATA_FORMAT_ABSOLUTE_TIME 0x0001 (for Absolute Time) */
                                            /* note Float_Type and SFloat_Type are represented by uint32 and uint16 basic
                                            data types respectively. */
    CsrUint16           numAttrib;  /* Number of attributes of the object */
}CsrBtPhdcAgDimObjectRes;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint16           objIndex;
    CsrUint16           attribIndex;
}CsrBtPhdcAgDimAttribInd;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint16           objIndex;
    CsrUint16           attribIndex;
    CsrUint16           attribId;           /*  Attribute identity. For standard attributes refer to IEEE 11073-20601 specification.
                                                Use attribute identity in the range 0xF040 to 0xF060 for vendor specific attributes. */
    CsrUint16           attribInfoLength;   /*  Attribute Information as given for standard attributes defined in IEEE 11073-20601
                                                specification  'attrInfo' is of type suitable for attribute identity. As an example, for attribute id
                                                'MDC_ATTR_TIME_ABS', type used should be 'AbsoluteTime'. Please refer to Table 2 (MDS attributes)
                                                and Table 5 (Metric attributes) of IEEE 11073-20601 for details on standard MDS attributes and Metric
                                                attributes respectively. */
    CsrUint8            *attribInfo;
} CsrBtPhdcAgDimAttribRes;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrBtPhdcAgResult   resultCode;
}CsrBtPhdcAgDimConfigureCfm;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrBtDeviceAddr     deviceAddr;
    CsrUint8            mdepDataTypeMask;    /*Bit Mask of the Device Specializations */
}CsrBtPhdcAgGetDeviceCapabReq;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrBtDeviceAddr     deviceAddr;
    CsrUint32           psmIdentifier;
    CsrCharString       *serviceName;
    CsrCharString       *providerName;
    CsrUint8            supportedFeatureListLength;
    CsrBtHdpMdep        *supportedFeatureList;
}CsrBtPhdcAgGetDeviceCapabInd;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
}CsrBtPhdcAgGetDeviceCapabCfm;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrBtDeviceAddr     deviceAddr;
} CsrBtPhdcAgAcceptBtLinkInd;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrBtDeviceAddr     deviceAddr;
    CsrBool             accept;
    CsrUint16           maxPacketLength;
} CsrBtPhdcAgAcceptBtLinkRes;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           psmIdentifier;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtMdepId         mdepId;
    CsrUint16           mdepDataType;
    CsrUint16           maxPacketLength;
    CsrUint16           devConfigId;
    CsrBool             release;
} CsrBtPhdcAgAssociateReq;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           psmIdentifier;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtMdepId         mdepId;
    CsrUint16           mdepDataType;
    CsrUint16           maxPacketLength;
    CsrUint32           dataChannelId;
    CsrBtPhdcAgResult   resultCode;
} CsrBtPhdcAgAssociateCfm;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           dataChannelId;
    CsrBtDeviceAddr     deviceAddr;
} CsrBtPhdcAgDisassociateInd;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           dataChannelId;
    CsrUint8            rlrqReason;
} CsrBtPhdcAgDisassociateReq;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           dataChannelId;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtPhdcAgResult   resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtPhdcAgDisassociateCfm;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           dataChannelId;
} CsrBtPhdcAgSuspendReq;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           dataChannelId;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtPhdcAgSuspendCfm;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           dataChannelId;
} CsrBtPhdcAgSuspendInd;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           dataChannelId;
} CsrBtPhdcAgResumeReq;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           dataChannelId;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtPhdcAgResumeCfm;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           dataChannelId;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtPhdcAgResumeInd;


typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           dataChannelId;
    CsrUint8            transmitFlag;
    CsrBtPhdcAgDataNode data;
}CsrBtPhdcAgDataReq;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint32           dataChannelId;
    CsrBtPhdcAgResult   resultCode;
}CsrBtPhdcAgDataCfm;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint16           objHandle;
    CsrUint16           unitCode;
} CsrBtPhdcAgChangeDimUnitCodeReq;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrUint16           objHandle;
    CsrBtPhdcAgResult   resultCode;
} CsrBtPhdcAgChangeDimUnitCodeCfm;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;
} CsrBtPhdcAgSetupBtLinkInd;

typedef struct
{
    CsrBtPhdcAgPrim     type;
    CsrBtDeviceAddr     deviceAddr;
    CsrBtResultCode     resultCode;
    CsrBtSupplier       resultSupplier;    
} CsrBtPhdcAgBtLinkRelInd;

#ifdef __cplusplus
}
#endif

#endif

