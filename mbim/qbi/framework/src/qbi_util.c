/*!
  @file
  qbi_util.c

  @brief
  Utility functions intended for internal use by QBI
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
08/02/13  bd   Add ISO/IEC 8859-1 to GSM default alphabet conversion
04/27/12  bd   Fix off-by-one error in qbi_util_ascii_to_utf16
09/02/11  bd   Updated to MBIM v0.81c
08/24/11  bd   Add qbi_util_list_iter_[de]reg()
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

/*=============================================================================

  Include Files

=============================================================================*/

#include "qbi_util.h"

#include "qbi_common.h"

/*=============================================================================

  Private Constants and Macros

=============================================================================*/

/*! @brief ASCII to GSM default alphabet conversion table
    @details If an ASCII value is used to index into the table, will give the
    GSM default alphabet encoding of the same character */
static const uint8 qbi_util_ascii_to_gsm_table[QBI_UTIL_ASCII_MAX + 1] =
{
  0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
  0x2E, 0x2E, 0x0A, 0x2E, 0x2E, 0x0D, 0x2E, 0x2E,
  0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
  0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x2E,
  0x20, 0x21, 0x22, 0x23, 0x02, 0x25, 0x26, 0x27,
  0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x00, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
  0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
  0x58, 0x59, 0x5A, 0x28, 0x2F, 0x29, 0x2E, 0x11,
  0x27, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
  0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7A, 0x28, 0x2F, 0x29, 0x2D, 0x2E
};

/*! GSM default alphabet to ASCII conversion table */
static const uint8 qbi_util_gsm_to_ascii_table[QBI_UTIL_GSM_ALPHABET_MAX + 1] =
{
  '@',  0xA3, '$',  0xA5, 0xE8, 0xE9, 0xF9, 0xEC,
  0xF2, 0xC7, 0x0A, 0xD8, 0xF8, 0x0D, 0xC5, 0xE5,
  '?',  '_',  '?',  '?',  '?',  '?',  0xB6, '?',
  '?',  '?',  '?',  0x1B, 0xC6, 0xE6, 0xDF, 0xC9,
  ' ',  '!',  0x22, '#',  0xA4,  '%',  '&', 0x27,
  '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/',
  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7',
  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?',
  0xA1, 'A',  'B',  'C',  'D',  'E',  'F',  'G',
  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O',
  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W',
  'X',  'Y',  'Z',  0xC4, 0xD6, 0xD1, 0xDC, 0xA7,
  0xBF, 'a',  'b',  'c',  'd',  'e',  'f',  'g',
  'h',  'i',  'j',  'k',  'l',  'm',  'n',  'o',
  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',
  'x',  'y',  'z',  0xE4, 0xF6, 0xF1, 0xFC, 0xE0
};

/*! ASCII and GSM default alphabet codepoint for carriage return */
#define QBI_UTIL_ASCII_GSM_CHAR_CR (0x0D)

/*! @brief UTF-16 Byte Order Mark (BOM)
    @details The codepoint 0xFEFF (zero-width non-breaking space) may appear as
    the first character in a UTF-16 string to indicate byte order. If the string
    uses little endian byte order, then we will correctly interpret it as
    0xFEFF. But if the string is big endian, we will interpret 0xFFFE and would
    need to perform byte swapping to convert to UTF-16LE as used by MBIM. */
#define QBI_UTIL_UTF16_BOM_BE (0xFFFE)
#define QBI_UTIL_UTF16_BOM_LE (0xFEFF)

/*! If a buffer chain has this many buffers, attempting to add a new one will
    automatically invoke the consolidation routine first */
#define QBI_UTIL_BUF_CHAIN_CONSOLIDATE_THRESHOLD    (20)

/*! Maximum size of a new buffer created to consolidate smaller buffers */
#define QBI_UTIL_BUF_CHAIN_CONSOLIDATE_BUF_SIZE_MAX (2048)

/* Bounds of the lower range of ISO/IEC 8859-1, which is equivalent to ASCII */
#define QBI_UTIL_ISO_8859_1_LOWER_RANGE_MIN (0x20)
#define QBI_UTIL_ISO_8859_1_LOWER_RANGE_MAX (0x7E)

/* Bounds of the upper range of ISO/IEC 8859-1, which encode special chars */
#define QBI_UTIL_ISO_8859_1_UPPER_RANGE_MIN (0xA0)
#define QBI_UTIL_ISO_8859_1_UPPER_RANGE_MAX (0xFF)
#define QBI_UTIL_ISO_8859_1_UPPER_RANGE_NUM_CODE_POINTS \
  (QBI_UTIL_ISO_8859_1_UPPER_RANGE_MAX - QBI_UTIL_ISO_8859_1_UPPER_RANGE_MIN + 1)

/*! ISO/IEC 8859-1 upper range conversion to GSM default alphabet. Not all
    characters are supported in GSM, so closest match is chosen where
    appropriate. Index into this table is the offset from the start of the
    8859-1 upper range (i.e. codepoint - 0xA0). */
static const uint8 qbi_util_iso_8859_1_upper_range_to_gsm_table[
  QBI_UTIL_ISO_8859_1_UPPER_RANGE_NUM_CODE_POINTS] =
{
  0x20, 0x40, 0x63, 0x01, 0x24, 0x03, 0x2F, 0x5F,
  0x22, 0x63, 0x61, 0x3C, 0x2D, 0x2D, 0x52, 0x2D,
  0x2A, 0x11, 0x32, 0x33, 0x27, 0x75, 0x50, 0x2E,
  0x2C, 0x31, 0x6F, 0x3E, 0x11, 0x11, 0x11, 0x60,
  0x7F, 0x41, 0x41, 0x41, 0x5B, 0x0E, 0x1C, 0x09,
  0x04, 0x1F, 0x45, 0x45, 0x07, 0x49, 0x49, 0x49,
  0x44, 0x5D, 0x08, 0x4F, 0x4F, 0x4F, 0x5C, 0x2A,
  0x0B, 0x06, 0x55, 0x55, 0x5E, 0x59, 0x11, 0x1E,
  0x7F, 0x61, 0x61, 0x61, 0x7B, 0x0F, 0x1D, 0x09,
  0x04, 0x05, 0x65, 0x65, 0x07, 0x69, 0x69, 0x69,
  0x64, 0x7D, 0x08, 0x6F, 0x6F, 0x6F, 0x7C, 0x2F,
  0x0C, 0x06, 0x75, 0x75, 0x7E, 0x79, 0x11, 0x79
};

#ifndef MAX_UINT8
  #define MAX_UINT8 (255)
#endif /* MAX_UINT8 */

/*=============================================================================

  Private Typedefs

=============================================================================*/

/*! Individual buffer linked in a buffer chain */
typedef struct qbi_util_buf_chain_entry_struct {
  /*! Must be first as we alias */
  qbi_util_list_entry_s list_entry;

  /*! Contents of the buffer */
  qbi_util_buf_s buf;
} qbi_util_buf_chain_entry_s;

/*=============================================================================

  Private Function Prototypes

=============================================================================*/

static boolean qbi_util_buf_chain_consolidate_add_new_buf
(
  qbi_util_buf_chain_s             *buf_chain,
  qbi_util_list_iter_s             *iter,
  const qbi_util_buf_chain_entry_s *start_buf,
  uint32                            new_buf_size,
  qbi_util_buf_chain_entry_s      **new_buf_pp
);

static boolean qbi_util_buf_chain_consolidate_combine_bufs
(
  qbi_util_buf_chain_s       *buf_chain,
  qbi_util_list_iter_s       *iter,
  qbi_util_buf_chain_entry_s *new_buf
);

static boolean qbi_util_buf_chain_consolidate_count
(
  qbi_util_list_iter_s *iter,
  uint32               *total_buf_size
);

static qbi_util_buf_chain_entry_s *qbi_util_buf_chain_consolidate_find_start
(
  qbi_util_list_iter_s *iter
);

static qbi_util_buf_chain_entry_s *qbi_util_buf_chain_entry_alloc
(
  uint32      new_buf_size,
  const void *initial_data,
  uint32      initial_data_size
);

static void qbi_util_buf_chain_entry_free
(
  qbi_util_buf_chain_entry_s *buf
);

static void *qbi_util_list_entry_get_data
(
  qbi_util_list_entry_s *entry
);

static void qbi_util_list_entry_insert_before
(
  qbi_util_list_s       *list,
  qbi_util_list_entry_s *entry_insert,
  qbi_util_list_entry_s *entry_before
);

static void *qbi_util_list_entry_remove
(
  qbi_util_list_s       *list,
  qbi_util_list_entry_s *entry
);

static void qbi_util_list_iter_reset
(
  qbi_util_list_iter_s *iter
);

static void qbi_util_list_iter_update_reg_iters_entry_removal
(
  qbi_util_list_s       *list,
  qbi_util_list_entry_s *entry
);

