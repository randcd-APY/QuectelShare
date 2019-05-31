#ifndef CSR_MBLK_H__
#define CSR_MBLK_H__

#include "csr_synergy.h"
/*****************************************************************************

Copyright (c) 2009-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXCLUDE_MBLK_BUFFER_SUPPORT
#define VAR_DEFINE(type, name, size) type name

struct CsrMblk_tag;


/*!\ brief Maximum size of MBLK
    Defines constant which is the maximum size of data that a MBLK can contain.
    When running on-chip the absolute maximum is 4K as there can be no buffers
    larger than this.  For off-chip builds the upper limit is 64K.

    If we are using bitfields to store the length then the maximum may be
    reduced below.
*/
#if 1
#define MBLK_MAX_SIZE_NOT_BITFIELD  0xFFFF
#else
#define MBLK_MAX_SIZE_NOT_BITFIELD  MAX_BUFFER_SIZE_BYTES
#endif

/*!\ brief When we're using bitfields, we crop the maximum MBLK
    size so that it will fit.
*/
#ifdef USE_BLUESTACK_BITFIELDS
#define MBLK_REF_COUNT_NUM_BITS 4
#define MBLK_MAX_SIZE_NUM_BITS 12
#define MBLK_MAX_SIZE (MBLK_MAX_SIZE_NOT_BITFIELD > ((1 << MBLK_DATA_SIZE_NUM_BITS) - 1) ? ((1 << MBLK_DATA_SIZE_NUM_BITS) - 1) : MBLK_MAX_SIZE_NOT_BITFIELD)
#else
#define MBLK_MAX_SIZE MBLK_MAX_SIZE_NOT_BITFIELD
#endif

/*! \brief Size of MBLK

    A type big enough to hold the maximum size of an mblk we're prepared to
    deal with.

    Strictly, the maximum size mblk we could theoretically see is
    65539 octets (65535 byte L2CAP payload with a 4 byte header).  In
    practice, we don't have enough memory to hold that on chip, so
    it's unlikely to happen.
*/
typedef CsrUint16 CsrMblkSize;

/*! \brief MBLK operator table

    This table contain pointers to functions for the various
    operations that can be performed on a MBLK. You should NOT use
    these functions directly - instead, use the generic functions
    below this structure
*/
typedef struct CsrMblkVtable_tag
{
    /*! Destroy MBLK */
    void (*destroy)(struct CsrMblk_tag *);

    /*! Read from head of MBLK */
    CsrMblkSize (*read_head)(struct CsrMblk_tag *mblk_ptr,
        void *buffer,
        CsrMblkSize *mblk_size);

    /*! Read from tail of MBLK */
    CsrMblkSize (*read_tail)(struct CsrMblk_tag *mblk_ptr,
        void *buffer,
        CsrMblkSize *mblk_size);

    /*! Copy from MBLK to memory */
    CsrMblkSize (*copy_to_memory)(const struct CsrMblk_tag *,
        CsrMblkSize mblk_offset,
        CsrMblkSize mblk_size,
        CsrUint8 *mem_ptr);

#ifndef EXCLUDE_MBLK_BUFFER_SUPPORT
    /*! Copy from MBLK to Buffer */
    /*    CsrMblkSize (*copy_to_buffer)(const struct CsrMblk_tag *,
                                      CsrMblkSize mblk_offset,
                                      CsrMblkSize mblk_size,
                                      BUFFER *buffer,
                                      CsrMblkSize buffer_offset);*/
#endif

    /*! Map MBLK into linear buffer */
    void *(*map)(const struct CsrMblk_tag *mblk_ptr,
                 CsrMblkSize mblk_offset,
                 CsrMblkSize mblk_size);

    /*! Unmap linear buffer */
    void (*unmap)(const struct CsrMblk_tag *mblk_ptr,
        void *map_ptr);

    struct CsrMblk_tag *(*split)(struct CsrMblk_tag *mblk,
                                 CsrMblkSize split_offset);

    CsrMblkSize (*coalesce)(struct CsrMblk_tag *mblk1,
        struct CsrMblk_tag **mblk2);
} CsrMblkVtable;

/*! \brief MBLK structure

    This structure is used to pass data up and down
    the stack.
*/
typedef struct CsrMblk_tag
{
#ifdef MBLK_DEBUG
    /*! Special guard value, used during debugging to detect invalid MBLK pointers */
    CsrUint16 guard;
#endif

    struct CsrMblk_tag  *next_ptr;           /*!< Pointer to next MBLK in chain */
    const CsrMblkVtable *vtable;            /*!< Pointer to function table */
    void (*destructor)(struct CsrMblk_tag *); /*!< User defined destructor to call on MBLK destruction */

    VAR_DEFINE(CsrUint8, ref_count, MBLK_REF_COUNT_NUM_BITS); /*!< Number of references to the MBLK */
    VAR_DEFINE(CsrMblkSize, data_size, MBLK_MAX_SIZE_NUM_BITS); /*!< Length of MBLK data (in bytes) */

    union
    {
        /*! Structure used for pmalloc type MBLKS */
        struct
        {
            CsrUint8 *data;                  /*!< Pointer to start of data */
            void     *block;                /*!< Pointer to address to give to CsrPmemFree() if block is to be released */
        } pmalloc;

#ifndef EXCLUDE_MBLK_BUFFER_SUPPORT
        /*! Structure used for Buffer type MBLKS */
        struct
        {
            MSGFRAG    *msgfrag;            /*!< Pointer to MSGFRAG */
            CsrMblkSize offset;             /*!< Offset from start of MSGFRAG before data */
        } msgfrag;
#endif

        /*! Structure used for MBLK duplicates */
        struct
        {
            struct CsrMblk_tag *mblk_ptr;    /*!< Pointer to original MBLK */
            CsrMblkSize         offset;     /*!< Offset into original MBLK */
        } duplicate;
    } u;
} CsrMblk;

