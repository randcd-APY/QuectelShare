#ifndef __QAPI_FATAL_ERR_H__ 
#define __QAPI_FATAL_ERR_H__

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*=================================================================================
 *
 *                            FATAL ERROR MANAGER
 *
 *===============================================================================*/
 /** @file qapi_fatal_err.h
 *
 * @addtogroup qapi_fatal_err
 * @{
 *
 * @brief Fatal Error Manager (FEM)
 *
 * @details Complex software systems often run into unrecoverable error
 *          scenarios. These fatal errors cause the system to
 *          abruptly abort execution, since there is no recovery path. By
 *          nature, fatal errors are difficult to debug because detailed
 *          information related to the error is not preserved. The fatal
 *          error manager (FEM) service provides its clients a way to handle
 *          unrecoverable errors in a graceful debug-friendly fashion. It
 *          exposes a macro which, when called after a catastrophic error,
 *          preserves pertinent information to aid in debug before resetting
 *          the system.
 *
 * @code {.c}
 *
 *    * The code snippet below demonstrates the use of this interface. The example
 *    * dynamically allocates a region of memory, failing in which it 
 *    * asserts the code. This macro populates the debug information in a global
 *    * variable 'coredump' with line number, file name, and user parameters.
 *    * It also dumps the contents of general purpose registers and invokes
 *    * various user callbacks before resetting the system. The header file 
 *    * qapi_fatal_err.h should be included before calling the macro.
 *
 *   char * c;
 *
 *   c = malloc(sizeof(char));
 *   if ( c == NULL )
 *   {
 *     QAPI_FATAL_ERR(0,0,0);
 *   }
 *
 * @endcode
 *
 * @}
 */

/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/v2/rom/release/api/debugtools/err/qapi_fatal_err.h#10 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
10/30/15   din     Updated documentation.
09/29/15   din     Initial version.
==================================================================================*/

/*==================================================================================

                               INCLUDE FILES

==================================================================================*/

#include "qapi/qapi_types.h"

/*==================================================================================

                                   MACROS

==================================================================================*/
#if !defined(_PACKED_START)
    #if (defined(__CC_ARM) || defined(__GNUC__))
        /* ARM and GCC compilers */
        #define _PACKED_START
    #elif (defined(__ICCARM__))
    /* IAR compiler */
        #define _PACKED_START       __packed
    #else
        #define _PACKED_START
    #endif
#endif

#if !defined(_PACKED_END)
    #if (defined(__CC_ARM) || defined(__GNUC__))
        /* ARM and GCC compilers */
        #define _PACKED_END  __attribute__ ((packed))
    #elif (defined(__ICCARM__))
        /* IAR compiler */
        #define _PACKED_END
    #else
        #define _PACKED_END
    #endif
#endif

/**
 * Coredump specific macros.
 */
/**< Task Control Block Type. */
#define QAPI_ERR_OS_TCB_TYPE            void
/**< Maximum string length for the caused error fatal. */
#define QAPI_ERR_LOG_MAX_MSG_LEN        80
/**< Maximum filename length that causes error fatal. */
#define QAPI_ERR_LOG_MAX_FILE_LEN       50
/**< Maximum parameters passed for caused error fatal. */
#define QAPI_ERR_LOG_NUM_PARAMS         3
/**< Maximum length for binary image build version string. */
#define QAPI_ERR_IMAGE_VERSION_SIZE     128

/**
 * Fatal error specific macros.
 */
/**< Maximum corename length which results in error fatal.
     e.g. "M4: " or "M0: ". */
#define QAPI_ERR_INFO_MAX_CORENAME_LEN  4
/**< Maximum error string length that comprises of corename followed by
     filename that causes error fatal. Error string format is as follows:
        <char string       >
        <Corename: Filename>
  e.g.  <M0: abc.c> */
#define QAPI_ERR_INFO_MAX_ERR_STR_LEN   (QAPI_ERR_INFO_MAX_CORENAME_LEN +\
                                            QAPI_ERR_LOG_MAX_FILE_LEN)

/*==================================================================================

                               TYPE DEFINITIONS

==================================================================================*/

/** @addtogroup qapi_fatal_err
@{ */

/**
* Debug information structure.
*
* This structure is used to capture the module name and line number in the 
* source file where a fatal error was detected. Reference to an instance of
* this structure is passed as a parameter to the qapi_err_fatal_internal()
*          function. 
*/
typedef _PACKED_START struct
{
  uint16_t     line;
  /**< Line number in the source module. */

  const char  *fname;
  /**< Pointer to the source file name. */ 
} _PACKED_END qapi_Err_const_t;


