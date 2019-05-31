/*
 * Copyright (c) 2015,2017 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <linux/major.h>
#include <mtd/ubi-user.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/ioctl.h>

#define PACKED __attribute__((packed, aligned(1)))
#define PACKED_POST __attribute__((packed, aligned(1)))

#include "common_v01.h"
#include "qmi_idl_lib.h"
#include "qmi_client.h"
#include "flash_driver_service_v01.h"

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

#define fs_gettid() (int)syscall(__NR_gettid)

#ifdef USE_GLIB
	#include <glib.h>
	#define strlcpy g_strlcpy
#endif

#ifdef FS_SCRUB_ANDROID_BUILD
#include "cutils/log.h"
#include "common_log.h"

#define SCRUB_LOGE(fmt, ...) LOGE("%d:%d Error: %s:%u " fmt, getpid(), fs_gettid(), __func__, __LINE__, ## __VA_ARGS__)
#define SCRUB_LOGI(fmt, ...) LOGI("%d:%d Info: %s:%u " fmt, getpid(), fs_gettid(), __func__, __LINE__,  ## __VA_ARGS__)
#define SCRUB_LOGV(fmt, ...) LOGV("%d:%d Verbose:%s:%u " fmt, getpid(), fs_gettid(), __func__, __LINE__, ## __VA_ARGS__)

#else
#include <sys/syslog.h>
#define SCRUB_LOGE(fmt, ...) syslog(LOG_ERR, "%d:%d Error: %s:%u " fmt, getpid(), fs_gettid(), __func__, __LINE__, ## __VA_ARGS__)
#define SCRUB_LOGI(fmt, ...) syslog(LOG_INFO, "%d:%d Info: %s:%u " fmt, getpid(), fs_gettid(), __func__, __LINE__,  ## __VA_ARGS__)
#define SCRUB_LOGV(fmt, ...) syslog(LOG_NOTICE, "%d:%d Verbose:%s:%u " fmt, getpid(), fs_gettid(), __func__, __LINE__, ## __VA_ARGS__)

// For debug
// #define SCRUB_LOGE(fmt, ...) printf("\n%d:%d Error: %s:%u " fmt "\n", getpid(), fs_gettid(), __func__, __LINE__, ## __VA_ARGS__)
// #define SCRUB_LOGI(fmt, ...) printf("\n%d:%d Info: %s:%u " fmt "\n", getpid(), fs_gettid(), __func__, __LINE__,  ## __VA_ARGS__)
// #define SCRUB_LOGV(fmt, ...) printf("\n%d:%d Verbose:%s:%u " fmt "\n", getpid(), fs_gettid(), __func__, __LINE__, ## __VA_ARGS__)
#endif


#define fs_scrub_thread_return()           \
          do {                             \
            pthread_exit((void *)NULL);    \
            return NULL;                   \
          } while (0)

#define fs_scrub_exit(fmt, ...)                 \
          do {                                  \
            SCRUB_LOGE(fmt, ## __VA_ARGS__);    \
            SCRUB_LOGE("FATAL ERROR EXITING!"); \
            exit(-1);                           \
          } while (0)


#define fs_scrub_thread_exit(fmt, ...)           \
          do {                                   \
            SCRUB_LOGE(fmt, ## __VA_ARGS__);     \
            SCRUB_LOGE("ERROR EXITING THREAD!"); \
            pthread_exit((void *)NULL);          \
          } while (0)

#ifdef FS_SCRUB_ANDROID_BUILD
#define SCRUB_DATA_DIR_PREFIX  "/persist"
#else
#define SCRUB_DATA_DIR_PREFIX  "/data"
#endif

#define SCRUB_DATA_DIR  SCRUB_DATA_DIR_PREFIX "/fs_scrub"

#define SCRUB_FILE_DATA_FILE SCRUB_DATA_DIR "/scrub_state"
#define SCRUB_TOKEN_FILE SCRUB_DATA_DIR "/last_token_received"

#define SCRUB_CORRUPTED_DIR  SCRUB_DATA_DIR "/corrupted"
#define SCRUB_CORRUPTED_FILE  SCRUB_CORRUPTED_DIR "/%s"

#define UBI_SYSFS_DIR_PREFIX  "ubi"

#define UBI_SYSFS_DIR	"/sys/class/ubi"
#define UBI_MAX_SQNUM	UBI_SYSFS_DIR "/ubi%u/scrub_max_sqnum"
#define UBI_SCRUB_ALL	UBI_SYSFS_DIR "/ubi%u/scrub_all"

#define FS_SCRUB_STATE_MAGIC  0x42524353 /* "SCRB" */
#define FS_SCRUB_STATE_VERSION 1

#define FS_SCRUB_STATE_MAX_ENTRIES 10

#define FS_SCRUB_DIR_MODE 0700

#define SCRUB_SLEEP_BEFORE_RESUME_TIME 10 /* 10 seconds sleep */
#define SCRUB_SLEEP_BEFORE_RETRY_TIME 2 /* 2 seconds sleep */

#define SCRUB_QMI_SERVICE_INSTANCE_ID 0
#define FS_SCRUB_QMI_WAIT_FOREVER 0

/*
 * Reserved this section for ubi-attach
 */

/* UBI control character device */
#define UBI_CTRL_DEV "/dev/ubi_ctrl"

struct scrub_extended_partitions_info {
	const char *name;
	const char *path;
	int8 status;
	int32 ubi_num;
};