/*! \brief MSGFRAG style MBLKs */
#ifndef EXCLUDE_MBLK_BUFFER_SUPPORT
CsrMblk *CsrMblkMsgfragCreate(MSGFRAG *msgfrag);
CsrMblk *CsrMblkBufferCreate(BUFFER *buffer,
    CsrMblkSize length);
#if 1
CsrMblk *CsrMblkMsgfragCreateMeta(MSGFRAG *msgfrag,
    const void *metadata,
    CsrUint16 metasize);
CsrMblk *CsrMblkBufferCreateMeta(BUFFER *buffer,
    CsrMblkSize length,
    const void *metadata,
    CsrUint16 metasize);
#endif
#endif

/*! \brief pmalloc style MBLKs */
extern CsrMblk *CsrMblkDataCreate(void *block,
    CsrMblkSize block_size,
    CsrBool free_block);
extern CsrMblk *CsrMblkMallocCreate(void **block_ptr,
    CsrMblkSize block_Size);
#if 1
CsrMblk *CsrMblkDataCreateMeta(void *block,
    CsrMblkSize block_size,
    CsrBool free_block,
    const void *metadata,
    CsrUint16 metasize);
CsrMblk *CsrMblkMallocCreateMeta(void **block_ptr,
    CsrMblkSize block_size,
    const void *metadata,
    CsrUint16 metasize);
#endif

/*! \brief Duplicate style MBLKs */
CsrMblk *CsrMblkDuplicateCreate(CsrMblk *dup_mblk,
    CsrMblkSize dup_offset,
    CsrMblkSize dup_length);
CsrMblk *CsrMblkDuplicateRegion(CsrMblk *mblk,
    CsrMblkSize dup_offset,
    CsrMblkSize dup_size);
#if 1
CsrMblk *CsrMblkCopy(CsrMblk *mblk);
#endif

/*! \brief Length and destruction handling */
CsrMblkSize CsrMblkGetLength(const CsrMblk *mblk);
void CsrMblkDestroy(CsrMblk *mblk);
void CsrMblkSetDestructor(CsrMblk *mblk,
    void (*destructor)(CsrMblk *));
void CsrMblkIncRefcount(CsrMblk *mblk, CsrUint8 inc);

/*! \brief MBLK data manipulation */
CsrMblk *CsrMblkJoinChain(CsrMblk *item,
    CsrMblk *chain);
#define CsrMblkAddHead(head, chain) (CsrMblkJoinChain((head), (chain)))
#define CsrMblkAddTail(tail, chain) (CsrMblkJoinChain((chain), (tail)))
CsrMblkSize CsrMblkReadHead(CsrMblk **mblk,
    void *data,
    CsrMblkSize size_left);
CsrBool CsrMblkReadHead8(CsrMblk **mblk, CsrUint8 *value);
CsrBool CsrMblkReadHead16(CsrMblk **mblk, CsrUint16 *value);
CsrMblkSize CsrMblkReadTail(CsrMblk **mblk,
    void *data,
    CsrMblkSize size_left);
CsrMblkSize CsrMblkDiscardHead(CsrMblk **mblk,
    CsrMblkSize size_left);
CsrMblkSize CsrMblkDiscardTail(CsrMblk **mblk,
    CsrMblkSize size_left);
CsrBool CsrMblkCoalesceToPmalloc(CsrMblk **mblk);

/*! \brief Copy data to buffer/memory */
CsrMblkSize CsrMblkCopyToMemory(const CsrMblk *mblk,
    CsrUint16 mblk_offset,
    CsrMblkSize mblk_size,
    CsrUint8 *mem_ptr);
#ifndef EXCLUDE_MBLK_BUFFER_SUPPORT
CsrMblkSize CsrMblkCopyToBuffer(const CsrMblk *mblk,
    CsrMblkSize mblk_offset,
    CsrMblkSize mblk_size,
    BUFFER *buffer,
    CsrUint16 buffer_offset);
#endif

/*! \brief Mapping functions and iterator */
void *CsrMblkMap(const CsrMblk *mblk,
    CsrMblkSize mblk_offset,
    CsrMblkSize mblk_size);
void CsrMblkUnmap(const CsrMblk *mblk,
    void *mapped_data);
void CsrMblkIterateRegion(const CsrMblk *mblk,
    CsrMblkSize mblk_offset,
    CsrMblkSize mblk_size,
    CsrBool (*itr_func)(const void *ifct_buffer,
        CsrMblkSize ifct_offset,
        CsrMblkSize ifct_length,
        void *ifct_data),
    void *itr_data);

CsrMblk *CsrMblkSplit(CsrMblk *mblk, CsrMblkSize split_offset);
CsrMblkSize CsrMblkCoalesce(CsrMblk *mblk1, CsrMblk **mblk2);

#if 1
/*! \brief Get meta-data for MBLK

    Return pointer to meta-data associated with MBLK.
    NOTE: It's the responsibility of the caller to know if ths MBLK
    actually has an meta-data or not.
    the stack.

    \param mblk Pointer to MBLK.
    \return Pointer to meta-data.
*/
#define CsrMblkGetMetadata(mblk) \
    ((void *) (((CsrMblk *) (mblk)) + 1))
#endif

#ifdef __cplusplus
}
#endif

#endif