/**
* Debug information structure.
*
* This type is used for post crash callbacks from the error handler. A reference to an 
* instance of this type is passed as a parameter to the qapi_err_crash_cb_reg()
* and qapi_err_crash_cb_dereg() functions. 
*/
typedef void (*qapi_err_cb_ptr)(void);

/**
 * Cortex-M4 general purpose registers enums.
 */
typedef enum _qapi_arch_m4_coredump_register_type
{
    ARM_M4_R0 = 0,
    ARM_M4_R1,
    ARM_M4_R2,
    ARM_M4_R3,
    ARM_M4_R4,
    ARM_M4_R5,
    ARM_M4_R6,
    ARM_M4_R7,
    ARM_M4_R8,
    ARM_M4_R9,
    ARM_M4_R10,
    ARM_M4_R11,
    ARM_M4_R12,
    ARM_M4_SP,
    ARM_M4_LR,
    ARM_M4_PC,
    ARM_M4_PSP,
    ARM_M4_MSP,
    ARM_M4_PSR,
    ARM_M4_APSR,
    ARM_M4_IPSR,
    ARM_M4_EPSR,
    ARM_M4_PRIMASK,
    ARM_M4_FAULTMASK,
    ARM_M4_BASEPRI,
    ARM_M4_CONTROL,
    ARM_M4_EXCEPTION_R0,
    ARM_M4_EXCEPTION_R1,
    ARM_M4_EXCEPTION_R2,
    ARM_M4_EXCEPTION_R3,
    ARM_M4_EXCEPTION_R12,
    ARM_M4_EXCEPTION_LR,
    ARM_M4_EXCEPTION_PC,
    ARM_M4_EXCEPTION_XPSR,
    SIZEOF_ARCH_M4_COREDUMP_REGISTERS
} qapi_arch_m4_coredump_register_type;

/**
 * Cortex-M0 general purpose registers enums.
 */
typedef enum _qapi_arch_m0_coredump_register_type
{
    ARM_M0_R0 = 0,
    ARM_M0_R1,
    ARM_M0_R2,
    ARM_M0_R3,
    ARM_M0_R4,
    ARM_M0_R5,
    ARM_M0_R6,
    ARM_M0_R7,
    ARM_M0_R8,
    ARM_M0_R9,
    ARM_M0_R10,
    ARM_M0_R11,
    ARM_M0_R12,
    ARM_M0_SP,
    ARM_M0_LR,
    ARM_M0_PC,
    ARM_M0_PSP,
    ARM_M0_MSP,
    ARM_M0_PSR,
    ARM_M0_PRIMASK,
    ARM_M0_CONTROL,
    ARM_M0_EXCEPTION_R0,
    ARM_M0_EXCEPTION_R1,
    ARM_M0_EXCEPTION_R2,
    ARM_M0_EXCEPTION_R3,
    ARM_M0_EXCEPTION_R12,
    ARM_M0_EXCEPTION_LR,
    ARM_M0_EXCEPTION_PC,
    ARM_M0_EXCEPTION_XPSR,
    SIZEOF_ARCH_M0_COREDUMP_REGISTERS
} qapi_arch_m0_coredump_register_type;

/**
 * Cortex-M4 general purpose registers struture.
 */
typedef struct _qapi_arch_m4_coredump_field_type
{
    uint32_t regs[13];
    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    uint32_t psp;
    uint32_t msp;
    uint32_t psr;
    uint32_t aspr;
    uint32_t ipsr;
    uint32_t epsr;
    uint32_t primask; 
    uint32_t faultmask;
    uint32_t basepri;  
    uint32_t control;
    uint32_t exception_r0;
    uint32_t exception_r1;
    uint32_t exception_r2;
    uint32_t exception_r3;
    uint32_t exception_r12;
    uint32_t exception_lr;
    uint32_t exception_pc;
    uint32_t exception_xpsr;
} qapi_arch_m4_coredump_field_type;

/**
 * Cortex-M0 general purpose registers struture.
 */
typedef struct _qapi_arch_m0_coredump_field_type
{
    uint32_t regs[13];
    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    uint32_t psp;
    uint32_t msp;
    uint32_t psr;
    uint32_t primask; 
    uint32_t control;
    uint32_t exception_r0;
    uint32_t exception_r1;
    uint32_t exception_r2;
    uint32_t exception_r3;
    uint32_t exception_r12;
    uint32_t exception_lr;
    uint32_t exception_pc;
    uint32_t exception_xpsr;
    /**< ARM_APSR, ARM_IPSR, ARM_EPSR not applicable for Cortex-M0 but wrongly
         defined in internally. These need to be masked out by these reserved
         fields. */
    uint32_t reserved[3];
} qapi_arch_m0_coredump_field_type;

/**
 * Union of Cortex-M4 general purpose registers enums and struture.
 */
