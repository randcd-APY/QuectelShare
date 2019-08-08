/******************************************************************************

    @file
    util_htables.c

    @brief
    Implements hash table utility.

    Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.

******************************************************************************/

#include "util_htable.h"


#define UTIL_HASH_TABLE_INIT_SIZE  (50)
#define UTIL_HASH_TABLE_EXP_INC_SZ (2)

typedef util_list_info_type *(*hashArrOfBuckets)[];

typedef struct utilHashTableIn
{
    unsigned int        bucket_size;
    utilHashFunc        hash_func;
    utilEqualFunc       key_equal_func;
    hashArrOfBuckets    hash_arr_of_buckets;
    util_sync_data_type hash_table_sync_data;

} *utilHashTableIn;


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
)
{
    int ret;
    utilHashTableIn hash_table_in = (utilHashTableIn)hash_table;

    ret = ESUCCESS;

    if (NULL != hash_table_in)
    {
        UTIL_LOG_MSG( "sync data %x", hash_table_in->hash_table_sync_data.sync_mutex);
        ret = util_sync_data_acquire_mutex(&hash_table_in->hash_table_sync_data);
    }

    return ret;
}

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
)
{
    int ret;
    utilHashTableIn hash_table_in = (utilHashTableIn)hash_table;

    ret = ESUCCESS;

    if (NULL != hash_table_in)
    {
        ret = util_sync_data_release_mutex(&hash_table_in->hash_table_sync_data);
    }

    return ret;
}

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
    utilHashFunc hash_func,
    utilEqualFunc key_equal_func
)
{
    utilHashTableIn hash_table = NULL;
    UTIL_LOG_MSG("entry");

    if (hash_func && key_equal_func)
    {
        hash_table = (utilHashTableIn)util_memory_alloc(sizeof(*hash_table));
        if (hash_table)
        {
            hash_table->hash_arr_of_buckets = (hashArrOfBuckets) util_memory_alloc(
                            sizeof(util_list_info_type *) * UTIL_HASH_TABLE_INIT_SIZE);

            if (hash_table->hash_arr_of_buckets)
            {
                hash_table->bucket_size    = UTIL_HASH_TABLE_INIT_SIZE;
                hash_table->hash_func      = hash_func;
                hash_table->key_equal_func = key_equal_func;
                util_sync_data_init(&hash_table->hash_table_sync_data, 0);
                UTIL_LOG_MSG( "sync data %x", hash_table->hash_table_sync_data.sync_mutex);
            }
            else
            {
                util_memory_free(&hash_table);
                hash_table = NULL;
            }
        }
    }

    return hash_table;
}

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
)
{
    utilHashTableIn hash_table_in = (utilHashTableIn)hash_table;
    int i;

    if (hash_table_in)
    {
        if (hash_table_in->hash_arr_of_buckets)
        {
            for (i = 0; i < hash_table_in->bucket_size; i++)
            {
                if ((*hash_table_in->hash_arr_of_buckets)[i])
                {
                    util_list_cleanup((*hash_table_in->hash_arr_of_buckets)[i],
                                        delete_evaluator);
                }
            }

            util_sync_data_destroy(&(hash_table_in->hash_table_sync_data));
            util_memory_free(&(hash_table_in->hash_arr_of_buckets));
        }

        util_memory_free(&hash_table_in);
    }
}