struct scrub_extended_partitions_info ext_parts[] = {
	{
	 .name = "recoveryfs",
	 /* path of recoveryfs mtd partition from start_fs-scrub-daemon script */
	 .path = "/tmp/recoveryfs-mtd",
	 .status = 0,
	 .ubi_num = -1
	},
};

static int fs_scrub_remove_ubi_partition(int ubi_num)
{
	int fd, ret;

	fd = open(UBI_CTRL_DEV, O_RDONLY);
	if (fd == -1) {
		SCRUB_LOGE("failed to open %s", UBI_CTRL_DEV);
		return -1;
	}
	ret = ioctl(fd, UBI_IOCDET, &ubi_num);
	if (ret)
		SCRUB_LOGE("error occured while removing ubi device %d", ubi_num);

	close(fd);
	return ret;
}

int fs_scrub_ubi_detach(void)
{
	int i, ret, err = 0;
	int len = sizeof(ext_parts)/sizeof(struct scrub_extended_partitions_info);

	SCRUB_LOGI("Starting ubi_detach for partitions(%d)\n", len);

	for (i = 0; i < len; i++) {
		if (!ext_parts[i].status || ext_parts[i].ubi_num == -1)
			continue;

		SCRUB_LOGI("Detaching(%s) ubi partition(%d)\n",
				   ext_parts[i].name, ext_parts[i].ubi_num);
		ret = fs_scrub_remove_ubi_partition(ext_parts[i].ubi_num);
		if (ret)
			err = ret;
	}
	return err;
}

static int fs_scrub_attach_ubi_partition(struct ubi_attach_req *ubi_req)
{
	int fd, ret;

	fd = open(UBI_CTRL_DEV, O_RDONLY);
	if (fd == -1) {
		SCRUB_LOGE("cannot open %s", UBI_CTRL_DEV);
		return -1;
	}

	ret = ioctl(fd, UBI_IOCATT, ubi_req);
	if (ret)
		SCRUB_LOGE("error occured while attaching mtd partition(%d) to ubi",
				   ubi_req->mtd_num);

	close(fd);
	return ret;
}

static int fs_scrub_mtd_path_to_mtd_num(const char *mtd_path)
{
	struct stat mtd_stat;
	int major, minor;
	int ret;

	ret = stat(mtd_path, &mtd_stat);
	if (ret < 0) {
		SCRUB_LOGE("%s: cannot stat %s\n", mtd_path);
		goto out;
	}

	if (!S_ISCHR(mtd_stat.st_mode)) {
		ret = -EINVAL;
		SCRUB_LOGE("%s dev not char device\n", mtd_path);
		goto out;
	}

	major = major(mtd_stat.st_rdev);
	minor = minor(mtd_stat.st_rdev);
	if (major != MTD_CHAR_MAJOR) {
		ret = -EINVAL;
		SCRUB_LOGE("%s dev not an MTD device\n", mtd_path);
		goto out;
	}

	ret = minor / 2;
out:
	return ret;
}

static int fs_scrub_ubi_set_default_args(struct ubi_attach_req *ubi_req,
								 const char *path)
{
	int ret;
	ubi_req->ubi_num = UBI_DEV_NUM_AUTO;
	ubi_req->vid_hdr_offset = 0;
	ubi_req->max_beb_per1024 = 0;
	ret = fs_scrub_mtd_path_to_mtd_num(path);
	if (ret < 0)
		return ret;
	ubi_req->mtd_num = ret;
	return ret;
}

int fs_scrub_ubi_attach(void)
{
	struct ubi_attach_req ubi_req;
	int ret, i, err = 0;
	int len = sizeof(ext_parts)/sizeof(struct scrub_extended_partitions_info);

	SCRUB_LOGI("Starting ubi_attach for partitions(%d)\n", len);

	for (i = 0; i < len; i++) {
		memset(&ubi_req, 0, sizeof(struct ubi_attach_req));
		ret = fs_scrub_ubi_set_default_args(&ubi_req, ext_parts[i].path);
		if (ret < 0) {
			err = -1;
			continue;
		}

		SCRUB_LOGI("Attaching(%s) mtd partition(%d)\n",
				   ext_parts[i].name, ubi_req.mtd_num);
		ret = fs_scrub_attach_ubi_partition(&ubi_req);
		if (ret == 0) {
			SCRUB_LOGI("Successfully attached ubidevice(%d)\n",
					   ubi_req.ubi_num);
			ext_parts[i].status = 1;
			ext_parts[i].ubi_num = ubi_req.ubi_num;
		} else {
			SCRUB_LOGE("Failed attaching ubidevice(%d), error(%d)\n",
					   ubi_req.ubi_num, ret);
		}
	}

	return err;
}

static void *fs_scrub_handle_ssr(void *arg);
static void *fs_scrub_handle_scrub_trigger(void *arg);

PACKED struct fs_scrub_state_entry {
	uint32 ubi_dev_num;
	uint64 scrub_sqnum;
	uint8  scrub_is_complete;
	uint8  is_valid;
} PACKED_POST;

PACKED struct fs_scrub_state_file_data {
	uint32 magic1;
	uint32 version;
	uint32 file_token;
	uint32 valid_entry_count;
	struct fs_scrub_state_entry entry[FS_SCRUB_STATE_MAX_ENTRIES];
	uint32 magic2;
} PACKED_POST;

struct fs_scrub_handle {
	qmi_client_type server;
	pthread_mutex_t scrub_mutex;
	pthread_mutex_t qmi_init_mutex;
	pthread_mutex_t token_mutex;
	uint32 current_token;
	int is_token_valid;
	struct fs_scrub_state_file_data file_data;
	int is_file_data_valid;
	int is_inited;
};