typedef union _qapi_arch_m4_coredump_union
{
    uint32_t                           array[SIZEOF_ARCH_M4_COREDUMP_REGISTERS];
    qapi_arch_m4_coredump_field_type   name;
} qapi_arch_m4_coredump_union;

/**
 * Union of Cortex-M0 general purpose registers enums and struture.
 */
typedef union _qapi_arch_m0_coredump_union
{
    uint32_t                           array[SIZEOF_ARCH_M0_COREDUMP_REGISTERS];
    qapi_arch_m0_coredump_field_type   name;
} qapi_arch_m0_coredump_union;

/**
 * Architecture type.
 */
typedef enum _qapi_err_arch_type
{
    QAPI_ERR_ARCH_UNKNOWN = 0,
    QAPI_ERR_ARCH_ARM,
    QAPI_ERR_ARCH_QDSP6,
    QAPI_SIZEOF_ERR_ARCH_TYPE
} qapi_err_arch_type;

/**
 * Operating System type.
 */
typedef enum _qapi_err_os_type
{
    QAPI_ERR_OS_UNKNOWN = 0,
    QAPI_ERR_OS_REX,
    QAPI_ERR_OS_L4, 
    QAPI_ERR_OS_BLAST, 
    QAPI_ERR_OS_QURT,
    QAPI_ERR_OS_ARMVS, 
    QAPI_SIZEOF_ERR_OS_TYPE
} qapi_err_os_type;

/**
 * Architecture specific coredump sub-structure.
 * Cortex-M4
 */
typedef struct _qapi_arch_m4_coredump_type
{
    qapi_err_arch_type              type;
    uint32_t                        version;
    qapi_arch_m4_coredump_union     regs;
} qapi_arch_m4_coredump_type;

/**
 * Architecture specific coredump sub-structure.
 * Cortex-M0
 */
typedef struct _qapi_arch_m0_coredump_type
{
    qapi_err_arch_type              type;
    uint32_t                        version;
    qapi_arch_m0_coredump_union     regs;
} qapi_arch_m0_coredump_type;

/**
 * Operating system specific coredump sub-structure.
 */
typedef struct _qapi_os_coredump_type
{
  qapi_err_os_type          type;
  uint32_t                  version;
  QAPI_ERR_OS_TCB_TYPE      *tcb_ptr;
} qapi_os_coredump_type;

/**
 * Binary Image specific coredump sub-structure.
 */
typedef struct _qapi_image_coredump_type
{
    char *qc_image_version_string;
    char *image_variant_string;
} qapi_image_coredump_type;

/**
 * Fatal error/panic specific coredump sub-structure.
 */
typedef struct
{
    uint32_t                  version;
    uint32_t                  linenum;
    uint64_t                  err_handler_start_time;
    uint64_t                  err_handler_end_time;
    char                      filename[QAPI_ERR_LOG_MAX_FILE_LEN];
    char                      message[QAPI_ERR_LOG_MAX_MSG_LEN];
    uint32_t                  param[QAPI_ERR_LOG_NUM_PARAMS];
    qapi_err_cb_ptr           err_current_cb;
    const qapi_Err_const_t    *compressed_ptr;
    uint8_t                   err_reentrancy;
} qapi_err_coredump_type;

/**
 * Overall coredump structure capturing verbose information corresponding to
 * fatal error/panic/exceptions.
 * Cortex-M4
 *
 * @note:
 *      When any core panics, it indicates to the other peripherals including
 *      the other core.
 *      Minimal fatal error information is stored in AON regions that can be
 *      retreived using qapi_get_err_info (Refer: qapi_fatal_err_ext.h for
 *      format and usage). On software cold reset after a fatal exception and
 *      regardless of ramdump mode being enabled, the AON stored error
 *      information corresponds to the core that first panic'd or error faulted.
 */
typedef struct
{
    /**< Coredump version. */
    uint32_t                      version;
    /**< Architecture information. */
    qapi_arch_m4_coredump_type    arch;
    /**< Operating system information. */
    qapi_os_coredump_type         os;
    /**< Fatal error information. */
    qapi_err_coredump_type        err;
    /**< Build version information (if any). Currently not available. */
    qapi_image_coredump_type      image;
} qapi_m4_coredump_type;

/**
 * Overall coredump structure capturing verbose information corresponding to
 * fatal error/panic/exceptions.
 * Cortex-M0
 *
 * @note:
 *      When any core panics, it indicates to the other peripherals including
 *      the other core.
 *      Minimal fatal error information is stored in AON regions that can be
 *      retreived using qapi_get_err_info (Refer: qapi_fatal_err_ext.h for
 *      format and usage). On software cold reset after a fatal exception and
 *      regardless of ramdump mode being enabled, the AON stored error
 *      information corresponds to the core that first panic'd or error faulted.
 */
