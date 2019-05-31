/*
 * Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * 2016 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */
// $QTI_LICENSE_QDN_C$

/*=================================================================================
 *
 *                       FILE-SYSTEM
 *
 *================================================================================*/
 /**
 *
 * @file qapi_securefs.h
 *
 * @addtogroup qapi_securefs
 * @{
 *
 * @brief Secure Storage Module
 *
 * @details The secure storage module provides APIs to carry out basic operations on
 *          encrypted files.  The APIs supported by this module allow clients to carry
 *          out following operations:
 *          - Open an existing secure storage file or create a new file,
 *          - Read and write to a secure storage file within file system,
 *          - Close a secure storage file
 *
 * @note    Secure storage APIs can take time in order of seconds to return
 *          depending on the access speeds of the underlying flash media.
 *          Calling tasks/clients should be prepared to be blocked
 *          for this duration in secure storage APIs. It is mandatory to
 *          avoid calling secure storage APIs in time critical sections of the code.
 *
 * @code {.c}
 *     * The code snippet below demonstrates the use of secure storage APIs.
 *     * In the example below, a client creates a secure storage file using
 *     * QAPI_FS_O_CREAT mode. It then performs a write operation on the file.
 *     * It then sets the current location to the beginning of the file and reads the file.
 *     * For brevity, the sequence assumes that all calls succeed. Clients should
 *     * check the return status from each call.
 *
 *     uint8 buf[100];
 *     uint8 read_buf[100];
 *     uint32 buf_size = 100;
 *     int32 actual_offset;
 *     uint32 bytes_written, bytes_read;
 *     qapi_Status_t status;
 *     void * securefs_ctxt
 *
 *     // Open a file with QAPI_FS_O_CREAT mode
 *     uint8_t user_password[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
 *                                 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
 *     status = qapi_Securefs_Open(&securefs_ctxt, "/spinor/encrypted_file.bin",
 *              QAPI_FS_O_RDWR | QAPI_FS_O_CREAT, user_password, sizeof(user_password));
 *
 *     // Write and Read from file
 *     qapi_Securefs_Write(securefs_ctxt, buf, buf_size, &bytes_written);
 *     qapi_Securefs_Lseek(securefs_ctxt, 0, SEEK_SET, &actual_offset);
 *     qapi_Securefs_Read(securefs_ctxt, read_buf, buf_size, &bytes_read)
 *
 *     //Closing the secure file
 *     qapi_Securefs_Close(securefs_ctxt);
 *
 * @endcode
 *
 * @}
 *
 */

#ifndef __QAPI_SECURE_FS_H__
#define __QAPI_SECURE_FS_H__

#include <qapi/qapi_types.h>
#include <qapi/qapi_status.h>
#include <qapi/qapi_fs.h>


/** @addtogroup qapi_securefs
@{ */

/**
 * @brief Securefs open flag to indicate that all the writes are to be synchronized.
 *
 * @details When this flag is supplied to the qapi_Securefs_Open() function, all the writes
 *          will be synchronized. The content of the file is encrypted and the file is being
 *          securely signed at the end of each qapi_Securefs_Write() function.
 */
#define QAPI_FS_O_SYNC 04000000


/**
 * @brief Maximum length of secure file system password.
 *
 * @details This is the maximum possible password length supported by secure file system.
 */
#define QAPI_SECUREFS_MAX_PASSWORD_SZ 16


