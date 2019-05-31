/*
 * Copyright (C) 2013 The Android Open Source Project
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

#ifndef _GL_UTIL_H_
#define _GL_UTIL_H_

#include <GLES/gl.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES/glext.h>
/* convenience */

#ifdef __cplusplus
extern "C" {
#endif

GLuint load_program(const char *vert_src, const char *frag_src);

/* context setup / teardown */
void egl_destroy(EGLDisplay display, EGLSurface surface);
/* internals needed by util.c */
#endif

#ifdef __cplusplus
}
#endif