static struct fs_scrub_handle scrub_handle;

static void fs_scrub_remove_file(char *path)
{
	int result;
	size_t size_val;
	uint32 index, path_len;
	char corrupted_file[100];
	char full_filepath[100];
	char cmd[100];


	size_val = strlcpy(full_filepath, path, sizeof(full_filepath));
	if (size_val >= sizeof(full_filepath))
		fs_scrub_exit("strlcpy failed");

	path_len = strlen(full_filepath);

	for (index = path_len - 1; index > 0; index--) {
		if (full_filepath[index] == '/') {
			index++;
			break;
		}
	}

	result = snprintf(corrupted_file, sizeof(corrupted_file),
				SCRUB_CORRUPTED_FILE, &full_filepath[index]);
	if (result < 0 || (uint32)result >= sizeof(corrupted_file))
		fs_scrub_exit("snprintf for SCRUB_CORRUPTED_FILE file failed");

	SCRUB_LOGE("Moving %s to %s", full_filepath, corrupted_file);

	result = snprintf(cmd, sizeof(cmd), "mv  --force %s %s",
				full_filepath, corrupted_file);
	if (result < 0 || (uint32)result >= sizeof(cmd))
		fs_scrub_exit("snprintf for SCRUB_CORRUPTED_FILE file failed");

	system(cmd);
	unlink(full_filepath);
}

static int32 os_mkdir(const char *path, uint16 mode)
{
	int32 result;

	/* This mode when used is modified by umask */
	result = mkdir(path, mode);

	if (result != 0)
		result = -(errno);
	return result;
}

static int32 fs_scrub_autodir(const char *path)
{
	int result = 0;
	uint32 index = 0;
	uint32 path_len = 0;
	char *temp_path_buf;
	uint32 temp_path_buf_size;
	uint16_t mode = FS_SCRUB_DIR_MODE;
	int32 gid = -1;

	if (path == NULL)
		return -EINVAL;

	path_len = strlen(path);

	if (path_len == 0)
		return -EINVAL;

	temp_path_buf_size = path_len + 1;
	temp_path_buf = malloc (temp_path_buf_size);
	if (temp_path_buf == NULL)
		return -ENOMEM;

	strlcpy(temp_path_buf, path, temp_path_buf_size);
	/* Set index to path_len - 1 as there is at least one char in the path.
	 * and then go backwards in the path and try to create the directories.
	 * We exit when the path runs out or we create one directory
	 * successfully or if an error is encountered.
	 */

	for (index = path_len - 1; index > 0; index--) {
		if (path[index] == '/') {
			temp_path_buf[index] = 0x00;
			/* Attempt to make subdirectories */
			result = os_mkdir(temp_path_buf, mode);

			/* Account for broken symbolic links. */
			if (result == -EEXIST)
				result = 0;

			/* Exit if we are successful in creating a directory.
			 */
			if (result == 0) {
				if (gid != -1)
					chown(temp_path_buf, -1, gid);
				break;
			}
			/* Exit if we encounter an error other than ENOENT.
			 * ENOENT implies the parent directory of the directory
			 * we tried to create doesn't exist so we try to create
			 * that directory. Any other error means we must exit.
			 */
			else if (result != -ENOENT)
				break;
		}
	}

	if (result == 0) {
		/* If the current index was a '/' put it back. */
		if (path[index] == '/')
			temp_path_buf[index++] = '/';

		for (/* Current index value */; index < path_len; index++) {
			/* Go forward up the path until we reach the end */
			if (path[index] == '/') {
				/* Attempt to make subdirectories */
				result = os_mkdir(temp_path_buf, mode);

				/* If we fail to create a directory and it
				 * doesn't exist we exit. */
				if (result != 0) {
					/* The directory exists we can proceed */
					if (result == -EEXIST)
						result = 0;
					else
						break;
				}
				else {
					if (gid != -1)
						chown(temp_path_buf, -1, gid);
				}

				temp_path_buf[index] = '/';
			}
		}
	}

	if (result != 0)
		SCRUB_LOGE("Failed to create  dirs for(%s) errno = %d (%s)", path,
		      result, strerror(-result));

	free(temp_path_buf);
	return result;
}

static int string_to_uint64(uint64 *output, char *string)
{
	char *end;
	unsigned long long value;

	value = strtoull(string, &end, 0);

	if (*end != 0)
		return -1;

	if ((errno == ERANGE && value == ULLONG_MAX) || (value > UINT64_MAX))
		return -1;

	*output = (uint64)value;
	return 0;
}

static int get_ubi_dev_num(uint32 *dev_num, char *dir_name)
{
	int index, result;
	uint64 output;
	index = strlen(UBI_SYSFS_DIR_PREFIX);

	result = string_to_uint64(&output, &dir_name[index]);
	if (result != 0)
		return -1;

	if(output > UINT32_MAX)
		return -1;

	*dev_num = (uint32_t)output;
	return 0;
}

