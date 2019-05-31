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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "util.h"

static GLuint load_shader(GLenum shaderType, const char *src) {
	GLint status = 0, len = 0;
	GLuint shader;

	if (!(shader = glCreateShader(shaderType)))
		return 0;

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status)
		return shader;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len) {
		char *msg = malloc(len);
		if (msg) {
			glGetShaderInfoLog(shader, len, NULL, msg);
			msg[len-1] = 0;
			fprintf(stderr, "error compiling shader:\n%s\n", msg);
			free(msg);
		}
	}
	glDeleteShader(shader);
	return 0;
}

GLuint load_program(const char *vert_src, const char *frag_src) {
	GLuint vert, frag, prog;
	GLint status = 0, len = 0;

	if (!(vert = load_shader(GL_VERTEX_SHADER, vert_src)))
		return 0;
	if (!(frag = load_shader(GL_FRAGMENT_SHADER, frag_src)))
		goto fail_frag;
	if (!(prog = glCreateProgram()))
		goto fail_prog;

	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);

	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if (status)
		return prog;

	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len) {
		char *buf = (char*) malloc(len);
		if (buf) {
			glGetProgramInfoLog(prog, len, NULL, buf);
			buf[len-1] = 0;
			fprintf(stderr, "error linking program:\n%s\n", buf);
			free(buf);
		}
	}
	glDeleteProgram(prog);
fail_prog:
	glDeleteShader(frag);
fail_frag:
	glDeleteShader(vert);
	return 0;
}

void egl_destroy(EGLDisplay display, EGLSurface surface) {
		eglDestroySurface(display, surface);
		eglTerminate(display);
}
