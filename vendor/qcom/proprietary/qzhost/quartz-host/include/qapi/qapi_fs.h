#ifndef __QAPI_FS_H__
#define __QAPI_FS_H__

/*
 * Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*=================================================================================
 *
 *                       FILE-SYSTEM
 *
 *================================================================================*/
 /**
 *
 * @file qapi_fs.h
 * 
 * @brief Filesystem Module
 * 
 * @addtogroup qapi_fs
 * @{
 * 
 *
 * The filesystem module provides APIs to carry out basic operations on
 * files.The APIs supported by this filesystem allow clients to carry
 * out following operations:
 *  - Open an existing file or create a new file
 *  - Reading and Writing to a files within filesystem
 *  - Closing a file
 *  - Rename an existing file
 *  - Unlink file operation which deletes a file
 *  - File stat and Filesystem stat operations
 *  - Iterate over all files in filesystem
 *
 * Filesystem APIs can take time in order of seconds to return
 * depending on the access speeds of the underlying flash media.
 * Calling tasks/clients should be prepared to be blocked
 * for this duration in Filesystem APIs. It is mandatory to
 * not call filesystem APIs in time crtical sections of the code.
 * @code {.c}
 *
 *     /\*
 *     * Code snippet below demonstrates usage of certain filesystem APIs.
 *     * In the example below a client creates a file in filesystem using
 *     * QAPI_FS_O_CREAT mode. Carries out a write operation on the file.
 *     * Then does a read and a stat operation on the same file.
 *     * For Brevity, the sequence assumes that all calls succeed. Clients should
 *     * check return status from each call.
 *     *\/
 *     uint8 buf[100];
 *     uint8 read_buf[100];
 *     uint32 buf_size = 100;
 *     int32 actual_offset;
 *     uint32 bytes_written, bytes_read;
 *     qapi_Status_t fd;
 *     struct qapi_fs_stat_type sbuf1;
 *
 *     // Open a file with QAPI_FS_O_CREAT mode
 *     fd = qapi_Fs_Open("/spinor/hello.txt", QAPI_FS_O_RDWR, QAPI_FS_O_CREAT, &fd);
 *
 *     // Write and Read from file
 *     qapi_Fs_Write(fd, buf, buf_size, &bytes_written);
 *     qapi_Fs_Lseek(fd, 0, SEEK_SET, &actual_offset); 
 *     qapi_Fs_Read (fd, read_buf, buf_size, &bytes_read)
 *
 *     //Carrying out stat operation on file
 *     qapi_Fs_Stat("/spinor/hello.txt", &sbuf1);
 *
 *     //Closing a file with filedescriptor
 *     qapi_Fs_Close(fd);
 *
 * @endcode
 * @}
 */


/*==================================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/api/storage/filesystem/apps_proc/qapi_fs.h#13 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------
05/06/16   leo     (TechComm) Edited/added Doxygen comments and markup
12/18/15   mj      Initial version
==================================================================================*/


/*==================================================================================

                               INCLUDE FILES

==================================================================================*/
/*==================================================================================

                                   MACROS

==================================================================================*/
/** @addtogroup qapi_fs
@{ */
/**
 * @brief Mode bits to open a file.
 *
 * @details The mode parameter is passed as argument to qapi_Fs_Open API.
 *          The mode bits specifies the mode in which the file needs to be opened.
 */
#define QAPI_FS_O_ACCMODE        0003
#define QAPI_FS_O_RDONLY         00
#define QAPI_FS_O_WRONLY         01
#define QAPI_FS_O_RDWR           02
#define QAPI_FS_O_CREAT          0100
#define QAPI_FS_O_EXCL           0200
#define QAPI_FS_O_TRUNC          01000
#define QAPI_FS_O_APPEND         02000

#define QAPI_FS_SEEK_SET       0       /**< Seek from beginning of file.  */
#define QAPI_FS_SEEK_CUR       1       /**< Seek from current position.  */
#define QAPI_FS_SEEK_END       2       /**< Seek from end of file.  */

/* File system Mount flags */
#define QAPI_FS_MOUNT_FLAG_CREATE_FS   1 /**< Open a FS or create a new FS if doesn't exist */
#define QAPI_FS_MOUNT_FLAG_REFORMAT_FS 2 /**< Format and Open a clean FS */


/**
 * @brief Maximum possible path length for a file.
 *
 * @details This is the maximum possible path length supported by the filesystem
 *          for a given file.
 */
