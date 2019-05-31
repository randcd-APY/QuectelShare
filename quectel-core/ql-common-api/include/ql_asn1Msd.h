/** @file asn1Msd.h
 *
 * This file contains the declarations of the ASN1 MSD builder.
 *
 * Copyright (C) Sierra Wireless Inc.
 */
/*================================================================
  Copyright (c) 2018 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
  Quectel Wireless Solution Proprietary and Confidential.
  =================================================================*/
/*=================================================================

  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  WHEN             WHO         WHAT, WHERE, WHY
  ------------     -------     ---------------------------------------
  04/12/2018       laurence    Init.
  =================================================================*/

#ifndef _ASN1_MSD_INCLUDE_GUARD_
#define _ASN1_MSD_INCLUDE_GUARD_

#include <stdint.h>
#include <stdbool.h>

#ifndef NULL
#define NULL     0
#endif

//--------------------------------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
/**
 * Position N-1 and N-2 are delta positions.
 * Limits as defined in FprEN 15722:2014
 * These are the max and min values for the msd_VehicleLocationDelta_t members
 */
//--------------------------------------------------------------------------------------------------
#define ASN1_LATITUDE_DELTA_MAX  511
#define ASN1_LATITUDE_DELTA_MIN  -512
#define ASN1_LONGITUDE_DELTA_MAX  511
#define ASN1_LONGITUDE_DELTA_MIN  -512
#define ASN1_ECALL_MSD_VERSION_1 1
#define ASN1_ECALL_MSD_VERSION_2 2

//--------------------------------------------------------------------------------------------------
// Symbols and enums.
//--------------------------------------------------------------------------------------------------
typedef enum
{
    E_QL_ERROR_MSD_SUCCESS              = 0,
    E_QL_ERROR_MSD_GENERIC              = -(E_QL_ERROR_MSD_SUCCESS+1),     /**<  MSD Generic ERROR. */
    E_QL_ERROR_MSD_BADPARM              = -(E_QL_ERROR_MSD_SUCCESS+2),     /**<  MSD Bad parameter ERROR. */
    E_QL_ERROR_MSD_VERSION              = -(E_QL_ERROR_MSD_SUCCESS+3),     /**<  MSD VERSION ERROR. */
    E_QL_ERROR_MSD_ISOWMI               = -(E_QL_ERROR_MSD_SUCCESS+2),     /**<  MSD wmi ERROR. */
    E_QL_ERROR_MSD_ISOVDS               = -(E_QL_ERROR_MSD_SUCCESS+4),     /**<  MSD vds ERROR. */
    E_QL_ERROR_MSD_ISOVIS_MODELYEAR     = -(E_QL_ERROR_MSD_SUCCESS+5),     /**<  MSD model year ERROR. */
    E_QL_ERROR_MSD_ISOVIS_SEQPLAN       = -(E_QL_ERROR_MSD_SUCCESS+6),     /**<  MSD seplan ERROR. */
    E_QL_ERROR_MSD_LATITUDE             = -(E_QL_ERROR_MSD_SUCCESS+7),     /**<  MSD latitude ERROR. */
    E_QL_ERROR_MSD_LONGITUDE            = -(E_QL_ERROR_MSD_SUCCESS+8),     /**<  MSD longitude ERROR. */
    E_QL_ERROR_MSD_VEH_DIRECTION        = -(E_QL_ERROR_MSD_SUCCESS+9),     /**<  MSD vehDirection ERROR. */
    E_QL_ERROR_MSD_VEH_LAT_LOCATION1    = -(E_QL_ERROR_MSD_SUCCESS+10),    /**<  MSD recentVehLocationN1 latitude ERROR. */
    E_QL_ERROR_MSD_VEH_LON_LOCATION1    = -(E_QL_ERROR_MSD_SUCCESS+11),    /**<  MSD recentVehLocationN1 longitude ERROR. */
    E_QL_ERROR_MSD_VEH_LAT_LOCATION2    = -(E_QL_ERROR_MSD_SUCCESS+12),    /**<  MSD recentVehLocationN2 latitude ERROR. */
    E_QL_ERROR_MSD_VEH_LON_LOCATION2    = -(E_QL_ERROR_MSD_SUCCESS+13),    /**<  MSD recentVehLocationN2 longitude ERROR. */
    E_QL_ERROR_MSD_VEH_IDENTIFI_NUMBER  = -(E_QL_ERROR_MSD_SUCCESS+14),    /**<  MSD vehIdentification Number ERROR. */
    E_QL_ERROR_MSD_OPTINAL_DATA_GLONASS = -(E_QL_ERROR_MSD_SUCCESS+15),    /**<  MSD Optional Data For Era Glonass  ERROR. */
}
ql_msd_result_t;