/*=============================================================================

  Private Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_consolidate_add_new_buf
===========================================================================*/
/*!
    @brief Allocates a new buffer chain entry and inserts it just before
    start_buf

    @details
    Upon successful return from this function, the list iterator will be
    positioned at start_buf.

    @param buf_chain
    @param iter
    @param start_buf
    @param new_buf_size
    @param new_buf_pp If this function returns TRUE, this will be set to
    point to the newly allocated buffer chain entry

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_util_buf_chain_consolidate_add_new_buf
(
  qbi_util_buf_chain_s             *buf_chain,
  qbi_util_list_iter_s             *iter,
  const qbi_util_buf_chain_entry_s *start_buf,
  uint32                            new_buf_size,
  qbi_util_buf_chain_entry_s      **new_buf_pp
)
{
  boolean success = FALSE;
  qbi_util_buf_chain_entry_s *new_buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(buf_chain);
  QBI_CHECK_NULL_PTR_RET_FALSE(new_buf_pp);

  new_buf = qbi_util_buf_chain_entry_alloc(new_buf_size, NULL, 0);
  QBI_CHECK_NULL_PTR_RET_FALSE(new_buf);

  if (!qbi_util_list_iter_seek(iter, start_buf))
  {
    QBI_LOG_E_0("Couldn't seek list iterator!");
  }
  else if (!qbi_util_list_iter_insert_before_aliased(
             iter, &new_buf->list_entry))
  {
    QBI_LOG_E_0("Couldn't add new field to list!");
  }
  else
  {
    buf_chain->num_entries++;
    *new_buf_pp = new_buf;
    success = TRUE;
  }

  if (!success)
  {
    qbi_util_buf_chain_entry_free(new_buf);
  }

  return success;
} /* qbi_util_buf_chain_consolidate_add_new_buf() */

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_consolidate_combine_bufs
===========================================================================*/
/*!
    @brief Combines num_bufs buffer chain entries into a single buffer,
    new_buf, starting from the current position of the provided list
    iterator

    @details
    Copies data from smaller buffers on the chain into the single
    consolidation buffer new_buf, and removes the smaller buffers. Upon
    successful return, this function will advance the list iterator such
    that the return value of qbi_util_list_iter_cur() will be the field
    immediately following the last buffer involved in consolidation.

    @param buf_chain
    @param iter
    @param new_buf

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_util_buf_chain_consolidate_combine_bufs
(
  qbi_util_buf_chain_s       *buf_chain,
  qbi_util_list_iter_s       *iter,
  qbi_util_buf_chain_entry_s *new_buf
)
{
  uint32 offset = 0;
  qbi_util_buf_chain_entry_s *cur_buf;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(buf_chain);
  QBI_CHECK_NULL_PTR_RET_FALSE(new_buf);

  cur_buf = (qbi_util_buf_chain_entry_s *) qbi_util_list_iter_cur(iter);
  QBI_CHECK_NULL_PTR_RET_FALSE(cur_buf);

  while (offset < new_buf->buf.size)
  {
    if (cur_buf == NULL)
    {
      QBI_LOG_E_1("Unexpected NULL pointer encountered at offset %d", offset);
      success = FALSE;
      break;
    }
    else if ((offset + cur_buf->buf.size) > new_buf->buf.size)
    {
      QBI_LOG_E_3("Buffer too large to fit in consolidated buffer! offset=%d "
                  "cur_buf_size=%d new_buf_size=%d",
                  offset, cur_buf->buf.size, new_buf->buf.size);
      success = FALSE;
      break;
    }
    else
    {
      QBI_MEMSCPY(((uint8 *) new_buf->buf.data + offset),
                  (new_buf->buf.size - offset),
                  cur_buf->buf.data, cur_buf->buf.size);
      offset += cur_buf->buf.size;

      /* Free the smaller buffer we just copied */
      qbi_util_list_iter_remove(iter);
      buf_chain->num_entries--;
      qbi_util_buf_chain_entry_free(cur_buf);

      /* Only advance the list iterator if we are expecting to copy another
         buffer */
      if (offset < new_buf->buf.size)
      {
        cur_buf = (qbi_util_buf_chain_entry_s *) qbi_util_list_iter_next(iter);
      }
    }
  }

  return success;
} /* qbi_util_buf_chain_consolidate_combine_bufs() */

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_consolidate_count
===========================================================================*/
/*!
    @brief Counts the ideal size to allocate for a new consolidation buffer

    @details
    Upon successful return from this function, the list iterator will be
    advanced past the end of the fields available for consolidation.

    @param iter
    @param new_buf_size

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
static boolean qbi_util_buf_chain_consolidate_count
(
  qbi_util_list_iter_s *iter,
  uint32               *total_buf_size
)
{
  uint32 num_bufs = 0;
  qbi_util_buf_chain_entry_s *cur_buf;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(total_buf_size);

  cur_buf = (qbi_util_buf_chain_entry_s *) qbi_util_list_iter_cur(iter);
  QBI_CHECK_NULL_PTR_RET_FALSE(cur_buf);

  *total_buf_size = 0;
  do
  {
    if ((*total_buf_size + cur_buf->buf.size) >
          QBI_UTIL_BUF_CHAIN_CONSOLIDATE_BUF_SIZE_MAX)
    {
      break;
    }
    else
    {
      num_bufs++;
      *total_buf_size += cur_buf->buf.size;
    }
  } while ((cur_buf = (qbi_util_buf_chain_entry_s *)
              qbi_util_list_iter_next(iter)) != NULL);

  if (num_bufs >= 2)
  {
    QBI_LOG_D_2("Consolidating %d fields with total size %d", num_bufs,
                *total_buf_size);
    success = TRUE;
  }
  else
  {
    QBI_LOG_E_2("Counting for buffer chain consolidation found only %d "
                "available fields (total size %d)", num_bufs, *total_buf_size);
  }

  return success;
} /* qbi_util_buf_chain_consolidate_count() */

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_consolidate_find_start
===========================================================================*/
/*!
    @brief Find the start point for a buffer chain consolidation run

    @details
    Starting with the first field returned by qbi_util_list_iter_next(),
    searches for a chained buffer that can be combined with its neighbor
    into a new buffer that is smaller than the maximum size, to reduce the
    total number of buffers. Note that more than 2 buffers could be combined
    into the new field, but that count is handled via
    qbi_util_buf_chain_consolidate_count().

    @param iter List iterator for a buffer chain list. Must have already
    been initialized via qbi_util_list_iter_init()

    @return qbi_util_buf_chain_entry_s* Pointer to chained buffer that can be
    combined with at least one additional buffer, or NULL if no suitable
    buffer was found
*/
/*=========================================================================*/
static qbi_util_buf_chain_entry_s *qbi_util_buf_chain_consolidate_find_start
(
  qbi_util_list_iter_s *iter
)
{
  qbi_util_buf_chain_entry_s *cur_buf;
  qbi_util_buf_chain_entry_s *next_buf;
  qbi_util_buf_chain_entry_s *start_buf = NULL;
/*-------------------------------------------------------------------------*/
  while ((cur_buf = (qbi_util_buf_chain_entry_s *)
            qbi_util_list_iter_next(iter)) != NULL)
  {
    /* Peek at the next item on the list without advancing the iterator */
    next_buf = (qbi_util_buf_chain_entry_s *) qbi_util_list_iter_peek(iter);

    /* If there is no next field, then cur_buf is the last one, so no
       consolidation is possible */
    if (next_buf != NULL)
    {
      /* We can start consolidation at cur_buf if it can fit within our
         maximum size buffer along with the next field (consolidating more than
         two fields will be handled later) */
      if ((cur_buf->buf.size + next_buf->buf.size) <=
            QBI_UTIL_BUF_CHAIN_CONSOLIDATE_BUF_SIZE_MAX)
      {
        start_buf = cur_buf;
        break;
      }
    }
  }

  return start_buf;
} /* qbi_util_buf_chain_consolidate_find_start() */

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_entry_alloc
===========================================================================*/
/*!
    @brief Allocates a new buffer to be linked on a buffer chain

    @details
    Does not link the buffer on the buffer chain. The newly allocated buffer
    area will be initialized to all zeros, unless initial_data is provided.

    @param new_buf_size Size in bytes of the buffer area to allocate
    @param initial_data Data to copy into the new buffer after allocation,
    or NULL to initialize entire buffer to all zeros
    @param initial_data_size Size of initial data buffer, in bytes. Can be
    less than new_buf_size, in which case the remaining portion of the buffer
    will be initialized to all zeros. If initial_data is NULL, then this
    argument will be ignored.

    @return qbi_util_buf_chain_entry_s* Pointer to newly allocated buffer
    chain entry structure, or NULL on allocation failure
*/
/*=========================================================================*/
static qbi_util_buf_chain_entry_s *qbi_util_buf_chain_entry_alloc
(
  uint32      new_buf_size,
  const void *initial_data,
  uint32      initial_data_size
)
{
  qbi_util_buf_chain_entry_s *new_buf;
/*-------------------------------------------------------------------------*/
  new_buf = QBI_MEM_MALLOC_CLEAR(sizeof(qbi_util_buf_chain_entry_s));
  if (new_buf == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate new buffer chain entry!");
  }
  else
  {
    qbi_util_buf_init(&new_buf->buf);
    if (qbi_util_buf_alloc_dont_clear(&new_buf->buf, new_buf_size) == NULL)
    {
      QBI_LOG_E_0("Couldn't allocate new buffer for buffer chain");
      QBI_MEM_FREE(new_buf);
      new_buf = NULL;
    }
    else
    {
      if (initial_data != NULL)
      {
        if (initial_data_size > new_buf_size)
        {
          QBI_LOG_W_2("Tried to initialize more data than allocated! (%d/%d)",
                      initial_data_size, new_buf_size);
          initial_data_size = new_buf_size;
        }
        QBI_MEMSCPY(new_buf->buf.data, new_buf->buf.size,
                    initial_data, initial_data_size);
      }
      else
      {
        initial_data_size = 0;
      }

      /* Clear any remaining buffer area */
      if (new_buf_size > initial_data_size)
      {
        QBI_MEMSET(((uint8 *) new_buf->buf.data + initial_data_size), 0,
                   (new_buf_size - initial_data_size));
      }
    }
  }

  return new_buf;
} /* qbi_util_buf_chain_entry_alloc() */

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_entry_free
===========================================================================*/
/*!
    @brief Frees memory associated with a buffer linked on a buffer chain

    @details
    After returning from this function, buf must not be referenced.

    @param buf
*/
/*=========================================================================*/
static void qbi_util_buf_chain_entry_free
(
  qbi_util_buf_chain_entry_s *buf
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf);

  qbi_util_buf_free(&buf->buf);
  QBI_MEM_FREE(buf);
} /* qbi_util_buf_chain_entry_free() */

