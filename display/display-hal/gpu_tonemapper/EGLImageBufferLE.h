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

#ifndef __TONEMAPPER_EGLIMAGE_BUFFER_LE_H__
#define __TONEMAPPER_EGLIMAGE_BUFFER_LE_H__

#include "engine.h"
#include "EGLImageBufferBase.h"
#include "drm_master.h"
#include <gbm.h>
#include <gbm_priv.h>
#include "glengine.h"

using namespace drm_utils;

class EGLImageBufferLE : public EGLImageBufferBase{

 public:
    EGLImageBufferLE(struct gbm_buf_info *gbo_info, void *userdata, void *userdata2);
    ~EGLImageBufferLE();
    EGLImageKHR create_eglImage(struct gbm_buf_info *gbo_info, void *userdata);
    static EGLImageBufferLE *from(const void *src);

    struct gbm_device *gbm_;
    int fd;
};

#endif  //__TONEMAPPER_EGLIMAGE_BUFFER_LE_H__
