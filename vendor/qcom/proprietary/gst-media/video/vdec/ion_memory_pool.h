/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#ifndef ION_MEMORY_POOL_H
#define ION_MEMORY_POOL_H
#include<linux/msm_ion.h>
#include<gst/gst.h>
/*
 ==============================================
 * ION Pool Memory Interface
 ==============================================
 */
typedef struct _IonMemInfo ion_mem_info_t;

struct _IonMemInfo {
    gint fd_map;
    ion_user_handle_t handle;
    gpointer buf;
    gpointer end;
    gsize size;
    gint used;
};


typedef struct _IonMemoryPool IonMemoryPool;


struct _IonMemoryPool {
    gint total_blocks;

    guint8 *buf;
    guint8 *end;
    gsize   size;

    gint nr_blocks;
    gsize block_size;

    gpointer *array;
    int pos;

    pthread_mutex_t lock;

    ion_mem_info_t *ion_mem_info;
};


/* ION Pool Memory APIs */
void ion_pool_create(IonMemoryPool *pool, gsize nr_blks, gsize blk_size);
void ion_pool_destroy(IonMemoryPool *pool);

gpointer ion_pool_alloc(IonMemoryPool *pool);
const gint ion_pool_dealloc(IonMemoryPool *pool, gpointer addr);

void ion_pool_dump(IonMemoryPool *pool);

#endif /* ION_MEMORY_POOL_H */