/*===========================================================================
  FUNCTION: qbi_util_list_entry_get_data
===========================================================================*/
/*!
    @brief Retrieves a pointer to the data associated with a list entry

    @details
    If the list entry was aliased, then entry->data will be NULL, and the
    data pointer is the address of entry. Otherwise, entry->data is used.

    @param entry

    @return void* Pointer to list entry's data, or NULL on failure
*/
/*=========================================================================*/
static void *qbi_util_list_entry_get_data
(
  qbi_util_list_entry_s *entry
)
{
  void *data;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(entry);

  if (entry->data == NULL)
  {
    data = (void *) entry;
  }
  else
  {
    data = entry->data;
  }

  return data;
} /* qbi_util_list_entry_get_data() */

/*===========================================================================
  FUNCTION: qbi_util_list_entry_insert_before
===========================================================================*/
/*!
    @brief Links a new list entry into the list, so that it appears
    immediately before another list entry

    @details
    If entry_before is NULL, inserts at the back of the list

    @param list
    @param entry_insert The list entry to insert
    @param entry_before The list entry that entry_insert should be
    immediately in front of, or NULL if the new entry should be added
    to the end of the list
*/
/*=========================================================================*/
static void qbi_util_list_entry_insert_before
(
  qbi_util_list_s       *list,
  qbi_util_list_entry_s *entry_insert,
  qbi_util_list_entry_s *entry_before
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(list);
  QBI_CHECK_NULL_PTR_RET(entry_insert);

  entry_insert->next = entry_before;
  if (entry_before == NULL)
  {
    entry_insert->prev = list->tail;
    if (list->tail != NULL)
    {
      list->tail->next = entry_insert;
    }
    list->tail = entry_insert;

    if (list->head == NULL)
    {
      list->head = entry_insert;
    }
  }
  else
  {
    entry_insert->prev = entry_before->prev;
    if (entry_before->prev != NULL)
    {
      entry_before->prev->next = entry_insert;
    }
    else
    {
      if (entry_before != list->head)
      {
        /* Signals corruption of list linkage */
        QBI_LOG_E_0("List entry has no previous link but is not the head of "
                    "the list!");
      }
      list->head = entry_insert;
    }
    entry_before->prev = entry_insert;
  }
} /* qbi_util_list_entry_insert_before() */

/*===========================================================================
  FUNCTION: qbi_util_list_remove
===========================================================================*/
/*!
    @brief Removes an entry from any location in the list

    @details

    @param list
    @param entry The entry to remove. This memory will be freed, so it must
    not be referenced again after returning from this function.

    @return void* Data pointer associated with the removed entry
*/
/*=========================================================================*/
static void *qbi_util_list_entry_remove
(
  qbi_util_list_s       *list,
  qbi_util_list_entry_s *entry
)
{
  void *data = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(list);
  QBI_CHECK_NULL_PTR_RET_NULL(entry);

  qbi_util_list_iter_update_reg_iters_entry_removal(list, entry);

  if (list->head == entry)
  {
    list->head = entry->next;
  }
  if (list->tail == entry)
  {
    list->tail = entry->prev;
  }
  if (entry->prev != NULL)
  {
    entry->prev->next = entry->next;
  }
  if (entry->next != NULL)
  {
    entry->next->prev = entry->prev;
  }
  data = entry->data;

  /* Zero out the memory before releasing it to help limit the damage of
     potential references to this entry lingering in an iterator, etc. */
  QBI_MEMSET(entry, 0, sizeof(qbi_util_list_entry_s));
  if (data != NULL)
  {
    #ifdef QBI_MEM_DEBUG
    QBI_LOG_D_2("Freeing entry %p for list %p", entry, list);
    #endif /* QBI_MEM_DEBUG */
    QBI_MEM_FREE(entry);
  }
  else
  {
    data = (void *) entry;
  }

  return data;
} /* qbi_util_list_entry_remove() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_reset
===========================================================================*/
/*!
    @brief Sets a list iterator's current position to the beginning of its
    list

    @details
    After this function returns, qbi_util_list_iter_next() will return the
    list entry at the front of the list (list->head)

    @param iter
*/
/*=========================================================================*/
static void qbi_util_list_iter_reset
(
  qbi_util_list_iter_s *iter
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(iter);
  QBI_CHECK_NULL_PTR_RET(iter->list);

  iter->cur  = NULL;
  iter->next = iter->list->head;
} /* qbi_util_list_iter_reset() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_update_reg_iters_entry_removal
===========================================================================*/
/*!
    @brief Updates the list iterators registered with the current list that
    the given entry will be removed

    @details

    @param list
    @param entry
*/
/*=========================================================================*/
static void qbi_util_list_iter_update_reg_iters_entry_removal
(
  qbi_util_list_s       *list,
  qbi_util_list_entry_s *entry
)
{
  qbi_util_list_iter_s *iter;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(list);
  QBI_CHECK_NULL_PTR_RET(entry);

  iter = list->reg_iters;
  while (iter != NULL)
  {
    if (iter->next == entry)
    {
      QBI_LOG_D_0("Updated next entry of list iter");
      iter->next = entry->next;
    }
    else if (iter->cur == entry)
    {
      QBI_LOG_D_0("Set current entry of list iter to NULL due to removal");
      iter->cur = NULL;
    }
    iter = iter->next_reg_iter;
  }
} /* qbi_util_list_iter_update_reg_iters_entry_removal() */

/*=============================================================================

  Public Function Definitions

=============================================================================*/

/*===========================================================================
  FUNCTION: qbi_util_7bit_pack
===========================================================================*/
/*!
    @brief Packs 7-bit characters from 8 bits per character to 7 bits
    per character following 3GPP packing rules

    @details

    @param packed Buffer to store packed data. Must be at least
    ceil((unpacked_size / 8) * 7) to avoid truncation (i.e. 160 bytes for
    USSD assuming unpacked_size <= 182)
    @param packed_size Size of the packed buffer
    @param unpacked Buffer containing unpacked characters
    @param unpacked_size Number of characters in unpacked buffer
    @param pad_with_cr Set to TRUE to include a trailing CR if the last
    byte only contains 1 bit of the last character (See 3GPP TS 23.038)

    @return uint32 Number of bytes set in packed buffer
*/
/*=========================================================================*/
uint32 qbi_util_7bit_pack
(
  uint8       *packed,
  uint32       packed_size,
  const uint8 *unpacked,
  uint32       unpacked_size,
  boolean      pad_with_cr
)
{
  uint32 packed_index = 0;
  uint32 unpacked_index = 0;
  uint8 shift;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(packed);
  QBI_CHECK_NULL_PTR_RET_ZERO(unpacked);

  /* Loop through the 7-bit string till the last but one character. */
  while (unpacked_index < (unpacked_size - 1))
  {
    shift = (unpacked_index  & 0x07);

    /* A byte of packed data is always made up of only 2 7-bit characters. The
       shift of the two characters always depends on their index in the
       string. */
    if (packed_index < packed_size)
    {
      packed[packed_index++] =
        ((unpacked[unpacked_index] >> shift) |
         (unpacked[unpacked_index + 1] << (7 - shift)));
    }
    else
    {
      break;
    }

    /* If the second characters fits inside the current packed byte, then skip
       it for the next iteration. */
    if (shift == 6)
    {
      unpacked_index++;
    }
    unpacked_index++;
  }

  /* Special case for the last 7-bit character. */
  if (unpacked_index < unpacked_size && packed_index < packed_size)
  {
    shift = (unpacked_index & 0x07);
    /* The tertiary operator (?:) takes care of the special case of (8n-1)
       7-bit characters which requires padding with CR (0x0D). */
    packed[packed_index++] =
      (((pad_with_cr && shift == 6) ? (QBI_UTIL_ASCII_GSM_CHAR_CR << 1) : 0) |
       (unpacked[unpacked_index++] >> shift));
  }

  /* Takes care of special case when there are 8n 7-bit characters and the
     last character is a CR (0x0D). */
  if (pad_with_cr && packed_index < packed_size &&
      (unpacked_size & 0x07) == 0 &&
      unpacked[unpacked_size - 1] == QBI_UTIL_ASCII_GSM_CHAR_CR)
  {
    packed[packed_index++] = QBI_UTIL_ASCII_GSM_CHAR_CR;
  }

  if (packed_index >= packed_size && unpacked_index < unpacked_size)
  {
    QBI_LOG_E_3("Truncated string when packing! Unpacked size %d (copied %d),"
                " packed buffer %d", unpacked_size, unpacked_index,
                packed_size);
  }

  return packed_index;
} /* qbi_util_7bit_pack() */

/*===========================================================================
  FUNCTION: qbi_util_7bit_unpack
===========================================================================*/
/*!
    @brief Unpacks a string of 7-bit characters into one character per 8 bits

    @details
    This function does not NULL terminate the resulting string.

    @param unpacked Buffer to store unpacked data. Must be at least
    floor((packed_size / 7) * 8) to avoid truncation (i.e. 182 bytes for
    USSD assuming packed_size <= 160)
    @param unpacked_size Size of unpacked buffer
    @param packed Buffer containing packed characters
    @param packed_size Size of packed buffer in bytes
    @param drop_trailing_cr Set to TRUE to drop a trailing carriage return
    character (0x0D) if present in the packed string (See 3GPP TS 23.038)

    @return uint32 Number of characters in the resulting string
*/
/*=========================================================================*/
uint32 qbi_util_7bit_unpack
(
  uint8       *unpacked,
  uint32       unpacked_size,
  const uint8 *packed,
  uint32       packed_size,
  boolean      drop_trailing_cr
)
{
  uint32 packed_index = 0;
  uint32 unpacked_index = 0;
  uint8 prev_byte = 0;
  uint8 curr_byte = 0;
  uint8 shift;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(packed);
  QBI_CHECK_NULL_PTR_RET_ZERO(unpacked);

  while (packed_index < packed_size)
  {
    if (unpacked_index >= unpacked_size)
    {
      QBI_LOG_W_2("Truncated input string because unpacked buffer too small! "
                  "Packed size %d unpacked buffer %d", packed_size,
                  unpacked_size);
      break;
    }
    shift = (unpacked_index & 0x07);
    curr_byte = packed[packed_index++];

    /* A 7-bit character can be split at the most between two bytes of packed
       data. */
    unpacked[unpacked_index++] =
      ((curr_byte << shift) | (prev_byte >> (8 - shift))) & 0x7F;

    /* Special case where the whole of the next 7-bit character fits inside
       the current byte of packed data. */
    if (shift == 6)
    {
      /* If the next 7-bit character is a CR (0x0D) and it is the last
         character, then it indicates a padding character. Drop it. */
      if (drop_trailing_cr && packed_index == packed_size &&
          (curr_byte >> 1) == QBI_UTIL_ASCII_GSM_CHAR_CR)
      {
        break;
      }
      else if (unpacked_index >= unpacked_size)
      {
        QBI_LOG_E_2("Truncated input string because unpacked buffer too "
                    "small! Packed size %d unpacked buffer %d", packed_size,
                    unpacked_size);
        break;
      }
      unpacked[unpacked_index++] = curr_byte >> 1;
    }

    prev_byte = curr_byte;
  }

  return unpacked_index;
} /* qbi_util_7bit_unpack() */

