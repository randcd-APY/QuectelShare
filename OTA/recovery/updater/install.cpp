/*
 * Copyright (C) 2009 The Android Open Source Project
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

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <selinux/selinux.h>
#include <ftw.h>
#include <sys/capability.h>
#include <sys/xattr.h>
#include <linux/xattr.h>
#include <inttypes.h>

#include <memory>
#include <string>
#include <vector>

#include <base/file.h>
#include <android-base/parseint.h>
#include <base/strings.h>
#include <base/stringprintf.h>

#include "bootloader.h"
#include "applypatch/applypatch.h"
#include "cutils/android_reboot.h"
#include "cutils/misc.h"
#include "cutils/properties.h"
#include "edify/expr.h"
#include "error_code.h"
#include "minzip/DirUtil.h"
#include "mtdutils/mounts.h"
#include "mtdutils/mtdutils.h"
#include "openssl/sha.h"
#include "otafault/ota_io.h"
#include "updater.h"
#include "install.h"

#ifndef USE_LE_MODE
#include "tune2fs.h"
#endif

#ifdef USE_EXT4
#include "make_ext4fs.h"
#include "wipe.h"
#endif

#ifdef USE_LE_MODE
#include "fs_mgr.h"
#include "common.h"
extern "C" {    // Use till system/core is updated
#include "wipe.h"
}

#ifdef TARGET_SUPPORTS_AB
#include <libabctl.h>
#include <errno.h>
#include <dirent.h>
#include "print_sha1.h"

#define BOOTDEVICE_DIR "/dev/block/bootdevice/by-name"
#define BLOCKSIZE 4096*1024
#endif

static int num_volumes = 0;
static Volume* device_volumes = NULL;
#endif

// Send over the buffer to recovery though the command pipe.
static void uiPrint(State* state, const std::string& buffer) {
    UpdaterInfo* ui = reinterpret_cast<UpdaterInfo*>(state->cookie);

    // "line1\nline2\n" will be split into 3 tokens: "line1", "line2" and "".
    // So skip sending empty strings to UI.
    std::vector<std::string> lines = android::base::Split(buffer, "\n");
    for (auto& line: lines) {
        if (!line.empty()) {
            fprintf(ui->cmd_pipe, "ui_print %s\n", line.c_str());
            fprintf(ui->cmd_pipe, "ui_print\n");
        }
    }

    // On the updater side, we need to dump the contents to stderr (which has
    // been redirected to the log file). Because the recovery will only print
    // the contents to screen when processing pipe command ui_print.
    fprintf(stderr, "%s", buffer.c_str());
}

__attribute__((__format__(printf, 2, 3))) __nonnull((2))
void uiPrintf(State* state, const char* format, ...) {
    std::string error_msg;

    va_list ap;
    va_start(ap, format);
    android::base::StringAppendV(&error_msg, format, ap);
    va_end(ap);

    uiPrint(state, error_msg);
}

// Take a sha-1 digest and return it as a newly-allocated hex string.
char* PrintSha1(const uint8_t* digest) {
    char* buffer = reinterpret_cast<char*>(malloc(SHA_DIGEST_LENGTH*2 + 1));
    const char* alphabet = "0123456789abcdef";
    size_t i;
    for (i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        buffer[i*2] = alphabet[(digest[i] >> 4) & 0xf];
        buffer[i*2+1] = alphabet[digest[i] & 0xf];
    }
    buffer[i*2] = '\0';
    return buffer;
}

int parse_fstab(FILE *logfd, char *name, int *alloc) {
    FILE* fstab;

    fstab = fopen(name, "r");
    if(!fstab){
        fprintf(logfd, "ui_print %s not found\n", name);
        return -1;
    }

    char buffer[1024];
    int i;
    while (fgets(buffer, sizeof(buffer)-1, fstab)) {
        for (i = 0; buffer[i] && isspace(buffer[i]); ++i);
        if (buffer[i] == '\0' || buffer[i] == '#') continue;

        char* original = strdup(buffer);
        char* saveptr = NULL; // to be passed to strtok_r()

        char* device = strtok_r(buffer+i, " \t\n", &saveptr);
        char* mount_point = strtok_r(NULL, " \t\n", &saveptr);
        char* fs_type = strtok_r(NULL, " \t\n", &saveptr);

        if (mount_point && fs_type && device) {
            while (num_volumes >= *alloc) {
                *alloc *= 2;
                device_volumes = (Volume*) realloc(device_volumes, (*alloc)*sizeof(Volume));
                if (!device_volumes) {
                    printf("parse_fstab: realloc() failed, line: %d", __LINE__);
                    return -1;
                }
            }
            device_volumes[num_volumes].mount_point = strdup(mount_point);
            device_volumes[num_volumes].fs_type = strdup(fs_type);
            device_volumes[num_volumes].blk_device = strdup(device);
            device_volumes[num_volumes].length = 0;
            ++num_volumes;
        } else {
            if (original)
                fprintf(logfd, "ui_print skipping malformed fstab (%s) line: %s\n", name, original);
        }
        free(original);
    }

    fclose(fstab);
    return 0;
}

void load_volume_table(FILE *logfd) {
    int alloc = 2;
    int i;

    if (device_volumes)
        return;

    device_volumes = (Volume*) malloc(alloc * sizeof(Volume));

    if (!device_volumes) {
        printf("load_volume_table: malloc() failed, line: %d", __LINE__);
        return;
    }

    // Insert an entry for /tmp, which is the ramdisk and is always mounted.
    if (asprintf(&device_volumes[0].mount_point, "/tmp") == -1)
        device_volumes[0].mount_point = NULL;
    if (asprintf(&device_volumes[0].fs_type, "ramdisk") == -1)
        device_volumes[0].fs_type = NULL;
    device_volumes[0].blk_device = NULL;
    //device_volumes[0].device2 = NULL;
    device_volumes[0].length = 0;
    num_volumes = 1;

    if (parse_fstab(logfd, "/res/recovery_volume_config", &alloc) < 0) {
        fprintf(logfd, "ui_print /res/recovery_volume_config not found\n");
    }
    if (parse_fstab(logfd, "/tmp/recovery_volume_detected", &alloc) < 0) {
        fprintf(logfd, "ui_print /tmp/recovery_volume_detected not found\n");
    }
}

void free_volume_table() {
    int i;

    if (!device_volumes)
        return;

    for (i = 0; i < num_volumes; ++i) {
        Volume* v = device_volumes + i;
        if (v->mount_point)
            free(v->mount_point);
        if (v->fs_type)
            free(v->fs_type);
        if (v->blk_device)
            free(v->blk_device);
        //if (v->device2)
        //    free(v->device2);
    }
    free(device_volumes);
    device_volumes = 0;
}

Volume* volume_for_path(const char* path) {
    int i;
    for (i = 0; i < num_volumes; ++i) {
        Volume* v = device_volumes+i;
        int len = strlen(v->mount_point);
        if (strncmp(path, v->mount_point, len) == 0 &&
            (path[len] == '\0' || path[len] == '/')) {
            return v;
        }
    }
    return NULL;
}

// Execute command
int exec_command(FILE *logfd, const char *name, char *const args[]) {
    int status = -1;
    int i;
    pid_t pid;

    pid = fork();
    if (pid == -1) {
        fprintf(logfd, "ui_print exec failed at fork \'%s\'\n", name);
        goto cleanup;
    } else if (pid == 0) {
        fprintf(logfd, "ui_print executing \'%s\'", name);
        for (i = 0; i < 10; i++) {  // limit logging to reduce verbage
            if (args[i]) {
                fprintf(logfd, "ui_print %s", args[i]);
            } else {
                break;
            }
        }
        fprintf(logfd, "ui_print \n", name);
        execvp(name, args);
        fprintf(logfd, "ui_print exec failed \'%s\'\n", name);
        _exit(-1);
    } else {
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(logfd, "ui_print exec returned error %d \'%s\'\n",
                    WEXITSTATUS(status), name);
        }
    }

cleanup:
    return status;
}

// mount(fs_type, partition_type, location, mount_point)
//
//    fs_type="yaffs2" partition_type="MTD"     location=partition
//    fs_type="ubifs"  partition_type="UBI"     location=volume_alias
//    fs_type="ext4"   partition_type="EMMC"    location=device
Value* MountFn(const char* name, State* state, int argc, Expr* argv[]) {
    char* result = NULL;
    if (argc != 4 && argc != 5) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 4-5 args, got %d", name, argc);
    }
    char* fs_type;
    char* partition_type;
    char* location;
    char* mount_point;
    char* mount_options;
    bool has_mount_options;
    if (argc == 5) {
        has_mount_options = true;
        if (ReadArgs(state, argv, 5, &fs_type, &partition_type,
                 &location, &mount_point, &mount_options) < 0) {
            return NULL;
        }
    } else {
        has_mount_options = false;
        if (ReadArgs(state, argv, 4, &fs_type, &partition_type,
                 &location, &mount_point) < 0) {
            return NULL;
        }
    }

    if (strlen(fs_type) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "fs_type argument to %s() can't be empty", name);
        goto done;
    }
    if (strlen(partition_type) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "partition_type argument to %s() can't be empty",
                   name);
        goto done;
    }
    if (strlen(location) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "location argument to %s() can't be empty", name);
        goto done;
    }
    if (strlen(mount_point) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "mount_point argument to %s() can't be empty",
                   name);
        goto done;
    }

    {
        char *secontext = NULL;

        if (sehandle) {
            selabel_lookup(sehandle, &secontext, mount_point, 0755);
            setfscreatecon(secontext);
        }

        mkdir(mount_point, 0755);

        if (secontext) {
            freecon(secontext);
            setfscreatecon(NULL);
        }
    }

    if (strcmp(partition_type, "MTD") == 0) {
        mtd_scan_partitions();
        const MtdPartition* mtd;
        mtd = mtd_find_partition_by_name(location);
        if (mtd == NULL) {
            uiPrintf(state, "%s: no mtd partition named \"%s\"\n",
                    name, location);
            result = strdup("");
            goto done;
        }
        if (mtd_mount_partition(mtd, mount_point, fs_type, 0 /* rw */) != 0) {
            uiPrintf(state, "mtd mount of %s failed: %s\n",
                    location, strerror(errno));
            result = strdup("");
            goto done;
        }
        result = mount_point;
    } else if ((strcmp(fs_type, "ubifs") == 0)
               || (strcmp(fs_type, "ext4") == 0)) {
        Volume *v = 0;;
        v = volume_for_path(mount_point);
        if (!v) {
            fprintf(stderr, "%s: failed to locate %s \"%s\"",
                    name, fs_type, location);
            result = strdup("");
            goto done;
        }
        if (mount(v->blk_device, mount_point, fs_type,
                  MS_NOATIME | MS_NODEV | MS_NODIRATIME, "") < 0) {
            fprintf(stderr, "%s: failed to mount %s at %s: %s\n",
                    name, location, mount_point, strerror(errno));
            result = strdup("");
        } else {
            result = mount_point;
        }
    } else {
        if (mount(location, mount_point, fs_type,
                  MS_NOATIME | MS_NODEV | MS_NODIRATIME,
                  has_mount_options ? mount_options : "") < 0) {
            uiPrintf(state, "%s: failed to mount %s at %s: %s\n",
                    name, location, mount_point, strerror(errno));
            result = strdup("");
        } else {
            result = mount_point;
        }
    }