static int read_max_sqnum(uint32 dev_num, uint64 *max_sqnum)
{
	FILE *fd;
	char max_sqnum_string[25];
	char file[100];
	char *fgets_ret;
	uint32 str_len;
	int result;

	result = snprintf(file, sizeof(file), UBI_MAX_SQNUM, dev_num);
	if (result < 0 || (uint32)result >= sizeof(file))
		fs_scrub_exit("snprintf for UBI_MAX_SQNUM file failed");

	fd = fopen(file, "r");
	if (!fd) {
		SCRUB_LOGE("Unable to open file %s\n", file);
		return -1;
	}
	fgets_ret = fgets(max_sqnum_string, sizeof(max_sqnum_string), fd);
	fclose(fd);

	if (!fgets_ret) {
		SCRUB_LOGE("fgets failed with %d [%s] for %s\n", errno,
			strerror(errno), file);
		return -1;
	}
	str_len = strlen(max_sqnum_string);
	if(str_len > 0)
		max_sqnum_string[str_len - 1] = 0;

	result = string_to_uint64(max_sqnum, max_sqnum_string);
	if (result != 0)
		return -1;
	return 0;
}

static int setup_scrub_for_dev(struct fs_scrub_state_entry *entry,
				uint32 dev_num)
{
	int result;
	uint64 max_sqnum;

	result = read_max_sqnum(dev_num, &max_sqnum);
	if (result != 0)
		return -1;
	entry->ubi_dev_num = dev_num;
	entry->scrub_sqnum = max_sqnum;
	entry->scrub_is_complete = 0;
	entry->is_valid = 1;

	SCRUB_LOGV("Setup Scrub ubi dev %u with sqnum %llu", dev_num, max_sqnum);
	return 0;
}

static int fs_scrub_init_state(struct fs_scrub_state_file_data *file_data)
{
	DIR *ubidir = NULL;
	struct dirent *ubidirent = NULL;
	struct fs_scrub_state_entry *entry = file_data->entry;
	int result;
	uint32 dev_count = 0;
	int ret;

	size_t ubi_sysfs_dir_prefix_len;
	uint32 dev_num;

	ret = fs_scrub_ubi_attach();
	if (ret)
		SCRUB_LOGE("Failed to attach some additional ubi partitions. Continue with existing partitions scrubbing...");

	SCRUB_LOGV("Initializing a new scrub");
	memset(file_data, 0, sizeof(*file_data));
	file_data->magic1 = FS_SCRUB_STATE_MAGIC;
	file_data->magic2 = FS_SCRUB_STATE_MAGIC;
	file_data->version = FS_SCRUB_STATE_VERSION;

	ubidir = opendir(UBI_SYSFS_DIR);
	if(!ubidir) {
		SCRUB_LOGE("open dir failed on %s, errno = %d [%s]\n", UBI_SYSFS_DIR,
			errno, strerror(errno));
		fs_scrub_ubi_detach();
		return -1;
	}

	ubi_sysfs_dir_prefix_len = strlen(UBI_SYSFS_DIR_PREFIX);

	/* Parse through every UBI device under /sys/class/ubi */
	while((ubidirent = readdir(ubidir))) {
		if(strlen(ubidirent->d_name) <= ubi_sysfs_dir_prefix_len)
			continue;

		result = strncasecmp(ubidirent->d_name, UBI_SYSFS_DIR_PREFIX,
					ubi_sysfs_dir_prefix_len);

		if(result != 0)
			continue;

		result = get_ubi_dev_num(&dev_num, ubidirent->d_name);
		if(result != 0)
			continue;

		SCRUB_LOGV("Found ubi device number %u", dev_num);
		result = setup_scrub_for_dev(entry, dev_num);
		if(result != 0)
			continue;

		entry++;
		dev_count++;
		if (dev_count >=  FS_SCRUB_STATE_MAX_ENTRIES){
			SCRUB_LOGE("Found max supported entries %u break",dev_count);
			break;
		}
	}
	closedir(ubidir);
	if (dev_count)
		file_data->valid_entry_count = dev_count;
	return 0;
}

static void fs_scrub_write_data_to_file(char *file, void *buffer, int32 size)
{
	int fd, result;
	char tmp_file[100];
	int32 remaining_bytes;

	result = snprintf(tmp_file, sizeof(tmp_file), "%s.tmp", file);
	if (result < 0 || (uint32)result >= sizeof(tmp_file))
		fs_scrub_exit("snprintf for tmp file failed %s", file);

	result = fs_scrub_autodir(tmp_file);
	if (result != 0)
		fs_scrub_exit("Dir_create failed %s", tmp_file);

	fd = open(tmp_file, O_CREAT|O_TRUNC|O_WRONLY, 0600);
	if (fd < 0)
		fs_scrub_exit("failed to open file %s error %d[%s]", tmp_file,
				errno, strerror(errno));

	remaining_bytes = size;
	while (remaining_bytes > 0) {
		result = write(fd, buffer, remaining_bytes);
		if (result <= 0)
			fs_scrub_exit("Write to %s failed error %d[%s]",
					tmp_file, errno, strerror(errno));
		remaining_bytes -= result;
	}

	result = fdatasync(fd);
	if (result != 0)
		fs_scrub_exit("fdatasync to %s failed error %d[%s]",
				tmp_file, errno, strerror(errno));
	close(fd);
	result = rename(tmp_file, file);
	if (result != 0)
		fs_scrub_exit("rename of %s to %s failed error %d[%s]",
				tmp_file, file, errno, strerror(errno));
}

static int fs_scrub_mutex_init (pthread_mutex_t *hdl_ptr)
{
  int result = 0;
  result = pthread_mutex_init (hdl_ptr, NULL);
  return result;
}

static int fs_scrub_lock (pthread_mutex_t *hdl_ptr)
{
  int result = 0;
  result = pthread_mutex_lock (hdl_ptr);
  return result;
}

