/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */


#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include"gstionallocator.h"

typedef struct _GstIonMemory GstIonMemory;

struct _GstIonMemory {
	GstMemory mem;

	gint fd_map;
	gpointer base;
	gpointer data;
    //GDestroyNotify notify;
};

/**
 * Buffer Pool Definition
 */
static IonMemoryPool __memory_pool = {
    .total_blocks = 0,

    .buf        = NULL,
    .end        = NULL,
    .size       = 0,

    .nr_blocks  = 0,
    .block_size = 0,

    .lock         = PTHREAD_MUTEX_INITIALIZER,
    .ion_mem_info = NULL,
};

// TODO: put variable into allocator object
static IonMemoryPool *__pool = &__memory_pool;

static GstAllocator *__ion_allocator=NULL;


/**
 * namespace: GstIon
 * type     : Allocator
 */
typedef struct _GstIonAllocator GstIonAllocator;
typedef struct _GstIonAllocatorClass GstIonAllocatorClass;

struct _GstIonAllocator {
	GstAllocator parent;
};

struct _GstIonAllocatorClass {
	GstAllocatorClass parent;
};

GType gst_ion_allocator_get_type(void);

G_DEFINE_TYPE(GstIonAllocator, gst_ion_allocator, GST_TYPE_ALLOCATOR);


/**
 * help function
 */
static GstBufferPool *gst_ion_buffer_pool_init(const gsize size, const gint, const gint);

/**
 * Ion Memory Allocator Class Methods
 */
static GstMemory *_ion_alloc(GstAllocator *allocator,
				             gsize size,
				             GstAllocationParams *params) {
    /**
     * asked size is the arg size
     */
	GstIonMemory *mem;

	gsize maxsize = size + params->prefix + params->padding;

	mem = g_slice_new(GstIonMemory);

    /* initialize base/parent, GstMemory, object */
	gst_memory_init(GST_MEMORY_CAST(mem),
			        params->flags,
			        allocator,
			        NULL,    /* memory parent */
			        maxsize,
			        params->align,
			        params->prefix,
			        size);

    /**
     * initialize GstIonMemory: each object has copy of fd_map and base
     */
	mem->fd_map = __pool->ion_mem_info->fd_map;
	mem->base   = __pool->ion_mem_info->buf;
	mem->data   = ion_pool_alloc(__pool);

	return GST_MEMORY_CAST(mem);
}

static void _ion_free(GstAllocator *allocator, GstMemory *mem) {
	GstIonMemory *memory = (GstIonMemory *) mem;

	if(memory->data) {
		(void)ion_pool_dealloc(__pool, memory->data);
	}

	/* free GstIonMemory structure */
	g_slice_free(GstIonMemory, memory);
}



/**
 * methods for GstIonMemoryAllocator structure
 */
static gpointer _ion_map(GstMemory *mem,
		                 gsize size,
		                 GstMapFlags flags) {

	return ((GstIonMemory *) mem)->data;
}


static gboolean _ion_unmap(GstMemory *mem) {

	return TRUE;
}

static GstMemory *_ion_share(GstMemory *mem,
		                     gssize offset,
		                     gsize size) {
    GstIonMemory *src = (GstIonMemory *) mem;
    GstIonMemory *dest;
    GstMemory *parent;

    printf("debug ---> %s:%p\n", __func__, mem);

    if((parent=mem->parent)==NULL) {
        parent = mem;
    }

	dest = g_slice_new(GstIonMemory);

    if(size==-1) {
        size = mem->size - offset;
    }

	gst_memory_init(GST_MEMORY_CAST(dest),
			        GST_MINI_OBJECT_FLAGS(parent) | GST_MINI_OBJECT_FLAG_LOCK_READONLY,
			        NULL,
			        parent,
			        mem->maxsize,
			        mem->align,
			        mem->offset + offset,
			        size);

    /**
     * initialize GstIonMemory: each object has copy of fd_map and base
     */
	dest->fd_map = src->fd_map;
	dest->base   = src->base;
	dest->data   = src->data;

	return (GstMemory *) dest;
}

static GstMemory *_ion_copy(GstMemory* mem,
		                    gssize offset,
		                    gssize size) {
  GstIonMemory *memory = (GstIonMemory *) mem;
  GstIonMemory *copy = NULL;
  GstAllocationParams params;

  gst_allocation_params_init(&params);

  printf("debug ---> %s:%p:%p\n", __func__, memory, memory->data);

  /* TODO size==-1 ? */
  if(size==-1) {
      size = (memory->mem.size>offset)? memory->mem.size - offset : 0;
  }

  copy = (GstIonMemory *)_ion_alloc(__ion_allocator, size, &params);

  {
      const guint8 *src_p = (guint8 *) memory->data  + memory->mem.offset + offset;
      memcpy(copy->data, (void *) src_p, size);
  }

  return (GstMemory *) copy;
}


void gst_ion_allocator_class_init(GstIonAllocatorClass *klass) {
	GstAllocatorClass *bclass = (GstAllocatorClass *) klass;

	bclass->alloc = _ion_alloc;
	bclass->free  = _ion_free;
}