//--------------------------------------------------------------------------------------------------
/**
 * Enumeration used to specify the type of vehicle.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    MSD_VEHICLE_PASSENGER_M1=1,  ///< Passenger vehicle (Class M1)
    MSD_VEHICLE_BUS_M2,          ///< Buses and coaches (Class M2)
    MSD_VEHICLE_BUS_M3,          ///< Buses and coaches (Class M3)
    MSD_VEHICLE_COMMERCIAL_N1,   ///< Light commercial vehicles (Class N1)
    MSD_VEHICLE_HEAVY_N2,        ///< Heavy duty vehicles (Class N2)
    MSD_VEHICLE_HEAVY_N3,        ///< Heavy duty vehicles (Class N3)
    MSD_VEHICLE_MOTORCYCLE_L1E,  ///< Motorcycles (Class L1e)
    MSD_VEHICLE_MOTORCYCLE_L2E,  ///< Motorcycles (Class L2e)
    MSD_VEHICLE_MOTORCYCLE_L3E,  ///< Motorcycles (Class L3e)
    MSD_VEHICLE_MOTORCYCLE_L4E,  ///< Motorcycles (Class L4e)
    MSD_VEHICLE_MOTORCYCLE_L5E,  ///< Motorcycles (Class L5e)
    MSD_VEHICLE_MOTORCYCLE_L6E,  ///< Motorcycles (Class L6e)
    MSD_VEHICLE_MOTORCYCLE_L7E,  ///< Motorcycles (Class L7e)
}
msd_VehicleType_t;

//--------------------------------------------------------------------------------------------------
/**
 * Enumeration used to specify the type of coordinate system.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    MSD_COORDINATE_SYSTEM_TYPE_ABSENT,   ///< Coordinate system type is absent
    MSD_COORDINATE_SYSTEM_TYPE_WGS84,    ///< Coordinate system type WGS-84
    MSD_COORDINATE_SYSTEM_TYPE_PZ90      ///< Coordinate system type PZ-90
}
msd_CoordinateSystemType_t;

typedef enum
{
    PA_ECALL_PAN_EUROPEAN,      //< PAN-European system.
    PA_ECALL_ERA_GLONASS        //< ERA-GLONASS system.
}ecall_system_std_t;
//--------------------------------------------------------------------------------------------------
// Data structures.
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
/**
 * Data structure describing the VIN (Vehicle Identification Number).
 *
 */
//--------------------------------------------------------------------------------------------------
typedef struct __attribute__ ((__packed__)) {
   char    isowmi[3];
   char    isovds[6];
   char    isovisModelyear[1];
   char    isovisSeqPlant[7];
} msd_Vin_t;

/* ControlType */
//--------------------------------------------------------------------------------------------------
/**
 * Data structure describing the main control elements of the MSD.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef struct {
   bool                 automaticActivation;
   bool                 testCall;
   bool                 positionCanBeTrusted;
   msd_VehicleType_t    vehType;
} msd_Control_t;

//--------------------------------------------------------------------------------------------------
/**
 * Data structure describing the vehicle propulsion storage type.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef struct {
   bool gasolineTankPresent;
   bool dieselTankPresent;
   bool compressedNaturalGas;
   bool liquidPropaneGas;
   bool electricEnergyStorage;
   bool hydrogenStorage;
   bool otherStorage;
} msd_VehiclePropulsionStorageType_t;

//--------------------------------------------------------------------------------------------------
/**
 * Data structure describing the last known vehicle location.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef struct {
   int32_t    latitude;
   int32_t    longitude;
} msd_VehicleLocation_t;

//--------------------------------------------------------------------------------------------------
/**
 * Data structure describing the location of the vehicle some time before the generation of the data
 * for the MSD message..
 *
 */
//--------------------------------------------------------------------------------------------------
typedef struct {
   int32_t    latitudeDelta;
   int32_t    longitudeDelta;
} msd_VehicleLocationDelta_t;

//--------------------------------------------------------------------------------------------------
/**
 * Data structure to gather optional data.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef struct {
   uint8_t  oidlen;
   uint8_t  dataLen;
   uint8_t* oid;
   uint8_t* data;
} msd_optionalData_t;

//--------------------------------------------------------------------------------------------------
/**
 * Data structure describing the MSD without the additional optional data.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef struct {
   uint8_t                             messageIdentifier;
   msd_Control_t                       control;
   msd_Vin_t                           vehIdentificationNumber;
   msd_VehiclePropulsionStorageType_t  vehPropulsionStorageType;
   uint32_t                            timestamp;
   msd_VehicleLocation_t               vehLocation;
   uint8_t                             vehDirection;
   /* Optional */
   bool                                recentVehLocationN1Pres;
   msd_VehicleLocationDelta_t          recentVehLocationN1;
   /* Optional */
   bool                                recentVehLocationN2Pres;
   msd_VehicleLocationDelta_t          recentVehLocationN2;
   /* Optional */
   bool                                numberOfPassengersPres;
   uint8_t                             numberOfPassengers;
} msd_Structure_t;

