/*
 * Copyright (C) 2007 The Android Open Source Project
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

#ifndef RECOVERY_COMMON_H
#define RECOVERY_COMMON_H

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#define LOGE(...) ui_print("E:" __VA_ARGS__)
#define LOGW(...) fprintf(stdout, "W:" __VA_ARGS__)
#define LOGI(...) fprintf(stdout, "I:" __VA_ARGS__)

#if 0
#define LOGV(...) fprintf(stdout, "V:" __VA_ARGS__)
#define LOGD(...) fprintf(stdout, "D:" __VA_ARGS__)
#else
#define LOGV(...) do {} while (0)
#define LOGD(...) do {} while (0)
#endif

#define STRINGIFY(x) #x
#define EXPAND(x) STRINGIFY(x)

#ifdef USE_LE_MODE
#define IS_LE_MODE() (true)
#else
#define IS_LE_MODE() (false)
#endif

extern bool modified_flash;
typedef struct fstab_rec Volume;

// fopen a file, mounting volumes and making parent dirs as necessary.
FILE* fopen_path(const char *path, const char *mode);

void ui_print(const char* format, ...);

bool is_ro_debuggable();

// used in fuse_sideload.c
typedef unsigned long long __u64;

// used in ui.h
#define __printflike(x, y) __attribute__((__format__(printf, x, y))) __nonnull((x))

//From vold/cryptfs.h
#define CRYPT_FOOTER_OFFSET 0x4000

int adb_server_main(int is_daemon, int server_port, int /* reply_fd */);

#endif  // RECOVERY_COMMON_H