#ifndef QAPI_FS_MAX_FILE_PATH_LEN
  #define QAPI_FS_MAX_FILE_PATH_LEN  (100)
#endif
/*==================================================================================

                               TYPE DEFINITIONS

==================================================================================*/


/**
* FS iterator client handle
*
* Handle provided by the module to the client. Clients must pass this
* handle as a token with subsequent calls. Note that the clients
* should cache the handle. Once lost, it cannot be queried back from
* the module.
*/

typedef void* qapi_fs_iter_handle_t;

/*============================================================================
  FUNCTION      qapi_Fs_Open
============================================================================*/
/**
 *  @brief Opens a file as per the given oflag
 *
 *  @details Open a file in the filesystem as per the given oflag. On success
 *           the file descriptor handle to this file is returned which is to
 *           be used as handle to the other file system APIs listed below.
 *
 *  Depending on the read/write speeds of the underlying flash media,
 *  this API can take time in the order of seconds to complete, and
 *  clients calling this API should be prepared to have their task
 *  blocked for this time duration.
 *
 *  @param[in]  path    Path of the file that is to be opened.
 *
 *  @param[in]  oflag   Argument that describes how this file should
 *                      be opened, and contains one of the following values: \n
 *                      QAPI_FS_O_RDONLY: Open for reading only. \n
 *                      QAPI_FS_O_WRONLY: Open for writing only. \n
 *                      QAPI_FS_O_RDWR: Open for reading and writing. \n
 *                      In addition, the following flags can be bitwise OR'd
 *                      with this value: \n
 *                      QAPI_FS_O_APPEND: All writes will self-seek to the end
 *                                          of the file before writing. \n
 *                      QAPI_FS_O_CREAT: Create the file if it does not exist. \n
 *                      QAPI_FS_O_TRUNC: Truncate the file to zero bytes on
 *                                          successful open. \n
 *                      These flags can be used to specify common desired
 *                      ways of opening files: \n
 *                      QAPI_FS_O_CREAT | QAPI_FS_O_TRUNC: Normal for writing
 *                             a file. Creates it if it does not exist.
 *                             Resulting file is zero bytes long. \n
 *                      QAPI_FS_O_CREAT | QAPI_FS_O_EXCL: Creates a file
 *                             but fails if it exists.
 *  @param[in] fd_ptr Address of file descriptor variable. \n
 *                    The file descriptor of an opened file on success is
 *                    written to the variable provided. On failure,
 *                    a negative value is written to this variable.
 *
 *  @return Returns QAPI_OK on success and -ve error code on failure.
 *          The -ve value is the errno value that provides the reason
 *          for the failure. \n
 *          QAPI_ERROR: Cannot create a file with the pathname because
 *                      another file with the same name exists and an
 *                      exclusive open is requested or a
 *                      special (exclusive) file with the same pathname
 *                      exists. \n
 *          QAPI_ERROR: No entry for the pathname is found, the file
 *                      cannot be opened and a new file is not created
 *                      because the FS_O_CREAT flag is not supplied. \n
 *          QAPI_ERROR: Maximum number of open descriptors is exceeded. \n
 *          QAPI_ERR_NO_MEMORY: No space left on the device. \n
 *          QAPI_ERR_NO_MEMORY: No more dynamic memory is available. \n
 *          QAPI_ERROR: The device does not exist. \n
 *          QAPI_ERR_INVALID_PARAM: Invalid parameter, path. \n
 *          QAPI_ERROR: File path exceeded PATH_MAX limit.
 *
 */
qapi_Status_t qapi_Fs_Open(const char *path, int oflag, int *fd_ptr);

/*============================================================================
  FUNCTION      qapi_Fs_Read
============================================================================*/
/**
 *  @brief Attempts to read 'count' bytes of data from the file associated wit
 *         the specified file descriptor.
 *
 *  Depending on the read/write speeds of the underlying flash media,
 *  this API can take time in the order of seconds to complete, and
 *  clients calling this API should be prepared to have their task
 *   blocked for this time duration.
 *
 *  @param[in]  fd     The file descriptor obtained earlier via fs_open function.
 *
 *  @param[in]  buf    The buffer where the read bytes from the file will be
 *                     stored.
 *
 *  @param[in]  count      The number of bytes to read from the file.
 *  @param[in]  bytes_read  This is a return from the function on the number of
 *                          bytes actually read.
 *
 *  @return Returns QAPI_OK on success and -ve error code on failure.
 *
 *  @note  It is permitted for qapi_Fs_Read to return fewer bytes than
 *         were requested, even if the data is available in the file.
 *
 */