void gst_ion_allocator_init(GstIonAllocator *self) {
	GstAllocator *alloc = GST_ALLOCATOR_CAST(self);

	alloc->mem_type   = "GstIonMemoryAllocator";
	alloc->mem_map    = (GstMemoryMapFunction)  _ion_map;
	alloc->mem_unmap  = (GstMemoryUnmapFunction)_ion_unmap;
	alloc->mem_copy   = (GstMemoryCopyFunction) _ion_copy;
	alloc->mem_share  = (GstMemoryShareFunction)_ion_share;
}


GstBufferPool *gst_ion_buffer_pool_init(const gsize size, const gint min, const gint max) {
    GstStructure *config_struct;
    GstCaps *caps;
    GstAllocationParams params;
    GstBufferPool *buffer_pool;

    /* create a pool object */
    buffer_pool = gst_buffer_pool_new();

    /* get the config object from the pool */
    config_struct = gst_buffer_pool_get_config(buffer_pool);

    /* create a caps obj */
    caps = gst_caps_new_empty_simple("IonMemory/Pool");

    if(caps==NULL) {
        printf("debug ---> !!!!!!!!!!!!! caps error\n");
        return NULL;
    }

    /**
     * config pool properties
     */

    /* add size, min and max to config structure */
    gst_buffer_pool_config_set_params(config_struct, caps, size, min, max);

    gst_allocation_params_init(&params);

    /* add the ION allocator and allocator params into the config struct */
    gst_buffer_pool_config_set_allocator(config_struct, __ion_allocator, &params);

    /* install the config struct to the pool */
    gst_buffer_pool_set_config(buffer_pool, config_struct);

    return buffer_pool;
}



/**
 * public API
 */
GstAllocator *gst_ion_memory_create(const gsize nr_blocks, 
                                    const gsize size) {
    /**
     * size is buffer block size
     * open ION device (make sure only once)
     */

    if(__ion_allocator) {
        return __ion_allocator;
    }

    ion_pool_create(__pool, nr_blocks, size);
    ion_pool_dump(__pool);


	while(TRUE) {
		if(g_atomic_pointer_get(&__ion_allocator)!=NULL) {
			break;
		}
		__ion_allocator = g_object_new(gst_ion_allocator_get_type(), NULL);

        if(__ion_allocator) {
		    gst_object_ref(__ion_allocator);
		    gst_allocator_register("GstIonAllocator", __ion_allocator);
            break;
        }
    }

    return __ion_allocator;
}

void gst_ion_memory_destroy(void) {
    if(__ion_allocator==NULL) {
        return;
    }

    ion_pool_destroy(__pool);
    gst_object_unref(__ion_allocator);

    __ion_allocator = NULL;
}

GstBufferPool *gst_ion_buffer_pool_create(const gsize nr_blocks,
                                          const gsize size,
                                          const gint min,
                                          const gint max) {
    (void)gst_ion_memory_create(nr_blocks, size);

    return gst_ion_buffer_pool_init(size, min, max);
}


void gst_ion_buffer_pool_destroy(GstBufferPool *pool) {

  //gst_buffer_pool_set_active(pool, FALSE);
  //TODO:
  ion_pool_destroy(__pool);

}

gint gst_ion_buffer_get_fd(GstBuffer *buffer) {
    GstIonMemory *memory = (GstIonMemory *) gst_buffer_get_memory(buffer, 0);
    const gint fd = memory->fd_map;

    gst_memory_unref((GstMemory *) memory);

    return fd;
}

gpointer gst_ion_buffer_get_base(GstBuffer *buffer) {
    GstIonMemory *memory = (GstIonMemory *) gst_buffer_get_memory(buffer, 0);
    const gpointer base = memory->base;

    gst_memory_unref((GstMemory *) memory);

    return base;
}

gsize gst_ion_buffer_get_offset(GstBuffer *buffer) {
	GstIonMemory *memory = (GstIonMemory *) gst_buffer_get_memory(buffer, 0);
	const gsize offset = (gsize) ((guint64) memory->data - (guint64) memory->base);

    gst_memory_unref((GstMemory *) memory);

    return offset;
}

void gst_ion_buffer_get_memory_info(GstBuffer *buffer,
                                    gint *fd,
                                    gsize *offset,
                                    gpointer *base) {

    GstIonMemory *memory = (GstIonMemory *) gst_buffer_get_memory(buffer, 0);

    *fd     = memory->fd_map;
	*offset = (gsize) ((guint64) memory->data - (guint64) memory->base);
    *base   = memory->base;

    gst_memory_unref((GstMemory *) memory);

}

GstAllocator *gst_ion_memory_allocator_get(void) {
    return __ion_allocator;
}

gboolean gst_is_ion_memory(GstMemory *mem) {
	return mem->allocator==__ion_allocator;
}

gboolean gst_is_ion_buffer(GstBuffer *buffer) {
    GstMemory *mem = gst_buffer_get_memory(buffer, 0);
    gboolean ret = (mem->allocator==__ion_allocator);

    gst_memory_unref(mem);
    return ret;
}

// TODO: test trigger CI
