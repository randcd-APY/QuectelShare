#ifndef __QAPI_DIAG_MSG_H__
#define __QAPI_DIAG_MSG_H__

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*=================================================================================
 *
 *                       DIAGNOSTICS - DEBUG MESSAGE INTERFACE
 *
 *===============================================================================*/

 /** @file qapi_diag_msg.h
 *
 * @addtogroup qapi_diag_msg
 * @{
 *
 * @brief Diagnostics Debug Message Interface (DIAG)
 *
 * @details This interface provides a mechanism for software modules to send debug strings
 *          to an external agent, which can be can be a software program running
 *          on a PC connected to the device via a serial interface like UART or USB.
 * 
 *          The interface allows clients to log debug strings with up to three integer arguments with
 *          three levels of verbosity. In the increasing verbosity, clients specify LOW,
 *          MEDIUM, or HIGH levels.        
 *
 * @code {.c}
 *
 *     * The code snippet below shows the use of macros to send debug messages.
 *     * Clients should use QAPI_DIAG_MSG_SYSBUF_HDL as the handle.
 *
 *     /// Clients should use Module IDs from 226 to 255
 *     /// Module IDs 225 and below are reserved for system modules 
 *     #define MY_MOD_ID        226 
 *
 *     int my_int1, my_int2, my_int3;
 *     char my_test_str_arg[] = "diag_str_arg";
 *     
 *     QAPI_DIAG_MSG_ARG0( QAPI_DIAG_MSG_SYSBUF_HDL, MY_MOD_ID, QAPI_DIAG_MSG_LVL_LOW, "diag test no arg" );
 *     QAPI_DIAG_MSG_ARG1( QAPI_DIAG_MSG_SYSBUF_HDL, MY_MOD_ID, QAPI_DIAG_MSG_LVL_MED, "diag test 1 arg: %d", my_int1 ); 
 *     QAPI_DIAG_MSG_ARG2( QAPI_DIAG_MSG_SYSBUF_HDL, MY_MOD_ID, QAPI_DIAG_MSG_LVL_HIGH, "diag test 2 args: arg1: %d arg2: %d", my_int1, my_int2 );
 *     QAPI_DIAG_MSG_ARG3( QAPI_DIAG_MSG_SYSBUF_HDL, MY_MOD_ID, QAPI_DIAG_MSG_LVL_LOW, "diag test 3 args: arg1: %d; arg2: %d; arg3: %d",my_int1, my_int2, my_int3 );
 * 
 *     QAPI_DIAG_MSG_STR_ARG1(QAPI_DIAG_MSG_SYSBUF_HDL, MY_MOD_ID, QAPI_DIAG_MSG_LVL_LOW, "diag string test : %s\n",my_test_str_arg);
 *
 * @endcode
 * @}
 */

/*===========================================================================
                              Edit History
 $Header: //components/rel/core.ioe/1.0/api/services/qapi_diag_msg.h#13 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/10/17   mad     Support for IAR tool-chain to enforce 4-byte alignment with 
                   pragma directive used by IAR
08/11/16   mad     Modified QAPI_DIAG_MSG_ARGx macros to enforce 4-byte alignment
                   of format strings, moved out of ROM
02/02/16   mad     Added DebugMessage with string argument 
12/03/15   leo     (TechComm) Edited/added Doxygen comments and markup.
10/29/15   mad     Created file
===========================================================================*/

/** @addtogroup qapi_diag_msg
@{ */

/** @name Supported Verbosity Levels
@{ */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define QAPI_DIAG_MSG_LVL_LOW  ((uint8_t) 0x1)
#define QAPI_DIAG_MSG_LVL_MED  ((uint8_t) 0x2)
#define QAPI_DIAG_MSG_LVL_HIGH ((uint8_t) 0x3)

#define QAPI_DIAG_MSG_MAX_FMTSTR_SIZE      50
/**< Maximum length of a format string. Format strings exceeding this maximum length will be dropped. */
#define QAPI_DIAG_MSG_SYSBUF_HDL           1
/**< Specify this value for the handle to log debug messages to the system-wide debug buffer. */

/** @} */ /* end_namegroup */

