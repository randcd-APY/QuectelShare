/*!
  @file
  qbi_mem.c

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
               support Windows OS, set sentry in qbi_mem_debug_realloc()
06/15/12  bd   Add byte reversal functionality
02/15/12  bd   Add buffer overflow detection
10/28/11  bd   Increased number of tracked allocations from 256 to 1024
09/02/11  bd   Added module
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_mem.h"

#include "qbi_common.h"
#include "qbi_os.h"
#include "qbi_log.h"
#include "qbi_task.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

#define QBI_MEM_DEBUG_MAX_TRACKED_ALLOCATIONS (1024)

/*=============================================================================

  Private Typedefs

=============================================================================*/

#ifdef QBI_MEM_DEBUG
/*! This structure is prepended to every memory allocation as header
    information to track the source of the allocation. */
typedef struct {
  const char      *file;
  uint32           line;
  int32            size;
  qbi_os_time_ms_t alloc_time;
} qbi_mem_debug_info_s;
#endif /* QBI_MEM_DEBUG*/

/*=============================================================================

  Private Variables

=============================================================================*/

#ifdef QBI_MEM_DEBUG
static struct {
  /*! Informative counters for the number of allocations (current and maximum) */
  int32 cur_allocations;
  int32 max_allocations;

  /*! Informative counters for the amount of dynamic memory usage in bytes */
  int32 cur_mem_usage;
  int32 max_mem_usage;

  /*! Mutex protecting the 4 statistic variables above */
  qbi_os_mutex_t stats_mutex;

  /*! Array holding pointers to debug information for all current allocations.
      As the number of concurrent allocations is typically low, this array is
      filled via linear search for the first open index. */
  qbi_mem_debug_info_s *alloc_info[QBI_MEM_DEBUG_MAX_TRACKED_ALLOCATIONS];

  /*! Mutex used to synchronize write access to alloc_info */
  qbi_os_mutex_t alloc_info_mutex;

  /*! Timer used to print allocation statistics at regular intervals */
  qbi_os_timer_t log_timer;
} qbi_mem_debug;

/*! Random bytes of data to use as sentry at end of every buffer to help detect
    potential buffer overflow issues */
static const uint8 qbi_mem_debug_sentry[] = {
  0x79, 0x4b, 0x06, 0x0c, 0xf2, 0x46, 0xf5, 0x7e
};
#endif /* QBI_MEM_DEBUG */

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

#ifdef QBI_MEM_DEBUG
static void qbi_mem_debug_add_entry
(
  qbi_mem_debug_info_s *debug_info
);

static void qbi_mem_debug_init
(
  void
);

static void qbi_mem_debug_print_stats
(
  qbi_ctx_s        *ctx_ignored,
  qbi_task_cmd_id_e cmd_id_ignored,
  void             *data_ignored
);

static void qbi_mem_debug_remove_entry
(
  const qbi_mem_debug_info_s *debug_info
);

static void qbi_mem_debug_timer_cb
(
  void *ignored
);
#endif /* QBI_MEM_DEBUG */

/*=============================================================================

  Private Function Definitions

=============================================================================*/