static int fs_scrub_unlock (pthread_mutex_t *hdl_ptr)
{
  int result = 0;
  result = pthread_mutex_unlock (hdl_ptr);
  return result;
}

static int fs_scrub_thread_create(void* (*thread_main)(void *), void *args)
{
  int result;
  pthread_attr_t pthread_attr;
  pthread_t thread;

  result = pthread_attr_init(&pthread_attr);
  if (result != 0)
  {
    return result;
  }

  result = pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);
  if (result != 0)
  {
    return result;
  }

  result = pthread_create (&thread, &pthread_attr, thread_main, args);

  (void) pthread_attr_destroy (&pthread_attr);
  return result;
}

static void fs_scrub_qmi_indication_cb(qmi_client_type user_handle,
				       unsigned int msg_id, void *ind_buf,
				       unsigned int ind_buf_len,
				       void *ind_cb_data)
{
	int ret_val;
	uint32 scrub_token;
	qmi_client_error_type qmi_ret;
	fds_scrub_apps_start_scrub_ind_msg_v01 scrub_indication;

	SCRUB_LOGE("MSGID=%d", msg_id);

	if (msg_id != QMI_FDS_SCRUB_APPS_START_SCRUB_IND_V01) {
		SCRUB_LOGE("Unsupported indication");
		return;
	}

	memset(&scrub_indication, 0, sizeof(scrub_indication));

	qmi_ret = qmi_client_message_decode(user_handle, QMI_IDL_INDICATION,
					    msg_id, ind_buf, ind_buf_len,
					    &scrub_indication,
					    sizeof(scrub_indication));
	if (qmi_ret != QMI_NO_ERR) {
		SCRUB_LOGE("Indication decode failed with error = %d!", qmi_ret);
		return;
	}

	scrub_token = scrub_indication.scrub_token;

	SCRUB_LOGE("Indication for scrub! token = %u!", scrub_token);

	ret_val = fs_scrub_thread_create(fs_scrub_handle_scrub_trigger,
					 (void *)(uintptr_t)scrub_token);
	if (ret_val != 0)
		fs_scrub_exit("Thread create failed when handling indication");
}

static void fs_scrub_qmi_error_cb(qmi_client_type user_handle,
				   qmi_client_error_type error,
				   void *err_cb_data)
{
	int ret_val;

	SCRUB_LOGE("SSR detected!");
	ret_val = fs_scrub_thread_create(fs_scrub_handle_ssr, NULL);
	if (ret_val != 0)
		fs_scrub_exit("Thread create failed when handling SSR");
}

static int fs_scrub_qmi_init(qmi_client_type *server)
{
  qmi_client_error_type qmi_ret;
  qmi_client_os_params os_params;
  qmi_idl_service_object_type service_object;
  fds_indication_register_req_msg_v01 indication_register_req;
  fds_indication_register_resp_msg_v01 indication_register_resp;
  int timeout_ms = FS_SCRUB_QMI_WAIT_FOREVER;

  service_object = fds_get_service_object_v01();
  if (!service_object) {
    SCRUB_LOGE("fds_get_service_object_v01 Failed!");
    return -1;
  }

  /* Wait for ever if the modem service is not up */
  /* TODO: consider a timeout of 1 minute? */
  qmi_ret = qmi_client_init_instance(service_object,
				     SCRUB_QMI_SERVICE_INSTANCE_ID,
				     fs_scrub_qmi_indication_cb, NULL,
				     &os_params,
                                     timeout_ms, server);

  if (qmi_ret != QMI_NO_ERR) {
    SCRUB_LOGE("qmi_client_init_instance error = %d!", qmi_ret);
    return -1;
  }

  qmi_ret = qmi_client_register_error_cb(*server, fs_scrub_qmi_error_cb, NULL);
  if (qmi_ret != QMI_NO_ERR) {
    SCRUB_LOGE("qmi_client_register_error_cb error = %d!", qmi_ret);
    return -1;
  }

  memset (&indication_register_req, 0, sizeof (indication_register_req));
  memset (&indication_register_resp, 0, sizeof (indication_register_resp));

  indication_register_resp.resp.result = QMI_RESULT_FAILURE_V01;

  indication_register_req.need_apps_start_scrub_indication_valid = 1;
  indication_register_req.need_apps_start_scrub_indication = 1;

  qmi_ret = qmi_client_send_msg_sync(*server,
				     QMI_FDS_INDICATION_REGISTER_REQ_V01,
				     &indication_register_req,
				     sizeof(indication_register_req),
				     &indication_register_resp,
				     sizeof(indication_register_resp),
				     FS_SCRUB_QMI_WAIT_FOREVER);
  if (qmi_ret != QMI_NO_ERR) {
    SCRUB_LOGE("Failed to register indication! error = %d!", qmi_ret);
    return -1;
  }

  SCRUB_LOGV("QMI init done!");
  return 0;
}

static void fs_scrub_qmi_cleanup(qmi_client_type *server)
{
  (void)qmi_client_release(*server);
}

static void fs_scrub_disconnect_from_server(void)
{
	int ret_val;

	ret_val = fs_scrub_lock(&scrub_handle.qmi_init_mutex);
	if (ret_val != 0)
		fs_scrub_exit("qmi_init_mutex lock failed");

	if (scrub_handle.is_inited == 1)
		fs_scrub_qmi_cleanup(&scrub_handle.server);

	scrub_handle.is_inited = 0;

	ret_val = fs_scrub_unlock(&scrub_handle.qmi_init_mutex);
	if (ret_val != 0)
		fs_scrub_exit("qmi_init_mutex unlock failed");
}

