/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#if !defined( PARSE_CONFIG_H__ )
#define PARSE_CONFIG_H__

#include <stddef.h>

#define CONFIG_FILE "iotd_config.ini"
#define CONFIG_FILE_MODE "rb"
#define CONFIG_FILE_MAX_SIZE (1024*1024)

#define MAX_CFG_INI_GROUPS  16
#define MAX_CFG_INI_ITEMS   512

#define MAX_DEVICE_NUM      2
#define MAX_INTERFACE_NUM   2
#define MAX_CLIENT_NUM      6

#define MAX_PER_DEVICE_SQ_NUM   6
#define MAX_TOTAL_SQ_NUM        (MAX_PER_DEVICE_SQ_NUM * MAX_DEVICE_NUM)

#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

//Group defines
#define CONFIG_GROUP_SYSTEM_NAME              "SYSTEM"

#define CONFIG_GROUP_INTERFACE_NAME           "INTERFACE"

#define CONFIG_GROUP_QAPI_NAME                "QAPI"

#define CONFIG_GROUP_DIAG_NAME                "DIAG"

#define CONFIG_GROUP_DATA_NAME                "DATA"

#define CONFIG_GROUP_MGMT_NAME                "MGMT"

//Entry defines
typedef struct
{
    unsigned int num_device;
    unsigned int num_interface;
    unsigned int num_clients;
    unsigned int num_buffer;
    unsigned int pwd_gpio;
    unsigned int force_reset;
    unsigned int heart_beat_enable;
    unsigned int heart_beat_interval;
    unsigned int throughput_test_enable;
    unsigned int throughput_test_mode;
    unsigned int throughput_test_interval;
    unsigned int throughput_test_packet_len;
    unsigned int dbg_lvl;    
} config_system_t;

#define CONFIG_SYSTEM_DEVICE_NUM_NAME       "num_device"
#define CONFIG_SYSTEM_DEVICE_NUM_MIN        ( 1 )
#define CONFIG_SYSTEM_DEVICE_NUM_MAX        ( MAX_DEVICE_NUM )
#define CONFIG_SYSTEM_DEVICE_NUM_DEFAULT    ( 1 )

#define CONFIG_SYSTEM_INTERFACE_NUM_NAME    "num_interface"
#define CONFIG_SYSTEM_INTERFACE_NUM_MIN     ( 1 )
#define CONFIG_SYSTEM_INTERFACE_NUM_MAX     ( MAX_INTERFACE_NUM )
#define CONFIG_SYSTEM_INTERFACE_NUM_DEFAULT ( 1 )

#define CONFIG_SYSTEM_CLIENT_NUM_NAME       "num_clients"
#define CONFIG_SYSTEM_CLIENT_NUM_MIN        ( 1 )
#define CONFIG_SYSTEM_CLIENT_NUM_MAX        ( MAX_CLIENT_NUM )
#define CONFIG_SYSTEM_CLIENT_NUM_DEFAULT    ( 1 )

#define CONFIG_SYSTEM_BUFFER_NUM_NAME       "num_buffer"
#define CONFIG_SYSTEM_BUFFER_NUM_MIN        ( 1 )
#define CONFIG_SYSTEM_BUFFER_NUM_MAX        ( 4096 )
#define CONFIG_SYSTEM_BUFFER_NUM_DEFAULT    ( 20 )

#define CONFIG_SYSTEM_PWD_GPIO_NAME         "pwd_gpio"
#define CONFIG_SYSTEM_PWD_GPIO_MIN          ( 0 )
#define CONFIG_SYSTEM_PWD_GPIO_MAX          ( 999 )
#define CONFIG_SYSTEM_PWD_GPIO_DEFAULT      ( 31 )

#define CONFIG_SYSTEM_FORCE_RESET_NAME      "force_reset"
#define CONFIG_SYSTEM_FORCE_RESET_MIN       ( 0 )
#define CONFIG_SYSTEM_FORCE_RESET_MAX       ( 1 )
#define CONFIG_SYSTEM_FORCE_RESET_DISABLE   ( 0 )
#define CONFIG_SYSTEM_FORCE_RESET_ENABLE    ( 1 )
#define CONFIG_SYSTEM_FORCE_RESET_DEFAULT   ( CONFIG_SYSTEM_FORCE_RESET_ENABLE )