/*============================================================================
  FUNCTION      qapi_Securefs_Open
============================================================================*/
/**
 *  @brief Opens a secure storage file as per the given oflag.
 *
 *  @details On success,
 *           the secure storage file handle is returned, which is to
 *           be used as an input handle to other secure storage APIs.
 *
 *           Depending on the read/write speeds of the underlying flash media,
 *           this API could take time in the order-of-seconds to complete, and
 *           clients calling this API should be prepared to have their task
 *           blocked for this time duration.
 *
 *  @note1hang  Definitions of oflags that are common for qapi_fs.h API and qapi_securefs.h API
 *              are located in qapi_fs.h. The definitions of the oflags that are specific to
 *              qapi_securefs.h API only are located at the top of qapi_securefs.h.
 *
 *  @param[out] securefsCtxtPtr Pointer to the secure storage file context. Upon
 *                      successful opening of the file, qapi_Securefs_Open()
 *                      allocates and initiates this context. Treat this context
 *                      as a secure storage file handle.
 *
 *  @param[in]  filePath Path of the secure storage file that is to be opened.
 *
 *  @param[in] oflags   Describes how this file should
 *                      be opened and contains one of the following values:
 *                      - QAPI_FS_O_RDONLY -- Open as read only
 *                      - QAPI_FS_O_WRONLY -- Open as write only
 *                      - QAPI_FS_O_RDWR   -- Open as read/write \n
 *                      In addition, the following flags can be bitwise ORed
 *                      with this value:
 *                      - QAPI_FS_O_APPEND -- All writes will self-seek to the end
 *                                          of the file before writing
 *                      - QAPI_FS_O_CREAT  -- Create the file if it does not exist
 *                      - QAPI_FS_O_TRUNC  -- Truncate the file to zero bytes on
 *                                          successful open
 *                      - QAPI_FS_O_SYNC   -- Make all writes to the file syncronized,
 *                                           i.e., at the end of each
 *                                           qapi_Securefs_Write() function, the file
 *                                           is encrypted and securely signed. \n
 *                      These flags can be used to specify the common intended
 *                      methods of opening files:
 *                      - QAPI_FS_O_CREAT | QAPI_FS_O_TRUNC -- Normal for writing
 *                             a file. Creates it if it does not exist. \n
 *                             The resulting file is zero bytes long.
 *                      - QAPI_FS_O_CREAT | QAPI_FS_O_EXCL -- Creates a file
 *                             but fails if it exists. \vertspace{-14}
  *  @param[in] userInputPassword Password to use to encrypt/decrypt the secure storage file.
 *                      Note that only up to 16 bytes of the password are being used.
 *
 *  @param[in] userInputPasswordSizeInBytes Length of the password that is used to
 *                      encrypt/decrypt the file. Note that only up to 16 bytes
 *                      of the password are being used.
 *
 *  @return Returns QAPI_OK on success or an error code on failure.
 */
qapi_Status_t qapi_Securefs_Open(void ** securefsCtxtPtr, const char * filePath, int oflags, const uint8_t * userInputPassword, uint32_t userInputPasswordSizeInBytes);


/*============================================================================
   FUNCTION      qapi_Securefs_Close
 ============================================================================*/
 /**
  *  @brief  Closes the secure storage file and frees the securefsCtxt.
  *
  *  @details  The descriptor will no longer refer to
  *            any file and will be allocated to subsequent calls to fs_open.
  *
  *  @param[in]  securefsCtxt   Secure storage context obtained earlier via
  *                             the qapi_Securefs_open() function.
  *
  *  @return Returns QAPI_OK on success or an errror code on error.
  *
  */
qapi_Status_t qapi_Securefs_Close(void * securefsCtxt);


/*============================================================================
  FUNCTION      qapi_Securefs_Lseek
============================================================================*/
/**
 *  @brief Changes the file offset for the opened secure storage file.
 *
 *  @details An attempt to write count bytes of data from the file associated
 *           with the specified file descriptor.
 *           Changing the file offset does not modify the file. Seeking beyond
 *           the end of file is not supported and returns an error.
 *
 *  @param[in]  securefsCtxt     Secure FS context obtained earlier via the qapi_Securefs_Open() function
 *
 *  @param[in]  offset           New offset within the secure storage file.
 *
 *  @param[in]  whence           Indicates how the new offset is computed:
 *                               - QAPI_FS_SEEK_SET -- Set to offset
 *                               - QAPI_FS_SEEK_CUR -- Set to offset + current position
 *                               - QAPI_FS_SEEK_END -- Set to offset + file size \vertspace{-14}
 *
 *  @param[in]  actualOffsetPtr  Return from the function on the
 *                               resulting offset, as bytes from the beginning
 *                               of the file.
 *
 *  @return  Returns QAPI_OK on success or an error code on failure.
 *
 */