/** @} */ /* end_addtogroup qapi_diag_msg */

/** @addtogroup qapi_diag_msg
@{ */

/*==================================================================================
  FUNCTION      _qapi_Diag_Msg_Write_Arg0
==================================================================================*/
/**
 * See _qapi_Diag_Msg_Write_Arg3().
 */
qapi_Status_t _qapi_Diag_Msg_Write_Arg0( void *handle, uint8_t module_ID, uint8_t level, const char* format);

/*==================================================================================
  FUNCTION      _qapi_Diag_Msg_Write_Arg1
==================================================================================*/
/**
 * See _qapi_Diag_Msg_Write_Arg3().
 */
qapi_Status_t _qapi_Diag_Msg_Write_Arg1( void *handle, uint8_t module_ID, uint8_t level, const char* format, uint32_t arg1);

/*==================================================================================
  FUNCTION      _qapi_Diag_Msg_Write_Arg2
==================================================================================*/
/**
 * See _qapi_Diag_Msg_Write_Arg3().
 */
qapi_Status_t _qapi_Diag_Msg_Write_Arg2( void *handle, uint8_t module_ID, uint8_t level, const char* format, uint32_t arg1, uint32_t arg2);

/*==================================================================================
  FUNCTION      _qapi_Diag_Msg_Write_Arg3
==================================================================================*/
/**
 * Adds a debug message packet to the log buffer specified by the handle.
 *
 * @note1hang Clients should not call the function directly. Instead, they should use
 *       QAPI_DIAG_MSG_ARG* macros.
 * 
 * @param[in] handle           Handle to the debug buffer in which to store this message.
 *                             The only allowed value for handle at the QAPI level is
 *                             QAPI_DIAG_MSG_SYSBUF_HDL. The message will be logged
 *                             in the system-wide buffer if level, module_id, etc., are valid,
 *                             otherwise the message will be dropped.
 *
 * @param[in] module_ID        Module ID. Module IDs range from 1 to 255 (module ID 0 is reserved for this module).
 *                             Module IDs through 225 are reserved for QTI modules. OEM modules can use 226 through 255.
 *                             Module IDs and their corresponding tech areas can be maintained as configuration information,
 *                             by the external agent (tool). There will be no per-processor module_id. The message will
 *                             be logged only if module_id is enabled in the system-wide module bitmask (32 bytes).
 *
 * @param[in] level            Message level can be QAPI_DIAG_MSG_LVL_LOW, QAPI_DIAG_MSG_LVL_MED or QAPI_DIAG_MSG_LVL_HIGH.
 *                             The message will be logged only if the system debug message level is @ge level.
 *
 * @param[in] format           printf-style format string. Can contain only an integer argument specifier,
 *                             and can have up to three arguments.
 *
 * @param[in] arg1,arg2,arg3   Arguments to the format string.
 *      
 * @return
 * QAPI_OK -- Message logged successfully. \n
 * QAPI_ERR_INVALID_PARAM -- Invalid handle was used. \n
 * QAPI_ERROR -- Other errors.
 *
 */
qapi_Status_t _qapi_Diag_Msg_Write_Arg3( void *handle, uint8_t module_ID, uint8_t level, const char* format, uint32_t arg1, uint32_t arg2, uint32_t arg3);

/**
 * Linker attribute to enforce 4-byte alignment of format string
 * addresses. 
 *  
 * GCC and ARMCT use a certain syntax and IAR uses a pragma 
 * directive. pragma cannot be used directly in a preprocessor
 * definition, so if required, we need to rely on the _Pragma
 * operator supported by C99 and can be used inside a 
 * preprocessor definition. 
 *
 * However, since IAR does 4-byte alignment by default, these directives 
 * will be defined to nothing for IAR. The directives are kept in 
 * place in the macro definitions as a reminder that alignment 
 * matters for these macros.
 *  
 * This header file supports GCC, ARMCT, and IAR. If a customer 
 * needs to use any other toolchain, relevant support would need 
 * to be added if required. Since the syntax used by GCC is more 
 * mainstream, that style is considered to be the default 
 * syntax. 
 */