qapi_Status_t qapi_Fs_Read(int fd, uint8_t *buf, uint32_t count,
			   uint32_t *bytes_read);

/*============================================================================
  FUNCTION      qapi_Fs_Write
============================================================================*/
/**
 *  @brief Attempts to write 'count' bytes of data from the file associated with
 *         the specified file descriptor.
 *
 *  Depending on the read/write speeds of the underlying flash media,
 *  this API can take time in the order of seconds to complete, and
 *  clients calling this API should be prepared to have their task
 *  blocked for this time duration.
 *
 *  @param[in]  fd     File descriptor obtained earlier via fs_open function.
 *
 *  @param[in]  buf    Buffer that is to be written to the file.
 *
 *  @param[in]  count         Number of bytes to write to the file.
 *  @param[in]  bytes_written  This is a return from the function on the number
 *                             of bytes actually written.
 *
 *  @return Returns QAPI_OK on success and -ve error code on failure.
 *
 *  @note   It is permitted for qapi_Fs_Write to return fewer bytes than
 *          were requested, even if space is available. If number of bytes
 *          written are zero it indicates that the filesystem is full (writing
 *          again should result in an ENOSPC error).
 */
qapi_Status_t qapi_Fs_Write(int fd, uint8_t *buf, uint32_t count,
			    uint32_t *bytes_written);

/*============================================================================
  FUNCTION      qapi_Fs_Lseek
============================================================================*/
/**
 *  @brief Changes the file offset for the opened file descriptor.
 *
 *  Attempts to write 'count' bytes of data from the file associated
 *  with the specified file descriptor.
 *  Changing the file offset does not modify the file. Seeking beyond
 *  the end of file is not support and will return error.
 *
 *  @param[in]  fd     File descriptor obtained earlier via fs_open function.
 *
 *  @param[in]  offset The new offset of the file.
 *
 *  @param[in]  whence  Indicates how the new offset is computed: \n
 *                      QAPI_FS_SEEK_SET: Set to 'offset'. \n
 *                      QAPI_FS_SEEK_CUR: Set to 'offset' + current position. \n
 *                      QAPI_FS_SEEK_END: Set to 'offset' + file size.
 *  @param[in]  actual_offset  This is a return from the function on the
 *                             resulting offset, as bytes from the beginning
 *                             of the file.
 *
 *  @return  Returns QAPI_OK on success and -ve error code on failure
 *
 */
qapi_Status_t qapi_Fs_Lseek(int fd, int32_t offset, int whence,
			    int32_t *actual_offset);

/*============================================================================
  FUNCTION      qapi_Fs_Close
============================================================================*/
/**
 *  @brief  Closes the file descriptor.
 *
 *  @details  The descriptor will no longer refer to
 *            any file and will be allocated to subsequent calls to fs_open.
 *
 *  @param[in]  fd     File descriptor obtained earlier via fs_open function.
 *
 *  @return Returns QAPI_OK on success and -ve error code on failure.
 *
 */
qapi_Status_t qapi_Fs_Close(int fd);

/*============================================================================
  FUNCTION      qapi_Fs_Put
============================================================================*/
/**
 *  @brief Create a file with the supplied data in one atomic transaction.
 *
 *  @details Create the supplied file path holding all the supplied data in one
 *           atomic transaction. When this API returns it is guarantted to
 *           create the supplied file containing the whole supplied data.
 *
 *  Depending on the read/write speeds of the underlying flash media,
 *  this API can take time in the order of seconds to complete, and
 *  clients calling this API should be prepared to have their task
 *  blocked for this time duration.
 *
 *  @param[in]  path   Path of the file that is to be created.
 *
 *  @param[in]  buf    Buffer that is to be written to the file.
 *
 *  @param[in]  count  Number of bytes to write to the file.
 *
 *  @param[in]  oflags   Argument that describes how this file should
 *                      be opened. Refer to the fs_open API for the
 *                      possible oflags values.
 *
 *  @param[in]  bytes_written  This is a return from the function on the number
 *                             of bytes actually written.
 *
 *  @return Returns QAPI_OK on success and -ve error code on failure.
 *
 */
qapi_Status_t qapi_Fs_Put(const char *path, void *buf, uint32_t count,
			   int oflags, uint32_t *bytes_written);