qapi_Status_t qapi_Securefs_Lseek(void * securefsCtxt, int32_t offset, int32_t whence, int32_t * actualOffsetPtr);


 /*============================================================================
   FUNCTION      qapi_Securefs_Tell
 ============================================================================*/
 /**
  *  @brief Tells the file offset for the opened secure storage file.
  *
  *  @param[in]  securefsCtxt     Secure FS context obtained earlier via the qapi_Securefs_Open() function.
  *
  *  @param[in]  actualOffsetPtr  Offset from the beginning of the file.
  *
  *  @return  Returns QAPI_OK on success or an error code on failure.
  *
  */
qapi_Status_t qapi_Securefs_Tell(void * securefsCtxt, int32_t * actualOffsetPtr);


/*============================================================================
  FUNCTION      qapi_Securefs_Read
============================================================================*/
/**
 *  @brief Reads count bytes of data from the secure storage file
 *   associated with the specified secure FS context.
 *
 *   Depending on the read/write speeds of the underlying flash media,
 *   this API could take time in the order-of-seconds to complete, so
 *   clients calling this API should be prepared to have their task
 *   blocked for this time duration.
 *
 *  @note1hang  It is permitted for qapi_Securefs_Read to return fewer bytes than
 *         were requested, even if the data is available in the file.
 *
 *  @param[in]  securefsCtxt     Secure FS context obtained earlier via the qapi_Securefs_Open function().
 *
 *  @param[in]  buf              Buffer where the read bytes from the file are to be
 *                               stored.
 *
 *  @param[in]  count            Number of bytes to read from the file.
 *
 *  @param[out] bytesReadPtr     Return from the function on the number of
 *                               bytes actually read.
 *
 *  @return Returns QAPI_OK on success or an error code on failure.
 *
 */
qapi_Status_t qapi_Securefs_Read(void * securefsCtxt, void * buf, size_t count, size_t * bytesReadPtr);


/*============================================================================
  FUNCTION      qapi_Securefs_Write
============================================================================*/
/**
 *  @brief Writes count bytes of data from the secure storage file
 *   associated with the specified secure FS context.
 *
 *   Depending on the read/write speeds of the underlying flash media,
 *   this API could take time in the order-of-seconds to complete, so
 *   clients calling this API should be prepared to have their task
 *   blocked for this time duration.
 *
 *  @note   It is permitted for qapi_Securefs_Write to return fewer bytes than
 *          were requested, even if space is available. If the number of bytes
 *          written is zero, it indicates that the file system is full (writing
 *          again should result in an error).
 *
 *  @param[in]  securefsCtxt     Secure FS context obtained earlier the via qapi_Securefs_Open() function.
 *
 *  @param[in]  buf              Buffer to which the file is to be written.
 *
 *  @param[in]  count            Number of bytes to write to the file.
 *
 *  @param[out] bytesWrittenPtr  Return from the function on the number
 *                               of bytes actually written.
 *
 *  @return Returns QAPI_OK on success or an error code on error.
 *
 */
qapi_Status_t qapi_Securefs_Write(void * securefsCtxt, const void * buf, size_t count, size_t * bytesWrittenPtr);


/*============================================================================
  FUNCTION      qapi_Securefs_Flush
============================================================================*/
/**
 *  @brief Flushes the securefs file.
 *
 *  @details Encrypts and writes the buffered content into the file, securely
 *           signing it at the end.
 *
 *  @param[in]  securefsCtxt     Secure FS context obtained earlier the via qapi_Securefs_Open function.
 *
 *  @return Returns QAPI_OK on success or an error code on error.
 *
 */
qapi_Status_t qapi_Securefs_Flush(void * securefsCtxt);

/** @} */

#endif