done:
    free(fs_type);
    free(partition_type);
    free(location);
    if (result != mount_point) free(mount_point);
    if (has_mount_options) free(mount_options);
    return StringValue(result);
}


// is_mounted(mount_point)
Value* IsMountedFn(const char* name, State* state, int argc, Expr* argv[]) {
    char* result = NULL;
    if (argc != 1) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 1 arg, got %d", name, argc);
    }
    char* mount_point;
    if (ReadArgs(state, argv, 1, &mount_point) < 0) {
        return NULL;
    }
    if (strlen(mount_point) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "mount_point argument to unmount() can't be empty");
        goto done;
    }

    scan_mounted_volumes();
    {
        const MountedVolume* vol = find_mounted_volume_by_mount_point(mount_point);
        if (vol == NULL) {
            result = strdup("");
        } else {
            result = mount_point;
        }
    }

done:
    if (result != mount_point) free(mount_point);
    return StringValue(result);
}


Value* UnmountFn(const char* name, State* state, int argc, Expr* argv[]) {
    char* result = NULL;
    if (argc != 1) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 1 arg, got %d", name, argc);
    }
    char* mount_point;
    if (ReadArgs(state, argv, 1, &mount_point) < 0) {
        return NULL;
    }
    if (strlen(mount_point) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "mount_point argument to unmount() can't be empty");
        goto done;
    }

    scan_mounted_volumes();
    {
        const MountedVolume* vol = find_mounted_volume_by_mount_point(mount_point);
        if (vol == NULL) {
            uiPrintf(state, "unmount of %s failed; no such volume\n", mount_point);
            result = strdup("");
        } else {
            int ret = unmount_mounted_volume(vol);
            if (ret != 0) {
                uiPrintf(state, "unmount of %s failed (%d): %s\n",
                         mount_point, ret, strerror(errno));
            }
            result = mount_point;
        }
    }

done:
    if (result != mount_point) free(mount_point);
    return StringValue(result);
}

static int exec_cmd(const char* path, char* const argv[]) {
    int status;
    pid_t child;
    if ((child = vfork()) == 0) {
        execv(path, argv);
        _exit(-1);
    }
    waitpid(child, &status, 0);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        printf("%s failed with status %d\n", path, WEXITSTATUS(status));
    }
    return WEXITSTATUS(status);
}


// format(fs_type, partition_type, location, fs_size, mount_point)
//
//    fs_type="yaffs2" partition_type="MTD"     location=partition fs_size=<bytes> mount_point=<location>
//    fs_type="ubifs"  partition_type="UBI"     location=path      fs_size=(ignored)
//    fs_type="ext4"   partition_type="EMMC"    location=device    fs_size=<bytes> mount_point=<location>
//    fs_type="f2fs"   partition_type="EMMC"    location=device    fs_size=<bytes> mount_point=<location>
//    if fs_size == 0, then make fs uses the entire partition.
//    if fs_size > 0, that is the size to use
//    if fs_size < 0, then reserve that many bytes at the end of the partition (not for "f2fs")
Value* FormatFn(const char* name, State* state, int argc, Expr* argv[]) {
    char* result = NULL;
    if (argc != 5) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 5 args, got %d", name, argc);
    }
    char* fs_type;
    char* partition_type;
    char* location;
    char* fs_size;
    char* mount_point;

    if (ReadArgs(state, argv, 5, &fs_type, &partition_type, &location, &fs_size, &mount_point) < 0) {
        return NULL;
    }

    if (strlen(fs_type) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "fs_type argument to %s() can't be empty", name);
        goto done;
    }
    if (strlen(partition_type) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "partition_type argument to %s() can't be empty",
                   name);
        goto done;
    }
    if (strlen(location) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "location argument to %s() can't be empty", name);
        goto done;
    }

    if (strlen(mount_point) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "mount_point argument to %s() can't be empty",
                   name);
        goto done;
    }

    if (strcmp(partition_type, "MTD") == 0) {
        mtd_scan_partitions();
        const MtdPartition* mtd = mtd_find_partition_by_name(location);
        if (mtd == NULL) {
            printf("%s: no mtd partition named \"%s\"",
                    name, location);
            result = strdup("");
            goto done;
        }
        MtdWriteContext* ctx = mtd_write_partition(mtd);
        if (ctx == NULL) {
            printf("%s: can't write \"%s\"", name, location);
            result = strdup("");
            goto done;
        }
        if (mtd_erase_blocks(ctx, -1) == -1) {
            mtd_write_close(ctx);
            printf("%s: failed to erase \"%s\"", name, location);
            result = strdup("");
            goto done;
        }
        if (mtd_write_close(ctx) != 0) {
            printf("%s: failed to close \"%s\"", name, location);
            result = strdup("");
            goto done;
        }
        result = location;
    } else if (strcmp(fs_type, "ubifs") == 0) {
        Volume *v = 0;;
        UpdaterInfo* ui = (UpdaterInfo*)(state->cookie);
        v = volume_for_path(mount_point);
        if (!v) {
            fprintf(stderr, "%s: failed to locate ubifs volume \"%s\"", name, location);
            result = strdup("");
            goto done;
        }
        char *argv[] = {"mkfs.ubifs", "-y", v->blk_device, 0};
        if (exec_command(ui->cmd_pipe, "/usr/sbin/mkfs.ubifs", argv) != 0) {
            fprintf(stderr, "%s: failed to create ubifs volume \"%s\"", name, location);
            fprintf(ui->cmd_pipe, "ui_print %s: failed to format ubifs volume \"%s\"\n", name, location);
            result = strdup("");
            goto done;
        }
        result = location;
#ifdef USE_EXT4  /* Use linked in ext4fs generation tool */
    } else if (strcmp(fs_type, "ext4") == 0) {
        int status = make_ext4fs(location, atoll(fs_size), mount_point, sehandle);
        if (status != 0) {
            printf("%s: make_ext4fs failed (%d) on %s",
                    name, status, location);
            result = strdup("");
            goto done;
        }
        result = location;
    } else if (strcmp(fs_type, "f2fs") == 0) {
        char *num_sectors;
        if (asprintf(&num_sectors, "%lld", atoll(fs_size) / 512) <= 0) {
            printf("format_volume: failed to create %s command for %s\n", fs_type, location);
            result = strdup("");
            goto done;
        }
        const char *f2fs_path = "/sbin/mkfs.f2fs";
        const char* const f2fs_argv[] = {"mkfs.f2fs", "-t", "-d1", location, num_sectors, NULL};
        int status = exec_cmd(f2fs_path, (char* const*)f2fs_argv);
        free(num_sectors);
        if (status != 0) {
            printf("%s: mkfs.f2fs failed (%d) on %s",
                    name, status, location);
            result = strdup("");
            goto done;
        }
        result = location;
#else  /* Use e2fsprogs modules */
    } else if (strcmp(fs_type, "ext4") == 0) {
        Volume *v = 0;;
        UpdaterInfo* ui = (UpdaterInfo*)(state->cookie);
        v = volume_for_path(mount_point);
        if (!v) {
            fprintf(stderr, "%s: failed to locate ext4 filesystem \"%s\"", name, location);
            result = strdup("");
            goto done;
        }
        char *argv[] = {"mkfs.ext4", "-b", "4096", "-O", "extent,uninit_bg,dir_index,has_journal,sparse_super", v->blk_device, 0};
        if (exec_command(ui->cmd_pipe, "/sbin/mkfs.ext4", argv) != 0) {
            fprintf(stderr, "%s: failed to create ext4 filesystem \"%s\"", name, location);
            fprintf(ui->cmd_pipe, "ui_print %s: failed to format ubifs volume \"%s\"\n", name, location);
            result = strdup("");
            goto done;
        }
        result = location;
#endif
    } else {
        printf("%s: unsupported fs_type \"%s\" partition_type \"%s\"",
                name, fs_type, partition_type);
    }

done:
    free(fs_type);
    free(partition_type);
    if (result != location) free(location);
    return StringValue(result);
}

Value* RenameFn(const char* name, State* state, int argc, Expr* argv[]) {
    char* result = NULL;
    if (argc != 2) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 2 args, got %d", name, argc);
    }

    char* src_name;
    char* dst_name;

    if (ReadArgs(state, argv, 2, &src_name, &dst_name) < 0) {
        return NULL;
    }
    if (strlen(src_name) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "src_name argument to %s() can't be empty", name);
        goto done;
    }
    if (strlen(dst_name) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "dst_name argument to %s() can't be empty", name);
        goto done;
    }
    if (make_parents(dst_name) != 0) {
        ErrorAbort(state, kFileRenameFailure, "Creating parent of %s failed, error %s",
          dst_name, strerror(errno));
    } else if (access(dst_name, F_OK) == 0 && access(src_name, F_OK) != 0) {
        // File was already moved
        result = dst_name;
    } else if (rename(src_name, dst_name) != 0) {
        ErrorAbort(state, kFileRenameFailure, "Rename of %s to %s failed, error %s",
          src_name, dst_name, strerror(errno));
    } else {
        result = dst_name;
    }