#define CONFIG_SYSTEM_HEART_BEAT_NAME      "heart_beat_enable"
#define CONFIG_SYSTEM_HEART_BEAT_MIN       ( 0 )
#define CONFIG_SYSTEM_HEART_BEAT_MAX       ( 1 )
#define CONFIG_SYSTEM_HEART_BEAT_DISABLE   ( 0 )
#define CONFIG_SYSTEM_HEART_BEAT_ENABLE    ( 1 )
#define CONFIG_SYSTEM_HEART_BEAT_DEFAULT   ( CONFIG_SYSTEM_HEART_BEAT_ENABLE )

#define CONFIG_SYSTEM_HEART_BEAT_INTERVAL_NAME      "heart_beat_interval"
#define CONFIG_SYSTEM_HEART_BEAT_INTERVAL_MIN       ( 1 )
#define CONFIG_SYSTEM_HEART_BEAT_INTERVAL_MAX       ( 600 )
#define CONFIG_SYSTEM_HEART_BEAT_INTERVAL_DEFAULT   ( 5 )

#define CONFIG_SYSTEM_THROUGHPUT_TEST_ENABLE_NAME   "throughput_test_enable"
#define CONFIG_SYSTEM_THROUGHPUT_TEST_ENABLE_MIN       ( 0 )
#define CONFIG_SYSTEM_THROUGHPUT_TEST_ENABLE_MAX       ( 1 )
#define CONFIG_SYSTEM_THROUGHPUT_TEST_ENABLE_DEFAULT   ( 0 )

#define CONFIG_SYSTEM_THROUGHPUT_TEST_MODE_NAME   "throughput_test_mode"
#define CONFIG_SYSTEM_THROUGHPUT_TEST_MODE_MIN       ( 0 )
#define CONFIG_SYSTEM_THROUGHPUT_TEST_MODE_MAX       ( 1 )
#define CONFIG_SYSTEM_THROUGHPUT_TEST_MODE_DEFAULT   ( 0 )


#define CONFIG_SYSTEM_THROUGHPUT_TEST_INTERVAL_NAME   "throughput_test_interval"
#define CONFIG_SYSTEM_THROUGHPUT_TEST_INTERVAL_MIN       ( 1 )
#define CONFIG_SYSTEM_THROUGHPUT_TEST_INTERVAL_MAX       ( 9999 )
#define CONFIG_SYSTEM_THROUGHPUT_TEST_INTERVAL_DEFAULT   ( 5 )

#define CONFIG_SYSTEM_THROUGHPUT_TEST_PACKET_LEN_NAME   "throughput_test_packet_len"
#define CONFIG_SYSTEM_THROUGHPUT_TEST_PACKET_LEN_MIN       ( 1 )
#define CONFIG_SYSTEM_THROUGHPUT_TEST_PACKET_LEN_MAX       ( 2000 )
#define CONFIG_SYSTEM_THROUGHPUT_TEST_PACKET_LEN_DEFAULT   ( 1000 )

#define CONFIG_SYSTEM_DBG_LVL_NAME       "dbg_level"
#define CONFIG_SYSTEM_DBG_LVL_MIN        ( 0 )
#define CONFIG_SYSTEM_DBG_LVL_MAX        ( 16)
#define CONFIG_SYSTEM_DBG_LVL_DEFAULT    ( 1 )

#define MAX_INTERFACE_NAME_LENGTH           32 
#define MAX_INTERFACE_QID_NUM               MAX_PER_DEVICE_SQ_NUM
typedef struct
{
    unsigned int enable;
    unsigned int type;
    unsigned int device_id;
    unsigned int num_service_q;
    char name[MAX_INTERFACE_NAME_LENGTH];
    unsigned int speed;
    unsigned int flow_control;
    unsigned int intr_gpio;
    unsigned int block_size;
    unsigned int qid[MAX_INTERFACE_QID_NUM];
} config_interface_t;
    
