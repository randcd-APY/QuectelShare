/*!
  @file
  qbi_util.h

  @brief
  Utility functions intended for internal use by QBI
*/

/*=============================================================================

  Copyright (c) 2011,2013 Qualcomm Technologies, Inc. All Rights Reserved.
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
09/02/11  bd   Updated to MBIM v0.81c
08/24/11  bd   Add qbi_util_list_iter_[de]reg()
07/28/11  bd   Initial release based on MBIM v0.3+
=============================================================================*/

#ifndef QBI_UTIL_H
#define QBI_UTIL_H

/*=============================================================================
  Include Files
=============================================================================*/

#include "qbi_os.h"

/*=============================================================================

  Constants and Macros

=============================================================================*/

#define QBI_UTIL_ASCII_NULL (0x00)
#define QBI_UTIL_ASCII_MAX  (0x7F)

#define QBI_UTIL_GSM_ALPHABET_MAX (0x7F)

/* ASCII code points for the start and end of the numeric range (0-9) */
#define QBI_UTIL_ASCII_NUMERIC_MIN (0x30)
#define QBI_UTIL_ASCII_NUMERIC_MAX (0x39)

/* ASCII code points for uppercase A, uppercase F, lowercase a, and lowercase f,
   respectively. */
#define QBI_UTIL_ASCII_UC_A (0x41)
#define QBI_UTIL_ASCII_UC_F (0x46)
#define QBI_UTIL_ASCII_LC_A (0x61)
#define QBI_UTIL_ASCII_LC_F (0x66)

/*! Extract the upper digit from a binary coded decimal (BCD) byte */
#define QBI_UTIL_BCD_HI(a) (((a) >> 4) & 0x0F)

/*! Extract the lower digit from a binary coded decimal (BCD) byte */
#define QBI_UTIL_BCD_LO(a) ((a) & 0x0F)

/*=============================================================================

  Typedefs

=============================================================================*/

/*! @brief Dynamically allocated buffer
*/
typedef struct {
  uint32 size;  /*!< Size of data buffer in bytes */
  void  *data;  /*!< Pointer to location of buffer */
} qbi_util_buf_s;

/*! @brief Dynamically allocated buffer with constant data pointer
    @details Use the normal qbi_util_buf functions then cast to this struct
*/
typedef struct {
  uint32 size;      /*!< Size of the data buffer in bytes */
  const void *data; /*!< Pointer to location of buffer */
} qbi_util_buf_const_s;

/*! @brief List entry
    @details This is generally only used inside qbi_util
*/
typedef struct qbi_util_list_entry_struct {
  struct qbi_util_list_entry_struct *next; /*!< Next entry in list */
  struct qbi_util_list_entry_struct *prev; /*!< Previous entry in list */

  /*! Pointer to data item (e.g. qbi_txn_s) */
  void *data;
} qbi_util_list_entry_s;

/*! @brief List structure
*/
typedef struct {
  qbi_util_list_entry_s *head;
  qbi_util_list_entry_s *tail;

  /*! Singly-linked list of list iterators registered for list entry removal
      updates */
  struct qbi_util_list_iter_struct *reg_iters;
} qbi_util_list_s;

/*! @brief List iterator
    @details Modules outside qbi_util can treat this as a "black box", and
    should not modify it directly.
*/
typedef struct qbi_util_list_iter_struct {
  /*! Pointer to current position */
  qbi_util_list_entry_s *cur;

  /*! Pointer to next position */
  qbi_util_list_entry_s *next;

  /*! Pointer to the list this iterator is traversing */
  qbi_util_list_s *list;

  /*! Pointer to the next list iterator registered for updates on the
      associated list */
  struct qbi_util_list_iter_struct *next_reg_iter;
} qbi_util_list_iter_s;

/*! @brief Linked chain of buffers that can be combined into a single buffer
*/
typedef struct qbi_util_buf_chain_struct {
  /*! List consisting of num_entries qbi_util_buf_chain_entry_s entries */
  qbi_util_list_s list;

  /*! Number of individual buffers that make up the buffer chain */
  uint32 num_entries;
} qbi_util_buf_chain_s;

/*! @brief Result of a comparison between two arbitrary values
*/
typedef enum {
  QBI_UTIL_COMPARISON_RESULT_ERROR = 0,
  QBI_UTIL_COMPARISON_RESULT_LESS_THAN,
  QBI_UTIL_COMPARISON_RESULT_GREATER_THAN,
  QBI_UTIL_COMPARISON_RESULT_EQUAL_TO
} qbi_util_comparison_result_e;

/*! Function to compare the value at one index in an array against a key.
    Returns an enum identifying whether the index is less than, greater than,
    or equal to the key. */
typedef qbi_util_comparison_result_e (qbi_util_array_compare_f)
(
  const void *array,
  uint32      index,
  const void *key
);

/*=============================================================================

  Function Prototypes

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
);

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
);

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
);

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
);

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
);

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
    does that)

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
  FUNCTION: qbi_util_list_init
===========================================================================*/
/*!
    @brief Initializes a list

    @details
    This is implemented as a doubly-linked list and is not thread safe.
    In situations where the list needs to be accessed by multiple threads,
    calls to qbi_util_list_* functions must be guarded by a mutex.

    @param list
*/
/*=========================================================================*/
void qbi_util_list_init
(
  qbi_util_list_s *list
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/*===========================================================================
  FUNCTION: qbi_util_list_iter_remove
===========================================================================*/
/*!
    @brief Removes the entry at the iterator's current position

    @details
    The current position is the last value returned by
    qbi_util_list_iter_next.

    @param iter

    @return void* Pointer to data at the iterator's current position
*/
/*=========================================================================*/
void *qbi_util_list_iter_remove
(
  qbi_util_list_iter_s *iter
);

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
);

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
);

/*===========================================================================
  FUNCTION: qbi_util_list_pop_front
===========================================================================*/
/*!
    @brief Removes the first entry from a list

    @details

    @param list

    @return void* Pointer to data item popped off list, or NULL if list
    is empty
*/
/*=========================================================================*/
void *qbi_util_list_pop_front
(
  qbi_util_list_s *list
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

#endif /* QBI_UTIL_H */