done:
    free(src_name);
    if (result != dst_name) free(dst_name);
    return StringValue(result);
}

Value* DeleteFn(const char* name, State* state, int argc, Expr* argv[]) {
    char** paths = reinterpret_cast<char**>(malloc(argc * sizeof(char*)));
    for (int i = 0; i < argc; ++i) {
        paths[i] = Evaluate(state, argv[i]);
        if (paths[i] == NULL) {
            for (int j = 0; j < i; ++j) {
                free(paths[j]);
            }
            free(paths);
            return NULL;
        }
    }

    bool recursive = (strcmp(name, "delete_recursive") == 0);

    int success = 0;
    for (int i = 0; i < argc; ++i) {
        if ((recursive ? dirUnlinkHierarchy(paths[i]) : unlink(paths[i])) == 0)
            ++success;
        free(paths[i]);
    }
    free(paths);

    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%d", success);
    return StringValue(strdup(buffer));
}


Value* ShowProgressFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc != 2) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 2 args, got %d", name, argc);
    }
    char* frac_str;
    char* sec_str;
    if (ReadArgs(state, argv, 2, &frac_str, &sec_str) < 0) {
        return NULL;
    }

    double frac = strtod(frac_str, NULL);
    int sec;
    android::base::ParseInt(sec_str, &sec);

    UpdaterInfo* ui = (UpdaterInfo*)(state->cookie);
    fprintf(ui->cmd_pipe, "progress %f %d\n", frac, sec);

    free(sec_str);
    return StringValue(frac_str);
}

Value* SetProgressFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc != 1) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 1 arg, got %d", name, argc);
    }
    char* frac_str;
    if (ReadArgs(state, argv, 1, &frac_str) < 0) {
        return NULL;
    }

    double frac = strtod(frac_str, NULL);

    UpdaterInfo* ui = (UpdaterInfo*)(state->cookie);
    fprintf(ui->cmd_pipe, "set_progress %f\n", frac);

    return StringValue(frac_str);
}

// package_extract_dir(package_path, destination_path)
Value* PackageExtractDirFn(const char* name, State* state,
                          int argc, Expr* argv[]) {
    if (argc != 2) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 2 args, got %d", name, argc);
    }
    char* zip_path;
    char* dest_path;
    if (ReadArgs(state, argv, 2, &zip_path, &dest_path) < 0) return NULL;

    ZipArchive* za = ((UpdaterInfo*)(state->cookie))->package_zip;

    // To create a consistent system image, never use the clock for timestamps.
    struct utimbuf timestamp = { 1217592000, 1217592000 };  // 8/1/2008 default

    bool success = mzExtractRecursive(za, zip_path, dest_path,
                                      &timestamp,
                                      NULL, NULL, sehandle);
    free(zip_path);
    free(dest_path);
    return StringValue(strdup(success ? "t" : ""));
}


// package_extract_file(package_path, destination_path)
//   or
// package_extract_file(package_path)
//   to return the entire contents of the file as the result of this
//   function (the char* returned is actually a FileContents*).
Value* PackageExtractFileFn(const char* name, State* state,
                           int argc, Expr* argv[]) {
    if (argc < 1 || argc > 2) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 1 or 2 args, got %d",
                          name, argc);
    }
    bool success = false;

    if (argc == 2) {
        // The two-argument version extracts to a file.

        ZipArchive* za = ((UpdaterInfo*)(state->cookie))->package_zip;

        char* zip_path;
        char* dest_path;
        if (ReadArgs(state, argv, 2, &zip_path, &dest_path) < 0) return NULL;

        const ZipEntry* entry = mzFindZipEntry(za, zip_path);
        if (entry == NULL) {
            printf("%s: no %s in package\n", name, zip_path);
            goto done2;
        }

        {
#ifdef TARGET_SUPPORTS_AB
            // Check if dest_path here is a block-devices or not
            if (strncmp(dest_path, BOOTDEVICE_DIR, strlen(BOOTDEVICE_DIR)) == 0) {
                // append the inactive-slot's suffix to the path
                char buffer[PATH_MAX];
                snprintf(buffer, PATH_MAX, "%s%s", dest_path,
                        slot_suffix_arr[inactive_slot]);
                dest_path = strdup(buffer);
                if (dest_path == NULL) {
                    printf("%s: strdup() failure at line %d: %s\n",
                            name, __LINE__, strerror(errno));
                    return NULL;
                }
                printf("%s: Writing %s to %s\n", name, zip_path, dest_path);
            }
#endif

            int fd = TEMP_FAILURE_RETRY(ota_open(dest_path, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC,
                  S_IRUSR | S_IWUSR));
            if (fd == -1) {
                printf("%s: can't open %s for write: %s\n", name, dest_path, strerror(errno));
                goto done2;
            }
            success = mzExtractZipEntryToFile(za, entry, fd);
            if (ota_fsync(fd) == -1) {
                printf("fsync of \"%s\" failed: %s\n", dest_path, strerror(errno));
                success = false;
            }
            if (ota_close(fd) == -1) {
                printf("close of \"%s\" failed: %s\n", dest_path, strerror(errno));
                success = false;
            }
            printf("%s %s to %s\n",
                    success ? "extracted" : "failed to extract", zip_path, dest_path);
        }

      done2:
        free(zip_path);
        free(dest_path);
        return StringValue(strdup(success ? "t" : ""));
    } else {
        // The one-argument version returns the contents of the file
        // as the result.

        char* zip_path;
        if (ReadArgs(state, argv, 1, &zip_path) < 0) return NULL;

        Value* v = reinterpret_cast<Value*>(malloc(sizeof(Value)));
        v->type = VAL_BLOB;
        v->size = -1;
        v->data = NULL;

        ZipArchive* za = ((UpdaterInfo*)(state->cookie))->package_zip;
        const ZipEntry* entry = mzFindZipEntry(za, zip_path);
        if (entry == NULL) {
            printf("%s: no %s in package\n", name, zip_path);
            goto done1;
        }

        v->size = mzGetZipEntryUncompLen(entry);
        v->data = reinterpret_cast<char*>(malloc(v->size));
        if (v->data == NULL) {
            printf("%s: failed to allocate %ld bytes for %s\n",
                    name, (long)v->size, zip_path);
            goto done1;
        }

        success = mzExtractZipEntryToBuffer(za, entry,
                                            (unsigned char *)v->data);

      done1:
        free(zip_path);
        if (!success) {
            free(v->data);
            v->data = NULL;
            v->size = -1;
        }
        return v;
    }
}

// Create all parent directories of name, if necessary.
static int make_parents(char* name) {
    char* p;
    for (p = name + (strlen(name)-1); p > name; --p) {
        if (*p != '/') continue;
        *p = '\0';
        if (make_parents(name) < 0) return -1;
        int result = mkdir(name, 0700);
        if (result == 0) printf("created [%s]\n", name);
        *p = '/';
        if (result == 0 || errno == EEXIST) {
            // successfully created or already existed; we're done
            return 0;
        } else {
            printf("failed to mkdir %s: %s\n", name, strerror(errno));
            return -1;
        }
    }
    return 0;
}

// symlink target src1 src2 ...
//    unlinks any previously existing src1, src2, etc before creating symlinks.
Value* SymlinkFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc == 0) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 1+ args, got %d", name, argc);
    }
    char* target;
    target = Evaluate(state, argv[0]);
    if (target == NULL) return NULL;

    char** srcs = ReadVarArgs(state, argc-1, argv+1);
    if (srcs == NULL) {
        free(target);
        return NULL;
    }

    int bad = 0;
    int i;
    for (i = 0; i < argc-1; ++i) {
        if (unlink(srcs[i]) < 0) {
            if (errno != ENOENT) {
                printf("%s: failed to remove %s: %s\n",
                        name, srcs[i], strerror(errno));
                ++bad;
            }
        }
        if (make_parents(srcs[i])) {
            printf("%s: failed to symlink %s to %s: making parents failed\n",
                    name, srcs[i], target);
            ++bad;
        }
        if (symlink(target, srcs[i]) < 0) {
            printf("%s: failed to symlink %s to %s: %s\n",
                    name, srcs[i], target, strerror(errno));
            ++bad;
        }
        free(srcs[i]);
    }
    free(srcs);
    if (bad) {
        return ErrorAbort(state, kSymlinkFailure, "%s: some symlinks failed", name);
    }
    return StringValue(strdup(""));
}