/*===========================================================================
  FUNCTION: qbi_util_ascii_decimal_str_to_uint8
===========================================================================*/
/*!
    @brief Converts a decimal (base 10) unsigned integer at the beginning of
    an ASCII-encoded string to binary uint8 representation

    @details
    Stops parsing at the first detected non-numeric character, or until
    str_len characters have been parsed.

    @param str Pointer to ASCII-encoded string to parse
    @param str_len Maximum number of characters to parse from the string
    @param value Populated with the parsed value upon successful return,
    otherwise unmodified

    @return boolean TRUE on success (at least 1 digit parsed and result fits
    in uint8), FALSE on failure
*/
/*=========================================================================*/
boolean qbi_util_ascii_decimal_str_to_uint8
(
  const char *str,
  uint32      str_len,
  uint8      *value
)
{
  uint32 offset = 0;
  uint32 tmp_value = 0;
  boolean success = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(str);
  QBI_CHECK_NULL_PTR_RET_FALSE(value);

  while (offset < str_len &&
         str[offset] >= QBI_UTIL_ASCII_NUMERIC_MIN &&
         str[offset] <= QBI_UTIL_ASCII_NUMERIC_MAX)
  {
    tmp_value *= 10; /* Radix fixed @ 10 for decimal */
    tmp_value += (str[offset] - QBI_UTIL_ASCII_NUMERIC_MIN);
    offset++;
    if (tmp_value > MAX_UINT8)
    {
      QBI_LOG_E_0("Overflow while attempting to parse uint8 from string");
      success = FALSE;
      break;
    }
    else
    {
      success = TRUE;
    }
  }

  /* If success is TRUE, we parsed at least one digit, and did not exceed the
     storage capacity of uint8. */
  if (success)
  {
    *value = (uint8) tmp_value;
  }
  return success;
} /* qbi_util_ascii_decimal_str_to_uint8() */

/*===========================================================================
  FUNCTION: qbi_util_ascii_to_utf16
===========================================================================*/
/*!
    @brief Transcode an ASCII string into UTF-16 (little endian)

    @details
    Copies from ascii to utf16, until either a NULL character is found in
    ascii, or the end of one of the arrays is reached. Does not guarantee
    that the resulting UTF-16 string will be terminated by a NULL character.

    @param ascii Input string
    @param ascii_len_bytes Size of input string buffer
    @param utf16 Buffer to store UTF-16 string
    @param utf16_len_bytes Size of output string buffer

    @return uint32 Size of the resulting UTF-16 string in bytes, not
    including the NULL character. This will be 0 if an error occurred and
    utf16 was not changed.
*/
/*=========================================================================*/
uint32 qbi_util_ascii_to_utf16
(
  const char *ascii,
  uint32      ascii_len_bytes,
  uint8      *utf16,
  uint32      utf16_len_bytes
)
{
  uint32 ascii_offset;
  uint32 utf16_offset = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(ascii);
  QBI_CHECK_NULL_PTR_RET_ZERO(utf16);

  if (utf16_len_bytes < 2)
  {
    QBI_LOG_E_1("Invalid destination length %d", utf16_len_bytes);
  }
  else
  {
    /* If the UTF-16 buffer has an odd number of bytes, ignore the last byte */
    if (utf16_len_bytes % 2)
    {
      utf16_len_bytes--;
    }

    for (ascii_offset = 0;
          ascii_offset < ascii_len_bytes && utf16_offset < utf16_len_bytes;
          ascii_offset++)
    {
      if (ascii[ascii_offset] == QBI_UTIL_ASCII_NULL)
      {
        break;
      }
      utf16[utf16_offset++] = ascii[ascii_offset];
      utf16[utf16_offset++] = 0x00;
    }

    /* Fill in the NULL character if we can, but don't change utf16_offset */
    if (utf16_offset <= (utf16_len_bytes - 2))
    {
      utf16[utf16_offset]     = 0x00;
      utf16[utf16_offset + 1] = 0x00;
    }

    /* Log a debug message if the input was truncated. Generally there is no
       option for recovery in QBI when this happens. */
    if (ascii_offset < ascii_len_bytes &&
        ascii[ascii_offset] != QBI_UTIL_ASCII_NULL)
    {
      QBI_LOG_W_2("ASCII string truncated when copying into UTF-16 buffer! "
                  "Input string has len %d, output buffer only %d bytes",
                  QBI_STRLEN(ascii), utf16_len_bytes);
    }
  }

  return utf16_offset;
} /* qbi_util_ascii_to_utf16() */

/*===========================================================================
  FUNCTION: qbi_util_binary_search
===========================================================================*/
/*!
    @brief Performs an iterative binary search on a generic sorted array

    @details
    The parameters index_min and index_max should be set to 0 and
    ARR_SIZE(array)-1, respectively, to search the entire array. Can also
    search a subset of the array by restricting the parameters.

    @param array Pointer to start of the array to search. Will be passed
    to compare_fcn() with an accompanying index to do comparisons.
    @param key Pointer to the key to search for
    @param index_min Lower bound of the search
    @param index_max Upper bound of the search
    @param compare_fcn Function used to compare the array entry at a given
    index against the key
    @param found_index If not NULL and this function returns TRUE, will be
    set to the index where the match was found, otherwise unchanged

    @return boolean TRUE if they key was found, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_util_binary_search
(
  const void               *array,
  const void               *key,
  uint32                    index_min,
  uint32                    index_max,
  qbi_util_array_compare_f *compare_fcn,
  uint32                   *found_index
)
{
  uint32 midpoint;
  boolean found = FALSE;
  qbi_util_comparison_result_e result;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(compare_fcn);

  while (index_min <= index_max)
  {
    midpoint = ((index_max - index_min) / 2) + index_min;
    result = compare_fcn(array, midpoint, key);
    if (result == QBI_UTIL_COMPARISON_RESULT_GREATER_THAN)
    {
      index_max = midpoint - 1;
    }
    else if (result == QBI_UTIL_COMPARISON_RESULT_LESS_THAN)
    {
      index_min = midpoint + 1;
    }
    else if (result == QBI_UTIL_COMPARISON_RESULT_EQUAL_TO)
    {
      if (found_index != NULL)
      {
        *found_index = midpoint;
      }
      found = TRUE;
      break;
    }
    else
    {
      QBI_LOG_E_1("Comparison at index %d failed", midpoint);
      break;
    }
  }

  return found;
} /* qbi_util_binary_search() */

/*===========================================================================
  FUNCTION: qbi_util_buf_alloc
===========================================================================*/
/*!
    @brief Allocates a dynamic buffer and populates the information in
    qbi_util_buf_s. The buffer must have been previously initialized via
    qbi_util_buf_init

    @details
    It is an error to call this function on a non-empty buffer, i.e. every
    call to buf_alloc must be 1:1 with a call to buf_free. The current
    implementation checks for these errors and will prevent a memory leak,
    but do not rely on this behavior.

    The data in the buffer (buf->data) will be initialized to 0, but this
    does not initialize the qbi_util_buf_s structure (qbi_util_buf_init
    does that).

    @param buf Struct to populate
    @param size Size of requested memory region

    @return void* Pointer to newly allocated memory region, or NULL on
    failure
*/
/*=========================================================================*/
void *qbi_util_buf_alloc
(
  qbi_util_buf_s *buf,
  uint32          size
)
{
  void *data_ptr;
/*-------------------------------------------------------------------------*/
  data_ptr = qbi_util_buf_alloc_dont_clear(buf, size);
  if (data_ptr != NULL)
  {
    QBI_MEMSET(data_ptr, 0, size);
  }

  return data_ptr;
} /* qbi_util_buf_alloc() */