/*===========================================================================

  FUNCTION:  util_hash_table_reinit_bucket

===========================================================================*/
/*!
    @brief
    re initialize the bucket array

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
void util_hash_table_reinit_bucket
(
    utilHashTableIn hash_table,
    unsigned int    bucket_size
)
{
    unsigned int new_bucket_size;
    hashArrOfBuckets new_array_of_buckets = NULL;

    if (hash_table && hash_table->hash_arr_of_buckets && (bucket_size >= hash_table->bucket_size))
    {
        new_bucket_size = hash_table->bucket_size;
        while (new_bucket_size < bucket_size)
        {
            new_bucket_size = new_bucket_size * UTIL_HASH_TABLE_EXP_INC_SZ;
        }

        new_array_of_buckets = (hashArrOfBuckets) util_memory_alloc(
                            sizeof(util_list_info_type *) * new_bucket_size);

        if (new_array_of_buckets)
        {
            memcpy(new_array_of_buckets,
                   hash_table->hash_arr_of_buckets,
                   (hash_table->bucket_size * sizeof(util_list_info_type *)));
            util_memory_free(&hash_table->hash_arr_of_buckets);
            hash_table->hash_arr_of_buckets = new_array_of_buckets;
        }
        else
        {
            UTIL_LOG_MSG("Unable to allocate memory for new array of buckets");
        }
    }
}

/*===========================================================================

  FUNCTION:  util_hash_add_evaluator

===========================================================================*/
/*!
    @brief
    Compare key with a list element

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
int util_hash_add_evaluator
(
    util_list_node_data_type *to_be_added_data,
    util_list_node_data_type *to_be_evaluated_data
)
{
    return FALSE;
}

/*===========================================================================

  FUNCTION:  util_hash_delete_evaluator

===========================================================================*/
/*!
    @brief
    Compare key with a list element

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
void util_hash_delete_evaluator
(
    util_list_node_data_type *to_be_deleted_data
)
{
    return;
}

/*===========================================================================

  FUNCTION:  util_hash_table_compare_list_element_key

===========================================================================*/
/*!
    @brief
    Compare key with a list element

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
int util_hash_table_compare_list_element_key
(
    const util_list_node_data_type *keyValue,
    void *key
)
{
    int ret = FALSE;
    int hash_key = (int)key;
    if (keyValue)
    {
        if(hash_key == *(int*)keyValue->user_data)
//        if(hash_key == (int)((utilHashKeyValue)keyValue)->key)
        {
            ret = TRUE;
        }
    }

    return ret;
}

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
    utilHashTable     hash_table,
    utilHashKeyValue  keyValue
)
{
    unsigned int bucket;
    utilHashTableIn hash_table_in = (utilHashTableIn)hash_table;
    bucket = NIL;

    UTIL_LOG_MSG("entry");
    if (hash_table_in && hash_table_in->hash_func && hash_table_in->key_equal_func && keyValue)
    {
        bucket = hash_table_in->hash_func(keyValue->key);
        UTIL_LOG_MSG("insert key %d to bucket %d", keyValue->key, bucket);

        if (bucket >= hash_table_in->bucket_size)
        {
            util_hash_table_reinit_bucket(hash_table_in, bucket);
        }
        else if (!(*hash_table_in->hash_arr_of_buckets)[bucket])
        {
            (*hash_table_in->hash_arr_of_buckets)[bucket] =
                        util_list_create(NULL,
                                         util_hash_add_evaluator,
                                         util_hash_delete_evaluator,
                                         UTIL_LIST_BIT_FIELD_CREATED_ON_HEAP |
                                         UTIL_LIST_BIT_FIELD_AUTO_LOCK);
        }

        UTIL_LOG_MSG("hash_table_in %x hash_table_in->hash_arr_of_buckets %x %d", hash_table_in, hash_table_in->hash_arr_of_buckets, bucket);
        if (!util_list_find_data_in_list_with_param((*hash_table_in->hash_arr_of_buckets)[bucket],
                                                    util_hash_table_compare_list_element_key,
                                                    keyValue->key))
        {
            util_list_enque((*hash_table_in->hash_arr_of_buckets)[bucket],
                                     keyValue, UTIL_LIST_BIT_FIELD_NONE);
        }
    }

    return 0;
}

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
)
{
    utilHashTableIn hash_table_in = (utilHashTableIn)hash_table;
    int bucket = 0;
    int ret = -1;

    if (hash_table_in && hash_table_in->hash_func && hash_table_in->key_equal_func && keyValue)
    {
        bucket = hash_table_in->hash_func(keyValue->key);

        if (bucket >= hash_table_in->bucket_size)
        {
            return -1;
        }
        else if (!(*hash_table_in->hash_arr_of_buckets)[bucket])
        {
            return -1;
        }

        if (!util_list_find_data_in_list_with_param((*hash_table_in->hash_arr_of_buckets)[bucket],
                                                     util_hash_table_compare_list_element_key,
                                                     keyValue->key))
        {
            util_list_delete((*hash_table_in->hash_arr_of_buckets)[bucket],
                                     keyValue, NULL);
            ret = 0;
        }
    }

    return ret;
}

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
)
{
    void *ret = NULL;
    int bucket = 0;
    util_list_node_data_type *list_data = NULL;
    utilHashKeyValue hash_entry = NULL;
    utilHashTableIn hash_table_in = (utilHashTableIn)hash_table;

    UTIL_LOG_MSG("entry");

    do {
        if (hash_table_in && hash_table_in->hash_func && hash_table_in->key_equal_func)
        {
            bucket = hash_table_in->hash_func(key);

            if (bucket >= hash_table_in->bucket_size)
            {
                break;
            }
            else if (!(*(hash_table_in->hash_arr_of_buckets))[bucket])
            {
                break;
            }

            list_data = util_list_find_data_in_list_with_param((*(hash_table_in->hash_arr_of_buckets))[bucket],
                                                               util_hash_table_compare_list_element_key,
                                                               key);
            if ( list_data )
            {
                hash_entry = (utilHashKeyValue)list_data->user_data;
                if ( hash_entry )
                {
                    ret = hash_entry->data;
                }
            }
        }
    } while(0);

    return ret;
}