Value* SetPermFn(const char* name, State* state, int argc, Expr* argv[]) {
    char* result = NULL;
    bool recursive = (strcmp(name, "set_perm_recursive") == 0);

    int min_args = 4 + (recursive ? 1 : 0);
    if (argc < min_args) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects %d+ args, got %d",
                          name, min_args, argc);
    }

    char** args = ReadVarArgs(state, argc, argv);
    if (args == NULL) return NULL;

    char* end;
    int i;
    int bad = 0;
    int gid = 0;

    int uid = strtoul(args[0], &end, 0);
    if (*end != '\0' || args[0][0] == 0) {
        ErrorAbort(state, kArgsParsingFailure, "%s: \"%s\" not a valid uid", name, args[0]);
        goto done;
    }

    gid = strtoul(args[1], &end, 0);
    if (*end != '\0' || args[1][0] == 0) {
        ErrorAbort(state, kArgsParsingFailure, "%s: \"%s\" not a valid gid", name, args[1]);
        goto done;
    }

    if (recursive) {
        int dir_mode = strtoul(args[2], &end, 0);
        if (*end != '\0' || args[2][0] == 0) {
            ErrorAbort(state, kArgsParsingFailure, "%s: \"%s\" not a valid dirmode", name, args[2]);
            goto done;
        }

        int file_mode = strtoul(args[3], &end, 0);
        if (*end != '\0' || args[3][0] == 0) {
            ErrorAbort(state, kArgsParsingFailure, "%s: \"%s\" not a valid filemode",
                       name, args[3]);
            goto done;
        }

        for (i = 4; i < argc; ++i) {
            dirSetHierarchyPermissions(args[i], uid, gid, dir_mode, file_mode);
        }
    } else {
        int mode = strtoul(args[2], &end, 0);
        if (*end != '\0' || args[2][0] == 0) {
            ErrorAbort(state, kArgsParsingFailure, "%s: \"%s\" not a valid mode", name, args[2]);
            goto done;
        }

        for (i = 3; i < argc; ++i) {
            // Do not set permissions on symlinks
            struct stat st;
            if (lstat(args[i], &st)) {
                ErrorAbort(state, kSetMetadataFailure,
                    "%s: Error on lstat of \"%s\": %s",
                    name, args[i], strerror(errno));
                goto done;
            }
            if (S_ISLNK(st.st_mode)) {
                printf("%s: Not setting permission on symlink file: %s\n",
                        name, args[i]);
                continue;
            }

            if (chown(args[i], uid, gid) < 0) {
                printf("%s: chown of %s to %d %d failed: %s\n",
                        name, args[i], uid, gid, strerror(errno));
                ++bad;
            }
            if (chmod(args[i], mode) < 0) {
                printf("%s: chmod of %s to %o failed: %s\n",
                        name, args[i], mode, strerror(errno));
                ++bad;
            }
        }
    }
    result = strdup("");

done:
    for (i = 0; i < argc; ++i) {
        free(args[i]);
    }
    free(args);

    if (bad) {
        free(result);
        return ErrorAbort(state, kArgsParsingFailure, "%s: some changes failed", name);
    }
    return StringValue(result);
}

struct perm_parsed_args {
    bool has_uid;
    uid_t uid;
    bool has_gid;
    gid_t gid;
    bool has_mode;
    mode_t mode;
    bool has_fmode;
    mode_t fmode;
    bool has_dmode;
    mode_t dmode;
    bool has_selabel;
    char* selabel;
    bool has_capabilities;
    uint64_t capabilities;
};

static struct perm_parsed_args ParsePermArgs(State * state, int argc, char** args) {
    int i;
    struct perm_parsed_args parsed;
    int bad = 0;
    static int max_warnings = 20;

    memset(&parsed, 0, sizeof(parsed));

    for (i = 1; i < argc; i += 2) {
        if (strcmp("uid", args[i]) == 0) {
            int64_t uid;
            if (sscanf(args[i+1], "%" SCNd64, &uid) == 1) {
                parsed.uid = uid;
                parsed.has_uid = true;
            } else {
                uiPrintf(state, "ParsePermArgs: invalid UID \"%s\"\n", args[i + 1]);
                bad++;
            }
            continue;
        }
        if (strcmp("gid", args[i]) == 0) {
            int64_t gid;
            if (sscanf(args[i+1], "%" SCNd64, &gid) == 1) {
                parsed.gid = gid;
                parsed.has_gid = true;
            } else {
                uiPrintf(state, "ParsePermArgs: invalid GID \"%s\"\n", args[i + 1]);
                bad++;
            }
            continue;
        }
        if (strcmp("mode", args[i]) == 0) {
            int32_t mode;
            if (sscanf(args[i+1], "%" SCNi32, &mode) == 1) {
                parsed.mode = mode;
                parsed.has_mode = true;
            } else {
                uiPrintf(state, "ParsePermArgs: invalid mode \"%s\"\n", args[i + 1]);
                bad++;
            }
            continue;
        }
        if (strcmp("dmode", args[i]) == 0) {
            int32_t mode;
            if (sscanf(args[i+1], "%" SCNi32, &mode) == 1) {
                parsed.dmode = mode;
                parsed.has_dmode = true;
            } else {
                uiPrintf(state, "ParsePermArgs: invalid dmode \"%s\"\n", args[i + 1]);
                bad++;
            }
            continue;
        }
        if (strcmp("fmode", args[i]) == 0) {
            int32_t mode;
            if (sscanf(args[i+1], "%" SCNi32, &mode) == 1) {
                parsed.fmode = mode;
                parsed.has_fmode = true;
            } else {
                uiPrintf(state, "ParsePermArgs: invalid fmode \"%s\"\n", args[i + 1]);
                bad++;
            }
            continue;
        }
        if (strcmp("capabilities", args[i]) == 0) {
            int64_t capabilities;
            if (sscanf(args[i+1], "%" SCNi64, &capabilities) == 1) {
                parsed.capabilities = capabilities;
                parsed.has_capabilities = true;
            } else {
                uiPrintf(state, "ParsePermArgs: invalid capabilities \"%s\"\n", args[i + 1]);
                bad++;
            }
            continue;
        }
        if (strcmp("selabel", args[i]) == 0) {
            if (args[i+1][0] != '\0') {
                parsed.selabel = args[i+1];
                parsed.has_selabel = true;
            } else {
                uiPrintf(state, "ParsePermArgs: invalid selabel \"%s\"\n", args[i + 1]);
                bad++;
            }
            continue;
        }
        if (max_warnings != 0) {
            printf("ParsedPermArgs: unknown key \"%s\", ignoring\n", args[i]);
            max_warnings--;
            if (max_warnings == 0) {
                printf("ParsedPermArgs: suppressing further warnings\n");
            }
        }
    }
    return parsed;
}

static int ApplyParsedPerms(
        State * state,
        const char* filename,
        const struct stat *statptr,
        struct perm_parsed_args parsed)
{
    int bad = 0;

    if (parsed.has_selabel) {
        if (lsetfilecon(filename, parsed.selabel) != 0) {
            uiPrintf(state, "ApplyParsedPerms: lsetfilecon of %s to %s failed: %s\n",
                    filename, parsed.selabel, strerror(errno));
            bad++;
        }
    }

    /* ignore symlinks */
    if (S_ISLNK(statptr->st_mode)) {
        return bad;
    }

    if (parsed.has_uid) {
        if (chown(filename, parsed.uid, -1) < 0) {
            uiPrintf(state, "ApplyParsedPerms: chown of %s to %d failed: %s\n",
                    filename, parsed.uid, strerror(errno));
            bad++;
        }
    }

    if (parsed.has_gid) {
        if (chown(filename, -1, parsed.gid) < 0) {
            uiPrintf(state, "ApplyParsedPerms: chgrp of %s to %d failed: %s\n",
                    filename, parsed.gid, strerror(errno));
            bad++;
        }
    }

    if (parsed.has_mode) {
        if (chmod(filename, parsed.mode) < 0) {
            uiPrintf(state, "ApplyParsedPerms: chmod of %s to %d failed: %s\n",
                    filename, parsed.mode, strerror(errno));
            bad++;
        }
    }

    if (parsed.has_dmode && S_ISDIR(statptr->st_mode)) {
        if (chmod(filename, parsed.dmode) < 0) {
            uiPrintf(state, "ApplyParsedPerms: chmod of %s to %d failed: %s\n",
                    filename, parsed.dmode, strerror(errno));
            bad++;
        }
    }

    if (parsed.has_fmode && S_ISREG(statptr->st_mode)) {
        if (chmod(filename, parsed.fmode) < 0) {
            uiPrintf(state, "ApplyParsedPerms: chmod of %s to %d failed: %s\n",
                   filename, parsed.fmode, strerror(errno));
            bad++;
        }
    }

    if (parsed.has_capabilities && S_ISREG(statptr->st_mode)) {
        if (parsed.capabilities == 0) {
            if ((removexattr(filename, XATTR_NAME_CAPS) == -1) && (errno != ENODATA)) {
                // Report failure unless it's ENODATA (attribute not set)
                uiPrintf(state, "ApplyParsedPerms: removexattr of %s to %" PRIx64 " failed: %s\n",
                       filename, parsed.capabilities, strerror(errno));
                bad++;
            }
        } else {
            struct vfs_cap_data cap_data;
            memset(&cap_data, 0, sizeof(cap_data));
            cap_data.magic_etc = VFS_CAP_REVISION | VFS_CAP_FLAGS_EFFECTIVE;
            cap_data.data[0].permitted = (uint32_t) (parsed.capabilities & 0xffffffff);
            cap_data.data[0].inheritable = 0;
            cap_data.data[1].permitted = (uint32_t) (parsed.capabilities >> 32);
            cap_data.data[1].inheritable = 0;
            if (setxattr(filename, XATTR_NAME_CAPS, &cap_data, sizeof(cap_data), 0) < 0) {
                uiPrintf(state, "ApplyParsedPerms: setcap of %s to %" PRIx64 " failed: %s\n",
                        filename, parsed.capabilities, strerror(errno));
                bad++;
            }
        }
    }

    return bad;
}

// nftw doesn't allow us to pass along context, so we need to use
// global variables.  *sigh*
static struct perm_parsed_args recursive_parsed_args;
static State* recursive_state;

static int do_SetMetadataRecursive(const char* filename, const struct stat *statptr,
        int fileflags, struct FTW *pfwt) {
    return ApplyParsedPerms(recursive_state, filename, statptr, recursive_parsed_args);
}

static Value* SetMetadataFn(const char* name, State* state, int argc, Expr* argv[]) {
    int bad = 0;
    struct stat sb;
    Value* result = NULL;

    bool recursive = (strcmp(name, "set_metadata_recursive") == 0);

    if ((argc % 2) != 1) {
        return ErrorAbort(state, kArgsParsingFailure,
                          "%s() expects an odd number of arguments, got %d", name, argc);
    }

    char** args = ReadVarArgs(state, argc, argv);
    if (args == NULL) return NULL;

    if (lstat(args[0], &sb) == -1) {
        result = ErrorAbort(state, kSetMetadataFailure, "%s: Error on lstat of \"%s\": %s",
                            name, args[0], strerror(errno));
        goto done;
    }

    {
        struct perm_parsed_args parsed = ParsePermArgs(state, argc, args);

        if (recursive) {
            recursive_parsed_args = parsed;
            recursive_state = state;
            bad += nftw(args[0], do_SetMetadataRecursive, 30, FTW_CHDIR | FTW_DEPTH | FTW_PHYS);
            memset(&recursive_parsed_args, 0, sizeof(recursive_parsed_args));
            recursive_state = NULL;
        } else {
            bad += ApplyParsedPerms(state, args[0], &sb, parsed);
        }
    }

done:
    for (int i = 0; i < argc; ++i) {
        free(args[i]);
    }
    free(args);

    if (result != NULL) {
        return result;
    }

    if (bad > 0) {
        return ErrorAbort(state, kSetMetadataFailure, "%s: some changes failed", name);
    }

    return StringValue(strdup(""));
}