/*============================================================================
  FUNCTION      qapi_Fs_Get
============================================================================*/
/**
 *  @brief Reads the file into the supplied buffer.
 *
 *  @details The supplied buffer
 *           must be big enough to hold the entire file.
 *
 *  Depending on the read/write speeds of the underlying flash media,
 *  this API can take time in the order of seconds to complete, and
 *  clients calling this API should be prepared to have their task
 *  blocked for this time duration.
 *
 *  @param[in]  path   Path of the file that is to be read.
 *
 *  @param[in]  buf    Buffer in which to store the read data.
 *
 *  @param[in]  count  Number of bytes to read from the file.
 *
 *  @param[in]  bytes_read  This is a return from the function on the number of
 *                          bytes actually read.
 *
 *  @return Returns QAPI_OK on success and -ve error code on failure.
 *
 */
qapi_Status_t qapi_Fs_Get(const char *path, void *buf, uint32_t count,
			  uint32_t *bytes_read);

/*============================================================================
  FUNCTION      qapi_Fs_Unlink
============================================================================*/
/**
 *  @brief Deletes the given file in the file system.
 *
 *  Depending on the read/write speeds of the underlying flash media,
 *  this API can take time in the order of seconds to complete, and
 *  clients calling this API should be prepared to have their task
 *  blocked for this time duration.
 *
 *  @param[in]  path   Path of the file that is to be read.
 *
 *  @return     Returns QAPI_OK on success and -ve error code on failure.
 *
 */
qapi_Status_t qapi_Fs_Unlink(const char *path);

/*============================================================================
  FUNCTION      qapi_Fs_Rename
============================================================================*/
/**
 *  @brief Renames a file.
 *
 *  @details Files and directories (under the same file system)
 *           can be renamed. The arguments 'oldpath' and 'newpath' do not have
 *           to be in the same directory (but must be on the same filesystem
 *           device).
 *
 *  Depending on the read/write speeds of the underlying flash media,
 *  this API can take time in the order of seconds to complete, and
 *  clients calling this API should be prepared to have their task
 *  blocked for this time duration.
 *
 *  @param[in]  old_path   Path name before the rename operation.
 *
 *  @param[in]  new_path   Path name after the rename operation.
 *
 *  @return Returns QAPI_OK on success and -ve error code on failure.
 *
 */
qapi_Status_t qapi_Fs_Rename(const char *old_path, const char *new_path);

struct qapi_fs_stat_type {
	uint32_t st_dev;
	uint32_t st_ino;
	uint32_t st_size;
	uint32_t st_blksize;
	uint32_t st_blocks;
};

/*============================================================================
  FUNCTION      qapi_Fs_Stat
============================================================================*/
/**
 *  @brief Gets the information about the file by path.
 *
 *  Depending on the read/write speeds of the underlying flash media,
 *  this API can take time in the order of seconds to complete, and
 *  clients calling this API should be prepared to have their task
 *  blocked for this time duration.
 *
 *  @param[in]  path   Path of the file to get stats about.
 *
 *  @param[out] sbuf  Structure that holds the information about the file: \n
 *  @verbatim
 *  struct qapi_fs_stat {
 *     uint32_t st_dev;
 *     uint32_t st_ino;
 *     uint32_t st_size;
 *     uint32_t st_blksize;
 *     uint32_t st_blocks;
 * };
 *
 * st_dev    : Unique Device ID
 * st_ino    : INode number associated with the
 *             file.
 * st_size   : File size in bytes.
 * st_blksize: Block size, smallest allocation
 *             unit of the file system. Unit in
 *             which block count is represented.
 * st_blocks : Number of blocks allocated for this
 *             this file in st_blksize units. @endverbatim @vertspace{-16}
 *
 *  @return Returns QAPI_OK on success and -ve error code on failure.
 *
 */
qapi_Status_t qapi_Fs_Stat(const char *path, struct qapi_fs_stat_type *sbuf);

struct qapi_fs_statvfs_type {
	uint32_t f_bsize;
	uint32_t f_blocks;
	uint32_t f_bfree;
	uint32_t f_bavail;
	uint32_t f_fsid;
	uint32_t f_pathmax;
	uint32_t f_max_file_size;
	uint32_t f_max_transaction_size;
	uint32_t f_max_open_files;
};

