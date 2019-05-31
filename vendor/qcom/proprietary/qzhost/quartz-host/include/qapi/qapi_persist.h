/*
 * Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/**
 * @file qapi_persist.h
 *
 * @brief
 * QAPI for persistent storage of generic data to the a filesystem.
 *
 * @details
 * This API wraps file system operations to provide: (1) a simpler
 * API interface, (2) data integrity checks, and (3) reverting to
 * previous revision on integrity check failure.
 */

#ifndef __QAPI_PERSISTH__
#define __QAPI_PERSISTH__

#include "qapi/qapi_types.h"
#include "qapi/qapi_status.h"

/**
   Handle of a Persistent Storage instance.
*/
typedef struct qapi_Persist_Handle_s *qapi_Persist_Handle_t;

/**
   @brief Initializes a persistent storage context for single storage application.

   The function creates the context for a storage application based on the the file
   naming information provided. Any number of storage contexts can be created for
   different base filenames.

   @param[out] Handle       is a pointer to the context handle that will be
                            allocated.
   @param[in]  Directory    Directory in which the files wile be stored. Note:
                            this should be located within a mounted filesystem.
   @param[in]  NamePrefix   is the beginning of the file name to be used. The
                            API will add extra information after this to create
                            multiple file revisions.
   @param[in]  NameSuffix   is the end of the file name to be used. This will be
                            placed after the API's additions. This is can be a
                            file extension.
   @param[in]  Password     Password used for encrypting the persistent data.
                            This may be set to NULL if the PasswordSize is zero.
   @param[in]  PasswordSize Length of the password provided.  This can be a
                            maximum of 16 bytes.

   @return
     - QAPI_OK if the context was initialized successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_Persist_Initialize(qapi_Persist_Handle_t *Handle, char *Directory, char *NamePrefix, char *NameSuffix, uint8_t *Password, uint32_t PasswordSize);

/**
   @brief Cleans up an existing persitent storage context.

   This function frees all memory and resources allocated for the persistent storage context.
   It does NOT delete any persistent data. The context can be re-initialized using the same
   parameters to reload existing data.

   @param[in] Handle is a storage handle created via a successful call to
                     qapi_Persist_Initialize().
*/
void qapi_Persist_Cleanup(qapi_Persist_Handle_t Handle);

/**
   @brief Store data to persistent storage.

   This function stores the supplied data to the filename indicated in the initialized
   storage handle.

   @param[in] Handle     is a storage handle created via a successful call to
                         Qapi_Persist_Initialize().
   @param[in] DataLength is the length of the supplied data buffer.
   @param[in] Data       is the buffer of data to be saved to persistent storage.

   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_Persist_Put(qapi_Persist_Handle_t Handle, uint32_t DataLength, uint8_t *Data);

/**
   @brief Retrieve data from persistent storage.

   This function retrieves data from the filesystem into the supplied buffer using the
   filename from the initialized storage handle.

   This function will allocate the data buffer internally. The called must then call
   qapi_Persist_Free() with the returned buffer if this function returns success.

   @param[in]  Handle     is a storage handle created via a successful call to
                          qapi_Persist_Initialize().
   @param[out] DataLength is the length of the data buffer allocated.
   @param[out] Data       is the allocated buffer if retrieved data.


   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
qapi_Status_t qapi_Persist_Get(qapi_Persist_Handle_t Handle, uint32_t *DataLength, uint8_t **Data);

/**
   @brief Free an allocated data buffer.

   This function frees an allocated data buffer returned from a successful call to
   qapi_Persist_Get().

   @param[in] Handle is a storage handle created via a successful call to
                     qapi_Persist_Initialize().
   @param[in] Data   is the data buffer to be freed.


   @return
     - QAPI_OK if the command executed successfully.
     - A negative value if there was an error.
*/
void qapi_Persist_Free(qapi_Persist_Handle_t Handle, uint8_t *Data);

/**
   @brief Delete files from persistent storage.

   This function deletes the any existing files specified by an initialized
   storage handle.

   @param[in] Handle is a storage handle created via a successful call to
                     qapi_Persist_Initialize().
*/
void qapi_Persist_Delete(qapi_Persist_Handle_t Handle);

#endif