#ifdef QBI_MEM_DEBUG
/*===========================================================================
  FUNCTION: qbi_mem_debug_add_entry
===========================================================================*/
/*!
    @brief Saves a reference to the debug info for a new allocation, and
    updates memory statistics (number of allocations, memory in use)

    @details

    @param debug_info
*/
/*=========================================================================*/
static void qbi_mem_debug_add_entry
(
  qbi_mem_debug_info_s *debug_info
)
{
  uint32 i;
/*-------------------------------------------------------------------------*/
  for (i = 0; i < QBI_MEM_DEBUG_MAX_TRACKED_ALLOCATIONS; i++)
  {
    if (qbi_mem_debug.alloc_info[i] == NULL)
    {
      /* The situation we are trying to avoid is when two threads pick the same
         index to store their pointer, meaning only the last one will 'win'.
         Since we don't want to hold the mutex while looping through the entire
         array, we only use the mutex to guard write operations. So we need to
         double-check that this location is still NULL after locking. */
      qbi_os_mutex_lock(&qbi_mem_debug.alloc_info_mutex);
      if (qbi_mem_debug.alloc_info[i] == NULL)
      {
        qbi_mem_debug.alloc_info[i] = debug_info;
        qbi_os_mutex_unlock(&qbi_mem_debug.alloc_info_mutex);
        break;
      }
      else
      {
        qbi_os_mutex_unlock(&qbi_mem_debug.alloc_info_mutex);
        /* If we are here, it means another thread grabbed the same slot between
           the time when we saw it was NULL, and were able to acquire the mutex.
           Crisis averted! Just continue the search as usual to find the next
           open index. */
      }
    }
  }

  if (i >= QBI_MEM_DEBUG_MAX_TRACKED_ALLOCATIONS)
  {
    QBI_LOG_E_2("Couldn't add memory debug entry for %p: %d current items",
                debug_info, qbi_mem_debug.cur_allocations);
  }

  qbi_os_mutex_lock(&qbi_mem_debug.stats_mutex);
  qbi_mem_debug.cur_allocations++;
  if (qbi_mem_debug.cur_allocations > qbi_mem_debug.max_allocations)
  {
    qbi_mem_debug.max_allocations = qbi_mem_debug.cur_allocations;
  }

  qbi_mem_debug.cur_mem_usage += debug_info->size;
  if (qbi_mem_debug.cur_mem_usage > qbi_mem_debug.max_mem_usage)
  {
    qbi_mem_debug.max_mem_usage = qbi_mem_debug.cur_mem_usage;
  }
  qbi_os_mutex_unlock(&qbi_mem_debug.stats_mutex);
} /* qbi_mem_debug_add_entry() */

/*===========================================================================
  FUNCTION: qbi_mem_debug_init
===========================================================================*/
/*!
    @brief Initializes variables used for memory debug

    @details
*/
/*=========================================================================*/
static void qbi_mem_debug_init
(
  void
)
{
/*-------------------------------------------------------------------------*/
  QBI_MEMSET(&qbi_mem_debug, 0, sizeof(qbi_mem_debug));
  qbi_os_mutex_init(&qbi_mem_debug.alloc_info_mutex);
  qbi_os_mutex_init(&qbi_mem_debug.stats_mutex);
  qbi_os_timer_init(&qbi_mem_debug.log_timer, qbi_mem_debug_timer_cb,
                    NULL);
  qbi_os_timer_set(&qbi_mem_debug.log_timer, 120000);
} /* qbi_mem_debug_init() */

/*===========================================================================
  FUNCTION: qbi_mem_debug_print_stats
===========================================================================*/
/*!
    @brief Timer callback that prints memory statistics to the log

    @details

    @param ctx_ignored
    @param cmd_id_ignored
    @param data_ignored
*/
/*=========================================================================*/
static void qbi_mem_debug_print_stats
(
  qbi_ctx_s        *ctx_ignored,
  qbi_task_cmd_id_e cmd_id_ignored,
  void             *data_ignored
)
{
  uint32 i;
  uint32 total_mem = 0;
  qbi_mem_debug_info_s *debug_info;
  qbi_os_time_ms_t cur_time;
  uint32 num_tracked = 0;
  int32 cur_mem_usage;
  int32 max_mem_usage;
  int32 cur_allocations;
  int32 max_allocations;
/*-------------------------------------------------------------------------*/
  cur_time = qbi_os_time_get_ms();

  /* Make a local copy of the stats so we only hold the mutex for a short
     period of time */
  qbi_os_mutex_lock(&qbi_mem_debug.stats_mutex);
  cur_mem_usage = qbi_mem_debug.cur_mem_usage;
  max_mem_usage = qbi_mem_debug.max_mem_usage;
  cur_allocations = qbi_mem_debug.cur_allocations;
  max_allocations = qbi_mem_debug.max_allocations;
  qbi_os_mutex_unlock(&qbi_mem_debug.stats_mutex);

  QBI_LOG_I_2("Total memory currently in use: %d bytes (peak %d bytes)",
              cur_mem_usage, max_mem_usage);
  QBI_LOG_I_2("There are %d active allocations (peak %d). Printing info for "
              "allocations lasting longer than 90 seconds",
              cur_allocations, max_allocations);
  for (i = 0; i < QBI_MEM_DEBUG_MAX_TRACKED_ALLOCATIONS; i++)
  {
    /* While allocations can happen from a different thread (for example,
       qbi_task_post_cmd()), currently QBI_MEM_FREE() is only executed outside the
       QBI task in the event of a critical failure. So no mutex is needed here
       to protect against debug_info being freed while we are accessing it */
    debug_info = qbi_mem_debug.alloc_info[i];
    if (debug_info != NULL)
    {
      if ((cur_time - debug_info->alloc_time) >= 90000)
      {
        QBI_LOG_STR_6(" %d. Addr %p size %d bytes; allocated %d seconds ago "
                      "from %s:%d", i,
                      ((uint8 *) debug_info + sizeof(qbi_mem_debug_info_s)),
                      debug_info->size,
                      ((cur_time - debug_info->alloc_time) / 1000),
                      debug_info->file, debug_info->line);
      }
      total_mem += debug_info->size;
      num_tracked++;
    }
  }
  QBI_LOG_I_2("Total tracked dynamic memory used: %d bytes in %d allocations",
              total_mem, num_tracked);
  qbi_os_timer_set(&qbi_mem_debug.log_timer, 30000);
} /* qbi_mem_debug_print_stats() */