static void fs_scrub_connect_to_server(void)
{
	int ret_val;

	ret_val = fs_scrub_lock(&scrub_handle.qmi_init_mutex);
	if (ret_val != 0)
		fs_scrub_exit("qmi_init_mutex lock failed");

	if (scrub_handle.is_inited == 1)
		fs_scrub_qmi_cleanup(&scrub_handle.server);

	fs_scrub_qmi_init(&scrub_handle.server);
	if (ret_val != 0)
		fs_scrub_exit("QMI init failed");

	scrub_handle.is_inited = 1;

	ret_val = fs_scrub_unlock(&scrub_handle.qmi_init_mutex);
	if (ret_val != 0)
		fs_scrub_exit("qmi_init_mutex unlock failed");
}

static void *fs_scrub_handle_ssr(void *arg)
{
	fs_scrub_connect_to_server();
	fs_scrub_thread_return();
}


static void
fs_scrub_commit_file_data(struct fs_scrub_state_file_data *file_data)
{

	fs_scrub_write_data_to_file(SCRUB_FILE_DATA_FILE, file_data,
					sizeof(*file_data));
}

static void scrub_ubi_dev(uint32 dev_num, uint64 sqnum)
{
	int result, fd, sqnum_string_len;
	char file[100];
	char sqnum_string[25];

	result = snprintf(file, sizeof(file), UBI_SCRUB_ALL, dev_num);
	if (result < 0 || (uint32)result >= sizeof(file))
		fs_scrub_exit("snprintf for UBI_SCRUB_ALL file failed");

	sqnum_string_len = snprintf(sqnum_string, sizeof(sqnum_string), "%llu\n", sqnum);
	if (sqnum_string_len < 0 || (uint32)sqnum_string_len >= sizeof(sqnum_string))
		fs_scrub_exit("snprintf for sqnum_string failed");

	fd = open(file, O_WRONLY);
	if (fd < 0)
		fs_scrub_exit("failed to open file %s error %d[%s]", file,
				errno, strerror(errno));

	while (1) {
		result = write(fd, sqnum_string, sqnum_string_len);
		if (result >= 0 || errno != EBUSY) {
			break;
		}
		SCRUB_LOGE("Scrub already in progress retry write after %u seconds",
			SCRUB_SLEEP_BEFORE_RETRY_TIME);
		sleep(SCRUB_SLEEP_BEFORE_RETRY_TIME);
	}
	close(fd);

	if (result < 0) {
		int ret_val;

		/* Cleanup the locks! */
		ret_val = fs_scrub_unlock(&scrub_handle.scrub_mutex);
		if (ret_val != 0)
			fs_scrub_exit("scrub_mutex lock failed");

		/* Disconnect from the server and only exit the thread when
		 * write fails with errno */
		fs_scrub_disconnect_from_server();
		fs_scrub_thread_exit("Write to %s failed error %d[%s]!", file,
					errno, strerror(errno));
	} else if (result != sqnum_string_len) {
		fs_scrub_exit("Write to %s failed expected %d written %d",
				file, sqnum_string_len, result);
	}
}

static
void fs_scrub_perform_scrubbing(struct fs_scrub_state_file_data *file_data)
{
	uint32 i;
	struct fs_scrub_state_entry *entry = file_data->entry;

	for (i = 0; i < file_data->valid_entry_count; i++) {
		if (entry->is_valid != 1) {
			fs_scrub_remove_file(SCRUB_FILE_DATA_FILE);
			fs_scrub_exit("Found invalid entry at index %u",i);
		}

		if (entry->scrub_is_complete == 1)
			continue;
		SCRUB_LOGV("Perform Scrub token = %u device = %u, sqnum = %llu",
			file_data->file_token, entry->ubi_dev_num,
			entry->scrub_sqnum);
		scrub_ubi_dev(entry->ubi_dev_num, entry->scrub_sqnum);
		entry->scrub_is_complete = 1;
		fs_scrub_commit_file_data(file_data);
		SCRUB_LOGV("Scrub complete for token = %u device = %u, sqnum = %llu",
			file_data->file_token, entry->ubi_dev_num,
			entry->scrub_sqnum);
		entry++;
	}
}



static void fs_scrub_main(uint32 scrub_token)
{
	int ret_val;
	struct fs_scrub_state_file_data *file_data;

	ret_val = fs_scrub_lock(&scrub_handle.scrub_mutex);
	if (ret_val != 0)
		fs_scrub_exit("scrub_mutex lock failed");

	SCRUB_LOGV("Scrubbing for token = %u", scrub_token);
	file_data = &scrub_handle.file_data;

	if (scrub_handle.is_file_data_valid == 1) {
		if (file_data->file_token >= scrub_token) {
			SCRUB_LOGV("Already finished scrubbing for token %u",
				scrub_token);
			goto out;
		}
	}

	scrub_handle.is_file_data_valid = 0;
	ret_val = fs_scrub_init_state(file_data);

	if (ret_val != 0)
		fs_scrub_exit("Failed to init scrub state");

	if (file_data->valid_entry_count == 0) {
		SCRUB_LOGV("Zero ubi devices found. No work required");
		goto out;
	}

	SCRUB_LOGV("Found a total of %u ubi devices ", file_data->valid_entry_count);
	file_data->file_token = scrub_token;
	scrub_handle.is_file_data_valid = 1;

	fs_scrub_commit_file_data(file_data);
	fs_scrub_perform_scrubbing(file_data);

out:
	ret_val = fs_scrub_ubi_detach();
	if (ret_val)
		SCRUB_LOGE("Failed to detach some additional ubi partitions. Continue...");

	ret_val = fs_scrub_unlock(&scrub_handle.scrub_mutex);
	if (ret_val != 0)
		fs_scrub_exit("scrub_mutex lock failed");
}


