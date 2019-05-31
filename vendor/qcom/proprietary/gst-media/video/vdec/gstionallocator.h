/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

#ifndef GST_ION_ALLOCATOR_H_
#define GST_ION_ALLOCATOR_H_


#include<gst/gst.h>
#include "ion_memory_pool.h"

GstAllocator *gst_ion_memory_create(const gsize nr_blocks, const gsize sz);
void gst_ion_memory_destroy(void);

GstBufferPool *gst_ion_buffer_pool_create(const gsize nr_blks,
                                          const gsize sz,
                                          const gint min,
                                          const gint max);

void gst_ion_buffer_pool_destroy(GstBufferPool *);

void gst_ion_buffer_get_memory_info(GstBuffer *,
                                    gint *fd,
                                    gsize *offset,
                                    gpointer *base);

GstAllocator * gst_ion_memory_allocator_get(void);

gint     gst_ion_buffer_get_fd(GstBuffer *);
gsize    gst_ion_buffer_get_offset(GstBuffer *);
gpointer gst_ion_buffer_get_base(GstBuffer *);

gboolean gst_is_ion_memory(GstMemory *);
gboolean gst_is_ion_buffer(GstBuffer *);

#endif /* GST_ION_ALLOCATOR_H_ */