/*===========================================================================
  FUNCTION: qbi_mem_debug_remove_entry
===========================================================================*/
/*!
    @brief Removes a reference to debug information for a memory allocation
    that is about to be freed

    @details

    @param debug_info
*/
/*=========================================================================*/
static void qbi_mem_debug_remove_entry
(
  const qbi_mem_debug_info_s *debug_info
)
{
  uint32 i;
/*-------------------------------------------------------------------------*/
  for (i = 0; i < QBI_MEM_DEBUG_MAX_TRACKED_ALLOCATIONS; i++)
  {
    if (qbi_mem_debug.alloc_info[i] == debug_info)
    {
      /* There is no need to use the mutex on this operation, since access to a
         dynamic buffer should already be protected by a mutex if necessary.
         So access to the index in alloc_info for this memory pointer therefore
         must already be synchronized. */
      qbi_mem_debug.alloc_info[i] = NULL;
      break;
    }
  }

  if (i >= QBI_MEM_DEBUG_MAX_TRACKED_ALLOCATIONS)
  {
    QBI_LOG_E_2("Couldn't remove memory debug entry for %p: %d current items",
                debug_info, qbi_mem_debug.cur_allocations);
  }

  qbi_os_mutex_lock(&qbi_mem_debug.stats_mutex);
  qbi_mem_debug.cur_allocations--;
  qbi_mem_debug.cur_mem_usage -= debug_info->size;
  qbi_os_mutex_unlock(&qbi_mem_debug.stats_mutex);
} /* qbi_mem_debug_remove_entry() */

/*===========================================================================
  FUNCTION: qbi_mem_debug_timer_cb
===========================================================================*/
/*!
    @brief Timer callback used to print memory debug stats at a regular
    interval

    @details
    Posts a command to the QBI task to print the stats

    @param unused
*/
/*=========================================================================*/
static void qbi_mem_debug_timer_cb
(
  void *ignored
)
{
/*-------------------------------------------------------------------------*/
  if (!qbi_task_cmd_send(
        NULL, QBI_TASK_CMD_ID_MEM_DBG, qbi_mem_debug_print_stats, NULL))
  {
    QBI_LOG_E_0("Couldn't post command to QBI task!");
  }
} /* qbi_mem_debug_timer_cb() */
#endif /* QBI_MEM_DEBUG */