#ifdef __ICCARM__ /* for IAR */
/** Since IAR takes care of 4-byte alignment by default, this
 *  will do nothing for IAR. */
#define QAPI_DIAG_MSG_FORMAT_ALIGN
#define QAPI_DIAG_MSG_FORMAT_ALIGN_REVERT
#define QAPI_DIAG_MSG_FORMAT_LINKER_ATTR
#else /** default case is GCC or ARMCT syntax */
/**
 * Linker attribute flag to move format strings into a separate section.
 * This section will be placed in an ELF segment which will be outside the
 * memory map, using scatter-loader file magic of placement.
 *
 * Compaction script will remove this ELF segment and the linker can re-claim
 * the memory saved by moving the strings out.
 */
#define QAPI_DIAG_MSG_FORMAT_LINKER_ATTR __attribute__((section (".diagmsg.format"),aligned (4)))
#define QAPI_DIAG_MSG_FORMAT_ALIGN
#define QAPI_DIAG_MSG_FORMAT_ALIGN_REVERT
#endif

/** Diag message compaction. */
typedef union
{                     
   const char *format; /**< Debug strings generated by QAPIs are not compacted, so this is not used. */
   uint32_t diag_Msg_ID; /**< This will take up 4 bytes in the client's execution region. 
                            It will initially have the format string address, but can be
							replaced by MessageID if compaction is enabled for QAPI. */
}qapi_Diag_Msg_Compact_t;  


/**
 * See QAPI_DIAG_MSG_ARG3(). @hideinitializer
 */
#define QAPI_DIAG_MSG_ARG0(handle, module_ID, level, format) \
do \
{ \
   QAPI_DIAG_MSG_FORMAT_ALIGN \
   static const char _qapi_Diag_Msg_Format[] QAPI_DIAG_MSG_FORMAT_LINKER_ATTR = format; \
   QAPI_DIAG_MSG_FORMAT_ALIGN_REVERT \
   static const qapi_Diag_Msg_Compact_t _qapi_Diag_Msg_Compact = {_qapi_Diag_Msg_Format}; \
   _qapi_Diag_Msg_Write_Arg0((void*)(handle), (module_ID), (level), ((const char*)(&_qapi_Diag_Msg_Compact))); \
}while (0)

/**
 * See QAPI_DIAG_MSG_ARG3(). @hideinitializer
 */
#define QAPI_DIAG_MSG_ARG1(handle, module_ID, level, format, arg1) \
do \
{ \
   QAPI_DIAG_MSG_FORMAT_ALIGN \
   static const char _qapi_Diag_Msg_Format[] QAPI_DIAG_MSG_FORMAT_LINKER_ATTR = format; \
   QAPI_DIAG_MSG_FORMAT_ALIGN_REVERT \
   static const qapi_Diag_Msg_Compact_t _qapi_Diag_Msg_Compact = {_qapi_Diag_Msg_Format}; \
   _qapi_Diag_Msg_Write_Arg1((void*)(handle), (module_ID), (level), ((const char*)(&_qapi_Diag_Msg_Compact)), (arg1)); \
}while (0)

/**
 * See QAPI_DIAG_MSG_ARG3(). @hideinitializer
 */
#define QAPI_DIAG_MSG_ARG2(handle, module_ID, level, format, arg1, arg2) \
do \
{ \
   QAPI_DIAG_MSG_FORMAT_ALIGN \
   static const char _qapi_Diag_Msg_Format[] QAPI_DIAG_MSG_FORMAT_LINKER_ATTR = format; \
   QAPI_DIAG_MSG_FORMAT_ALIGN_REVERT \
   static const qapi_Diag_Msg_Compact_t _qapi_Diag_Msg_Compact = {_qapi_Diag_Msg_Format}; \
   _qapi_Diag_Msg_Write_Arg2((void*)(handle), (module_ID), (level), ((const char*)(&_qapi_Diag_Msg_Compact)), (arg1), (arg2)); \
}while (0)

