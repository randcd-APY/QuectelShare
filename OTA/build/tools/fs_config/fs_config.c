/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>

#include <selinux/selinux.h>
#include <selinux/label.h>

#include "private/android_filesystem_config.h"

// This program takes a list of files and directories (indicated by a
// trailing slash) on the stdin, and prints to stdout each input
// filename along with its desired uid, gid, and mode (in octal).
// The leading slash should be stripped from the input.
//
// After the first 4 columns, optional key=value pairs are emitted
// for each file.  Currently, the following keys are supported:
// * -S: selabel=[selinux_label]
// * -C: capabilities=[hex capabilities value]
//
// Example input:
//
//      system/etc/dbus.conf
//      data/app/
//
// Output:
//
//      system/etc/dbus.conf 1002 1002 440
//      data/app 1000 1000 771
//
//   or if, for example, -S is used:
//
//      system/etc/dbus.conf 1002 1002 440 selabel=u:object_r:system_file:s0
//      data/app 1000 1000 771 selabel=u:object_r:apk_data_file:s0
//
// Note that the output will omit the trailing slash from
// directories.

static struct selabel_handle* get_sehnd(const char* context_file) {
  struct selinux_opt seopts[] = { { SELABEL_OPT_PATH, context_file } };
  struct selabel_handle* sehnd = selabel_open(SELABEL_CTX_FILE, seopts, 1);

  if (!sehnd) {
    perror("error running selabel_open");
    exit(EXIT_FAILURE);
  }
  return sehnd;
}

static void usage() {
  fprintf(stderr, "Usage: fs_config [-D product_out_path] [-S context_file] [-C]\n");
}

int main(int argc, char** argv) {
  char buffer[1024];
  const char* context_file = NULL;
  const char* product_out_path = NULL;
  struct selabel_handle* sehnd = NULL;
  int print_capabilities = 0;
  int opt;
  while((opt = getopt(argc, argv, "CS:D:")) != -1) {
    switch(opt) {
    case 'C':
      print_capabilities = 1;
      break;
    case 'S':
      context_file = optarg;
      break;
    case 'D':
      product_out_path = optarg;
      break;
    default:
      usage();
      exit(EXIT_FAILURE);
    }
  }

  if (product_out_path == NULL) {
    fprintf(stderr, "fs_config: rootfs/product_out_path not specified!!\n");
    printf("fs_config: rootfs/product_out_path not specified!!\n");
    usage();
    exit(EXIT_FAILURE);
  }

  if (context_file != NULL) {
    sehnd = get_sehnd(context_file);
  }

  while (fgets(buffer, 1023, stdin) != NULL) {
    int is_dir = 0;
    int i;
    for (i = 0; i < 1024 && buffer[i]; ++i) {
      switch (buffer[i]) {
        case '\n':
          buffer[i-is_dir] = '\0';
          i = 1025;
          break;
        case '/':
          is_dir = 1;
          break;
        default:
          is_dir = 0;
          break;
      }
    }

    unsigned uid = 0, gid = 0, mode = 0;
    uint64_t capabilities;
    fs_config(buffer, is_dir, product_out_path, &uid, &gid, &mode, &capabilities);
    printf("%s %d %d %o", buffer, uid, gid, mode);

    if (sehnd != NULL) {
      // if sehnd is not NULL,
      // compute the file "mode" to be passed to
      // selabel_lookup.

      // assuming that all filenames lead with "system/",
      // ignore the first 6 chars from the given filename.
      char* file_to_check = (char*) malloc(strlen(buffer+6) + \
          strlen(product_out_path) + 1);

      if (file_to_check == NULL) {
        perror("malloc");
        printf("fs_config: malloc failed, exiting!\n");
        exit(EXIT_FAILURE);
      }
      strcpy(file_to_check, product_out_path);
      strcat(file_to_check, buffer+6);

      // printf("checking file %s", file_to_check);
      struct stat info;
      if (lstat(file_to_check, &info) != 0) {
        perror("lstat() error");
        // incase of error, set mode to REG or DIR
        mode = mode | (is_dir ? S_IFDIR : S_IFREG);
      } else {
        // printf("lstat() returned:");
        // printf("   mode:   %08x\n",       info.st_mode);
        // printf("    uid:   %d\n",   (int) info.st_uid);
        // printf("    gid:   %d\n",   (int) info.st_gid);
        if (S_ISLNK(info.st_mode)) {
          // printf ("stat says link\n");
          mode = mode | S_IFLNK;
        } else if (S_ISDIR(info.st_mode)) {
          // printf ("stat says dir\n");
          mode = mode | S_IFDIR;
        } else {
          // printf ("stat says regular file\n");
          mode = mode | S_IFREG;
        }
      }

      size_t buffer_strlen = strnlen(buffer, sizeof(buffer));
      if (buffer_strlen >= sizeof(buffer)) {
        fprintf(stderr, "non null terminated buffer, aborting\n");
        exit(EXIT_FAILURE);
      }
      size_t full_name_size = buffer_strlen + 2;
      char* full_name = (char*) malloc(full_name_size);
      if (full_name == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
      }

      full_name[0] = '/';
      strncpy(full_name + 1, buffer, full_name_size - 1);
      full_name[full_name_size - 1] = '\0';

      char* secontext;
      if (selabel_lookup(sehnd, &secontext, full_name, mode)) {
        secontext = strdup("u:object_r:unlabeled:s0");
      }

      printf(" selabel=%s", secontext);
      free(full_name);
      freecon(secontext);
    }

    if (print_capabilities) {
      printf(" capabilities=0x%" PRIx64, capabilities);
    }

    printf("\n");
  }
  return 0;
}