#define CONFIG_INTERFACE_ENABLE_NAME        "enable"
#define CONFIG_INTERFACE_ENABLE_MIN         ( 0 )
#define CONFIG_INTERFACE_ENABLE_MAX         ( 1 )
#define CONFIG_INTERFACE_DISABLE            ( 0 )
#define CONFIG_INTERFACE_ENABLE             ( 1 )

#define CONFIG_INTERFACE_TYPE_NAME          "type"
#define CONFIG_INTERFACE_TYPE_MIN           ( 0 )
#define CONFIG_INTERFACE_TYPE_MAX           ( 2 )
#define CONFIG_INTERFACE_TYPE_UART          ( 0 )
#define CONFIG_INTERFACE_TYPE_SPI           ( 1 )
#define CONFIG_INTERFACE_TYPE_SDIO          ( 2 )

#define CONFIG_INTERFACE_DEVICE_ID_NAME     "device_id"
#define CONFIG_INTERFACE_DEVICE_ID_MIN      ( 0 )
#define CONFIG_INTERFACE_DEVICE_ID_MAX      ( MAX_DEVICE_NUM - 1 )
#define CONFIG_INTERFACE_DEVICE_ID_DEFAULT  ( 0 )

#define CONFIG_INTERFACE_SERVICE_Q_NUM_NAME     "num_service_q"
#define CONFIG_INTERFACE_SERVICE_Q_NUM_MIN      ( 1 )
#define CONFIG_INTERFACE_SERVICE_Q_NUM_MAX      ( MAX_CLIENT_NUM )
#define CONFIG_INTERFACE_SERVICE_Q_NUM_DEFAULT  ( 1 )

#define CONFIG_INTERFACE_NAME               "name"
#define CONFIG_INTERFACE_NAME_UART_DEFAULT  "/dev/ttyUSB0"
#define CONFIG_INTERFACE_NAME_SPI_DEFAULT   "/dev/spidev32765.0"

#define CONFIG_INTERFACE_SPEED_NAME     "speed"
#define CONFIG_INTERFACE_SPEED_MIN      ( 0 )
#define CONFIG_INTERFACE_SPEED_MAX      ( 20000000 )
#define CONFIG_INTERFACE_SPEED_DEFAULT  ( 2000000 )

#define CONFIG_INTERFACE_QID_NAME      "service_qid"
#define CONFIG_INTERFACE_QID_DEFAULT   "0,255,255,255,255,255"
#define CONFIG_INTERFACE_QID_ARRAY_NUM (6)

#define CONFIG_INTERFACE_FLOW_CONTROL_NAME        "flow_control"
#define CONFIG_INTERFACE_FLOW_CONTROL_MIN         ( 0 )
#define CONFIG_INTERFACE_FLOW_CONTROL_MAX         ( 1 )
#define CONFIG_INTERFACE_FLOW_CONTROL_DISABLE     ( 0 )
#define CONFIG_INTERFACE_FLOW_CONTROL_ENABLE      ( 1 )

#define CONFIG_INTERFACE_BLOCK_SIZE_NAME          "block_size"
#define CONFIG_INTERFACE_BLOCK_SIZE_MIN           ( 32 )
#define CONFIG_INTERFACE_BLOCK_SIZE_MAX           ( 2048 )
#define CONFIG_INTERFACE_BLOCK_SIZE_DEFAULT       ( 1024 )

#define CONFIG_INTERFACE_INTR_GPIO_NAME         "intr_gpio"
#define CONFIG_INTERFACE_INTR_GPIO_MIN          ( 0 )
#define CONFIG_INTERFACE_INTR_GPIO_MAX          ( 999 )
#define CONFIG_INTERFACE_INTR_GPIO_DEFAULT      ( 42 )

typedef struct
{
    char ipc_server[16];
    unsigned int msg_num;
    unsigned int msg_sz;
    unsigned int num_service_q;
    unsigned int qid[MAX_TOTAL_SQ_NUM];
} config_service_t;

