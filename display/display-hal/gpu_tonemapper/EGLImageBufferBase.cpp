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

#include "EGLImageBufferBase.h"
#include <map>
#include "glengine.h"

//-----------------------------------------------------------------------------
int EGLImageBufferBase::getWidth()
//-----------------------------------------------------------------------------
{
  return width;
}

//-----------------------------------------------------------------------------
int EGLImageBufferBase::getHeight()
//-----------------------------------------------------------------------------
{
  return height;
}

//-----------------------------------------------------------------------------
unsigned int EGLImageBufferBase::getTexture()
//-----------------------------------------------------------------------------
{
  if (textureID == 0) {
    bindAsTexture();
  }

  return textureID;
}

//-----------------------------------------------------------------------------
unsigned int EGLImageBufferBase::getFramebuffer()
//-----------------------------------------------------------------------------
{
  if (framebufferID == 0) {
    bindAsFramebuffer();
  }

  return framebufferID;
}

//-----------------------------------------------------------------------------
void EGLImageBufferBase::bindAsTexture()
//-----------------------------------------------------------------------------
{
  if (textureID == 0) {
    GL(glGenTextures(1, &textureID));
    int target = 0x8D65;
    GL(glBindTexture(target, textureID));
    GL(glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL(glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL(glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL(glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GL(glEGLImageTargetTexture2DOES(0x8D65, eglImageID));
  }

  GL(glBindTexture(0x8D65, textureID));
}

//-----------------------------------------------------------------------------
void EGLImageBufferBase::bindAsFramebuffer()
//-----------------------------------------------------------------------------
{
  if (renderbufferID == 0) {
    GL(glGenFramebuffers(1, &framebufferID));
    GL(glGenRenderbuffers(1, &renderbufferID));

    GL(glBindRenderbuffer(GL_RENDERBUFFER, renderbufferID));
    GL(glEGLImageTargetRenderbufferStorageOES(GL_RENDERBUFFER, eglImageID));

    GL(glBindFramebuffer(GL_FRAMEBUFFER, framebufferID));
    GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                                 renderbufferID));
    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) {
      fprintf(stderr, "%s Framebuffer Invalid***************", __FUNCTION__);
    }
  }

  GL(glBindFramebuffer(GL_FRAMEBUFFER, framebufferID));
}
