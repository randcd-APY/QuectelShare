/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include<sys/mman.h>

#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/ioctl.h>
#include<pthread.h>

#include "ion_memory_pool.h"
/*
 ============================================================================
 * ION memory implementation
 ============================================================================
 */
#define container_of(ptr, type, member) \
    (type *)((char *)(ptr) - (char *) &((type *)0)->member)

#define NR_BLOCKS_CONST 16
static void _ion_open(void);
static void _ion_close(void);

static int _ion_alloc(const int fd, struct ion_allocation_data *);
static int _ion_dealloc(const int fd, ion_user_handle_t);
static int _ion_mmap(const int fd,
                    const ion_user_handle_t handle,
                    const gsize len,
                    off_t offset,
                    void **ptr,
                    int *fd_map);

static void _check_ion_mem_info_tbl_entry(void);
static void *_ion_alloc_mem(const gsize gsize, ion_mem_info_t **record);
static gint  _ion_free_mem(const gsize gsize, void *buf);
static void  _ion_mem_info_tbl_init(void);
static const int _get_ion_mem_info_tbl_entry(void);



/*
 ============================================================================
 * ION memory implementation
 ============================================================================
*/
static pthread_mutex_t _ion_lock = PTHREAD_MUTEX_INITIALIZER;
static int _fd_ion = -1;

void _ion_open(void) {

    _ion_mem_info_tbl_init();

    if(_fd_ion>0) {
        return;
    }

    _fd_ion = open("/dev/ion", O_RDWR);

    if(_fd_ion<0) {
        GST_ERROR("%s: open /dev/ion failed with error %s\n", __func__, strerror(errno));
    }
}


void _ion_close(void) {
    int stat=0;

    _check_ion_mem_info_tbl_entry();

    if(_fd_ion>0) {
        stat = close(_fd_ion);
        _fd_ion = -1;
    }

    if(stat<0) {
        GST_WARNING("%s: close /dev/ion failed with error %s\n", __func__, strerror(errno));
    }
}


int _ion_alloc(const int fd, struct ion_allocation_data *data) {

    int ret;

    /* send the memory alloc request to the driver */
    if((ret = ioctl(fd, ION_IOC_ALLOC, data))<0) {
        GST_ERROR("%s: ioctl ION_IOC_ALLOC request failed with error %s\n", __func__, strerror(errno));
        return ret;
    }

    return ret;
}


int _ion_dealloc(const int fd,
                ion_user_handle_t handle) {

    struct ion_handle_data data = {
        .handle = handle,
    };

    int ret;

    if((ret=ioctl(fd, ION_IOC_FREE, &data))<0) {
        GST_ERROR("ioctl ION_IOC_FREE failed with error %s\n", strerror(errno));
    }

    return ret;
}

int _ion_mmap(const int fd,
             const ion_user_handle_t handle,
             const gsize len,
             off_t offset,
             void **ptr,
             int *fd_map) {

    int ret;
    void *tmp_p;
    struct ion_fd_data fd_data = {
        .handle= handle,
    };

    if((ret=ioctl(fd, ION_IOC_MAP, &fd_data))<0) {
        GST_ERROR("ioctl ION_IOC_MAP failed with error %s\n", strerror(errno));
        return ret;
    }

    tmp_p = mmap(NULL,
                 len,
                 PROT_READ | PROT_WRITE,
                 MAP_SHARED,
                 fd_data.fd,
                 offset);

    if(tmp_p==MAP_FAILED) {
        GST_ERROR("%s: mmap failed with error %s\n", __func__, strerror(errno));
        return -errno;
    }

    *fd_map = fd_data.fd;
    *ptr = tmp_p;

    return ret;
}


/**
 * ION alloc and free memory implementation
 */

static pthread_mutex_t ion_info_tbl_lock;

#define ION_MEM_INFO_TBL_SIZE 8 // per ION open
static ion_mem_info_t ion_mem_info_tbl[ION_MEM_INFO_TBL_SIZE];

void _ion_mem_info_tbl_init(void) {
    int i;

    pthread_mutex_init(&ion_info_tbl_lock, NULL);

    for(i=0; i<ION_MEM_INFO_TBL_SIZE; i++) {
        ion_mem_info_tbl[i].used = 0;
        ion_mem_info_tbl[i].size = 0;
    }
}