/**
 * Macros to log debug strings.
 *
 * The debug system will support run-time filtering only. Compile-time filtering is
 *       not supported. A message will be logged only if the module is enabled and the
 *       message level is @ge the system message level.
 *
 *       Format strings that have more than QAPI_DIAG_MSG_MAX_FMTSTR_SIZE characters will be dropped.
 *
 *       The debug system will enable all modules and set the system debug level to QAPI_DIAG_MSG_LVL_MED
 *       on startup. This is to ensure that some messages get logged even when there is no
 *       tool connected during a run. The startup system debug level can be changed at compile time if required.
 *       The system debug level can be changed at run time with CLI commands. 
 *
 * @param[in] handle    Handle to the debug buffer in which to store this message. 
 *                      The only allowed value for handle at the QAPI level is QAPI_DIAG_MSG_SYSBUF_HDL.
 *                      The message will be logged in the system-wide buffer if level, module_id, etc., are enabled,
 *                      otherwise the message will be dropped.
 * @param[in] module_ID Module ID. Module IDs range from 1 to 255 (module ID 0 is reserved for the Diag module).
 *                      Module IDs through 225 are reserved for QTI modules. OEM modules can use 226 through 255.
 *                      Module IDs and their corresponding tech areas can be maintained as configuration information
 *                      by the external agent (tool).
 *                      There will be no per-processor module_id.
 *                      The message will be logged only if module_id is enabled in the system-wide
 *                      module bitmask (32 bytes).
 * @param[in] level     Message level. Can be QAPI_DIAG_MSG_LVL_LOW, QAPI_DIAG_MSG_LVL_MED or QAPI_DIAG_MSG_LVL_HIGH.
 *                      The message will be logged only if the system debug message level is @ge level. 
 * @param[in] format    printf-style format string. Can contain only an integer argument specifier. 
 * @param[in] arg1,arg2,arg3 Integer (4 byte) argument(s).
 *
 */
#define QAPI_DIAG_MSG_ARG3(handle, module_ID, level, format, arg1, arg2, arg3) \
do \
{ \
   QAPI_DIAG_MSG_FORMAT_ALIGN \
   static const char _qapi_Diag_Msg_Format[] QAPI_DIAG_MSG_FORMAT_LINKER_ATTR = format; \
   QAPI_DIAG_MSG_FORMAT_ALIGN_REVERT \
   static const qapi_Diag_Msg_Compact_t _qapi_Diag_Msg_Compact = {_qapi_Diag_Msg_Format}; \
   _qapi_Diag_Msg_Write_Arg3((void*)(handle), (module_ID), (level), ((const char*)(&_qapi_Diag_Msg_Compact)), (arg1), (arg2), (arg3)); \
}while (0)


/*==================================================================================
  FUNCTION      qapi_Diag_Msg_Str_Write_Arg1 
==================================================================================*/
/**
 * @brief Adds a debug message packet (with a 1-string argument) to the log buffer specified by the handle.
 *
 * @note1hang Do not call this function directly. Instead, use the
 *            QAPI_DIAG_MSG_STR_ARG1 macro.
 *
 * @note1hang Use this sparingly to optimize use of buffer space.
 *            This API is not available in Sensor Operating mode or other low-power modes. 
 * 
 * @param[in] handle     Handle to the Debug Buffer to store this message into.
 *                       Only allowed value for handle at QAPI level is
 *                       QAPI_DIAG_MSG_SYSBUF_HDL. The message will be logged
 *                       in the system-wide buffer if level, module_id, etc., are valid,
 *                       otherwise, it will be dropped.
 *
 * @param[in] module_ID  Module ID. Module IDs range from 1 to 255 (module ID 0 is reserved for this module).
 *                       Module IDs through 225 are reserved for QTI modules. OEM modules can use 226 through 255.
 *                       Module IDs and corresponding tech areas can be maintained as configuration information,
 *                       by the external agent (tool). There is no per-processor module_id. The message will
 *                       be logged only if module_id is enabled in the system-wide module-bit-mask (32-bytes).
 *
 * @param[in] level      Message level can be QAPI_DIAG_MSG_LVL_LOW, QAPI_DIAG_MSG_LVL_MED or QAPI_DIAG_MSG_LVL_HIGH.
 *                       The message will be logged only if the system debug message level is @ge level.
 *
 * @param[in] format     printf-style format string. Can contain only %s argument specifiers, can have 1 string argument.
 *
 * @param[in] str_Arg1   String argument to the format string.
 *   
 * @return
 * QAPI_OK -- Message logged successfully \n
 * QAPI_ERR_INVALID_PARAM -- Invalid handle used \n
 * QAPI_ERROR -- Other errors
 *
 * @note Clients should \b NOT call the function directly. Instead they should use
 *            \b QAPI_DIAG_MSG_STR_ARG1 macro.
 * @warning 
 * Use this sparingly to optimize use of buffer-space.
 * This API will NOT be available in Sensor Operating Mode or other low-power modes. 
 *
 */
