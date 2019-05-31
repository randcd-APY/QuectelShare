/*!
  @file
  qbi_mem.h

  @brief
  Common memory management functionality including debugging capabilities
*/

/*=============================================================================

  Copyright (c) 2011-2013 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header$

when      who  what, where, why
--------  ---  ---------------------------------------------------------------
05/24/13  bd   Require size of current buffer in QBI_OS_REALLOC macro to
               support Windows OS
06/15/12  bd   Add byte reversal functionality
02/10/12  bd   Add support for Linux
09/02/11  bd   Added module
=============================================================================*/

#ifndef QBI_MEM_H
#define QBI_MEM_H

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_common.h"
#include "qbi_os.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

/*! Switch to toggle logging information about dynamic memory usage. This should
    not be defined in commercial release builds as it adds memory and
    performance overhead to every allocation. */
//#define QBI_MEM_DEBUG

#ifdef FEATURE_LINUX
  #define QBI_MEM_FILE_NAME __FILE__
#else
  #define QBI_MEM_FILE_NAME __MODULE__
#endif /* FEATURE_LINUX */

#ifdef QBI_MEM_DEBUG
  #define QBI_MEM_MALLOC(size) \
    qbi_mem_debug_malloc(size, QBI_MEM_FILE_NAME, __LINE__)
  #define QBI_MEM_FREE(ptr)    qbi_mem_debug_free(ptr)
  #define QBI_MEM_REALLOC(ptr, cur_size, new_size) \
    qbi_mem_debug_realloc(ptr, cur_size, new_size, QBI_MEM_FILE_NAME, __LINE__)

  #define QBI_MEM_MALLOC_CLEAR(size) \
    qbi_mem_debug_malloc_clear(size, QBI_MEM_FILE_NAME, __LINE__)
#else /* QBI_MEM_DEBUG */
  #define QBI_MEM_MALLOC(size) QBI_OS_MALLOC(size)
  #define QBI_MEM_FREE(ptr)    QBI_OS_FREE(ptr)
  #define QBI_MEM_REALLOC(ptr, cur_size, new_size) \
    QBI_OS_REALLOC(ptr, cur_size, new_size)

  #define QBI_MEM_MALLOC_CLEAR(size) qbi_mem_malloc_clear(size)
#endif /* QBI_MEM_DEBUG */

/* Convert a 32-bit integer between host byte order (little endian) and network
   byte order (big endian) */
#define QBI_MEM_HTONL(x)                 \
  ((((uint32)(x) & 0x000000FFU) << 24) | \
   (((uint32)(x) & 0x0000FF00U) <<  8) | \
   (((uint32)(x) & 0x00FF0000U) >>  8) | \
   (((uint32)(x) & 0xFF000000U) >> 24))

#define QBI_MEM_NTOHL(x) QBI_MEM_HTONL(x)

/* Convert a 16-bit integer between host byte order (little endian) and network
   byte order (big endian) */
#define QBI_MEM_HTONS(x) \
  ((((uint16)(x) & 0x00FF) << 8) | \
   (((uint16)(x) & 0xFF00) >> 8))

#define QBI_MEM_NTOHS(x) QBI_MEM_HTONS(x)

/*=============================================================================

  Typedefs

=============================================================================*/

/*=============================================================================

  Function Prototypes

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_mem_init
===========================================================================*/
/*!
    @brief Performs common memory management layer initialization

    @details
    Initializes memory debug functionality if compiled in.
*/
/*=========================================================================*/
void qbi_mem_init
(
  void
);

#ifndef QBI_MEM_DEBUG
/*===========================================================================
  FUNCTION: qbi_mem_malloc_clear
===========================================================================*/
/*!
    @note DON'T CALL THIS FUNCTION DIRECTLY! Use the macro
    QBI_MEM_MALLOC_CLEAR

    @brief Allocate and zero out memory

    @details
    Equivalent to using calloc for a single element of the given size. Also
    logs an error message on allocation failures.

    @param size Size of memory to allocate in bytes

    @return void* Pointer to newly allocated memory, or NULL on failure
*/
/*=========================================================================*/
void *qbi_mem_malloc_clear
(
  uint32 size
);
#endif /* !QBI_MEM_DEBUG */

#ifdef QBI_MEM_DEBUG
/*===========================================================================
  FUNCTION: qbi_mem_debug_free
===========================================================================*/
/*!
    @note DON'T CALL THIS FUNCTION DIRECTLY! Use the macro QBI_MEM_FREE

    @brief Wrapper to free a memory allocation and clear debug info

    @details

    @param ptr
*/
/*=========================================================================*/
void qbi_mem_debug_free
(
  const void *ptr
);

/*===========================================================================
  FUNCTION: qbi_mem_debug_malloc
===========================================================================*/
/*!
    @note DON'T CALL THIS FUNCTION DIRECTLY! Use the macro QBI_MEM_MALLOC

    @brief Wrapper to allocate memory and save debug information

    @details

    @param size
    @param file
    @param line

    @return void*
*/
/*=========================================================================*/
void *qbi_mem_debug_malloc
(
  uint32      size,
  const char *file,
  uint32      line
);

/*===========================================================================
  FUNCTION: qbi_mem_debug_malloc_clear
===========================================================================*/
/*!
    @note DON'T CALL THIS FUNCTION DIRECTLY! Use the macro
    QBI_MEM_MALLOC_CLEAR

    @brief Allocate and zero out memory

    @details
    Equivalent to using calloc for a single element of the given size. Also
    logs an error message on allocation failures.

    @param size Size of memory to allocate in bytes
    @param file File where allocation was requested
    @param line Line number where allocation was requested

    @return void* Pointer to newly allocated memory, or NULL on failure
*/
/*=========================================================================*/
void *qbi_mem_debug_malloc_clear
(
  uint32      size,
  const char *file,
  uint32      line
);

/*===========================================================================
  FUNCTION: qbi_mem_debug_realloc
===========================================================================*/
/*!
    @note DON'T CALL THIS FUNCTION DIRECTLY! Use the macro QBI_MEM_REALLOC

    @brief Wrapper to re-allocate memory and update debug information

    @details

    @param ptr
    @param cur_size
    @param new_size
    @param file
    @param line

    @return void*
*/
/*=========================================================================*/
void *qbi_mem_debug_realloc
(
  const void *ptr,
  uint32      cur_size,
  uint32      new_size,
  const char *file,
  uint32      line
);
#endif /* QBI_MEM_DEBUG */

/*===========================================================================
  FUNCTION: qbi_mem_reverse_bytes
===========================================================================*/
/*!
    @brief Copy a buffer from one location to another, reversing the byte
    order along the way

    @details
    Can specify src_buf and dst_buf to the same location to perform an
    in-place byte swap.

    @param src_buf Pointer to source buffer
    @param dst_buf Pointer to destination buffer which will be filled with
    the contents of src_buf in reverse byte order. Can be equal to src_buf
    to perform an in-place swap.
    @param size Size of src_buf in bytes. To prevent overflow, dst_buf must
    be at least this size as well.
*/
/*=========================================================================*/
void qbi_mem_reverse_bytes
(
  const uint8 *src_buf,
  uint8       *dst_buf,
  uint32       size
);

#endif /* QBI_MEM_H */