/*===========================================================================
  FUNCTION: qbi_util_buf_alloc_dont_clear
===========================================================================*/
/*!
    @brief A version of qbi_util_buf_alloc that does not zero out the buffer

    @details
    The buffer must have been initialized via qbi_util_buf_init()

    Generally, this should only be picked over qbi_util_buf_alloc() in
    situations where the newly allocated buffer will be immediately filled.

    @param buf Struct to populate
    @param size Size of requested memory region

    @return void* Pointer to newly allocated memory region, or NULL on
    failure

    @see qbi_util_buf_alloc
*/
/*=========================================================================*/
void *qbi_util_buf_alloc_dont_clear
(
  qbi_util_buf_s *buf,
  uint32          size
)
{
  void *data_ptr;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(buf);

  /* Protect against memory leaks via successive calls to buf_alloc */
  if (buf->data != NULL)
  {
    QBI_LOG_E_2("Attempted double-alloc of buffer! Old size %d new size %d",
                buf->size, size);
    qbi_util_buf_free(buf);
  }

  data_ptr = QBI_MEM_MALLOC(size);
  if (data_ptr == NULL)
  {
    buf->size = 0;
  }
  else
  {
    #ifdef QBI_MEM_DEBUG
    QBI_LOG_D_3("Allocated memory at %p for buffer %p with size %d", data_ptr,
                buf, size);
    #endif /* QBI_MEM_DEBUG */

    buf->data = data_ptr;
    buf->size = size;
  }

  return data_ptr;
} /* qbi_util_buf_alloc_dont_clear() */

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_add
===========================================================================*/
/*!
    @brief Allocates and links a new buffer to a buffer chain, and optionally
    initializes it with initial_data

    @details
    The buffer chain must have been previously initialized with
    qbi_util_buf_chain_init()

    @param buf_chain Chain to link the new buffer with
    @param new_buf_size Requested size of the new buffer
    @param initial_data Pointer to data to copy into newly allocated buffer,
    or NULL to initialize buffer to all zeros
    @param initial_data_size Size of initial_data in bytes. Can be less than
    new_buf_size, in which case the remaining portion of the buffer will be
    initialized to zeros. If initial_data is NULL, then this argument will
    be ignored, and the entire buffer will be initialized to all zeros.

    @return void* Pointer to newly allocated buffer area of size new_buf_size
    bytes (filled with uninitialized data), or NULL on failure
*/
/*=========================================================================*/
void *qbi_util_buf_chain_add
(
  qbi_util_buf_chain_s *buf_chain,
  uint32                new_buf_size,
  const void           *initial_data,
  uint32                initial_data_size
)
{
  void *data = NULL;
  qbi_util_buf_chain_entry_s *new_buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(buf_chain);

  new_buf = qbi_util_buf_chain_entry_alloc(
    new_buf_size, initial_data, initial_data_size);
  QBI_CHECK_NULL_PTR_RET_NULL(new_buf);

  if (!qbi_util_list_push_back_aliased(
        &buf_chain->list, &new_buf->list_entry))
  {
    QBI_LOG_E_0("Couldn't add new buffer chain entry to list!");
    qbi_util_buf_chain_entry_free(new_buf);
  }
  else
  {
    buf_chain->num_entries++;
    data = new_buf->buf.data;
  }

  return data;
} /* qbi_util_buf_chain_add() */

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_consolidate
===========================================================================*/
/*!
    @brief Consoliates a buffer chain into as few individual buffers as
    possible if number of buffers exceeds threshold; see note below

    @details
    Combines smaller buffers into as few buffers as possible, while
    maintaining the size of any single buffer below a predefined maximum.

    The consolidation algorithm is outlined below:
      1. Initialize list iterator to the beginning of the list
      2. Find the first available buffer with at least one subsequent buffer
         that can be combined into a new buffer of size <= max
      3. Determine the number of buffers and the total size that will go into
         the new consolidated buffer
      4. Allocate a new buffer, and insert it into the chain before the
         buffer from step 2
      5. For each buffer starting from the buffer from step 2 and continuing
         until the endpoint found in step 3: copy the smaller buffer into
         the new consolidation buffer and delete the smaller buffer
      6. Repeat at step 2, starting after the newly combined buffer

    @param buf_chain

    @return TRUE on success, FALSE on unexpected failure that could leave
    the buffer chain in a bad state. A return value of TRUE does not
    necessarily mean that consolidation was actually performed.

    @note As consolidation involves freeing smaller buffers, any pointers
    returned by previous calls to qbi_util_buf_chain_add() must be considered
    invalid after returning from this function. Therefore this function
    should only be called after modifying the already allocated contents of
    the buffer chain is complete.
*/
/*=========================================================================*/
boolean qbi_util_buf_chain_consolidate
(
  qbi_util_buf_chain_s *buf_chain
)
{
  qbi_util_list_iter_s iter;
  qbi_util_buf_chain_entry_s *start_buf;
  qbi_util_buf_chain_entry_s *new_buf;
  uint32 new_buf_size;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(buf_chain);

  if (buf_chain->num_entries >= QBI_UTIL_BUF_CHAIN_CONSOLIDATE_THRESHOLD)
  {
    QBI_LOG_I_1("Consolidating buffer chain with %d entries",
                buf_chain->num_entries);
    qbi_util_list_iter_init(&buf_chain->list, &iter);
    do
    {
      start_buf = qbi_util_buf_chain_consolidate_find_start(&iter);
      if (start_buf != NULL)
      {
        if (!qbi_util_buf_chain_consolidate_count(
              &iter, &new_buf_size) ||
            !qbi_util_buf_chain_consolidate_add_new_buf(
              buf_chain, &iter, start_buf, new_buf_size, &new_buf))
        {
          /* Failures at these stages aren't ideal, but the chain will be
             left unmodified, so we can still return success = TRUE */
          QBI_LOG_W_0("Consolidation failed while counting available entries "
                      "or allocating new entry!");
          break;
        }
        else if (!qbi_util_buf_chain_consolidate_combine_bufs(
                   buf_chain, &iter, new_buf))
        {
          /* Failure during this stage means the data buffer chain may be in an
             inconsistent state - this shouldn't happen under normal
             circumstances, so we should return FALSE */
          QBI_LOG_E_0("Consolidation failed while copying smaller fields into "
                      "new field!");
          success = FALSE;
          break;
        }
      }
    } while (qbi_util_list_iter_has_next(&iter));

    QBI_LOG_I_1("After consolidation, buffer chain has %d field(s)",
                buf_chain->num_entries);
  }

  return success;
} /* qbi_util_buf_chain_consolidate() */

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_extract
===========================================================================*/
/*!
    @brief Extracts data from a buffer chain into a contiguous buffer

    @details
    Copies up to req_size bytes from the provided buffer chain into dst,
    starting at the offset given by req_chain_offset. The buffer chain
    must have been previously initialized via qbi_util_buf_chain_init()

    @param buf_chain
    @param req_chain_offset Offset in the buffer chain to the first byte
    where copying should start
    @param req_size Requested number of bytes to copy from the buffer chain
    into dst
    @param dst Continguous buffer to store up to req_size bytes from the
    buffer chain

    @return uint32 Number of bytes set in dst
*/
/*=========================================================================*/
uint32 qbi_util_buf_chain_extract
(
  qbi_util_buf_chain_s *buf_chain,
  uint32                req_chain_offset,
  uint32                req_size,
  uint8                *dst
)
{
  qbi_util_list_iter_s iter;
  qbi_util_buf_chain_entry_s *cur_buf;
  uint32 cur_buf_offset;
  uint32 cur_chain_offset = 0;
  uint32 bytes_to_copy;
  uint32 bytes_copied_total = 0;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(buf_chain);
  QBI_CHECK_NULL_PTR_RET_ZERO(dst);

  qbi_util_list_iter_init(&buf_chain->list, &iter);
  while (req_size > 0 &&
         (cur_buf = (qbi_util_buf_chain_entry_s *)
            qbi_util_list_iter_next(&iter)) != NULL)
  {
    /* If this buffer has data that we want to copy into dst */
    if (req_chain_offset < (cur_chain_offset + cur_buf->buf.size))
    {
      cur_buf_offset = (req_chain_offset - cur_chain_offset);
      bytes_to_copy  = (cur_buf->buf.size - cur_buf_offset);
      if (bytes_to_copy > req_size)
      {
        bytes_to_copy = req_size;
      }

      QBI_MEMSCPY((dst + bytes_copied_total), req_size,
                  ((uint8 *) cur_buf->buf.data + cur_buf_offset),
                  bytes_to_copy);
      req_chain_offset   += bytes_to_copy;
      bytes_copied_total += bytes_to_copy;
      req_size           -= bytes_to_copy;
    }
    cur_chain_offset += cur_buf->buf.size;
  }

  return bytes_copied_total;
} /* qbi_util_buf_chain_extract() */

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_free_all
===========================================================================*/
/*!
    @brief Frees all memory associated with a buffer chain

    @details
    Does not free the buffer chain structure itself. The buffer chain must
    have been previously initialized via qbi_util_buf_chain_init()

    @param buf_chain
*/
/*=========================================================================*/
void qbi_util_buf_chain_free_all
(
  qbi_util_buf_chain_s *buf_chain
)
{
  qbi_util_buf_chain_entry_s *buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf_chain);

  while ((buf = (qbi_util_buf_chain_entry_s *)
            qbi_util_list_pop_back(&buf_chain->list)) != NULL)
  {
    buf_chain->num_entries--;
    qbi_util_buf_chain_entry_free(buf);
  }
} /* qbi_util_buf_chain_free_all() */

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_free_last
===========================================================================*/
/*!
    @brief Frees the most recently allocated buffer on the given buffer chain

    @details
    The buffer chain must have been previously initialized via
    qbi_util_buf_chain_init()

    @param buf_chain
*/
/*=========================================================================*/
void qbi_util_buf_chain_free_last
(
  qbi_util_buf_chain_s *buf_chain
)
{
  qbi_util_buf_chain_entry_s *buf;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf_chain);

  buf = (qbi_util_buf_chain_entry_s *) qbi_util_list_pop_back(&buf_chain->list);
  if (buf == NULL)
  {
    QBI_LOG_W_0("Tried to free last buffer from chain, but list is empty!");
  }
  else
  {
    buf_chain->num_entries--;
    qbi_util_buf_chain_entry_free(buf);
  }
} /* qbi_util_buf_chain_free_last() */

/*===========================================================================
  FUNCTION: qbi_util_buf_chain_init
===========================================================================*/
/*!
    @brief Initializes a buffer chain

    @details
    Must be called before invoking any other qbi_util_buf_chain_* function

    @param buf_chain
*/
/*=========================================================================*/
void qbi_util_buf_chain_init
(
  qbi_util_buf_chain_s *buf_chain
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf_chain);

  qbi_util_list_init(&buf_chain->list);
  buf_chain->num_entries = 0;
} /* qbi_util_buf_chain_init() */