qapi_Status_t _qapi_Diag_Msg_Str_Write_Arg1( void *handle, uint8_t module_ID, uint8_t level, const char* format, char* str_Arg1);

/**
 * @brief Macro to log Debug strings with a 1-string argument.
 *
 * @note1hang The debug system supports run-time filtering only. Compile-time filtering is
 *            not supported. A message will be logged only if ((the module is
 *            enabled) and (message level is @ge system message level)).
 *
 * The format string and string arguments can have a total size of
 * QAPI_DIAG_MSG_MAX_FMTSTR_SIZE characters. If the size
 * exceeds the limit, the message will be dropped.
 *
 * The debug system will enable all modules and set the system debug
 * level to QAPI_DIAG_MSG_LVL_MED on start-up. This is to
 * ensure that some messages (MED and HIGH level messages)
 * get logged even when there is no tool connected at all
 * during one run. The start-up system debug level can be
 * changed at compile time if required. The sSystem debug level
 * can be changed during run-time with CLI commands.
 *
 * @note1hang Use this sparingly to optimize use of buffer space.
 *            This API is not available in Sensor Operating mode or other low-power modes.  
 *
 * @param[in] handle    Handle to the debug buffer in which to store this message. 
 *                      The only allowed value for handle at QAPI level is QAPI_DIAG_MSG_SYSBUF_HDL.
 *                      The message will be logged in the system-wide buffer if level, module_id, etc., are enabled,
 *                      otherwise it will be dropped.
 * @param[in] module_ID Module ID. Module IDs range from 1 to 255 (module Id 0 is reserved for the Diag module).
 *                      Module IDs through 225 are reserved for QTI modules. OEM modules can use 226 through 255.
 *                      Module IDs and corresponding tech areas can be maintained as configuration information
 *                      by the external agent (tool).
 *                      There is no per-processor module_id.
 *                      The message will be logged only if module_id is enabled in the system-wide
 *                      module-bitmask (32-bytes).
 * @param[in] level     Message level. Can be QAPI_DIAG_MSG_LVL_LOW, QAPI_DIAG_MSG_LVL_MED, or QAPI_DIAG_MSG_LVL_HIGH. The message 
 *                      will be logged only if the system debug message level is @ge level. 
 * @param[in] format    printf-style format string. Can contain only %s argument specifiers.
 * @param[in] str_Arg1  String argument to the format string.
 *
 */
#define QAPI_DIAG_MSG_STR_ARG1(handle, module_ID, level, format, str_Arg1) \
   _qapi_Diag_Msg_Str_Write_Arg1((void*)(handle), (module_ID), (level), (format), (str_Arg1))

/*==============================================================================
  FUNCTION      _qapi_Diag_Enable_FlashLogs
==============================================================================*/
/**
 * Enable/Disable debug message logging/routing to Flash at runtime. Enable must
 * be used after erasing the reserved Logging partitions otherwise it fails.
 *
 * @note Logging to Flash is implicitly enabled at boot if Flash is the output
 *       drain channel mentioned in device configuration & the Flash partitions
 *       reserved for logging are already erased.
 *
 * @param[in] isEnable      Enable/Disable debug message logging to Flash.
 *
 * @return
 * true  -- Enable/Disable operation successful \n
 * false -- Enable/Disable operation failed \n
 *
 */
qbool_t _qapi_Diag_Enable_FlashLogs(qbool_t isEnable);

/** @} */ /* end_addtogroup qapi_diag_msg */

#ifdef __cplusplus
}
#endif

#endif //__QAPI_DIAG_MSG_H__