Value* GetPropFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc != 1) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 1 arg, got %d", name, argc);
    }
    char* key = Evaluate(state, argv[0]);
    if (key == NULL) return NULL;

    char value[PROPERTY_VALUE_MAX];
    property_get(key, value, "");
    free(key);

    return StringValue(strdup(value));
}

// file_getprop(file, key)
//
//   interprets 'file' as a getprop-style file (key=value pairs, one
//   per line. # comment lines,blank lines, lines without '=' ignored),
//   and returns the value for 'key' (or "" if it isn't defined).
Value* FileGetPropFn(const char* name, State* state, int argc, Expr* argv[]) {
    char* result = NULL;
    char* buffer = NULL;
    char* filename;
    char* key;
    if (ReadArgs(state, argv, 2, &filename, &key) < 0) {
        return NULL;
    }

    struct stat st;
    if (stat(filename, &st) < 0) {
        ErrorAbort(state, kFileGetPropFailure, "%s: failed to stat \"%s\": %s", name, filename,
                   strerror(errno));
        goto done;
    }

#define MAX_FILE_GETPROP_SIZE    65536

    if (st.st_size > MAX_FILE_GETPROP_SIZE) {
        ErrorAbort(state, kFileGetPropFailure, "%s too large for %s (max %d)", filename, name,
                   MAX_FILE_GETPROP_SIZE);
        goto done;
    }

    buffer = reinterpret_cast<char*>(malloc(st.st_size+1));
    if (buffer == NULL) {
        ErrorAbort(state, kFileGetPropFailure, "%s: failed to alloc %lld bytes", name,
                   (long long)st.st_size+1);
        goto done;
    }

    FILE* f;
    f = fopen(filename, "rb");
    if (f == NULL) {
        ErrorAbort(state, kFileOpenFailure, "%s: failed to open %s: %s", name, filename,
                   strerror(errno));
        goto done;
    }

    if (ota_fread(buffer, 1, st.st_size, f) != static_cast<size_t>(st.st_size)) {
        ErrorAbort(state, kFreadFailure, "%s: failed to read %lld bytes from %s",
                   name, (long long)st.st_size+1, filename);
        fclose(f);
        goto done;
    }
    buffer[st.st_size] = '\0';

    fclose(f);

    char* line;
    line = strtok(buffer, "\n");
    do {
        // skip whitespace at start of line
        while (*line && isspace(*line)) ++line;

        // comment or blank line: skip to next line
        if (*line == '\0' || *line == '#') continue;

        char* equal = strchr(line, '=');
        if (equal == NULL) {
            continue;
        }

        // trim whitespace between key and '='
        char* key_end = equal-1;
        while (key_end > line && isspace(*key_end)) --key_end;
        key_end[1] = '\0';

        // not the key we're looking for
        if (strcmp(key, line) != 0) continue;

        // skip whitespace after the '=' to the start of the value
        char* val_start = equal+1;
        while(*val_start && isspace(*val_start)) ++val_start;

        // trim trailing whitespace
        char* val_end = val_start + strlen(val_start)-1;
        while (val_end > val_start && isspace(*val_end)) --val_end;
        val_end[1] = '\0';

        result = strdup(val_start);
        break;

    } while ((line = strtok(NULL, "\n")));

    if (result == NULL) result = strdup("");

  done:
    free(filename);
    free(key);
    free(buffer);
    return StringValue(result);
}

// write_raw_image(filename_or_blob, partition)
Value* WriteRawImageFn(const char* name, State* state, int argc, Expr* argv[]) {
    char* result = NULL;

    Value* partition_value;
    Value* contents;
    if (ReadValueArgs(state, argv, 2, &contents, &partition_value) < 0) {
        return NULL;
    }

    char* partition = NULL;
    if (partition_value->type != VAL_STRING) {
        ErrorAbort(state, kArgsParsingFailure, "partition argument to %s must be string", name);
        goto done;
    }
    partition = partition_value->data;
    if (strlen(partition) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "partition argument to %s can't be empty", name);
        goto done;
    }
    if (contents->type == VAL_STRING && strlen((char*) contents->data) == 0) {
        ErrorAbort(state, kArgsParsingFailure, "file argument to %s can't be empty", name);
        goto done;
    }

    mtd_scan_partitions();
    const MtdPartition* mtd;
    mtd = mtd_find_partition_by_name(partition);
    if (mtd == NULL) {
        printf("%s: no mtd partition named \"%s\"\n", name, partition);
        result = strdup("");
        goto done;
    }

    MtdWriteContext* ctx;
    ctx = mtd_write_partition(mtd);
    if (ctx == NULL) {
        printf("%s: can't write mtd partition \"%s\"\n",
                name, partition);
        result = strdup("");
        goto done;
    }

    bool success;

    if (contents->type == VAL_STRING) {
        // we're given a filename as the contents
        char* filename = contents->data;
        FILE* f = ota_fopen(filename, "rb");
        if (f == NULL) {
            printf("%s: can't open %s: %s\n", name, filename, strerror(errno));
            result = strdup("");
            goto done;
        }

        success = true;
        char* buffer = reinterpret_cast<char*>(malloc(BUFSIZ));
        int read;
        while (success && (read = ota_fread(buffer, 1, BUFSIZ, f)) > 0) {
            int wrote = mtd_write_data(ctx, buffer, read);
            success = success && (wrote == read);
        }
        free(buffer);
        ota_fclose(f);
    } else {
        // we're given a blob as the contents
        ssize_t wrote = mtd_write_data(ctx, contents->data, contents->size);
        success = (wrote == contents->size);
    }
    if (!success) {
        printf("mtd_write_data to %s failed: %s\n",
                partition, strerror(errno));
    }

    if (mtd_erase_blocks(ctx, -1) == -1) {
        printf("%s: error erasing blocks of %s\n", name, partition);
    }
    if (mtd_write_close(ctx) != 0) {
        printf("%s: error closing write of %s\n", name, partition);
    }

    printf("%s %s partition\n",
           success ? "wrote" : "failed to write", partition);

    result = success ? partition : strdup("");

done:
    if (result != partition) FreeValue(partition_value);
    FreeValue(contents);
    return StringValue(result);
}

// apply_patch_space(bytes)
Value* ApplyPatchSpaceFn(const char* name, State* state,
                         int argc, Expr* argv[]) {
    char* bytes_str;
    if (ReadArgs(state, argv, 1, &bytes_str) < 0) {
        return NULL;
    }

    size_t bytes;
    if (!android::base::ParseUint(bytes_str, &bytes)) {
        ErrorAbort(state, kArgsParsingFailure, "%s(): can't parse \"%s\" as byte count\n\n",
                   name, bytes_str);
        free(bytes_str);
        return nullptr;
    }

    return StringValue(strdup(CacheSizeCheck(bytes) ? "" : "t"));
}

// apply_patch(file, size, init_sha1, tgt_sha1, patch)

Value* ApplyPatchFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc < 6 || (argc % 2) == 1) {
        return ErrorAbort(state, kArgsParsingFailure, "%s(): expected at least 6 args and an "
                                 "even number, got %d", name, argc);
    }

    char* source_filename;
    char* target_filename;
    char* target_sha1;
    char* target_size_str;
    if (ReadArgs(state, argv, 4, &source_filename, &target_filename,
                 &target_sha1, &target_size_str) < 0) {
        return NULL;
    }

#ifdef TARGET_SUPPORTS_AB
    // For A/B targets, The blk_dev in "EMMC:<blk_dev>:...."
    // should be appended with the inactive slot
    std::string copy(source_filename), tmp(slot_suffix_arr[inactive_slot]);
    std::vector<std::string> pieces = android::base::Split(copy, ":");
    // The second "piece" should be the block device
    if (pieces.size() >= 2 && pieces[0] == "EMMC" &&
        strncmp(pieces[1].c_str(), BOOTDEVICE_DIR, strlen(BOOTDEVICE_DIR)) == 0) {
        pieces[1] += tmp;
        copy = android::base::Join(pieces, ':');
        source_filename = strdup(copy.c_str());
        if (source_filename == NULL) {
            printf("%s: strdup() failure at line %d: %s\n",
                    name, __LINE__, strerror(errno));
            return NULL;
        }
    }
    // We do not modify the target filename as that is "-"
    // i.e. same as source filename
    printf("%s: applypatch() will be performed on %s\n", name, source_filename);
#endif

    size_t target_size;
    if (!android::base::ParseUint(target_size_str, &target_size)) {
        ErrorAbort(state, kArgsParsingFailure, "%s(): can't parse \"%s\" as byte count",
                   name, target_size_str);
        free(source_filename);
        free(target_filename);
        free(target_sha1);
        free(target_size_str);
        return nullptr;
    }

    int patchcount = (argc-4) / 2;
    std::unique_ptr<Value*, decltype(&free)> arg_values(ReadValueVarArgs(state, argc-4, argv+4),
                                                        free);
    if (!arg_values) {
        return nullptr;
    }
    std::vector<std::unique_ptr<Value, decltype(&FreeValue)>> patch_shas;
    std::vector<std::unique_ptr<Value, decltype(&FreeValue)>> patches;
    // Protect values by unique_ptrs first to get rid of memory leak.
    for (int i = 0; i < patchcount * 2; i += 2) {
        patch_shas.emplace_back(arg_values.get()[i], FreeValue);
        patches.emplace_back(arg_values.get()[i+1], FreeValue);
    }

    for (int i = 0; i < patchcount; ++i) {
        if (patch_shas[i]->type != VAL_STRING) {
            ErrorAbort(state, kArgsParsingFailure, "%s(): sha-1 #%d is not string", name, i);
            return nullptr;
        }
        if (patches[i]->type != VAL_BLOB) {
            ErrorAbort(state, kArgsParsingFailure, "%s(): patch #%d is not blob", name, i);
            return nullptr;
        }
    }

    std::vector<char*> patch_sha_str;
    std::vector<Value*> patch_ptrs;
    for (int i = 0; i < patchcount; ++i) {
        patch_sha_str.push_back(patch_shas[i]->data);
        patch_ptrs.push_back(patches[i].get());
    }

    int result = applypatch(source_filename, target_filename,
                            target_sha1, target_size,
                            patchcount, patch_sha_str.data(), patch_ptrs.data(), NULL);

    return StringValue(strdup(result == 0 ? "t" : ""));
}