/*===========================================================================
  FUNCTION: qbi_util_buf_free
===========================================================================*/
/*!
    @brief Release memory associated with a dynamic buffer

    @details
    The buffer may not have any memory allocated, but it must have been
    initialized via qbi_util_buf_init.

    @param buf
*/
/*=========================================================================*/
void qbi_util_buf_free
(
  qbi_util_buf_s *buf
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf);

  if (buf->data != NULL)
  {
    #ifdef QBI_MEM_DEBUG
    QBI_LOG_D_3("Freeing memory at %p for buffer %p (size %d)", buf->data,
                buf, buf->size);
    #endif /* QBI_MEM_DEBUG */

    QBI_MEM_FREE(buf->data);
    buf->data = NULL;
    buf->size = 0;
  }
} /* qbi_util_buf_free() */

/*===========================================================================
  FUNCTION: qbi_util_buf_init
===========================================================================*/
/*!
    @brief Initializes a buffer data structure. This function must be called
    before the other qbi_util_buf_* functions.

    @details

    @param buf
*/
/*=========================================================================*/
void qbi_util_buf_init
(
  qbi_util_buf_s *buf
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf);
  QBI_MEMSET(buf, 0, sizeof(qbi_util_buf_s));
} /* qbi_util_buf_init() */

/*===========================================================================
  FUNCTION: qbi_util_buf_resize
===========================================================================*/
/*!
    @brief Resizes a buffer. The buffer must have been previously initialized
    via qbi_util_buf_init.

    @details
    It is OK if the buffer has not been allocated or has been freed, in
    which case this will behave like qbi_util_buf_alloc. But it must have
    been initialized via qbi_util_buf_init.

    Resizing a buffer to 0 will invoke qbi_util_buf_free, but it is
    recommended to call qbi_util_buf_free to be more explicit about intent.

    This function will initialize any newly allocated memory to 0 before
    returning, but will leave existing data intact.

    @param buf Buffer to resize
    @param new_size Requested new size of the buffer

    @return boolean TRUE on success, or FALSE on failure. If a new memory
    region couldn't be allocated, the current data will be preserved.

    @note Be careful when using this with the response InformationBuffer in
    a QBI transaction. If any fields have already been added to the
    DataBuffer, their offsets will need to be recalculated. There is
    currently no automated method of doing this.
*/
/*=========================================================================*/
boolean qbi_util_buf_resize
(
  qbi_util_buf_s *buf,
  uint32          new_size
)
{
  uint8 *new_data;
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(buf);

  if (new_size == 0)
  {
    QBI_LOG_W_0("Freeing buffer as result of resize to 0");
    qbi_util_buf_free(buf);
    result = TRUE;
  }
  else
  {
    new_data = QBI_MEM_REALLOC(buf->data, buf->size, new_size);
    if (new_data == NULL)
    {
      QBI_LOG_E_2("Couldn't allocate memory to resize from %d to %d bytes!",
                  buf->size, new_size);
    }
    else
    {
      /* Clear any newly allocated memory */
      if (new_size > buf->size)
      {
        QBI_MEMSET(new_data + buf->size, 0, new_size - buf->size);
      }
      buf->size = new_size;
      buf->data = new_data;

      result = TRUE;
    }
  }

  return result;
} /* qbi_util_buf_resize() */

/*===========================================================================
  FUNCTION: qbi_util_buf_swap
===========================================================================*/
/*!
    @brief Swaps the contents of two buffer handles

    @details
    Takes no action if one of the arguments is NULL.

    @param buf1
    @param buf2
*/
/*=========================================================================*/
void qbi_util_buf_swap
(
  qbi_util_buf_s *buf1,
  qbi_util_buf_s *buf2
)
{
  qbi_util_buf_s tmp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(buf1);
  QBI_CHECK_NULL_PTR_RET(buf2);

  QBI_MEMSCPY(&tmp, sizeof(qbi_util_buf_s), buf2, sizeof(qbi_util_buf_s));
  QBI_MEMSCPY(buf2, sizeof(qbi_util_buf_s), buf1, sizeof(qbi_util_buf_s));
  QBI_MEMSCPY(buf1, sizeof(qbi_util_buf_s), &tmp, sizeof(qbi_util_buf_s));
} /* qbi_util_buf_swap() */

/*===========================================================================
  FUNCTION: qbi_util_convert_ascii_to_gsm_alphabet
===========================================================================*/
/*!
    @brief Performs in-place conversion of an ASCII encoded string into
    unpacked (8 bits per character) GSM default alphabet encoding

    @details
    Does not handle language shift tables. If a character is out of the
    convertible range (0x00 - 0x7E), then a space character is used and
    this function will return FALSE.

    @param convert_string ASCII encoded string to be converted to GSM
    default alphabet
    @param num_bytes Number of bytes in ascii_string to convert to GSM
    default alphabet

    @return boolean TRUE on success, FALSE on failure (i.e. NULL pointer or
    character out of convertible range)
*/
/*=========================================================================*/
boolean qbi_util_convert_ascii_to_gsm_alphabet
(
  uint8 *convert_string,
  uint32 num_bytes
)
{
  uint32 i;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(convert_string);

  /* Use the mapping table to convert the string */
  for (i = 0; i < num_bytes; i++)
  {
    if (convert_string[i] > QBI_UTIL_ASCII_MAX)
    {
      QBI_LOG_E_2("Character 0x%02x at index %d is out of convertible ASCII "
                  "range!", convert_string[i], i);
      /* Use a space in place of the inconvertible value */
      convert_string[i] = qbi_util_ascii_to_gsm_table[' '];
      success = FALSE;
    }
    else
    {
      convert_string[i] = qbi_util_ascii_to_gsm_table[convert_string[i]];
    }
  }

  return success;
} /* qbi_util_convert_ascii_to_gsm_alphabet */

/*===========================================================================
  FUNCTION: qbi_util_convert_gsm_alphabet_to_ascii
===========================================================================*/
/*!
    @brief Performs in-place conversion of an unpacked (8 bits per character)
    GSM default alphabet encoded string into ASCII encoding

    @details
    If a character is out of the convertible range (0x00 - 0x7E), then a
    space character is used and this function will return FALSE.

    @param convert_string Unpacked (8 bits per character) string using
    GSM default alphabet
    @param num_bytes Number of characters in convert_string to convert from
    GSM default alphabet to ASCII

    @return boolean TRUE on success, FALSE on failure (i.e. NULL pointer or
    character out of convertible range)
*/
/*=========================================================================*/
boolean qbi_util_convert_gsm_alphabet_to_ascii
(
  uint8 *convert_string,
  uint32 num_bytes
)
{
  uint32 i;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(convert_string);

  /* Use the mapping table to convert the string */
  for (i = 0; i < num_bytes; i++)
  {
    if (convert_string[i] > QBI_UTIL_GSM_ALPHABET_MAX)
    {
      QBI_LOG_E_2("Character 0x%02x at index %d is out of valid GSM default "
                  "alphabet range!", convert_string[i], i);
      /* Use a space in place of the inconvertible value */
      convert_string[i] = ' ';
      success = FALSE;
    }
    else
    {
      convert_string[i] = qbi_util_gsm_to_ascii_table[convert_string[i]];
    }
  }

  return success;
} /* qbi_util_convert_gsm_alphabet_to_ascii */

/*===========================================================================
  FUNCTION: qbi_util_convert_iso_8859_1_to_gsm_default
===========================================================================*/
/*!
    @brief Performs in-place conversion of an ISO/IEC 8859-1 (Latin) encoded
    string into GSM default alphabet encoding (unpacked/8 bits per char)

    @details
    If a character is out of the defined range per the specification
    (0x20 - 0x7E, 0xA0 - 0xFF), then a space character is used and this
    function will return FALSE.

    @param convert_string String encoded with ISO/IEC 8859-1 to convert to
    GSM default alphabet encoding (unpacked)
    @param num_bytes Number of bytes of convert_string to convert

    @return boolean TRUE on success, FALSE on failure (i.e. NULL pointer or
    character out of range)
*/
/*=========================================================================*/
boolean qbi_util_convert_iso_8859_1_to_gsm_default
(
  uint8 *convert_string,
  uint32 num_bytes
)
{
  uint32 i;
  boolean success = TRUE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(convert_string);

  for (i = 0; i < num_bytes; i++)
  {
    if (convert_string[i] >= QBI_UTIL_ISO_8859_1_LOWER_RANGE_MIN &&
        convert_string[i] <= QBI_UTIL_ISO_8859_1_LOWER_RANGE_MAX)
    {
      /* ISO/IEC 8859-1 is equivalent to ASCII for the range 0x20-0x7E */
      convert_string[i] = qbi_util_ascii_to_gsm_table[convert_string[i]];
    }
    else if (convert_string[i] >= QBI_UTIL_ISO_8859_1_UPPER_RANGE_MIN)
    {
      /* Upper range encodes special characters, of which not all are supported
         in the GSM default alphabet. Use the closest match where applicable. */
      convert_string[i] = qbi_util_iso_8859_1_upper_range_to_gsm_table[
        (convert_string[i] - QBI_UTIL_ISO_8859_1_UPPER_RANGE_MIN)];
    }
    else
    {
      QBI_LOG_E_2("Encountered invalid ISO/IEC 8859-1 code point 0x%02x at "
                  "index %d", convert_string[i], i);
      convert_string[i] = qbi_util_ascii_to_gsm_table[' '];
      success = FALSE;
    }
  }

  return success;
} /* qbi_util_convert_iso_8859_1_to_gsm_default() */