static int fs_scrub_update_token(uint32 new_token)
{
	int ret_val, result;

	ret_val = -1;
	result = fs_scrub_lock(&scrub_handle.token_mutex);
	if (result != 0)
		fs_scrub_exit("token_mutex lock failed");

	if (scrub_handle.is_token_valid == 1) {
		if (scrub_handle.current_token >= new_token) {
			SCRUB_LOGV("Ignoring trigger current_token=%u, received %u",
				scrub_handle.current_token, new_token);
			goto out;
		}
	}
	scrub_handle.current_token = new_token;
	scrub_handle.is_token_valid = 1;
	fs_scrub_write_data_to_file(SCRUB_TOKEN_FILE, &new_token,
					sizeof(new_token));
	ret_val = 0;
out:
	result = fs_scrub_unlock(&scrub_handle.token_mutex);
	if (result != 0)
		fs_scrub_exit("token_mutex lock failed");
	return ret_val;
}


static void *fs_scrub_handle_scrub_trigger(void *arg)
{
	int result;
	uint32 scrub_token;

	scrub_token = (uint32)(uintptr_t)arg;
	result = fs_scrub_update_token(scrub_token);
	if (result == 0)
		fs_scrub_main(scrub_token);

	fs_scrub_thread_return();
}

static void fs_scrub_resume_main(void)
{
	int ret_val;
	uint32 i;
	struct fs_scrub_state_entry *entry;
	struct fs_scrub_state_file_data *file_data;
	int is_resume_needed = 0;

	ret_val = fs_scrub_lock(&scrub_handle.scrub_mutex);
	if (ret_val != 0)
		fs_scrub_exit("scrub_mutex lock failed");

	SCRUB_LOGV("Check if resume is needed");
	if (scrub_handle.is_file_data_valid == 0) {
		SCRUB_LOGV("invalid file data for resume bailing!");
		goto cleanup;
	}

	file_data = &scrub_handle.file_data;
	entry = file_data->entry;
	for(i = 0; i < file_data->valid_entry_count; i++) {
		if (entry->is_valid != 1) {
			fs_scrub_remove_file(SCRUB_FILE_DATA_FILE);
			fs_scrub_exit("Found invalid entry at index %u",i);
		}

		if (entry->scrub_is_complete != 1)
			is_resume_needed = 1;
		entry++;
	}
	if (is_resume_needed != 1) {
		SCRUB_LOGV("Nothing to resume! now simply wait for next trigger");
		goto cleanup;
	}

	SCRUB_LOGV("Resume required!");
	SCRUB_LOGV("Sleeping for %u seconds before resume",
		SCRUB_SLEEP_BEFORE_RESUME_TIME);
	sleep (SCRUB_SLEEP_BEFORE_RESUME_TIME);
	/* Sleep for a few minutes before a resume */

	SCRUB_LOGV("Done sleeping ..resuming");

	fs_scrub_perform_scrubbing(file_data);
	SCRUB_LOGV("Scrub resume is complete");

cleanup:
	ret_val = fs_scrub_ubi_detach();
	if (ret_val)
		SCRUB_LOGE("Failed to detach some additional ubi partitions. Continue...");

	ret_val = fs_scrub_unlock(&scrub_handle.scrub_mutex);
	if (ret_val != 0)
		fs_scrub_exit("scrub_mutex lock failed");
}

static void *fs_scrub_handle_scrub_resume(void *arg)
{
	(void) arg;
	fs_scrub_resume_main();

	fs_scrub_thread_return();
}

static void fs_scrub_init_handle(void)
{
	int ret_val;

	memset(&scrub_handle, 0, sizeof(scrub_handle));

	ret_val = fs_scrub_mutex_init(&scrub_handle.scrub_mutex);
	if (ret_val != 0)
		fs_scrub_exit("scrub_mutex init failed!");

	ret_val = fs_scrub_mutex_init(&scrub_handle.qmi_init_mutex);
	if (ret_val != 0)
		fs_scrub_exit("qmi_init_mutex init failed");

	ret_val = fs_scrub_mutex_init(&scrub_handle.token_mutex);
	if (ret_val != 0)
		fs_scrub_exit("token_mutex init failed");
}

static int fs_scrub_read_data_from_file(char *file, void *buffer, int32 size)
{
	int fd, result;
	int32 remaining_bytes, file_size;
	struct stat stat_inst;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		if (errno == ENOENT)
			return -1;
		else
			fs_scrub_exit("File open of %s with error %d[%s]",
					file, errno, strerror(errno));
	}

	result = fstat(fd, &stat_inst);
	if (result != 0) {
		close(fd);
		fs_scrub_remove_file(file);
		fs_scrub_exit("fstat on %s failed error %d[%s]",
				file, errno, strerror(errno));
	}
	file_size = stat_inst.st_size;
	if (file_size != size) {
		SCRUB_LOGE("File size mismatch on %s, expected %d was %d, unlinking",
				file, size, file_size);
		fs_scrub_remove_file(file);
		return -1;
	}

	remaining_bytes = size;
	while (remaining_bytes > 0) {
		result = read(fd, buffer, remaining_bytes);
		if (result <= 0)
			fs_scrub_exit("Read %s failed error %d[%s]",
					file, errno, strerror(errno));
		remaining_bytes -= result;
	}
	close(fd);
	return 0;
}

