//**************************************************************************************************
// Copyright (c) 2017 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//**************************************************************************************************

#include "msmgbm_mapper.h"

namespace msm_gbm {

extern "C" {

static msmgbm_mapper *msmgbm_mapper_ = NULL;

/**
 * C wrapper function to create a cpp object of msmgbm mapper class
 * @input param: None
 * @return   0 : success
 *           1 : failure
 */
bool msmgbm_mapper_instnce(void) {
    if (msmgbm_mapper_ == NULL) {
        msmgbm_mapper_ = new msmgbm_mapper();
        if (!msmgbm_mapper_->init()) {
          return 1;
       }
    }
    return 0;
}

/**
 * C wrapper function to register to the hash map a gbm_buf_info indexed by an ion_fd
 * @input param: ion_fd , gbm_buf_info object handle
 * @return     : none
 */
void register_to_hashmap(int fd, struct gbm_buf_info * gbm_buf,
                                       struct msmgbm_private_info * gbo_private_info) {
    msmgbm_mapper_->register_to_map(fd, gbm_buf, gbo_private_info);
}

/**
 * C wrapper function to search hash map using ion_fd and retrieve the gbm_buf_info
 * @input param: ion_fd , gbm_buf_info object handle
 * @return     : GBM error status
 *
 */
int  search_hashmap(int fd, struct gbm_buf_info *buf_info,
                                struct msmgbm_private_info * gbo_private_info) {
      if(msmgbm_mapper_->search_map(fd,buf_info, gbo_private_info))
        return GBM_ERROR_NONE;
      else
        return GBM_ERROR_BAD_HANDLE;
}

/**
 * C wrapper function to update hash map using ion_fd and retrieve the gbm_buf_info
 * @input param: ion_fd , gbm_buf_info object handle
 * @return     : GBM error status
 *
 */
int  update_hashmap(int fd, struct gbm_buf_info *buf_info,
                                struct msmgbm_private_info * gbo_private_info) {
      if(msmgbm_mapper_->update_map(fd,buf_info, gbo_private_info))
        return GBM_ERROR_NONE;
      else
        return GBM_ERROR_BAD_HANDLE;
}

/**
 * C wrapper function to dump hash map
 * @input param: void
 * @return     : none
 *
 */
void  dump_hashmap(void) {
      msmgbm_mapper_->map_dump();
}



/**
 * C wrapper function to increment the reference count for the valid map entry
 * @input param: ion_fd
 * @return     : none
 *
 */
void  incr_refcnt(int fd) {
     msmgbm_mapper_->add_map_entry(fd);
}

/**
 * C wrapper function to decrement the reference count for the valid map entry
 * @input param: ion_fd
 * @return     : 1 for delete map entry /0 for decremented ref count
 *
 */
int  decr_refcnt(int fd){
    if (msmgbm_mapper_)
        return msmgbm_mapper_->del_map_entry(fd);
    else {
        LOG(LOG_INFO,"gbm mapper had been de-instantiated\n");
        return 1;
    }
}

/**
 * C wrapper function to delete msmsgbm mapper object
 * @input param: None
 * @return     : None
 *
 */
void msmgbm_mapper_deinstnce(void) {
    if(msmgbm_mapper_) {
       delete msmgbm_mapper_;
       msmgbm_mapper_ = NULL;
    }
}

}

/**
 * Constructor
 */
msmgbm_mapper::msmgbm_mapper() {
    LOG(LOG_INFO,"gbm mapper instantiated\n");
}

/**
 * Destructor
 */
msmgbm_mapper::~msmgbm_mapper() {
    LOG(LOG_INFO,"gbm mapper de-instantiated\n");
}

bool msmgbm_mapper::init() {
  gbm_buf_map_.clear();
  return true;
}

/**
 * Function to register to the hash map a gbm_buf_info indexed by an ion_fd
 * @input param: ion_fd , gbm_buf_info object handle
 * @return    : none
 */
void msmgbm_mapper::register_to_map(int fd,      struct gbm_buf_info * gbm_buf,
                                                 struct msmgbm_private_info *gbo_private_info) {
  auto buffer = std::make_shared<msmgbm_buffer>(fd,gbm_buf->metadata_fd,
                                  gbm_buf->width,gbm_buf->height,gbm_buf->format,
                                  gbo_private_info->cpuaddr, gbo_private_info->mt_cpuaddr);
  gbm_buf_map_.emplace(std::make_pair(fd, buffer));
}

/**
 * Function to search hash map using ion_fd and retrieve the gbm_buf_info
 * @input param: ion_fd , gbm_buf_info object handle, msmgbm_private_info object handle
 * @return    : 1 or O
 *
 */
int msmgbm_mapper::search_map(int fd, struct gbm_buf_info *buf_info,
                                         struct msmgbm_private_info *gbo_private_info){
  auto it = gbm_buf_map_.find(fd);
  if (it != gbm_buf_map_.end()) {
    buf_info->fd=it->second->ion_fd;
    buf_info->metadata_fd=it->second->ion_metadata_fd;
    buf_info->width=it->second->width;
    buf_info->height=it->second->height;
    buf_info->format=it->second->format;
    gbo_private_info->cpuaddr = it->second->cpuaddr;
    gbo_private_info->mt_cpuaddr = it->second->mt_cpuaddr;
    return 1;
  } else {
    return 0;
  }
}

/**
 * Function to update hash map entry using ion_fd as hash key
 * @input param: ion_fd , gbm_buf_info object handle
 * @return    : 1 or O
 *
 */
int msmgbm_mapper::update_map(int fd, struct gbm_buf_info *buf_info,
                                         struct msmgbm_private_info *gbo_private_info){
  auto it = gbm_buf_map_.find(fd);
  if (it != gbm_buf_map_.end()) {
    it->second->ion_fd=buf_info->fd;
    it->second->ion_metadata_fd=buf_info->metadata_fd;
    it->second->width=buf_info->width;
    it->second->height=buf_info->height;
    it->second->format=buf_info->format;
    it->second->cpuaddr=gbo_private_info->cpuaddr;
    it->second->mt_cpuaddr=gbo_private_info->mt_cpuaddr;
    return 1;
  } else {
    return 0;
  }
}

/**
 * Function to dump the hash map table
 * @input void
 * @return void
 *
 */
void msmgbm_mapper::map_dump(void) {
  int count=1;
  printf("Hash Table dump\n");
  printf("***********************************************\n");
  for (auto it : gbm_buf_map_) {
    auto buf = it.second;
    printf("ENTRY %d\n",count++);
    printf("---------------------------------\n");
    printf("----ion_fd          = %d\n",buf->ion_fd);
    printf("----ion_metadata_fd = %d\n",buf->ion_metadata_fd);
    printf("----width           = %u\n",buf->width);
    printf("----height          = %u\n",buf->height);
    printf("----format          = %u\n",buf->format);
    printf("----cpuaddr         = %p\n",buf->cpuaddr);
    printf("----mt_cpuaddr      = %p\n",buf->mt_cpuaddr);
    printf("---------------------------------\n");
  }
  printf("Total no.of entries=%d\n",count-1);
  printf("***********************************************\n");
}


/**
 * Function to increment the reference count for the valid map entry
 * @input param: ion_fd
 * @return     : none
 *
 */
void msmgbm_mapper::add_map_entry(int fd) {
  auto it = gbm_buf_map_.find(fd);
  if (it != gbm_buf_map_.end()){
      it->second->IncRef();
  }
}

/**
 * Function to decrement the reference count for the valid map entry
 * @input param: ion_fd
 * @return    : none
 *
 */
int msmgbm_mapper::del_map_entry(int fd) {
    auto it = gbm_buf_map_.find(fd);
    if (it!= gbm_buf_map_.end())
       if(it->second->DecRef()){
           gbm_buf_map_.erase(fd);
           return 1;
       }else
           return 0;
    return 1;
}
}  // namespace msm_gbm