/*===========================================================================
  FUNCTION: qbi_util_list_init
===========================================================================*/
/*!
    @brief Initializes a list

    @details

    @param list
*/
/*=========================================================================*/
void qbi_util_list_init
(
  qbi_util_list_s *list
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(list);
  QBI_MEMSET(list, 0, sizeof(qbi_util_list_s));
} /* qbi_util_list_init() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_cur
===========================================================================*/
/*!
    @brief Returns the data pointer for the item on the list that this
    iterator currently points to

    @details
    Will not change the position of the iterator

    @param iter

    @return void* Pointer to the data contained in the current list entry,
    or NULL if the iterator is not currently pointing to a list item
*/
/*=========================================================================*/
void *qbi_util_list_iter_cur
(
  const qbi_util_list_iter_s *iter
)
{
  void *data = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(iter);

  if (iter->cur != NULL)
  {
    data = qbi_util_list_entry_get_data(iter->cur);
  }

  return data;
} /* qbi_util_list_iter_cur() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_dereg
===========================================================================*/
/*!
    @brief Deregisters a list iterator from notifications of list structure
    changes

    @details
    If a list iterator was previously registered via
    qbi_util_list_iter_reg(), then this function MUST be called before the
    list iterator is discarded.

    @param iter
*/
/*=========================================================================*/
void qbi_util_list_iter_dereg
(
  qbi_util_list_iter_s *iter
)
{
  qbi_util_list_iter_s *last;
  qbi_util_list_iter_s *next;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(iter);
  QBI_CHECK_NULL_PTR_RET(iter->list);

  last = NULL;
  next = iter->list->reg_iters;
  while (next != NULL)
  {
    if (next == iter)
    {
      if (last == NULL)
      {
        iter->list->reg_iters = iter->next_reg_iter;
      }
      else
      {
        last->next_reg_iter = iter->next_reg_iter;
      }
      iter->next_reg_iter = NULL;
      break;
    }
    last = next;
    next = next->next_reg_iter;
  }
} /* qbi_util_list_iter_dereg() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_init
===========================================================================*/
/*!
    @brief Initializes a list iterator

    @details
    This simple list iterator implementation is not thread safe and must
    be registered via qbi_util_list_iter_reg() if it is to be intermixed
    with calls to qbi_util_list_remove, qbi_util_list_purge, or
    qbi_util_list_pop_*.

    @param list
    @param iter
*/
/*=========================================================================*/
void qbi_util_list_iter_init
(
  qbi_util_list_s      *list,
  qbi_util_list_iter_s *iter
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(iter);
  QBI_CHECK_NULL_PTR_RET(list);

  iter->list          = list;
  iter->next_reg_iter = NULL;

  qbi_util_list_iter_reset(iter);
} /* qbi_util_list_iter_init() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_has_next
===========================================================================*/
/*!
    @brief Checks whether there is a next item on the list

    @details
    This does NOT guarantee that qbi_util_list_iter_next will return a
    non-NULL value.

    @param iter

    @return boolean TRUE if there is a next item on the list, FALSE
    otherwise
*/
/*=========================================================================*/
boolean qbi_util_list_iter_has_next
(
  qbi_util_list_iter_s *iter
)
{
/*-------------------------------------------------------------------------*/
  return (iter != NULL && iter->next != NULL);
} /* qbi_util_list_iter_has_next() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_insert_before_aliased
===========================================================================*/
/*!
    @brief Inserts a new list entry (aliased) in the adjacent position that
    precedes the given list iterator's current position (i.e. the last entry
    returned by qbi_util_list_iter_next())

    @details
    The list iterator must have been previously initialized via
    qbi_util_list_iter_init(). The given aliased list entry must not be
    already linked to any list.

    @param iter
    @param entry

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_util_list_iter_insert_before_aliased
(
  qbi_util_list_iter_s  *iter,
  qbi_util_list_entry_s *entry
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(iter);
  QBI_CHECK_NULL_PTR_RET_FALSE(entry);

  QBI_MEMSET(entry, 0, sizeof(qbi_util_list_entry_s));
  qbi_util_list_entry_insert_before(iter->list, entry, iter->cur);

  return TRUE;
} /* qbi_util_list_iter_insert_before_aliased() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_next
===========================================================================*/
/*!
    @brief Advances the list iterator to the next item on the list

    @details

    @param iter

    @return void* Pointer to data contained by the next list entry, or NULL
    if the end of the list has been reached or there was an error
*/
/*=========================================================================*/
void *qbi_util_list_iter_next
(
  qbi_util_list_iter_s *iter
)
{
  void *data = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(iter);

  iter->cur = iter->next;
  if (iter->cur != NULL)
  {
    iter->next = iter->cur->next;
    data = qbi_util_list_entry_get_data(iter->cur);
  }

  return data;
} /* qbi_util_list_iter_next() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_peek
===========================================================================*/
/*!
    @brief Gets a pointer to the next item on the list without advancing the
    iterator to it

    @details

    @param iter

    @return void* Pointer to data contained by the next list entry, or NULL
    if there is none
*/
/*=========================================================================*/
void *qbi_util_list_iter_peek
(
  qbi_util_list_iter_s *iter
)
{
  void *data = NULL;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(iter);

  if (iter->next != NULL)
  {
    data = qbi_util_list_entry_get_data(iter->next);
  }

  return data;
} /* qbi_util_list_iter_peek() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_reg
===========================================================================*/
/*!
    @brief Registers a list iterator with the given list to be updated of
    changes to the list structure

    @details
    This registration effectively allows removal of entries within the list
    while iterating through it.  A registered list iterator MUST be
    deregistered via qbi_util_list_iter_dereg() once it is no longer in use.

    @param iter
*/
/*=========================================================================*/
void qbi_util_list_iter_reg
(
  qbi_util_list_iter_s *iter
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET(iter);
  QBI_CHECK_NULL_PTR_RET(iter->list);

  iter->next_reg_iter = iter->list->reg_iters;
  iter->list->reg_iters = iter;
} /* qbi_util_list_iter_reg() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_remove
===========================================================================*/
/*!
    @brief Removes the entry at the iterator's current position

    @details
    The current position is the last value returned by
    qbi_util_list_iter_next(). Upon returning from this function, the list
    iterator's current position will be the previous list entry.

    @param iter

    @return void* Pointer to data at the iterator's current position
*/
/*=========================================================================*/
void *qbi_util_list_iter_remove
(
  qbi_util_list_iter_s *iter
)
{
  void *data;
  qbi_util_list_entry_s *entry;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_NULL(iter);
  QBI_CHECK_NULL_PTR_RET_NULL(iter->cur);

  entry = iter->cur;
  iter->cur = entry->prev;
  data = qbi_util_list_entry_remove(iter->list, entry);

  return data;
} /* qbi_util_list_iter_remove() */

/*===========================================================================
  FUNCTION: qbi_util_list_iter_seek
===========================================================================*/
/*!
    @brief Seeks a list iterator to the entry provided

    @details
    Performs a linear search for a list entry with matching data pointer,
    starting from the front.

    @param iter
    @param pos Data pointer to seek the list iterator to

    @return boolean TRUE if successfully seeked to matching list entry,
    FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_util_list_iter_seek
(
  qbi_util_list_iter_s *iter,
  const void           *pos
)
{
  const void *data;
  boolean found = FALSE;
/*-------------------------------------------------------------------------*/
  qbi_util_list_iter_reset(iter);
  while ((data = qbi_util_list_iter_next(iter)) != NULL)
  {
    if (data == pos)
    {
      found = TRUE;
      break;
    }
  }

  return found;
} /* qbi_util_list_iter_seek() */

/*===========================================================================
  FUNCTION: qbi_util_list_pop_back
===========================================================================*/
/*!
    @brief Removes the last entry from a list

    @details

    @param list

    @return void* Pointer to data item popped off list, or NULL if list
    is empty
*/
/*=========================================================================*/
void *qbi_util_list_pop_back
(
  qbi_util_list_s *list
)
{
  void *data = NULL;
/*-------------------------------------------------------------------------*/
  if (list != NULL && list->tail != NULL)
  {
    data = qbi_util_list_entry_remove(list, list->tail);
  }

  return data;
} /* qbi_util_list_pop_front() */

/*===========================================================================
  FUNCTION: qbi_util_list_pop_front
===========================================================================*/
/*!
    @brief Removes the first entry on a list

    @details

    @param list

    @return void* Pointer to data item popped off list, or NULL if list
    is empty
*/
/*=========================================================================*/
void *qbi_util_list_pop_front
(
  qbi_util_list_s *list
)
{
  void *data = NULL;
/*-------------------------------------------------------------------------*/
  if (list != NULL && list->head != NULL)
  {
    data = qbi_util_list_entry_remove(list, list->head);
  }

  return data;
} /* qbi_util_list_pop_front() */

/*===========================================================================
  FUNCTION: qbi_util_list_purge
===========================================================================*/
/*!
    @brief Removes all entries from a list

    @details

    @param list
*/
/*=========================================================================*/
void qbi_util_list_purge
(
  qbi_util_list_s *list
)
{
/*-------------------------------------------------------------------------*/
  while (qbi_util_list_pop_front(list) != NULL);
} /* qbi_util_list_purge() */

/*===========================================================================
  FUNCTION: qbi_util_list_push_back
===========================================================================*/
/*!
    @brief Adds a new entry onto the back of a list

    @details
    This function involves a separate allocation for the list entry
    information - to reduce the number of allocations, the list entry info
    can be included with the data: see qbi_util_list_push_back_aliased

    @param list
    @param data

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_util_list_push_back
(
  qbi_util_list_s *list,
  void            *data
)
{
  qbi_util_list_entry_s *entry;
  boolean result = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(list);
  QBI_CHECK_NULL_PTR_RET_FALSE(data);

  entry = QBI_MEM_MALLOC(sizeof(qbi_util_list_entry_s));
  if (entry == NULL)
  {
    QBI_LOG_E_0("Couldn't allocate new list entry!");
  }
  else
  {
    #ifdef QBI_MEM_DEBUG
    QBI_LOG_D_2("Allocated list entry %p for list %p", entry, list);
    #endif /* QBI_MEM_DEBUG */

    entry->data = data;
    qbi_util_list_entry_insert_before(list, entry, NULL);
    result = TRUE;
  }

  return result;
} /* qbi_util_list_push_back() */