#define CONFIG_IPC_SERVER_NAME                     "ipc_server"
#define CONFIG_IPC_SERVER_QAPI_NAME_DEFAULT        "/iotd"
#define CONFIG_IPC_SERVER_DIAG_NAME_DEFAULT        "/iotdiag"
#define CONFIG_IPC_SERVER_NAME_DEFAULT             ""

#define CONFIG_IPC_MSG_NUMBER_NAME      "msg_num"
#define CONFIG_IPC_MSG_NUMBER_MIN       ( 1 )
#define CONFIG_IPC_MSG_NUMBER_MAX       ( 10 )
#define CONFIG_IPC_MSG_NUMBER_DEFAULT   ( 10 )

#define CONFIG_IPC_MSG_SIZE_NAME         "msg_sz"
#define CONFIG_IPC_MSG_SIZE_MIN          ( 1 )
#define CONFIG_IPC_MSG_SIZE_MAX          ( 10000 )
#define CONFIG_IPC_MSG_SIZE_DEFAULT      ( 2000 )

#define CONFIG_IPC_SERVICE_Q_NUMBER_NAME        "num_service_q"
#define CONFIG_IPC_SERVICE_Q_NUMBER_MIN         ( 1 )
#define CONFIG_IPC_SERVICE_Q_NUMBER_MAX         ( 12 )
#define CONFIG_IPC_SERVICE_Q_NUMBER_DEFAULT     ( 1 )

#define CONFIG_SERVICE_Q_ID_NAME        "service_qid"
#define CONFIG_QAPI_Q_ID_DEFAULT        "0x00,0x10,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF"
#define CONFIG_DATA_Q_ID_DEFAULT        "0x01,0x02,0x03,0x11,0x12,0x13,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF"
#define CONFIG_MGMT_Q_ID_DEFAULT        "0x04,0x14,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF"
#define CONFIG_DIAG_Q_ID_DEFAULT        "0x05,0x15,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF"

#define CONFIG_SERVICE_Q_ID_ARRAY_NUM   (MAX_TOTAL_SQ_NUM)

//Structure to store each group in qcom_cfg.ini file
//Format: [name]
typedef struct
{
   char *name;
   unsigned int  start_idx;
   unsigned int  count;
}tCfgIniGroup;

//Format: name=value
typedef struct
{
   char *name;
   char *value;
}tCfgIniEntry;

#define VAR_OFFSET( _Struct, _Var ) (offsetof(_Struct, _Var))
#define VAR_SIZE( _Struct, _Var ) (sizeof(((_Struct *)0)->_Var))

#define VAR_FLAGS_NONE         (      0 )
#define VAR_FLAGS_REQUIRED     ( 1 << 0 )   // bit 0 is Required or Optional
#define VAR_FLAGS_OPTIONAL     ( 0 << 0 )

#define VAR_FLAGS_RANGE_CHECK  ( 1 << 1 )   // bit 1 tells if range checking is required.
                                            // If less than MIN, assume MIN.
                                            // If greater than MAX, assume MAX.

#define VAR_FLAGS_RANGE_CHECK_ASSUME_MINMAX ( VAR_FLAGS_RANGE_CHECK )

#define VAR_FLAGS_RANGE_CHECK_ASSUME_DEFAULT ( 1 << 2 )  // bit 2 is range checking that assumes the DEFAULT value
                                                         // If less than MIN, assume DEFAULT,
                                                         // If grateer than MAX, assume DEFAULT.

//#define VAR_FLAGS_DYNAMIC_CFG ( 1 << 3 )  // Bit 3 indicates that
                                          // the config item can be
                                          // modified dynamicially
                                          // on a running system

#define REG_VARIABLE( _Name, _Type,  _Struct, _VarName,          \
                      _Flags, _Default, _Min, _Max )             \
{                                                                \
  ( _Name ),                                                     \
  ( _Type ),                                                     \
  ( _Flags ),                                                    \
  VAR_OFFSET( _Struct, _VarName ),                               \
  VAR_SIZE( _Struct, _VarName ),                                 \
  ( _Default ),                                                  \
  ( _Min ),                                                      \
  ( _Max ),                                                      \
  1                                                              \
}