/*============================================================================
  FUNCTION      qapi_Fs_Statvfs
============================================================================*/
/**
 *  @brief  Obtains information about an entire filesystem.
 *
 *  Gets detailed information about the filesystem specified by the path.
 *  The information is returned in the structure fs_statvfs. The
 *  content details are as given below.
 *
 *  Depending on the read/write speeds of the underlying flash media,
 *  this API can take time in the order of seconds to complete, and
 *  clients calling this API should be prepared to have their task
 *  blocked for this time duration.
 *
 *  @param[in]  path   Path of the file system to get stats about.
 *
 *  @param[out] sbuf  Structure that holds the information about the file system:
 *  @verbatim
 *    struct qapi_fs_statvfs_type {
 *        uint32_t f_bsize;
 *        uint32_t f_blocks;
 *        uint32_t f_bfree;
 *        uint32_t f_bavail;
 *        uint32_t f_fsid;
 *        uint32_t f_pathmax;
 *        uint32_t f_max_file_size;
 *        uint32_t f_max_transaction_size;
 *        uint32_t f_max_open_files;
 *    }; @endverbatim @vertspace{-16}
 *
 *  @return Returns QAPI_OK on success and -ve erro code on failure.
 */
qapi_Status_t qapi_Fs_Statvfs(const char *path,
			      struct qapi_fs_statvfs_type *sbuf);


struct qapi_fs_iter_entry {
	char file_path[QAPI_FS_MAX_FILE_PATH_LEN];
	struct qapi_fs_stat_type sbuf;
};

/*============================================================================
  FUNCTION      qapi_Fs_Iter_Open
============================================================================*/
/**
 *  @brief Iterates over all the files in the file system.
 *
 *  Depending on the read/write speeds of the underlying flash media,
 *  this API can take time in the order of seconds to complete, and
 *  clients calling this API should be prepared to have their task
 *  blocked for this time duration.
 *
 *  @param[in]  path     path of the file system to iterate.
 *
 *  @param[in]  handle   Returns non NULL iterator handle  on success, or NULL
 *                       for an error.
 *
 *  @return Returns QAPI_OK on success and -ve erro code on failure.
 *
 */
 qapi_Status_t qapi_Fs_Iter_Open(const char *path,
				 qapi_fs_iter_handle_t *handle);

/*============================================================================
  FUNCTION      qapi_Fs_Iter_Next
============================================================================*/
/**
 *  @brief Returns the next file system entry while iterating.
 *
 *  Depending on the read/write speeds of the underlying flash media,
 *  this API can take time in the order of seconds to complete, and
 *  clients calling this API should be prepared to have their task
 *  blocked for this time duration.
 *
 *  @param[in]  iter_hdl Handle to the iterator.
 *  @param[in]  qapi_iter_entry Returns non NULL iterator entry  on success, or NULL
 *                         for an error.
 *
 *  @return Returns QAPI_OK on success and -ve erro code on failure. \n
 *          On termination , it returns QAPI_ERR_NO_ENTRY.
 *
 *  @note
 *      If the iterator chain is navigated without any modifiable operations
 *      like file delete then return value of QAPI_ERR_NO_ENTRY truly indicates
 *      that the iterator chain is completely navigated till the end.
 *      But if modifiable operations on file are done on the fly when the
 *      iterator is navigated then the return value of QAPI_ERR_NO_ENTRY doesn't
 *      imply the whole iterator chain is navigated. Rather the iterator chain
 *      must be retraced until the return value of QAPI_ERR_NO_ENTRY is returned
 *      on the 1st attempt after iteration initialization
 *      (i.e. qapi_Fs_Iter_Open).
 *      In other words, whenever delete operation is done on an active iteration
 *      navigation then as the database entry table is re-ordered its
 *      recommended to rescan the iterator chain though QAPI_ERR_NO_ENTRY is
 *      returned qapi_Fs_Iter_Next() until QAPI_ERR_NO_ENTRY is returned on the
 *      1st attempt after iteration initialization which confirms that the
 *      iterator chain is empty.
 *
 *  @verbatim
 *    struct qapi_fs_iter_entry {
 *                               char file_path[FS_MAX_FILE_PATH_LEN];
 *                               struct fs_stat sbuf;
 *     };
 *
 *     struct qapi_fs_stat_type {
 *          uint32_t st_dev;
 *          uint32_t st_ino;
 *          uint32_t st_size;
 *          uint32_t st_blksize;
 *          uint32_t st_blocks;
 *     };
 *
 *      st_dev     : Unique Device ID
 *      st_ino     : INode number associated with the file.
 *      st_size    : File size in bytes.
 *      st_blksize : Block size, smallest allocation unit of
 *                   the file system. Unit in which block count
 *                   is represented.
 *      st_blocks  : Number of blocks allocated for this file
 *                   in st_blksize units.
 *  @endverbatim
 */
  qapi_Status_t qapi_Fs_Iter_Next(qapi_fs_iter_handle_t iter_hdl, \
                                  struct qapi_fs_iter_entry *qapi_iter_entry);