typedef struct
{
    /**< Coredump version. */
    uint32_t                      version;
    /**< Architecture information. */
    qapi_arch_m0_coredump_type    arch;
    /**< Operating system information. */
    qapi_os_coredump_type         os;
    /**< Fatal error information. */
    qapi_err_coredump_type        err;
    /**< Build version information (if any). Currently not available. */
    qapi_image_coredump_type      image;
} qapi_m0_coredump_type;

/** @} */ /* end_addtogroup qapi_fatal_err */

/*==================================================================================

                            FUNCTION DECLARATIONS

==================================================================================*/

/*==================================================================================
  FUNCTION      qapi_err_fatal_internal
==================================================================================*/

/** @addtogroup qapi_fatal_err
@{ */

/**
 * Fatal error handler.
 *
 * This function implements back-end functionality supported by macro
 * QAPI_FATAL_ERR. It preserves debug information at a well-known location
 * (typically a global variable "coredump"). Preserved information captures
 * the source module name and line number, user-provided values, and contents
 *          of general purpose registers for underlying CPU architecture. After
 *          invoking several notification callbacks, it resets the system.
 *
 * @param[in] err_const Reference to the structure record line number and module name.
 * @param[in] param1    Client-provided parameter saved with debug information.
 * @param[in] param2    Client-provided parameter saved with debug information.
 * @param[in] param3    Client-provided parameter saved with debug information.
 *
 * @note1hang This function does not return. It should only be used to gracefully
 *       handle unrecoverable errors and restart the system. Clients should 
 *       not call the function directly. Instead, they should use the macro
 *       QAPI_FATAL_ERR to access the functionality to ensure that all
 *       relevant debug information is carried forward.
 */
void qapi_err_fatal_internal
(
  const qapi_Err_const_t * err_const, 
  uint32_t                 param1, 
  uint32_t                 param2, 
  uint32_t                 param3 
);

/** @} */ /* end_addtogroup qapi_fatal_err */ 

/*==================================================================================
  MACRO         QAPI_FATAL_ERR
==================================================================================*/

/** @addtogroup qapi_fatal_err
@{ */

/**
 * Fatal error handler macro.
 *
 * This function allows for graceful handling of fatal errors. It
 * preserves information related to fatal crashes at a well-known location
 * (typically a global variable "coredump"). Preserved information captures
 * the source module name and line number, user-provided values, and contents
 * of general purpose registers used by the underlying CPU architecture.
 * After invoking several notification callbacks, it resets the system. @newpage
 *
 * @param[in] param1   User-provided parameter to be logged in coredump.
 * @param[in] param2   User-provided parameter to be logged in coredump.
 * @param[in] param3   User-provided parameter to be logged in coredump.
 *
 * @note1hang This macro does not return. It should only be used to gracefully
 *       handle unrecoverable errors and restart the system.
 @hideinitializer */
#define QAPI_FATAL_ERR(param1,param2,param3)                             \
do                                                                       \
{                                                                        \
   static const qapi_Err_const_t xx_err_const = {__LINE__, __FILENAME__};\
   qapi_err_fatal_internal(&xx_err_const, param1,param2,param3);         \
}while (0)

/** @} */ /* end_addtogroup qapi_fatal_err */

/*==================================================================================
  FUNCTION      qapi_err_crash_cb_reg
==================================================================================*/

/** @addtogroup qapi_fatal_err
@{ */

/**
 * Registers a callback with the fatal crash handler.
 *
 * This function can be used to 
 * save client-specific information in the ramdumps and carry out some clean up 
 * during fatal error handling.
 *
 * @note1hang The callback must not call ERR_FATAL under any circumstance. This callback should not 
 * rely on any messaging, task switching (or system calls that may invoke task switching), 
 * interrupts, or any blocking opertaions. 
 * 
 * @param[in] cb  Client callback to be invoked during fatal crash handling.
 *
 * @return TRUE if the callback is added, false otherwise.
 */
qbool_t qapi_err_crash_cb_reg
(
  qapi_err_cb_ptr          cb 
);


/*==================================================================================
  FUNCTION      qapi_err_crash_cb_dereg
==================================================================================*/

/**
 * Deregisters a callback with the fatal crash handler.
 *
 * @param[in] cb  Client callback to be deregistered.
 *
 * @return TRUE if callback is removed, false otherwise.
 *
 */
qbool_t qapi_err_crash_cb_dereg
(
  qapi_err_cb_ptr          cb 
);

/** @} */ /* end_addtogroup qapi_fatal_err */ 

#endif /* __QAPI_FATAL_ERR_H__ */