const int _get_ion_mem_info_tbl_entry(void) {
    int i;


    for(i=0; i<ION_MEM_INFO_TBL_SIZE; i++) {
        pthread_mutex_lock(&ion_info_tbl_lock);
        if(ion_mem_info_tbl[i].used==0) {
            ion_mem_info_tbl[i].used = 1;
            pthread_mutex_unlock(&ion_info_tbl_lock);
            return i;
        }
        pthread_mutex_unlock(&ion_info_tbl_lock);
    }

    GST_WARNING("%s: !!!!!! ION memory info table entry full error\n", __func__);
    return -1;
}

void _check_ion_mem_info_tbl_entry(void) {

    int i;
    int ret;

    for(i=0; i<ION_MEM_INFO_TBL_SIZE; i++) {
        if(ion_mem_info_tbl[i].used==1 && ion_mem_info_tbl[i].size>0) {
            GST_DEBUG("leaking memory error and clean up leaked memory: %p, 0x%lx\n",
                                                                ion_mem_info_tbl[i].buf,
                                                                ion_mem_info_tbl[i].size);

            /**
             * clean up the leaked memory
             */

            /* unmap leaked memory */
            ret = munmap(ion_mem_info_tbl[i].buf,
                         ion_mem_info_tbl[i].size);

            ion_mem_info_tbl[i].size = 0;
            ion_mem_info_tbl[i].used = 0;

            if(ret<0) {
                 GST_ERROR("%s: !!!! error munmap failed\n", __func__);
            }

            /* close leaked memory file desc */
            if(ion_mem_info_tbl[i].fd_map>0) {
                ret = close(ion_mem_info_tbl[i].fd_map);
                ion_mem_info_tbl[i].fd_map = -1;

                if(ret<0) {
                     GST_ERROR("%s: !!!! error close failed\n", __func__);
                }
            }

            /* dealloc handle if not released  */
            if(ion_mem_info_tbl[i].handle>0) {
                _ion_dealloc(_fd_ion, ion_mem_info_tbl[i].handle);
                ion_mem_info_tbl[i].handle = -1;
            }
        }
    }

}

void *_ion_alloc_mem(const gsize size, ion_mem_info_t **record) {
    /**
     * 4k alignment
     */
    struct ion_allocation_data alloc_data = {
        .len = size,
        .align = 4096,
        .heap_id_mask = ION_HEAP(ION_IOMMU_HEAP_ID),
        //.heap_id_mask = 0x1<<ION_IOMMU_HEAP_ID,
        //.heap_id_mask = ION_HEAP_SYSTEM_MASK,
        .flags = 0,
    };

    int fd_map;
    void *buf;
    int ret;
    const int idx = _get_ion_mem_info_tbl_entry();

    if(idx<0) {
        GST_ERROR("%s: Warning out of memory info table entry error\n", __func__ );
        return NULL;
    }


    if((ret=_ion_alloc(_fd_ion, &alloc_data))<0) {
        GST_ERROR("%s: Warning ION Alloc error\n", __func__ );
        return NULL;
    }

    ret = _ion_mmap(_fd_ion,
                    alloc_data.handle,
                    alloc_data.len,
                    0,  /* offset */
                    &buf,
                    &fd_map);

    if(ret<0) {
        GST_ERROR("%s: Warning ION mmap failed error\n", __func__);
        return NULL;
    }


    ion_mem_info_tbl[idx].handle = alloc_data.handle;
    ion_mem_info_tbl[idx].fd_map = fd_map;
    ion_mem_info_tbl[idx].size   = alloc_data.len;
    ion_mem_info_tbl[idx].buf    = buf;
    ion_mem_info_tbl[idx].end    = (guint8 *) buf + alloc_data.len;

    *record = &ion_mem_info_tbl[idx];

    return buf;
}