// apply_patch_check(file, [sha1_1, ...])
Value* ApplyPatchCheckFn(const char* name, State* state,
                         int argc, Expr* argv[]) {
    if (argc < 1) {
        return ErrorAbort(state, kArgsParsingFailure, "%s(): expected at least 1 arg, got %d",
                          name, argc);
    }

    char* filename;
    if (ReadArgs(state, argv, 1, &filename) < 0) {
        return NULL;
    }

#ifdef TARGET_SUPPORTS_AB
    // For A/B targets, The blk_dev in "EMMC:<blk_dev>:...."
    // should be appended with the inactive slot.
    std::string copy(filename), tmp(slot_suffix_arr[inactive_slot]);
    std::vector<std::string> pieces = android::base::Split(copy, ":");
    // The second "piece" should be the block device
    if (pieces.size() >= 2 && pieces[0] == "EMMC" &&
        strncmp(pieces[1].c_str(), BOOTDEVICE_DIR, strlen(BOOTDEVICE_DIR)) == 0) {
        pieces[1] += tmp;
        copy = android::base::Join(pieces, ':');
        filename = strdup(copy.c_str());
        if (filename == NULL) {
            printf("%s: strdup() failure at line %d: %s\n",
                    name, __LINE__, strerror(errno));
            return NULL;
        }
    }
    printf("%s: checking %s\n", name, filename);
#endif

    int patchcount = argc-1;
    char** sha1s = ReadVarArgs(state, argc-1, argv+1);

    int result = applypatch_check(filename, patchcount, sha1s);

    int i;
    for (i = 0; i < patchcount; ++i) {
        free(sha1s[i]);
    }
    free(sha1s);

    return StringValue(strdup(result == 0 ? "t" : ""));
}

// This is the updater side handler for ui_print() in edify script. Contents
// will be sent over to the recovery side for on-screen display.
Value* UIPrintFn(const char* name, State* state, int argc, Expr* argv[]) {
    char** args = ReadVarArgs(state, argc, argv);
    if (args == NULL) {
        return NULL;
    }

    std::string buffer;
    for (int i = 0; i < argc; ++i) {
        buffer += args[i];
        free(args[i]);
    }
    free(args);

    buffer += "\n";
    uiPrint(state, buffer);
    return StringValue(strdup(buffer.c_str()));
}

Value* WipeCacheFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc != 0) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects no args, got %d", name, argc);
    }
    fprintf(((UpdaterInfo*)(state->cookie))->cmd_pipe, "wipe_cache\n");
    return StringValue(strdup("t"));
}

Value* RunProgramFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc < 1) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects at least 1 arg", name);
    }
    char** args = ReadVarArgs(state, argc, argv);
    if (args == NULL) {
        return NULL;
    }

    char** args2 = reinterpret_cast<char**>(malloc(sizeof(char*) * (argc+1)));
    memcpy(args2, args, sizeof(char*) * argc);
    args2[argc] = NULL;

    printf("about to run program [%s] with %d args\n", args2[0], argc);

    pid_t child = fork();
    if (child == 0) {
        execv(args2[0], args2);
        printf("run_program: execv failed: %s\n", strerror(errno));
        _exit(1);
    }
    int status;
    waitpid(child, &status, 0);
    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) != 0) {
            printf("run_program: child exited with status %d\n",
                    WEXITSTATUS(status));
        }
    } else if (WIFSIGNALED(status)) {
        printf("run_program: child terminated by signal %d\n",
                WTERMSIG(status));
    }

    int i;
    for (i = 0; i < argc; ++i) {
        free(args[i]);
    }
    free(args);
    free(args2);

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d", status);

    return StringValue(strdup(buffer));
}

// sha1_check(data)
//    to return the sha1 of the data (given in the format returned by
//    read_file).
//
// sha1_check(data, sha1_hex, [sha1_hex, ...])
//    returns the sha1 of the file if it matches any of the hex
//    strings passed, or "" if it does not equal any of them.
//
Value* Sha1CheckFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc < 1) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects at least 1 arg", name);
    }

    std::unique_ptr<Value*, decltype(&free)> arg_values(ReadValueVarArgs(state, argc, argv), free);
    if (arg_values == nullptr) {
        return nullptr;
    }
    std::vector<std::unique_ptr<Value, decltype(&FreeValue)>> args;
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(arg_values.get()[i], FreeValue);
    }

    if (args[0]->size < 0) {
        return StringValue(strdup(""));
    }
    uint8_t digest[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<uint8_t*>(args[0]->data), args[0]->size, digest);

    if (argc == 1) {
        return StringValue(PrintSha1(digest));
    }

    int i;
    uint8_t arg_digest[SHA_DIGEST_LENGTH];
    for (i = 1; i < argc; ++i) {
        if (args[i]->type != VAL_STRING) {
            printf("%s(): arg %d is not a string; skipping",
                    name, i);
        } else if (ParseSha1(args[i]->data, arg_digest) != 0) {
            // Warn about bad args and skip them.
            printf("%s(): error parsing \"%s\" as sha-1; skipping",
                   name, args[i]->data);
        } else if (memcmp(digest, arg_digest, SHA_DIGEST_LENGTH) == 0) {
            break;
        }
    }
    if (i >= argc) {
        // Didn't match any of the hex strings; return false.
        return StringValue(strdup(""));
    }
    // Found a match.
    return args[i].release();
}

// Read a local file and return its contents (the Value* returned
// is actually a FileContents*).
Value* ReadFileFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc != 1) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 1 arg, got %d", name, argc);
    }
    char* filename;
    if (ReadArgs(state, argv, 1, &filename) < 0) return NULL;

    Value* v = static_cast<Value*>(malloc(sizeof(Value)));
    if (v == nullptr) {
        return nullptr;
    }
    v->type = VAL_BLOB;
    v->size = -1;
    v->data = nullptr;

    FileContents fc;
    if (LoadFileContents(filename, &fc) == 0) {
        v->data = static_cast<char*>(malloc(fc.data.size()));
        if (v->data != nullptr) {
            memcpy(v->data, fc.data.data(), fc.data.size());
            v->size = fc.data.size();
        }
    }
    free(filename);
    return v;
}

// write_value(value, filename)
//   Writes 'value' to 'filename'.
//   Example: write_value("960000", "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq")
Value* WriteValueFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc != 2) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 2 args, got %d", name, argc);
    }

    char* value;
    char* filename;
    if (ReadArgs(state, argv, 2, &value, &filename) < 0) {
        return ErrorAbort(state, kArgsParsingFailure, "%s(): Failed to parse the argument(s)",
                          name);
    }

    bool ret = android::base::WriteStringToFile(value, filename);
    if (!ret) {
        printf("%s: Failed to write to \"%s\": %s\n", name, filename, strerror(errno));
    }

    free(value);
    free(filename);
    return StringValue(strdup(ret ? "t" : ""));
}

// Immediately reboot the device.  Recovery is not finished normally,
// so if you reboot into recovery it will re-start applying the
// current package (because nothing has cleared the copy of the
// arguments stored in the BCB).
//
// The argument is the partition name passed to the android reboot
// property.  It can be "recovery" to boot from the recovery
// partition, or "" (empty string) to boot from the regular boot
// partition.
Value* RebootNowFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc != 2) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 2 args, got %d", name, argc);
    }

    char* filename;
    char* property;
    if (ReadArgs(state, argv, 2, &filename, &property) < 0) return NULL;

    char buffer[80];

    // zero out the 'command' field of the bootloader message.
    memset(buffer, 0, sizeof(((struct bootloader_message*)0)->command));
    FILE* f = fopen(filename, "r+b");
    fseek(f, offsetof(struct bootloader_message, command), SEEK_SET);
    ota_fwrite(buffer, sizeof(((struct bootloader_message*)0)->command), 1, f);
    fclose(f);
    free(filename);

    strcpy(buffer, "reboot,");
    if (property != NULL) {
        strncat(buffer, property, sizeof(buffer)-10);
    }

    property_set(ANDROID_RB_PROPERTY, buffer);

    sleep(5);
    free(property);
    ErrorAbort(state, kRebootFailure, "%s() failed to reboot", name);
    return NULL;
}

// Store a string value somewhere that future invocations of recovery
// can access it.  This value is called the "stage" and can be used to
// drive packages that need to do reboots in the middle of
// installation and keep track of where they are in the multi-stage
// install.
//
// The first argument is the block device for the misc partition
// ("/misc" in the fstab), which is where this value is stored.  The
// second argument is the string to store; it should not exceed 31
// bytes.
Value* SetStageFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc != 2) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 2 args, got %d", name, argc);
    }

    char* filename;
    char* stagestr;
    if (ReadArgs(state, argv, 2, &filename, &stagestr) < 0) return NULL;

    // Store this value in the misc partition, immediately after the
    // bootloader message that the main recovery uses to save its
    // arguments in case of the device restarting midway through
    // package installation.
    FILE* f = fopen(filename, "r+b");
    fseek(f, offsetof(struct bootloader_message, stage), SEEK_SET);
    int to_write = strlen(stagestr)+1;
    int max_size = sizeof(((struct bootloader_message*)0)->stage);
    if (to_write > max_size) {
        to_write = max_size;
        stagestr[max_size-1] = 0;
    }
    ota_fwrite(stagestr, to_write, 1, f);
    fclose(f);

    free(stagestr);
    return StringValue(filename);
}