/*============================================================================
  FUNCTION      qapi_Fs_Iter_Close
============================================================================*/
/**
 *  @brief Closes the file iterator.
 *
 *  @param[in]  iter_hdl   File Iterator handle.
 *
 *  @return Returns QAPI_OK on success and -ve erro code on failure.
 *
 */
qapi_Status_t qapi_Fs_Iter_Close(qapi_fs_iter_handle_t iter_hdl);

/*============================================================================
  FUNCTION      qapi_Fs_Mount
============================================================================*/

/**
 *  @brief Mount the given file system.
 *
 *  @details Mounts the file system in the supplied device_name and the
 *           partition-id is supplied in the last argument.
 *
 *           Depending on the read/write speeds of the underlying flash media
 *           this API could take time in the order-of-seconds to complete and
 *           clients calling this API should be prepared to have their task
 *           blocked for this time duration.
 *           qapi_Fs_Mount shall return an error if an attempt is made to
 *           re-use a mount point that'salready in use.
 *
 *  @param[in]  device_name    The name of the device. Usually, it will be "spinor"
 *
 *  @param[in]  mount_dir  The directory in which to mount this file system.
 *
 *  @param[in]  mount_flags  Mount option flags
 *                           QAPI_FS_MOUNT_FLAG_CREATE_FS : create a brand new
 *                           filesystem when there is no valid file system
 *                           image in the given partition.
 *                           QAPI_FS_MOUNT_FLAG_REFORMAT_FS: Reformats the
 *                           current filesystem thereby creating a brand new
 *                           filesystem.
 *
 *  @param[in]  PARTITION_ID The partition-id in which the FS will be created or exists.
 *
 *  @return Returns 0 on success and negative value on failure. On success
 *          the file system will be mounted at the given mount-point.
 *
 *          Following are most common error codes for this API
 *          QAPI_ERR_NO_MEMORY    : No memory. malloc failed.
 *          QAPI_ERR_BAD_PAYLOAD  : The given partition does not have a valid
 *                             file system image. Use QAPI_FS_MOUNT_FLAG_CREATE_FS
 *                             mount-flag to create a brand new file system
 *                             or use the fs_create_filesystem() API first
 *                             to create a brand new file system first and
 *                             then call this fs_mount API to mount this
 *                             newly created file system.
 *          QAPI_ERR_NO_RESOURCE  : Partition missing or device failure.
 *          QAPI_ERR_INVALID_PARAM: Invalid arguments.\n
 *
 *  @note
 *      Its recommended that application should never mount a file system
 *      partition simultaneously on multiple mount points. This may be
 *      disallowed in the near future.
 */
qapi_Status_t qapi_Fs_Mount(const char *device_name, const char *mount_dir, uint32_t mount_flags, uint32_t PARTITION_ID);


/*============================================================================
  FUNCTION      qapi_Fs_Unmount
============================================================================*/
/**
 *  @brief Unmount the given file system.
 *
 *  @details Unmounts the file system in the supplied device_name and the
 *           partition-id is supplied in the last argument.
 *
 *           Depending on the read/write speeds of the underlying flash media
 *           this API could take time in the order-of-seconds to complete and
 *           clients calling this API should be prepared to have their task
 *           blocked for this time duration.
 *
 *  @param[in]  unmount_dir  The directory which needs to be unmounted.
 *
 *  @return Returns 0 on success and negative value on failure. On success
 *          the file system will be unmounted at the given mount-point.
 */
qapi_Status_t qapi_Fs_Unmount(const char *unmount_dir);


/*============================================================================
  FUNCTION      qapi_Fs_check
============================================================================*/
/**
 *  @brief FS Check for a given mount directory
 *
 *  @details Performs FS check on mounted directory.
 *
 *           Depending on the read/write speeds of the underlying flash media
 *           this API could take time in the order-of-seconds to complete and
 *           clients calling this API should be prepared to have their task
 *           blocked for this time duration.
 *
 *  @param[in]  mount_dir  The directory which should undergo FS check.
 *
 *  @return Returns 0 on success and negative value on failure. On success
 *          the file system is successfully verified.
 */
qapi_Status_t qapi_Fs_Check(const char *mount_dir);


/** @} */ /* end_addtogroup qapi_fs */
#endif /* __QAPI_FS_H__ */
