/******************************************************************************

    @file
    util_htables.h

    @brief
    APIs for hash table utility.

    Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.

******************************************************************************/

#include "util_list.h"
#include "util_memory.h"
#include "util_log.h"

/* abstract hash table type */
typedef void* utilHashTable;

/* hash key value pair */
typedef struct utilHashKeyValue
{
    void *key;
    void *data;
} *utilHashKeyValue;

/*===========================================================================

  TYPE: utilHashFunc

===========================================================================*/
/*!
    @brief
    hash function type definition
*/
/*=========================================================================*/
typedef unsigned int (*utilHashFunc) (void *key);

/*===========================================================================

  TYPE: utilEqualFunc

===========================================================================*/
/*!
    @brief
    compares two keys for equality
*/
/*=========================================================================*/
typedef int (*utilEqualFunc) (void *lookup_key, void *key);


/*===========================================================================

  FUNCTION:  util_hash_table_create

===========================================================================*/
/*!
    @brief
    create hash table

    @return
    Pointer to create hash table.
*/
/*=========================================================================*/
utilHashTable util_hash_table_create
(
    utilHashFunc  hash_func,
    utilEqualFunc key_equal_func
);

/*===========================================================================

  FUNCTION:  util_hash_table_destroy

===========================================================================*/
/*!
    @brief
    destroy hash table

    @return
    None
*/
/*=========================================================================*/
void util_hash_table_destroy
(
    utilHashTable         hash_table,
    delete_evaluator_type delete_evaluator
);

/*===========================================================================

  FUNCTION:  util_hash_table_insert

===========================================================================*/
/*!
    @brief
    insert entry to hash table

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int util_hash_table_insert
(
    utilHashTable    hash_table,
    utilHashKeyValue keyValue
);

/*===========================================================================

  FUNCTION:  util_hash_table_delete

===========================================================================*/
/*!
    @brief
    delete entry from hash table

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int util_hash_table_delete
(
    utilHashTable hash_table,
    utilHashKeyValue  keyValue
);

/*===========================================================================

  FUNCTION:  util_hash_table_lookup

===========================================================================*/
/*!
    @brief
    lookup for a key from hash table

    @return
    data corresponding to key or NULL
*/
/*=========================================================================*/
void *util_hash_table_lookup
(
    utilHashTable hash_table,
    void         *key
);


/*===========================================================================

  FUNCTION: util_hash_table_lock

===========================================================================*/
/*!
    @brief
    lock hash table

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int util_hash_table_lock
(
    utilHashTable hash_table
);


/*===========================================================================

  FUNCTION: util_hash_table_unlock

===========================================================================*/
/*!
    @brief
    unlock hash table

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int util_hash_table_unlock
(
    utilHashTable hash_table
);