gint _ion_free_mem(const gsize size, void *buf) {
    int i;
    int ret;

    /**
     * find the ION memory meta data:
     *     - user id handle
     *     - memory address/size
     */

    for(i=0; i<ION_MEM_INFO_TBL_SIZE; i++) {
        if(ion_mem_info_tbl[i].buf==buf && ion_mem_info_tbl[i].size==size) {

            /* free memory */
           ret = munmap(buf, size);
           ion_mem_info_tbl[i].size = 0;
           ion_mem_info_tbl[i].buf = MAP_FAILED;

           if(ret<0) {
               GST_WARNING("!!!! error munmap failed\n");
           }

           /* if map fd not freed */
           if(ion_mem_info_tbl[i].fd_map>0) {
                ret = close(ion_mem_info_tbl[i].fd_map);
                ion_mem_info_tbl[i].fd_map=-1;

                if(ret<0) {
                    GST_WARNING("!!!! error close map fd, %d, failed\n", ion_mem_info_tbl[i].fd_map);
                }
           }


           /* handle if not freed */
           if(ion_mem_info_tbl[i].handle>0) {
               _ion_dealloc(_fd_ion, ion_mem_info_tbl[i].handle);
               ion_mem_info_tbl[i].handle = -1;
           }

           ion_mem_info_tbl[i].used = 0;

           return 0;
        }
    }

    GST_WARNING("%s Warning ION memory free error (%p, 0x%lx)\n", __func__, buf, size);
    return -1;
}



/*
============================================================================
*  Pool Memory Implementation
============================================================================
*/

/**
 *  Pool Memory APIs
 */
void ion_pool_create(IonMemoryPool *this, gsize nr_blocks, gsize blk_size) {
  _ion_open();

  pthread_mutex_lock(&this->lock);

  /* buf should be initialized to zero in the GstIonAllocator */
  if(this->buf) {
      pthread_mutex_unlock(&this->lock);
      return;
  }

  this->total_blocks = nr_blocks + NR_BLOCKS_CONST;
  GST_DEBUG("total blocks = %d\n", this->total_blocks);

  /* buffer/block size */
  this->block_size = blk_size;

  /* total memory size */
  this->size = blk_size * this->total_blocks;
  this->buf = (guint8 *) _ion_alloc_mem(this->size, &(this->ion_mem_info));
  this->end = this->buf + this->size;
  pthread_mutex_unlock(&this->lock);

  this->nr_blocks = 0;

  this->array = (gpointer) malloc(this->total_blocks * sizeof(gpointer));
  this->pos = this->total_blocks;

  /**
   * initialize array with the block addresses
   */
  {
    int i;
    guint8 *addr;
    for (i=0; i<this->total_blocks; i++) {
        addr = this->buf + (blk_size * i);
        ion_pool_dealloc(this, (gpointer) addr);
    }
  }
}


void ion_pool_destroy(IonMemoryPool *this) {

  if(this->buf==NULL) {
    return;
  }

  _ion_free_mem(this->size, this->buf);
  this->buf = NULL;

  free(this->array);

  _ion_close();
}


gpointer ion_pool_alloc(IonMemoryPool *this) {
  gpointer addr;

  if (this->pos==this->total_blocks) {
    return NULL;
  }

  addr = this->array[this->pos];

  pthread_mutex_lock(&this->lock);
  this->pos += 1;
  this->nr_blocks -= 1;
  pthread_mutex_unlock(&this->lock);

  return addr;
}

const gint ion_pool_dealloc(IonMemoryPool *this, gpointer addr) {
  /* check memory range if released address in the pool */
  if((guint64)addr < (guint64)this->buf ||
    (guint64)addr > (guint64) (this->end)) {
    GST_WARNING("called a wrong dealloc %p\n", addr);

    /* memory is NOT in the pool */
    return -1;
  }

  pthread_mutex_lock(&this->lock);
  this->pos -= 1;
  this->array[this->pos] = addr;

  //TOOD remove nr_blocks
  this->nr_blocks += 1;
  pthread_mutex_unlock(&this->lock);

}


#if 1
void ion_pool_dump(IonMemoryPool *this) {
    gint i;
    guint8 *p;

    pthread_mutex_lock(&this->lock);

    for(i=0; i<this->nr_blocks; i++) {
        GST_DEBUG("debug ---> %p\n", this->array[i]);
    }
    pthread_mutex_unlock(&this->lock);
}
#endif