// Return the value most recently saved with SetStageFn.  The argument
// is the block device for the misc partition.
Value* GetStageFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc != 1) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 1 arg, got %d", name, argc);
    }

    char* filename;
    if (ReadArgs(state, argv, 1, &filename) < 0) return NULL;

    char buffer[sizeof(((struct bootloader_message*)0)->stage)];
    FILE* f = fopen(filename, "rb");
    fseek(f, offsetof(struct bootloader_message, stage), SEEK_SET);
    ota_fread(buffer, sizeof(buffer), 1, f);
    fclose(f);
    buffer[sizeof(buffer)-1] = '\0';

    return StringValue(strdup(buffer));
}

Value* WipeBlockDeviceFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc != 2) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects 2 args, got %d", name, argc);
    }

    char* filename;
    char* len_str;
    if (ReadArgs(state, argv, 2, &filename, &len_str) < 0) return NULL;

    size_t len;
    android::base::ParseUint(len_str, &len);
    int fd = ota_open(filename, O_WRONLY, 0644);
    int success = wipe_block_device(fd, len);

    free(filename);
    free(len_str);

    ota_close(fd);

    return StringValue(strdup(success ? "t" : ""));
}

Value* EnableRebootFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc != 0) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects no args, got %d", name, argc);
    }
    UpdaterInfo* ui = (UpdaterInfo*)(state->cookie);
    fprintf(ui->cmd_pipe, "enable_reboot\n");
    return StringValue(strdup("t"));
}

#ifndef USE_LE_MODE
Value* Tune2FsFn(const char* name, State* state, int argc, Expr* argv[]) {
    if (argc == 0) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() expects args, got %d", name, argc);
    }

    char** args = ReadVarArgs(state, argc, argv);
    if (args == NULL) {
        return ErrorAbort(state, kArgsParsingFailure, "%s() could not read args", name);
    }

    char** args2 = reinterpret_cast<char**>(malloc(sizeof(char*) * (argc+1)));
    // Tune2fs expects the program name as its args[0]
    args2[0] = strdup(name);
    for (int i = 0; i < argc; ++i) {
       args2[i + 1] = args[i];
    }
    int result = tune2fs_main(argc + 1, args2);
    for (int i = 0; i < argc; ++i) {
        free(args[i]);
    }
    free(args);

    free(args2[0]);
    free(args2);
    if (result != 0) {
        return ErrorAbort(state, kTune2FsFailure, "%s() returned error code %d",
                          name, result);
    }
    return StringValue(strdup("t"));
}
#endif

#ifdef TARGET_SUPPORTS_AB
/* Checks if a file exists.
   Takes as argument absolute filename path.
 */
Value* CheckIfFileExistsFn(const char* name, State* state,
        int argc, Expr* argv[]) {
    if (argc != 1) {
        return ErrorAbort(state, kArgsParsingFailure,
                "%s() expects exactly 1 arg, got %d", name, argc);
    }
    char *filename;
    if (ReadArgs(state, argv, 1, &filename) < 0) {
        return ErrorAbort(state, kArgsParsingFailure,
                "%s: couldn't parse args!", name);
    }

    if (access(filename, F_OK) != -1) {
        return StringValue(strdup("exists"));
    } else {
        return StringValue(strdup(""));
    }
}

/* Creates an empty file.
   Takes as argument absolute filename path.
 */
Value* CreateFileFn(const char* name, State* state,
        int argc, Expr* argv[]) {
    if (argc != 1) {
        return ErrorAbort(state, kArgsParsingFailure,
                "%s() expects exactly 1 arg, got %d", name, argc);
    }
    char *filename;
    if (ReadArgs(state, argv, 1, &filename) < 0) {
            return ErrorAbort(state, kArgsParsingFailure,
                "%s: couldn't parse args!", name);
    }
    if (make_parents(filename) != 0) {
        printf("%s: Failed to make parents of %s", name, filename);
        return StringValue(strdup(""));
    }

    int fd = open(filename, O_WRONLY | O_CREAT, 0600);
    if (fd != -1) {
        return StringValue(strdup("success"));
    } else {
        return StringValue(strdup(""));
    }
}

/* Low-level block copy from source blk-device
   to target blk-device.
   returns false if any I/O error is encountered.
   Additionally, also compute the SHA1 over the partitions
   (source and destination) and return false if
   they do not match.
 */
bool PerformBlockCopyOperation(char* source, char* dest) {
    int source_fd = open(source, O_RDONLY);
    if (source_fd == -1) {
        printf("PerformBlockCopyOperation: open failed \"%s\": %s\n",
                source, strerror(errno));
        return false;
    }

    int dest_fd = open(dest, O_WRONLY);
    if (dest_fd == -1) {
        printf("PerformBlockCopyOperation: open failed \"%s\": %s\n",
                dest, strerror(errno));
        return false;
    }

    bool success = true;
    ssize_t read;

    // Object to hold the current state of the hash
    SHA_CTX src_ctx, dest_ctx;
    SHA1_Init(&src_ctx);
    uint8_t src_hash[SHA_DIGEST_LENGTH], dest_hash[SHA_DIGEST_LENGTH];

    char* buffer = reinterpret_cast<char*>(malloc(BLOCKSIZE));
    while (success && (read =
            TEMP_FAILURE_RETRY(ota_read(source_fd, buffer, BLOCKSIZE))) > 0) {
        // printf("Read %zd bytes from source_fd\n", read);
        SHA1_Update(&src_ctx, buffer, read); //update sha1 with was just read
        ssize_t wrote = TEMP_FAILURE_RETRY(ota_write(dest_fd, buffer, read));
        // printf("Wrote %zd bytes to target_fd\n", wrote);
        success = success && (wrote == read);
    }

    if (success) {
        SHA1_Final(src_hash, &src_ctx); // finalize src SHA1

        if (ota_fsync(dest_fd) == -1) {
            printf("PerformBlockCopyOperation: fsync of \"%s\" failed: %s\n",
                    dest, strerror(errno));
            success = false;
            goto end;
        }

        // Close and open dest_fd to reset read offset to start
        ota_close(dest_fd);
        dest_fd = open(dest, O_RDONLY);
        if (dest_fd == -1) {
            printf("PerformBlockCopyOperation: SHA1: open failed \"%s\": %s\n",
                    dest, strerror(errno));
            success = false;
            goto end;
        }

        SHA1_Init(&dest_ctx);
        while ((read = TEMP_FAILURE_RETRY(
                ota_read(dest_fd, buffer, BLOCKSIZE))) > 0) {
            SHA1_Update(&dest_ctx, buffer, read);
        }
        SHA1_Final(dest_hash, &dest_ctx); // finalize dest SHA1

        // compare src vs dest SHA1 and return
        success = (memcmp(src_hash, dest_hash, SHA_DIGEST_LENGTH) == 0);
        printf("PerformBlockCopyOperation: src hash: %s\n",
                print_sha1(src_hash).c_str());
        printf("PerformBlockCopyOperation: dest hash: %s\n",
                print_sha1(dest_hash).c_str());
        printf("PerformBlockCopyOperation: src & dest SHA1s %s\n",
                success ? "match": "do not match");
    }

end:
    ota_close(source_fd);
    ota_close(dest_fd);
    return success;
}

/* Copies blocks from active slot of all **A/B**
   partitions to their respective inactive slots.
   Takes as argument a comma-separated list of partitions
   that need to be **excluded** from being copied.
   If no argument is supplied, **all** AB partitions
   are copied from active to inactive slots */
Value* CopyABPartitionsFn(const char* name, State* state,
        int argc, Expr* argv[]) {
    if (argc > 1) {
        // Only expect a max of single argument with space-separated list of partitions.
        return ErrorAbort(state, kArgsParsingFailure,
                "%s() expects <=1 arg, got %d", name, argc);
    }

    char *exclude_arg;
    bool exclude_from_copy = false;
    int exclude_length = 0;
    char partitions_to_exclude[20][PATH_MAX];

    if (argc == 1) {
        if (ReadArgs(state, argv, 1, &exclude_arg) < 0) {
            return ErrorAbort(state, kArgsParsingFailure,
                "%s: couldn't parse args!", name);
        }
        char *p = strtok (exclude_arg, ",");
        while (p != NULL) {
            // append the boot/active slot to the name and then save it
            char buffer[PATH_MAX];
            snprintf(partitions_to_exclude[exclude_length], PATH_MAX, "%s%s",
                    p, slot_suffix_arr[boot_slot]);
            printf("%s: Excluding partition \"%s\" from being copied\n", name, p);
            exclude_length ++;
            p = strtok (NULL, ",");
        }
        if (exclude_length > 0)
            exclude_from_copy = true; // we have partitions that need not be copied
    }

    /* iterate through A/B partitions */

    // open the dir first
    DIR *dir = opendir(BOOTDEVICE_DIR);
    if (dir == NULL) {
        return ErrorAbort(state, kFileOpenFailure,
                "opendir(%s) failed, aborting!", BOOTDEVICE_DIR);
    }

    while (1) {
        struct dirent *de = readdir(dir);
        char active_block_dev_filename[PATH_MAX];
        char inactive_block_dev_filename[PATH_MAX];
        if (de != NULL) {
            if (de->d_name[0] == '.') {
                continue;
            }
            snprintf(active_block_dev_filename, PATH_MAX, "%s/%s",
                    BOOTDEVICE_DIR, de->d_name);
            // printf("Checking whether %s needs to be copied..\n", active_block_dev_filename);

            // stat to check if this is a block-device file
            // copy should be done only on block-devices
            struct stat st;
            stat(active_block_dev_filename, &st);

            if (!S_ISBLK(st.st_mode))
                continue;

            // if the filename doesn't have the boot/active slot
            // as the last 2 chars, ignore copy operation
            const char *suffix = &((de->d_name)[strlen(de->d_name)-2]);
            if (strncmp(suffix, slot_suffix_arr[boot_slot], 2))
                continue;

            if (exclude_from_copy) {
                // there are some partitions that need not be copied.
                // check if the current entry is one of them
                bool match_found = false;
                for (int i = 0; i < exclude_length; i++) {
                    int maxlen = strlen(partitions_to_exclude[i]);
                    // printf("Matching %s against %s\n", de->d_name, partitions_to_exclude[i]);
                    if (strcmp(de->d_name, partitions_to_exclude[i]) == 0) {
                        match_found = true;
                        break;
                    }
                }
                if (match_found)
                    continue; // ignore copy operation
            }
            snprintf(inactive_block_dev_filename, PATH_MAX, "%s/%s",
                    BOOTDEVICE_DIR, de->d_name);
            char *p = strstr(inactive_block_dev_filename,
                              slot_suffix_arr[boot_slot]);
            // p shouldn't be null as we already checked for the suffix earlier
            strncpy(p,  slot_suffix_arr[inactive_slot], 2); // replace the slot

            /* Perform the actual copy */
            printf("%s: Copying from %s to %s\n", name, active_block_dev_filename,
                    inactive_block_dev_filename);
            bool ret = PerformBlockCopyOperation(active_block_dev_filename,
                    inactive_block_dev_filename);
            if (!ret) {
                printf("%s: PerformBlockCopyOperation failed for %s\n", name,
                        active_block_dev_filename);
                // Abort
                return StringValue(strdup(""));
            }
        } else {
            break;
        }
    }
    return StringValue(strdup("success"));
}