/*=============================================================================

  Public Function Definitions

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
)
{
/*-------------------------------------------------------------------------*/
  #ifdef QBI_MEM_DEBUG
  qbi_mem_debug_init();
  #endif
} /* qbi_mem_init() */

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
)
{
  void *data;
/*-------------------------------------------------------------------------*/
  data = QBI_MEM_MALLOC(size);
  if (data == NULL)
  {
    QBI_LOG_E_1("Memory allocation failure! Tried to allocate %d bytes", size);
  }
  else
  {
    QBI_MEMSET(data, 0, size);
  }

  return data;
} /* qbi_mem_malloc_clear() */
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
)
{
  uint8 *sentry;
  qbi_mem_debug_info_s *debug_info;
/*-------------------------------------------------------------------------*/
  if (ptr == NULL || (uint32) ptr < sizeof(qbi_mem_debug_info_s))
  {
    QBI_LOG_E_0("Invalid pointer passed to free()!!!");
  }
  else
  {
    debug_info = (qbi_mem_debug_info_s *)
      ((uint8 *) ptr - sizeof(qbi_mem_debug_info_s));
    sentry = (uint8 *) ((uint8 *) ptr + debug_info->size);
    if (QBI_MEMCMP(
          sentry, qbi_mem_debug_sentry, sizeof(qbi_mem_debug_sentry)) != 0)
    {
      QBI_LOG_E_0("Buffer overflow detected in heap!!!");
      QBI_LOG_E_8("Sentry value is 0x%02x%02x%02x%02x %02x%02x%02x%02x",
                  sentry[0], sentry[1], sentry[2], sentry[3],
                  sentry[4], sentry[5], sentry[6], sentry[7]);
      QBI_LOG_E_8("Expected sentry is 0x%02x%02x%02x%02x %02x%02x%02x%02x",
                  qbi_mem_debug_sentry[0], qbi_mem_debug_sentry[1],
                  qbi_mem_debug_sentry[2], qbi_mem_debug_sentry[3],
                  qbi_mem_debug_sentry[4], qbi_mem_debug_sentry[5],
                  qbi_mem_debug_sentry[6], qbi_mem_debug_sentry[7]);
      QBI_LOG_STR_4("Memory allocated from %s:%d size %d bytes (%d ms ago)",
                    debug_info->file, debug_info->line, debug_info->size,
                    qbi_os_time_diff(qbi_os_time_get_ms(),
                                     debug_info->alloc_time));
    }
    qbi_mem_debug_remove_entry(debug_info);
    QBI_MEMSET(sentry, 0, sizeof(qbi_mem_debug_sentry));

    QBI_OS_FREE(debug_info);
  }
} /* qbi_mem_debug_free() */

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
)
{
  uint8 *ptr;
  uint8 *sentry;
  qbi_mem_debug_info_s *debug_info;
  int32 cur_mem_usage;
  int32 max_mem_usage;
  int32 cur_allocations;
  int32 max_allocations;
/*-------------------------------------------------------------------------*/
  if (size == 0)
  {
    QBI_LOG_W_0("Requested memory allocation of 0 bytes!");
    QBI_LOG_STR_2("Allocation tried from %s:%d", file, line);
    ptr = NULL;
  }
  else
  {
    ptr = QBI_OS_MALLOC(size + sizeof(qbi_mem_debug_info_s) +
                        sizeof(qbi_mem_debug_sentry));
    if (ptr == NULL)
    {
      QBI_LOG_E_1("Memory allocation failure. Tried to allocate %d bytes", size);
      QBI_LOG_STR_2("Allocation tried from %s:%d", file, line);

      qbi_os_mutex_lock(&qbi_mem_debug.stats_mutex);
      cur_mem_usage = qbi_mem_debug.cur_mem_usage;
      max_mem_usage = qbi_mem_debug.max_mem_usage;
      cur_allocations = qbi_mem_debug.cur_allocations;
      max_allocations = qbi_mem_debug.max_allocations;
      qbi_os_mutex_unlock(&qbi_mem_debug.stats_mutex);

      QBI_LOG_E_2("Total memory currently in use: %d bytes (peak %d bytes)",
                  cur_mem_usage, max_mem_usage);
      QBI_LOG_E_2("There are %d active allocations (peak %d)",
                  cur_allocations, max_allocations);
    }
    else
    {
      debug_info = (qbi_mem_debug_info_s *) ptr;
      ptr += sizeof(qbi_mem_debug_info_s);
      debug_info->file = file;
      debug_info->size = size;
      debug_info->line = line;
      debug_info->alloc_time = qbi_os_time_get_ms();
      qbi_mem_debug_add_entry(debug_info);

      sentry = (uint8 *) ptr + size;
      QBI_MEMSCPY(sentry, sizeof(qbi_mem_debug_sentry),
                  qbi_mem_debug_sentry, sizeof(qbi_mem_debug_sentry));
    }
  }

  return ptr;
} /* qbi_mem_debug_malloc() */

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
)
{
  void *data;
/*-------------------------------------------------------------------------*/
  data = qbi_mem_debug_malloc(size, file, line);
  if (data != NULL)
  {
    QBI_MEMSET(data, 0, size);
  }

  return data;
} /* qbi_mem_debug_malloc_clear() */

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
)
{
  uint8 *new_ptr;
  uint8 *sentry;
  qbi_mem_debug_info_s *debug_info;
/*-------------------------------------------------------------------------*/
  if (ptr == NULL)
  {
    new_ptr = qbi_mem_debug_malloc(new_size, file, line);
  }
  else if (new_size == 0)
  {
    qbi_mem_debug_free(ptr);
    new_ptr = NULL;
  }
  else
  {
    debug_info = (qbi_mem_debug_info_s *)
      ((uint8 *) ptr - sizeof(qbi_mem_debug_info_s));
    new_ptr = QBI_OS_REALLOC(
      debug_info, cur_size + sizeof(mem_debug_info_s) + sizeof(qbi_mem_debug_sentry),
      new_size + sizeof(qbi_mem_debug_info_s) + sizeof(qbi_mem_debug_sentry));
    if (new_ptr == NULL)
    {
      QBI_LOG_E_2("Memory re-allocation failure! Tried to resize buffer at %p "
                  "to %d bytes", ptr, new_size);
      QBI_LOG_STR_2("Allocation tried from %s:%d", file, line);
    }
    else
    {
      /* If the allocation changed location, update the references */
      if (new_ptr != (uint8 *) debug_info)
      {
        qbi_mem_debug_remove_entry(debug_info);
        debug_info = (qbi_mem_debug_info_s *) new_ptr;
        debug_info->size = new_size;
        qbi_mem_debug_add_entry(debug_info);
      }
      else
      {
        qbi_os_mutex_lock(&qbi_mem_debug.stats_mutex);
        qbi_mem_debug.cur_mem_usage += (new_size - debug_info->size);
        if (qbi_mem_debug.cur_mem_usage > qbi_mem_debug.max_mem_usage)
        {
          qbi_mem_debug.max_mem_usage = qbi_mem_debug.cur_mem_usage;
        }
        qbi_os_mutex_unlock(&qbi_mem_debug.stats_mutex);

        debug_info->size = new_size;
      }

      new_ptr += sizeof(qbi_mem_debug_info_s);
      sentry = (uint8 *) new_ptr + new_size;
      QBI_MEMSCPY(sentry, sizeof(qbi_mem_debug_sentry),
                  qbi_mem_debug_sentry, sizeof(qbi_mem_debug_sentry));
    }
  }

  return new_ptr;
} /* qbi_mem_debug_realloc() */
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
    to perform an in-place swap, but otherwise must not overlap with src_buf.
    @param size Size of src_buf in bytes. To prevent overflow, dst_buf must
    be at least this size as well.
*/
/*=========================================================================*/
void qbi_mem_reverse_bytes
(
  const uint8 *src_buf,
  uint8       *dst_buf,
  uint32       size
)
{
  uint32 i;
  uint8 temp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(src_buf);
  QBI_CHECK_NULL_PTR_RET(dst_buf);

  if (size == 0)
  {
    QBI_LOG_W_0("Requested zero-length byte reversal; no-op");
  }
  else if (src_buf == dst_buf)
  {
    /* In-place byte reversal */
    for (i = 0; i < (size / 2); i++)
    {
      temp = dst_buf[i];
      dst_buf[i] = dst_buf[size - 1 - i];
      dst_buf[size - 1 - i] = temp;
    }
  }
  else
  {
    /* Copy to new buffer in reverse order */
    for (i = 0; i < size; i++)
    {
      dst_buf[i] = src_buf[size - 1 - i];
    }
  }
} /* qbi_mem_reverse_bytes() */