//--------------------------------------------------------------------------------------------------
/**
 * Data structure describing the MSD with the additional optional data.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef struct {
   msd_Structure_t    msdStruct;
   /* Optional */
   bool               optionalDataPres;
   msd_optionalData_t optionalData;
} msd_Message_t;

//--------------------------------------------------------------------------------------------------
/**
 * Data structure describing the MSD message.
 *
 */
//--------------------------------------------------------------------------------------------------
typedef struct {
    uint8_t       version;
    msd_Message_t msdMsg;
} msd_t;


/* ERA GLONASS specific types for the OptionalData_t parts */
//--------------------------------------------------------------------------------------------------
/**
 * Data structure to gather the ERA GLONASS specific data.
 *
 */
//--------------------------------------------------------------------------------------------------

typedef struct {
   /*
    * ERAAdditionalData ::= SEQUENCE {
    * crashSeverity INTEGER(0..2047) OPTIONAL,
    * diagnosticResult DiagnosticResult OPTIONAL,
    * crashInfo CrashInfo OPTIONAL,
    * coordinateSystemType CoordinateSystemType DEFAULT wgs84,
    * ...
    * }
    */
   uint8_t msdVersion;

   bool presentCrashSeverity;
   uint32_t crashSeverity; /* INTEGER(0..2047) OPTIONAL */

   bool presentDiagnosticResult;
   struct
   {
      bool presentMicConnectionFailure;
      bool micConnectionFailure;

      bool presentMicFailure;
      bool micFailure;

      bool presentRightSpeakerFailure;
      bool rightSpeakerFailure;

      bool presentLeftSpeakerFailure;
      bool leftSpeakerFailure;

      bool presentSpeakersFailure;
      bool speakersFailure;

      bool presentIgnitionLineFailure;
      bool ignitionLineFailure;

      bool presentUimFailure;
      bool uimFailure;

      bool presentStatusIndicatorFailure;
      bool statusIndicatorFailure;

      bool presentBatteryFailure;
      bool batteryFailure;

      bool presentBatteryVoltageLow;
      bool batteryVoltageLow;

      bool presentCrashSensorFailure;
      bool crashSensorFailure;

      bool presentFirmwareImageCorruption;
      bool firmwareImageCorruption;

      bool presentCommModuleInterfaceFailure;
      bool commModuleInterfaceFailure;

      bool presentGnssReceiverFailure;
      bool gnssReceiverFailure;

      bool presentRaimProblem;
      bool raimProblem;

      bool presentGnssAntennaFailure;
      bool gnssAntennaFailure;

      bool presentCommModuleFailure;
      bool commModuleFailure;

      bool presentEventsMemoryOverflow;
      bool eventsMemoryOverflow;

      bool presentCrashProfileMemoryOverflow;
      bool crashProfileMemoryOverflow;

      bool presentOtherCriticalFailures;
      bool otherCriticalFailures;

      bool presentOtherNotCriticalFailures;
      bool otherNotCriticalFailures;
   } diagnosticResult;

   bool presentCrashInfo;
   struct
   {
      bool presentCrashFront;
      bool crashFront;

      bool presentCrashLeft;
      bool crashLeft;

      bool presentCrashRight;
      bool crashRight;

      bool presentCrashRear;
      bool crashRear;

      bool presentCrashRollover;
      bool crashRollover;

      bool presentCrashSide;
      bool crashSide;

      bool presentCrashFrontOrSide;
      bool crashFrontOrSide;

      bool presentCrashAnotherType;
      bool crashAnotherType;
   } crashType;

   bool presentCoordinateSystemTypeInfo;
   msd_CoordinateSystemType_t coordinateSystemType;
} msd_EraGlonassData_t;

ql_msd_result_t QL_Set_EcallSysStd(ecall_system_std_t system_std);

ql_msd_result_t QL_Set_MsdVersion(uint8_t msd_version);

ql_msd_result_t QL_Set_MsdMessageData(msd_Message_t *pMsdMsg);

ql_msd_result_t QL_Set_EraGlonassData(msd_EraGlonassData_t *pEraGlonassData);

ql_msd_result_t QL_Msd_Encode
(
    uint8_t  *pBuiltMsd,     //< built MSD
    uint32_t *pBuiltMsdSize   //< Size of the built MSDvoid
);

#endif // _ASN1_MSD_INCLUDE_GUARD_