/* Calculates SHA1 hash of contents of a  block-device
   and compares against a reference.
   Takes 3 arguments: the block-device name,
   the number of bytes to be read from the block_device
   and the reference SHA1.
 */
Value* BlockDeviceCheckFn(const char* name, State* state,
        int argc, Expr* argv[]) {
    if (argc != 3) {
        return ErrorAbort(state, kArgsParsingFailure,
                "%s() expects exactly 3 arg, got %d", name, argc);
    }
    char *block_dev, *total_read_size, *reference_sha1_str;
    if (ReadArgs(state, argv, 3, &block_dev, &total_read_size,
            &reference_sha1_str) < 0) {
        return ErrorAbort(state, kArgsParsingFailure,
                "%s: couldn't parse args!", name);
    }

    // Check if we were given a block-device or not
    if (strncmp(block_dev, BOOTDEVICE_DIR, strlen(BOOTDEVICE_DIR)) == 0) {
        // append the inactive-slot's suffix to the path
        char buffer[PATH_MAX];
        snprintf(buffer, PATH_MAX, "%s%s", block_dev,
                slot_suffix_arr[inactive_slot]);
        block_dev = strdup(buffer);
        printf("%s: Checking sanity of %s\n", name, block_dev);
    } else {
        printf("%s: Expecting block-device but received something else!\n", name);
        return StringValue(strdup(""));
    }

    // Parse the incoming SHA1
    uint8_t reference_sha1[SHA_DIGEST_LENGTH];
    if (ParseSha1(reference_sha1_str, reference_sha1) != 0) {
        printf("%s: failed to parse sha1 \"%s\"\n", name, reference_sha1_str);
        return StringValue(strdup(""));
    }

    printf("%s: reference SHA1: %s\n", name,
            print_sha1(reference_sha1).c_str());

    int block_dev_fd = open(block_dev, O_RDONLY);
    if (block_dev_fd == -1) {
        printf("%s: open failed \"%s\": %s\n", name,
                block_dev, strerror(errno));
        return StringValue(strdup(""));
    }

    // Object to hold the current state of the hash
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    uint8_t block_dev_sha1[SHA_DIGEST_LENGTH];
    char* buffer = reinterpret_cast<char*>(malloc(BLOCKSIZE));
    size_t to_read = atoll(total_read_size), so_far = 0;
    size_t read = (size_t)min(BLOCKSIZE, to_read - so_far);

    while (so_far < to_read) {
        ssize_t read_count =
                TEMP_FAILURE_RETRY(ota_read(block_dev_fd, buffer, read));
        so_far += read_count;
        read = (size_t)min(BLOCKSIZE, to_read - so_far);
        SHA1_Update(&ctx, buffer, read_count); // update SHA1 with current buffer
    }
    SHA1_Final(block_dev_sha1, &ctx); //Finalize the SHA1

    printf("%s: block device SHA1: %s\n", name,
            print_sha1(block_dev_sha1).c_str());

    // Now compare this against the reference SHA1
    if (memcmp(block_dev_sha1, reference_sha1, SHA_DIGEST_LENGTH) == 0) {
        printf("%s: The block device SHA1 matches the reference SHA1\n", name);
        return StringValue(strdup("success"));
    }

    printf("%s: The block device SHA1 doesn't match the reference SHA1\n", name);
    return StringValue(strdup(""));
}

Value* SetInactiveAsUnbootableFn(const char* name, State* state,
        int argc, Expr* argv[]) {
    if (argc != 0) {
        return ErrorAbort(state, kArgsParsingFailure,
                "%s() expects no args, got %d", name, argc);
    }

    printf("%s: Setting inactive_slot(%s) as unbootable\n", name,
            slot_suffix_arr[inactive_slot]);

    int ret = libabctl_setUnbootable(inactive_slot);

    if (ret == 0) {
        printf("%s: %s set as unbootable successfully\n", name,
                slot_suffix_arr[inactive_slot]);
        return StringValue(strdup("Success"));
    } else {
        printf("%s: Couldn't set inactive slot as unbootable!\n", name);
        return StringValue(strdup("")); // abort, if you want
    }
}

Value* SetInactiveSlotAsActiveFn(const char* name, State* state,
        int argc, Expr* argv[]) {
    if (argc != 0) {
        return ErrorAbort(state, kArgsParsingFailure,
                "%s() expects no args, got %d", name, argc);
    }

    printf("%s: Setting inactive_slot(%s) as active\n", name,
            slot_suffix_arr[inactive_slot]);

    int ret = libabctl_setActive(inactive_slot);

    if (ret == 0) {
        // Check again if it is actually set as active
        ret = libabctl_getActiveStatus(inactive_slot);
        if (ret == 1) { // slot is active
            printf("%s: Set %s as active slot successfully\n", name,
                    slot_suffix_arr[inactive_slot]);
            return StringValue(strdup("Success"));
        } else {
            printf("That's weird! setActive() didn't return any errors "
                   "but looks like the active status is not set..\n");
        }
    }

    printf("%s: Couldn't set inactive slot as active!\n", name);
    return StringValue(strdup("")); // abort, if you want
}
#endif

void RegisterInstallFunctions() {
    RegisterFunction("mount", MountFn);
    RegisterFunction("is_mounted", IsMountedFn);
    RegisterFunction("unmount", UnmountFn);
    RegisterFunction("format", FormatFn);
    RegisterFunction("show_progress", ShowProgressFn);
    RegisterFunction("set_progress", SetProgressFn);
    RegisterFunction("delete", DeleteFn);
    RegisterFunction("delete_recursive", DeleteFn);
    RegisterFunction("package_extract_dir", PackageExtractDirFn);
    RegisterFunction("package_extract_file", PackageExtractFileFn);
    RegisterFunction("symlink", SymlinkFn);

    // Maybe, at some future point, we can delete these functions? They have been
    // replaced by perm_set and perm_set_recursive.
    RegisterFunction("set_perm", SetPermFn);
    RegisterFunction("set_perm_recursive", SetPermFn);

    // Usage:
    //   set_metadata("filename", "key1", "value1", "key2", "value2", ...)
    // Example:
    //   set_metadata("/system/bin/netcfg", "uid", 0, "gid", 3003, "mode", 02750, "selabel", "u:object_r:system_file:s0", "capabilities", 0x0);
    RegisterFunction("set_metadata", SetMetadataFn);

    // Usage:
    //   set_metadata_recursive("dirname", "key1", "value1", "key2", "value2", ...)
    // Example:
    //   set_metadata_recursive("/system", "uid", 0, "gid", 0, "fmode", 0644, "dmode", 0755, "selabel", "u:object_r:system_file:s0", "capabilities", 0x0);
    RegisterFunction("set_metadata_recursive", SetMetadataFn);

    RegisterFunction("getprop", GetPropFn);
    RegisterFunction("file_getprop", FileGetPropFn);
    RegisterFunction("write_raw_image", WriteRawImageFn);

    RegisterFunction("apply_patch", ApplyPatchFn);
    RegisterFunction("apply_patch_check", ApplyPatchCheckFn);
    RegisterFunction("apply_patch_space", ApplyPatchSpaceFn);

    RegisterFunction("wipe_block_device", WipeBlockDeviceFn);

    RegisterFunction("read_file", ReadFileFn);
    RegisterFunction("sha1_check", Sha1CheckFn);
    RegisterFunction("rename", RenameFn);
    RegisterFunction("write_value", WriteValueFn);

    RegisterFunction("wipe_cache", WipeCacheFn);

    RegisterFunction("ui_print", UIPrintFn);

    RegisterFunction("run_program", RunProgramFn);

    RegisterFunction("reboot_now", RebootNowFn);
    RegisterFunction("get_stage", GetStageFn);
    RegisterFunction("set_stage", SetStageFn);

    RegisterFunction("enable_reboot", EnableRebootFn);
#ifndef USE_LE_MODE
    RegisterFunction("tune2fs", Tune2FsFn);
#endif

#ifdef TARGET_SUPPORTS_AB
    RegisterFunction("if_copy_done_cookie_exists", CheckIfFileExistsFn);
    RegisterFunction("write_copy_done_cookie", CreateFileFn);
    RegisterFunction("delete_copy_done_cookie", DeleteFn);
    RegisterFunction("copy_all_source_partitions_except", CopyABPartitionsFn);
    RegisterFunction("block_device_check", BlockDeviceCheckFn);
    RegisterFunction("set_inactive_slot_as_unbootable", SetInactiveAsUnbootableFn);
    RegisterFunction("set_inactive_slot_as_active", SetInactiveSlotAsActiveFn);
#endif
}
