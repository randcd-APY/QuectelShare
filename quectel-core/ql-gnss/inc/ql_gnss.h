#ifndef QL_GNSS_H
#define QL_GNSS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOCATIONDATATYPES_H
#define GNSS_SV_MAX            (64)
typedef enum {
    GNSS_SV_TYPE_UNKNOWN = 0,
    GNSS_SV_TYPE_GPS,
    GNSS_SV_TYPE_SBAS,
    GNSS_SV_TYPE_GLONASS,
    GNSS_SV_TYPE_QZSS,
    GNSS_SV_TYPE_BEIDOU,
    GNSS_SV_TYPE_GALILEO,
} GnssSvType;

typedef uint16_t GnssSvOptionsMask;
typedef enum {
    GNSS_SV_OPTIONS_HAS_EPHEMER_BIT             = (1<<0),
    GNSS_SV_OPTIONS_HAS_ALMANAC_BIT             = (1<<1),
    GNSS_SV_OPTIONS_USED_IN_FIX_BIT             = (1<<2),
    GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT   = (1<<3),
} GnssSvOptionsBits;

typedef struct {
    size_t size;       // set to sizeof(GnssSv)
    uint16_t svId;     // Unique Identifier
    GnssSvType type;   // type of SV (GPS, SBAS, GLONASS, QZSS, BEIDOU, GALILEO)
    float cN0Dbhz;     // signal strength
    float elevation;   // elevation of SV (in degrees)
    float azimuth;     // azimuth of SV (in degrees)
    GnssSvOptionsMask gnssSvOptionsMask; // Bitwise OR of GnssSvOptionsBits
    float carrierFrequencyHz; // carrier frequency of the signal tracked
} GnssSv;

typedef struct {
    size_t size;                 // set to sizeof(GnssSvNotification)
    size_t count;                // number of SVs in the GnssSv array
    GnssSv gnssSvs[GNSS_SV_MAX]; // information on a number of SVs
} GnssSvNotification;

//Flags to indicate which values are valid in a Location
typedef uint16_t LocationFlagsMask;
typedef enum {
    LOCATION_HAS_LAT_LONG_BIT          = (1<<0), // location has valid latitude and longitude
    LOCATION_HAS_ALTITUDE_BIT          = (1<<1), // location has valid altitude
    LOCATION_HAS_SPEED_BIT             = (1<<2), // location has valid speed
    LOCATION_HAS_BEARING_BIT           = (1<<3), // location has valid bearing
    LOCATION_HAS_ACCURACY_BIT          = (1<<4), // location has valid accuracy
    LOCATION_HAS_VERTICAL_ACCURACY_BIT = (1<<5), // location has valid vertical accuracy
    LOCATION_HAS_SPEED_ACCURACY_BIT    = (1<<6), // location has valid speed accuracy
    LOCATION_HAS_BEARING_ACCURACY_BIT  = (1<<7), // location has valid bearing accuracy
} LocationFlagsBits;

typedef uint16_t LocationTechnologyMask;
typedef enum {
    LOCATION_TECHNOLOGY_GNSS_BIT     = (1<<0), // location was calculated using GNSS
    LOCATION_TECHNOLOGY_CELL_BIT     = (1<<1), // location was calculated using Cell
    LOCATION_TECHNOLOGY_WIFI_BIT     = (1<<2), // location was calculated using WiFi
    LOCATION_TECHNOLOGY_SENSORS_BIT  = (1<<3), // location was calculated using Sensors
} LocationTechnologyBits;

typedef struct {
    size_t size;             // set to sizeof(Location)
    LocationFlagsMask flags; // bitwise OR of LocationFlagsBits to mark which params are valid
    uint64_t timestamp;      // UTC timestamp for location fix, milliseconds since January 1, 1970
    double latitude;         // in degrees
    double longitude;        // in degrees
    double altitude;         // in meters above the WGS 84 reference ellipsoid
    float speed;             // in meters per second
    float bearing;           // in degrees; range [0, 360)
    float accuracy;          // in meters
    float verticalAccuracy;  // in meters
    float speedAccuracy;     // in meters/second
    float bearingAccuracy;   // in degrees (0 to 359.999)
    LocationTechnologyMask techMask;
} Location;

#endif


typedef void (* ql_location_callback)(Location loc);
typedef void (* ql_nmea_callback) (uint64_t time,const char *nmea,int len);
typedef void (* ql_sv_callback) (GnssSvNotification Gsv);
typedef struct {
        ql_location_callback location_cb;
        ql_nmea_callback nmea_cb;
        ql_sv_callback svinfo_cb;
}QlLocationCallbacks;


int QL_Gnss_Init(QlLocationCallbacks cb);
int QL_Gnss_Start(void);
int QL_Gnss_Stop(void);


#ifdef __cplusplus
}
#endif


#endif