#if 0
#define REG_DYNAMIC_VARIABLE( _Name, _Type,  _Struct, _VarName,  \
                              _Flags, _Default, _Min, _Max,      \
                              _CBFunc, _CBParam )                \
{                                                                \
  ( _Name ),                                                     \
  ( _Type ),                                                     \
  ( VAR_FLAGS_DYNAMIC_CFG | ( _Flags ) ),                        \
  VAR_OFFSET( _Struct, _VarName ),                               \
  VAR_SIZE( _Struct, _VarName ),                                 \
  ( _Default ),                                                  \
  ( _Min ),                                                      \
  ( _Max ),                                                      \
  ( _CBFunc ),                                                   \
  ( _CBParam )                                                   \
}
#endif

#define REG_VARIABLE_STRING( _Name, _Type,  _Struct, _VarName,   \
                             _Flags, _Default )                  \
{                                                                \
  ( _Name ),                                                     \
  ( _Type ),                                                     \
  ( _Flags ),                                                    \
  VAR_OFFSET( _Struct, _VarName ),                               \
  VAR_SIZE( _Struct, _VarName ),                                 \
  (unsigned long)( _Default ),                                   \
  0,                                                             \
  0,                                                             \
  1,                                                             \
}

#define REG_VARIABLE_ARRAY( _Name, _Type,  _Struct, _VarName,    \
                             _Flags, _Default, _ArrayNum )       \
{                                                                \
  ( _Name ),                                                     \
  ( _Type ),                                                     \
  ( _Flags ),                                                    \
  VAR_OFFSET( _Struct, _VarName ),                               \
  VAR_SIZE( _Struct, _VarName ),                                 \
  (unsigned long)( _Default ),                                   \
  0,                                                             \
  0,                                                             \
  ( _ArrayNum ),                                                 \
}

typedef enum
{
  PARAM_Integer,
  PARAM_SignedInteger,
  PARAM_HexInteger,
  PARAM_String,
  PARAM_IntegerArray
}PARAMETER_TYPE;

typedef struct tREG_TABLE_ENTRY {

  char*               RegName;            // variable name in the .ini file
  PARAMETER_TYPE      RegType;            // variable type in the structure
  unsigned long       Flags;              // Specify optional parms and if RangeCheck is performed
  unsigned short      VarOffset;          // offset to field from the base address of the structure
  unsigned short      VarSize;            // size (in bytes) of the field
  unsigned long       VarDefault;         // default value to use
  unsigned long       VarMin;             // minimum value, for range checking
  unsigned long       VarMax;             // maximum value, for range checking
  unsigned long       VarNum;             // Only available for array, array[VarNum]
#if 0
                                          // Dynamic modification notifier
  void (*pfnDynamicNotify)(hdd_context_t *pHddCtx, unsigned long NotifyId);
  unsigned long       NotifyId;           // Dynamic modification identifier
#endif
} REG_TABLE_ENTRY;

typedef struct
{
    config_system_t config_system;
    config_interface_t config_interface[MAX_INTERFACE_NUM];
    config_service_t config_qapi;
    config_service_t config_diag;
    config_service_t config_data;
    config_service_t config_mgmt;
} cfg_ini;

#define REG_Group( _Name, _Struct, _Var,            \
                   _Flags, _Entry)                  \
{                                                   \
  ( _Name ),                                        \
  ( sizeof(_Entry)/sizeof(_Entry[0]) ),             \
  ( _Flags ),                                       \
  VAR_OFFSET( _Struct, _Var ),                      \
  VAR_SIZE( _Struct, _Var ),                        \
  ( _Entry )                                        \
}

typedef struct tREG_TABLE_GROUP {
    char*               GroupName;
    unsigned int        EntryNumber;
    unsigned long       Flags;              // Specify optional parms and if RangeCheck is performed
    unsigned long       GroupOffset;
    unsigned long       GroupSize;
    REG_TABLE_ENTRY*    TableEntry;
} REG_TABLE_GROUP; 

#endif