static void fs_scrub_read_token_file(void)
{
	uint32 token_file_value;
	int32 ret_val;

	ret_val = fs_scrub_read_data_from_file (SCRUB_TOKEN_FILE,
				  &token_file_value, sizeof(token_file_value));

	if (ret_val == 0) {
		scrub_handle.current_token = token_file_value;
		scrub_handle.is_token_valid = 1;
		SCRUB_LOGI("Found token %u in token file",
						scrub_handle.current_token);
	}
}
static void fs_scrub_read_file_data(void)
{
	int32 ret_val;
	uint32 i;
	struct fs_scrub_state_file_data *file_data;

	file_data = &scrub_handle.file_data;

	ret_val = fs_scrub_read_data_from_file (SCRUB_FILE_DATA_FILE, file_data,
						sizeof(*file_data));

	if (ret_val == 0) {
		scrub_handle.is_file_data_valid = 1;
		if (file_data->magic1 !=  FS_SCRUB_STATE_MAGIC){
			SCRUB_LOGE("File MAGIC1 mismatch expected %u but is %u",
			      FS_SCRUB_STATE_MAGIC, file_data->magic1);
			scrub_handle.is_file_data_valid = 0;
		}

		if (file_data->magic2 !=  FS_SCRUB_STATE_MAGIC){
			SCRUB_LOGE("File MAGIC2 mismatch expected %u but is %u",
			       FS_SCRUB_STATE_MAGIC, file_data->magic2);
			scrub_handle.is_file_data_valid = 0;
		}

		if (file_data->version !=  FS_SCRUB_STATE_VERSION){
			SCRUB_LOGE("File Version mismatch expected %u but is %u",
			      FS_SCRUB_STATE_VERSION, file_data->version);
			scrub_handle.is_file_data_valid = 0;
		}

		if (file_data->valid_entry_count >  FS_SCRUB_STATE_MAX_ENTRIES){
			SCRUB_LOGE("Found %u valid entries expected <= %u ",
			      file_data->valid_entry_count,
			      FS_SCRUB_STATE_MAX_ENTRIES);
			scrub_handle.is_file_data_valid = 0;
		}

		if (scrub_handle.is_file_data_valid == 1) {
			for(i = 0; i < file_data->valid_entry_count; i++) {
				if (file_data->entry[i].is_valid != 1) {
					SCRUB_LOGE("Invalid entry index %d", i);
					scrub_handle.is_file_data_valid = 0;
					break;
				}
			}
		}

		if (scrub_handle.is_file_data_valid != 1)
			fs_scrub_remove_file(SCRUB_FILE_DATA_FILE);
		else
			SCRUB_LOGI("Found valid file_data file");
	}
}

static void fs_scrub_read_state_and_resume(void)
{
	int32 ret_val;
	struct fs_scrub_state_file_data *file_data;
	file_data = &scrub_handle.file_data;

	scrub_handle.is_file_data_valid = 0;
	scrub_handle.is_token_valid = 0;

	ret_val = fs_scrub_autodir(SCRUB_DATA_DIR "/");
	if (ret_val != 0)
		fs_scrub_exit("Dir_create failed %s", SCRUB_DATA_DIR);

	ret_val = fs_scrub_autodir(SCRUB_CORRUPTED_DIR "/");
	if (ret_val != 0)
		fs_scrub_exit("Dir_create failed %s", SCRUB_CORRUPTED_DIR);

	fs_scrub_read_token_file();
	fs_scrub_read_file_data();

	if (scrub_handle.is_token_valid == 1){
		if (scrub_handle.is_file_data_valid == 1) {
			if (scrub_handle.current_token > file_data->file_token) {
				SCRUB_LOGV("Token file token overriding file token");
				scrub_handle.is_file_data_valid = 0;
			}
		}

		if (scrub_handle.is_file_data_valid == 0) {
			ret_val = fs_scrub_init_state(file_data);
			if (ret_val != 0)
				fs_scrub_exit("Failed to init scrub state");

			if (file_data->valid_entry_count == 0) {
				SCRUB_LOGV("Zero ubi devices found. No work required");
				return;
			}
			SCRUB_LOGV("Found a total of %u ubi devices ",
					file_data->valid_entry_count);
			file_data->file_token = scrub_handle.current_token;
			scrub_handle.is_file_data_valid = 1;
		}

	} else {
		if (scrub_handle.is_file_data_valid == 1) {
			scrub_handle.current_token = file_data->file_token;
			scrub_handle.is_token_valid = 1;
		}else {

			SCRUB_LOGE("No files found .. likely is first boot!");
			return;
		}
	}

	if (scrub_handle.is_file_data_valid != 1)
		fs_scrub_exit("BUG! file data is not valid after resume checks");

	ret_val = fs_scrub_thread_create(fs_scrub_handle_scrub_resume, NULL);
	if (ret_val != 0)
		fs_scrub_exit("Thread create fail for scrub resume");
	return;
}


int main(int argc, char **argv)
{
	fs_scrub_init_handle();

	/* Resume before connecting to ensure no locking issues. */
	fs_scrub_read_state_and_resume();

	fs_scrub_connect_to_server();

	/* Wait forever */
	pause();

	return 0;
}
