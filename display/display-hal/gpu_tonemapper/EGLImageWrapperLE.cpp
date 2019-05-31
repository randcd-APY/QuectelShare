/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Copyright 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "EGLImageWrapperLE.h"

EGLImageWrapperLE::EGLImageWrapperLE()
//-----------------------------------------------------------------------------
{
}

//-----------------------------------------------------------------------------
EGLImageWrapperLE::~EGLImageWrapperLE()
//-----------------------------------------------------------------------------
{
}

EGLImageBufferLE *EGLImageWrapperLE::wrap(void *buf_info, void *userdata, void *userdata2)
//-----------------------------------------------------------------------------
{
  struct gbm_buf_info *src = reinterpret_cast<struct gbm_buf_info *>(buf_info);
  EGLImageBufferLE *result = 0;

  std::map<int, EGLImageBufferLE *>::iterator it = eglImageBufferMap.find(src->fd);

  if (it == eglImageBufferMap.end()) {
    result = new EGLImageBufferLE(src, userdata, userdata2);
    eglImageBufferMap[src->fd] = result;
  } else {
    result = it->second;
  }

  return result;
}

//-----------------------------------------------------------------------------
void EGLImageWrapperLE::destroy()
//-----------------------------------------------------------------------------
{
  std::map<int, EGLImageBufferLE *>::iterator it = eglImageBufferMap.begin();
  for (; it != eglImageBufferMap.end(); it++) {
    delete it->second;
  }
  eglImageBufferMap.clear();
}