/*===========================================================================
  FUNCTION: qbi_util_list_push_back_aliased
===========================================================================*/
/*!
    @brief Adds a new entry onto the back of a list, utilizing a
    qbi_util_list_entry_s that is aliased with the data structure

    @details
    To use this function, a qbi_util_list_entry_s must appear as the first
    field in the data structure, thereby aliasing qbi_util_list_entry_s with
    the data will be added to the list.

    Compared to qbi_util_list_push_back(), this method gives the advantage
    of not needing a separate memory allocation for the
    qbi_util_list_entry_s, with the tradeoff that the data can only appear
    on one list at a time. When an entry is added using this method, all
    other list & iterator functions can be used normally.

    @param list
    @param entry Pointer to list entry structure that is the first field
    in the data structure that will be linked in the list. This entry must
    not already be linked with another list.

    @return boolean TRUE on success, FALSE on failure
*/
/*=========================================================================*/
boolean qbi_util_list_push_back_aliased
(
  qbi_util_list_s       *list,
  qbi_util_list_entry_s *entry
)
{
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(list);
  QBI_CHECK_NULL_PTR_RET_FALSE(entry);

  QBI_MEMSET(entry, 0, sizeof(qbi_util_list_entry_s));
  qbi_util_list_entry_insert_before(list, entry, NULL);

  return TRUE;
} /* qbi_util_list_push_back_aliased() */

/*===========================================================================
  FUNCTION: qbi_util_list_remove
===========================================================================*/
/*!
    @brief Removes all list entries associated with the given data pointer

    @details
    Since this list implementation does not enforce uniqueness, more than
    one entry could exist with the given data. This function will remove
    all such entries.

    @param list
    @param data data item to remove from the list

    @return boolean TRUE if at least one entry was removed, FALSE otherwise

    @note In general, this function is not safe to use in conjunction with
    a list iterator - use qbi_util_list_iter_remove or re-initialize the
    list iterator every time after calling this function.
*/
/*=========================================================================*/
boolean qbi_util_list_remove
(
  qbi_util_list_s *list,
  void            *data
)
{
  qbi_util_list_entry_s *entry;
  qbi_util_list_entry_s *temp;
  boolean removed = FALSE;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_FALSE(list);
  QBI_CHECK_NULL_PTR_RET_FALSE(data);

  entry = list->head;
  while (entry != NULL)
  {
    temp = entry->next;
    if (data == qbi_util_list_entry_get_data(entry))
    {
      (void) qbi_util_list_entry_remove(list, entry);
      removed = TRUE;
    }
    entry = temp;
  }

  return removed;
} /* qbi_util_list_remove() */

/*===========================================================================
  FUNCTION: qbi_util_utf16_copy_convert_to_le_if_needed
===========================================================================*/
/*!
    @brief Copy a UTF-16 string, performing byte swapping to correct
    endianness if needed

    @details
    If a byte order mark (BOM) is present at the beginning of the source
    string, uses that to determine endianness. Copies until a NULL character
    is encountered in the source buffer, or the size of either buffer is
    exhausted. Only NULL-terminates the destination string if there is
    room.

    This function supports in-place byte swapping, e.g. passing in the same
    buffer as the source and destination buffer arguments.

    @param dst Destination buffer
    @param dst_size Size of the destination buffer in bytes
    @param src Source buffer containing UTF-16 string
    @param src_size Size of the source buffer in bytes (this is not
    necessarily the length of the string)
    @param default_is_be Set to TRUE if big endian byte order of src should
    be assumed when no byte order mark is present

    @return uint32 Number of bytes set in the destination buffer not
    including the NULL character (if set)
*/
/*=========================================================================*/
uint32 qbi_util_utf16_copy_convert_to_le_if_needed
(
  uint8       *dst,
  uint32       dst_size,
  const uint8 *src,
  uint32       src_size,
  boolean      default_is_be
)
{
  uint32 bytes_copied = 0;
  uint16 utf16_char;
  boolean swap_bytes;
  uint32 src_index;
  uint8 tmp;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(dst);
  QBI_CHECK_NULL_PTR_RET_ZERO(src);

  if (dst_size == 0)
  {
    QBI_LOG_E_1("Invalid dst size %d", dst_size);
  }
  else
  {
    /* Interpret the byte order mark (BOM) at the beginning of the string, if
       present. Set initial src_index to 2 if BOM is present to skip copying it
       to the destination buffer, since MBIM always expects UTF-16LE */
    utf16_char = src[0] | (src[1] << 8);
    if (utf16_char == QBI_UTIL_UTF16_BOM_BE)
    {
      QBI_LOG_D_0("Swapping bytes due to big endian BOM");
      swap_bytes = TRUE;
      src_index = 2;
    }
    else if (utf16_char == QBI_UTIL_UTF16_BOM_LE)
    {
      QBI_LOG_D_0("Not swapping bytes due to little endian BOM");
      swap_bytes = FALSE;
      src_index = 2;
    }
    else
    {
      QBI_LOG_D_2("No BOM found (first char 0x%04x); using default swap "
                  "(%d)", utf16_char, default_is_be);
      swap_bytes = default_is_be;
      src_index = 0;
    }


    /* Round the destination and source buffer size down to the nearest multiple
       of 2 */
    dst_size &= ~1;
    src_size &= ~1;

    /* Copy src to dst, skipping */
    while (src_index < src_size && bytes_copied < dst_size &&
           (src[src_index] != 0x00 || src[src_index + 1] != 0x00))
    {
      if (swap_bytes)
      {
        /* Use a temporary variable to allow in-place byte swapping (src and
           dst buffer both pointing to the same location) */
        tmp = src[src_index];
        dst[bytes_copied++] = src[src_index + 1];
        dst[bytes_copied++] = tmp;
        src_index += 2;
      }
      else
      {
        dst[bytes_copied++] = src[src_index++];
        dst[bytes_copied++] = src[src_index++];
      }
    }

    /* NULL-terminate if there's room */
    if (bytes_copied <= (dst_size - 2))
    {
      dst[bytes_copied]     = 0x00;
      dst[bytes_copied + 1] = 0x00;
    }
  }

  return bytes_copied;
} /* qbi_util_utf16_copy_convert_to_le_if_needed() */

/*===========================================================================
  FUNCTION: qbi_util_utf16_str_is_empty
===========================================================================*/
/*!
    @brief Checks whether a UTF-16 string is empty, i.e. the first character
    is NULL

    @details
    Returns TRUE if the passed in pointer is NULL.

    @param utf16_str

    @return boolean TRUE if string is empty, FALSE otherwise
*/
/*=========================================================================*/
boolean qbi_util_utf16_str_is_empty
(
  const uint8 *utf16_str
)
{
/*-------------------------------------------------------------------------*/
  return ((utf16_str == NULL) ||
          (utf16_str[0] == 0x00 && utf16_str[1] == 0x00));
} /* qbi_util_utf16_str_is_empty() */

/*===========================================================================
  FUNCTION: qbi_util_utf16_to_ascii
===========================================================================*/
/*!
    @brief Transcode a UTF-16 (little endian) string into ASCII

    @details
    Runs until either the NULL character is found in utf16, or the end of
    an array is reached. Skips UTF-16 characters that can't be represented
    in ASCII. Guarantees to NULL-terminate the resulting ASCII string.

    If the return value is greater than ascii_len, then truncation of the
    input string occurred.

    @param utf16
    @param utf16_len
    @param ascii
    @param ascii_len

    @return uint32 Number of bytes set in the ASCII string (includes NULL).
    This will be 0 if an error occurred and ascii was not changed. If the
    UTF-16 string was too long to be completely copied into the ASCII buffer,
    returns the number of bytes that would have been needed to fit the UTF-16
    string.
*/
/*=========================================================================*/
uint32 qbi_util_utf16_to_ascii
(
  const uint8 *utf16,
  uint32       utf16_len_bytes,
  char        *ascii,
  uint32       ascii_len_bytes
)
{
  uint32 ascii_offset = 0;
  uint32 utf16_offset;
/*-------------------------------------------------------------------------*/
  QBI_CHECK_NULL_PTR_RET_ZERO(utf16);
  QBI_CHECK_NULL_PTR_RET_ZERO(ascii);

  if (ascii_len_bytes == 0)
  {
    QBI_LOG_E_1("Invalid dst size %d", ascii_len_bytes);
  }
  else
  {
    /* Decrement the destination buffer size by one to make sure we will have
       room for NULL termination when the loop exits */
    ascii_len_bytes--;

    for (utf16_offset = 0; utf16_offset < utf16_len_bytes && ascii_offset < ascii_len_bytes; utf16_offset += 2)
    {
      if (utf16[utf16_offset] == QBI_UTIL_ASCII_NULL)
      {
        break;
      }
      else if (utf16[utf16_offset] > QBI_UTIL_ASCII_MAX ||
               utf16[utf16_offset + 1] != 0x00)
      {
        QBI_LOG_W_3("UTF-16 character 0x%02x%02x at byte offset %d can't be "
                    "encoded to ASCII!", utf16[utf16_offset],
                    utf16[utf16_offset + 1], utf16_offset);
      }
      else
      {
        ascii[ascii_offset++] = (char) utf16[utf16_offset];
      }
    }

    ascii[ascii_offset++] = QBI_UTIL_ASCII_NULL;

    if (utf16_offset < utf16_len_bytes &&
        utf16[utf16_offset] != QBI_UTIL_ASCII_NULL)
    {
      QBI_LOG_E_0("UTF-16 string truncated when copying into ASCII buffer!");
      for (; utf16_offset < utf16_len_bytes; utf16_offset += 2)
      {
        if (utf16[utf16_offset] == QBI_UTIL_ASCII_NULL)
        {
          break;
        }
        else
        {
          ascii_offset++;
        }
      }
    }
  }

  return ascii_offset;
} /* qbi_util_utf16_to_ascii() */

