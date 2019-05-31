/* Copyright (c) 2015, 2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <sys/mman.h>
#include <cstdlib>
#include "camera_memory.h"
#include "camera_log.h"

namespace camera
{

/* create a new memory object and map/allocate the buffer */
CameraMemory::CameraMemory(int fd, uint32_t size)
{
    int rc;
    valid_ = false;

    mem_ = (camera_memory_t*)malloc(sizeof(camera_memory_t));
    if (mem_ == NULL) {
        CAM_ERR("malloc() failed");
        return;
    }
    if (fd < 0) {
        /* allocate new memory */
        struct ion_allocation_data alloc;
        struct ion_fd_data ion_info_fd;

        main_ion_fd_ = open("/dev/ion", O_RDONLY);
        if (main_ion_fd_ < 0) {
            CAM_ERR("Ion dev open failed: %s\n", strerror(errno));
            return;
        }

        memset(&alloc, 0, sizeof(alloc));
        memset(&handle_data_, 0, sizeof(struct ion_handle_data));
        alloc.len = size;
        /* to make it page size aligned */
        alloc.len = (alloc.len + 4095) & (~4095);
        alloc.align = 4096;

        alloc.heap_id_mask = 0x1 << ION_IOMMU_HEAP_ID;;
        rc = ioctl(main_ion_fd_, ION_IOC_ALLOC, &alloc);
        if (rc < 0) {
            CAM_ERR("ION allocation failed: %s\n", strerror(errno));
            return;
        }

        memset(&ion_info_fd, 0, sizeof(ion_info_fd));
        ion_info_fd.handle = alloc.handle;
        rc = ioctl(main_ion_fd_, ION_IOC_SHARE, &ion_info_fd);
        if (rc < 0) {
            CAM_ERR("ION map failed %s\n", strerror(errno));
            return;
        }
        mem_->data = mmap(NULL, alloc.len, PROT_READ | PROT_WRITE, MAP_SHARED,
                          ion_info_fd.fd, 0);

        if (mem_->data == MAP_FAILED) {
               CAM_ERR("malloc() failed");
			   return;
        }
        mem_->size = alloc.len;
        frame.fd = ion_info_fd.fd;
        handle_data_.handle = ion_info_fd.handle;
        type_ = MEM_ALLOCATED;
    } else {
        /* map the memory specified by fd using mmap */
        main_ion_fd_ = 0;
        mem_->data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED,
                          fd, 0);
        if (mem_->data == MAP_FAILED) {
            CAM_ERR("mmap() failed\n");
			return;
        }
        type_ = MEM_MAPPED;
        mem_->size = size;

        /* populate buffer metadata */
        metadata_.buffer_type = android::kMetadataBufferTypeCameraSource;
        native_handle_t *nh =  (native_handle_t *)nh_mem_;
        nh->numFds = NH_NUM_FDS;
        nh->numInts = NH_NUM_INTS;
        nh->data[0] = fd;
        nh->data[1] = 0; /* offset */
        nh->data[2] = size;
        metadata_.meta_handle = nh;
        frame.metadata = &metadata_;
        frame.fd = fd;
    }
    valid_ = true;
    mem_->handle = this;
    mem_->release = releaseMemory;

    /* initialize frame */
    frame.data = (uint8_t *)mem_->data;
    frame.size = mem_->size;
}

/* unmap/free the buffer and destroy the memory object */
CameraMemory::~CameraMemory()
{
    switch (type_) {
      case MEM_ALLOCATED:
          if (main_ion_fd_ && mem_->data != NULL) {
            munmap(mem_->data, mem_->size);
            close(frame.fd);
            ioctl(main_ion_fd_, ION_IOC_FREE, &handle_data_);
            close(main_ion_fd_);
            mem_->data = NULL;
            frame.fd = -1;
          }
          break;
      case MEM_MAPPED:
          if (munmap(mem_->data, mem_->size) < 0) {
              CAM_ERR("munmap() failed");
          }
          break;
      default:
          CAM_ERR("failed, invalid type, %d", type_);
          break;
    }
    free(mem_);
}

/* function to serve the request_memory_callback from camera HAL */
camera_memory_t* CameraMemory::requestMemory(int fd, size_t buf_size,
                                      unsigned int num_bufs,
                                      void* user)
{
    CAM_DBG("");
    CameraMemory* me = new CameraMemory(fd, buf_size * num_bufs);
    if (!me || (me && me->valid_ == false)) {
        CAM_ERR("failed, obj=%p", me);
        return NULL;
    }
    return me->mem_;
}

/* function to serve the release_memory_callback from camera HAL */
void CameraMemory::releaseMemory(struct camera_memory* mem)
{
    CAM_DBG("");
    CameraMemory* me = (CameraMemory*) mem->handle;
    if (!me) {
        CAM_ERR("failed");
        return;
    }
    delete me;
}

} /* namespace camera */
