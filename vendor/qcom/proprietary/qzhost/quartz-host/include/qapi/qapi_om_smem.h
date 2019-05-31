#ifndef __QAPI_OM_SMEM_H__
#define __QAPI_OM_SMEM_H__

/*
 * Copyright (c) 2015,2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*==================================================================================

                             EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file. Notice that
changes are listed in reverse chronological order.

$Header: //components/rel/core.ioe/1.0/v2/rom/release/api/platform/qapi_om_smem.h#6 $

when         who     what, where, why
--------   ---     -----------------------------------------------------------------
02/05/16   gnv     Initial version
==================================================================================*/

/**
  
 @file qapi_om_smem.h

 @brief Operating mode shared memory allocator
 
 @details

OM SMEM module is a shared memory manager to create memory pools that is used
by clients to save and restore data across Operating Mode transitions. Facilitates 
creation of multiple pools (maximum 4 pools at a time). Each pool can be used 
by multiple clients to allocate buffers, store data and commit. Then retrieve stored
data after Operating Mode transitions.

@verbatim

Expected Call Flow

                    +----------------------------------+
                    |     qapi_OMSM_Pool_Init          |
                    | (Called by framework if it       |
                    |  decides to create a custom Pool)|
                    +---------------+------------------+
                                    |
                                    v
                           Decision to transition       +------------------+
+----------------+         to a new Operating Mode+---->+  qapi_OMSM_Alloc |
|qapi_OMSM_Free  +-------> and Drivers/Clients are      +-------+----------+
+--------+-------+         required to Suspend                  |
         ^                                                      v
         |                                              Client copies data to
         +                                              be saved to the OM SMEM
Client Copies retrieved                                       buffer
 data to local buffer                                            +
         ^                                                       |
         |                                                       v
   +-----+--------------+ ----------------------------> +--------+---------+
   | qapi_OMSM_Retrieve |                               | qapi_OMSM_Commit |
   +----------+---------+ <---------------------------- +--------+---------+
              ^                                                  |
              +---------+Possible Operating Mode<----------------+
                               Transition




 +-------------------------+
 |  qapi_OMSM_Check_Status +---->Called by Clients to query the state of
 +-------------------------+     their buffer in the above state diagram
 +-------------------------+
 |  qapi_OMSM_Pool_Deinit  +---->Can be called by Framework to De-Initialize
 +-------------------------+     a custom Pool
 

@endverbatim

 @code {.c}
 */  

 /*
  Code sample below demonstrate use of this interface.
 */

/* Example Framework Code

void enter_transition_mode(void){

	void *pool;
	local_buf = malloc(SIZE); //Allocate space in a memory region that will be retained after the transition
	if(qapi_OMSM_Pool_Init(QAPI_OMSM_POOL_A_E, SIZE, local_buf) == QAPI_OK)
	{
		// Call Driver De-init Functions indicating Pool ID where drivers can save context
	}

}

//Example Driver De-Init Code

void driver_deinit(...)
{
	if(driver_suspend){
		if(QAPI_OK != qapi_OMSM_Alloc(QAPI_OMSM_POOL_A_E,DRIVER_CLIENT_ID, driver_context_size,&smem_buf)){
			//Copy Driver data to smem_buf
			qapi_OMSM_Commit(QAPI_OMSM_POOL_A_E, DRIVER_CLIENT_ID);
		}
	}else{
		//Call Driver shut down functions
	}
}

//Example Framework code after mode transition

void exit_transition_mode(void){

	if(resume_drivers){
		//Call driver init functions with details of POOL ID to recover driver data from
	}
	//Once all Drivers are re-initialized De-Init the pool and reclaim pool region for re-use
	qapi_OMSM_Pool_Deinit(QAPI_OMSM_POOL_A_E);
}

// Example code Driver Init code after Mode Transition

void driver_init(...)
{
	if(driver_recover){
		qapi_OMSM_Check_Status(QAPI_OMSM_POOL_A_E, DRIVER_CLIENT_ID, &alloc_status)
		if(QAPI_OMSM_BUF_COMMITTED_E == alloc_status){
			if(QAPI_OK == qapi_OMSM_Retrieve(QAPI_OMSM_POOL_A_E, DRIVER_CLIENT_ID, &size, &smem_buf_ptr))
				//Copy Driver data from smem_buf_ptr to local buffer

			qapi_OMSM_Free(QAPI_OMSM_POOL_A_E, DRIVER_CLIENT_ID);

		}

	}else{
		// Call regular driver init functions
	}

}



  @endcode

*/



/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/**  @addtogroup om_smem
 @{ */
/**
  @namegroup OMSM Driver-specific Error Codes

  These are driver-specific error codes that provide more information on 
  the failure reason.
  @{
*/

#define QAPI_OMSM_INVALID_BUFFER          __QAPI_ERROR(QAPI_MOD_BSP_OMSM, 1)
#define QAPI_OMSM_INVALID_POOL_ID         __QAPI_ERROR(QAPI_MOD_BSP_OMSM, 2)
#define QAPI_OMSM_INVALID_CLIENT_ID       __QAPI_ERROR(QAPI_MOD_BSP_OMSM, 3)
#define QAPI_OMSM_DRIVER_NOT_INITIALIZED  __QAPI_ERROR(QAPI_MOD_BSP_OMSM, 4)
#define QAPI_OMSM_POOL_NOT_INITIALIZED    __QAPI_ERROR(QAPI_MOD_BSP_OMSM, 5)
#define QAPI_OMSM_CLIENT_NOT_FOUND        __QAPI_ERROR(QAPI_MOD_BSP_OMSM, 6)
#define QAPI_OMSM_INVALID_CALL_SEQUENCE   __QAPI_ERROR(QAPI_MOD_BSP_OMSM, 7)
#define QAPI_OMSM_OUT_OF_MEMORY           __QAPI_ERROR(QAPI_MOD_BSP_OMSM, 8)
#define QAPI_OMSM_ERROR                   __QAPI_ERROR(QAPI_MOD_BSP_OMSM, 9)
#define QAPI_OMSM_FATAL_ERROR             __QAPI_ERROR(QAPI_MOD_BSP_OMSM, 10)

/** @} */
/**
  @namegroup OMSM Pool IDs

  These are the Pool IDs to be used while allocating buffers. The default 
  Pool is initialized by BSP. The rest of the Pool IDs can be used for custom 
  Pools. A maximum of 4 pools (including default) is supported.
  @{
*/

#define QAPI_OMSM_DEFAULT_AON_POOL    0 /**< Default AON pool initialized by BSP. */
#define QAPI_OMSM_POOL_A              1
#define QAPI_OMSM_POOL_B              2
#define QAPI_OMSM_POOL_C              3

/** @} */

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

/**
  @brief OM SMEM client buffer allocation status
  @details
  Allocation status of a client buffer in a pool. 
*/

typedef enum
{
	QAPI_OMSM_BUF_ALLOCATED_E, /**< Client Buffer Allocated. */
	QAPI_OMSM_BUF_COMMITTED_E, /**< Client Buffer Committed. Previous buffer handles not to be used. */
	QAPI_OMSM_BUF_RETRIEVING_E,/**< Client Buffer being Retrieved. */
	QAPI_OMSM_BUF_ALLOC_ERROR  /**< Client Buffer invalid or freed up */

} qapi_OMSM_alloc_status_t;


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/** @brief qapi_OMSM_Pool_Init
	@details
	This function is called to initialize a OM shared memory pool in a memory location. 
	The default AON pool is initialized by default by the BSP. This function is called
	by the framework to initialize other custom pools. This function registers the pool in
	internal data structures, and also initializes pool specific data structure at the begining 
	of the pool. It also memset the pool region to a pattern to remove stale data. Returns
	QAPI_OMSM_ERR_OUT_OF_MEMORY if the pool is too small to accomodate the 
	datastructure required to maintain the pool Returns QAPI_OK if this api is called 
	more than once for the same pool without De-Initializing.
	These resources are releases in the ::qapi_OMSM_Pool_Denit. 

	@param[in]	pool_id			ID of the pool to be initialized

	@param[in]  pool_size		Size of the pool to be initialized in Bytes

	@param[in]  pool_size		Size of the pool to be initialized in Bytes

	@return \ref QAPI_OK
	@return \ref QAPI_OMSM_ERR_INVALID_PARAMETER
	@return \ref QAPI_OMSM_ERR_INVALID_CALL_SEQUENCE
	@return \ref QAPI_OMSM_ERR_OUT_OF_MEMORY 

*/

qapi_Status_t
qapi_OMSM_Pool_Init
(
	uint8_t               pool_id,
	uint32_t              pool_size,
	void                 *pool_buffer
);


/** @brief qapi_OMSM_Alloc
	@details
	This function is called by clients who need to save data before the framework
	decides to do a Operating Mode [OM] transition. The Pool from which the buffer
	is requested is assumed to be retained across the pending Mode Transition.
	The client will get a valid handle to a buffer in the pool, if the allocation succeeds. 
	These resources are releases in the ::qapi_OMSM_Free. 

	@param[in]	  pool_id			ID of the pool from which the buffer needs to 
								    be allocated from. 

	@param[in]    client_id			Unique client ID corresponding to the caller. 

	@param[in]    buffer_size		Size of the buffer requested by the client in Bytes. 
								    Max Buffer size requested limited to 2^16 Bytes

	@param[out]   buffer_ptr		Valid handle to the buffer if the allocation is 
								    successful. If not successful handle will be NULL.


	@return \ref QAPI_OK
	@return \ref QAPI_OMSM_ERR_INVALID_PARAMETER
	@return \ref QAPI_OMSM_ERR_INVALID_CALL_SEQUENCE
	@return \ref QAPI_OMSM_ERR_OUT_OF_MEMORY 
	@return \ref QAPI_OMSM_ERR_FATAL

*/

qapi_Status_t
qapi_OMSM_Alloc
(
	uint8_t               pool_id,
	uint16_t              client_id,
	uint16_t              buffer_size,
	void                **buffer_ptr
);


/** @brief qapi_OMSM_Commit
	@details
	This function is called by client after they have copied the data that needs
	to be saved to the buffer allocated through qapi_OMSM_Alloc or through 
	qapi_OMSM_Retrieve. After OM transition clients can retrieve data from their
	buffer and either close the buffer through ( qapi_OMSM_Free ) or again commit 
	the data after data manipulation.

	@attention
	This api is equivalent to closing a file handle. The handle obtained through 
	qapi_OMSM_Alloc/ qapi_OMSM_Retrieve is not be used after this api is called.
	The handle is 	not guaranteed to point to the client's data	and can lead to 
	corruption if used,	after qapi_OMSM_Commit is called.
	This api can be called again after retrieving data through qapi_OMSM_Retrieve.
	If the client decides to manipulate the data, and commit it back, pay attention to 
	the size allocated in the buffer. Size of the data should not be different from
	what is returned in qapi_OMSM_Retrieve, to avoid corruption.

	@param[in]	pool_id			ID of the pool from which the buffer needs to 
								be allocated from. 

	@param[in]  client_id		Unique client ID corresponding to the caller. 

	@return \ref QAPI_OK
	@return \ref QAPI_OMSM_ERR_INVALID_PARAMETER
	@return \ref QAPI_OMSM_ERR_INVALID_CALL_SEQUENCE
*/

qapi_Status_t
qapi_OMSM_Commit
(
	uint8_t               pool_id,
	uint16_t              client_id
);


/** @brief qapi_OMSM_Retrieve
	@details
	This function is called by clients to retrieve data saved in a Pool before performing
	Operating Mode transition. The client will get a valid handle to a buffer in the pool,
	and size of the buffer, if the allocation was valid. The client can then copy over
	the data to their local memory.

	@param[in]	 pool_id			ID of the pool from which the buffer needs to 
    								be allocated from. 

	@param[in]   client_id			Unique client ID corresponding to the caller. 

	@param[out]  buffer_size		Returns the size of the buffer orinally allocated to 
	    							the client through qapi_OMSM_Alloc

	@param[out]  buffer_ptr		    Valid handle to the buffer if the allocation is 
								    successful. If not successful handle will be NULL.


	@return \ref QAPI_OK
	@return \ref QAPI_OMSM_ERR_INVALID_PARAMETER
	@return \ref QAPI_OMSM_ERR_INVALID_CALL_SEQUENCE

*/

qapi_Status_t
qapi_OMSM_Retrieve
(
	uint8_t               pool_id,
	uint16_t              client_id,
	uint16_t             *buffer_size,
	void                **buffer_ptr
);


/** @brief qapi_OMSM_Free
	@details
	De-initialize and frees the resources allocated through 
	:: qapi_OMSM_Alloc API.

	@attention
	This api is equivalent to closing a file handle. The handle obtained through 
	qapi_OMSM_Retrieve	is not be used after this api is called. The handle is 
	not guaranteed to point to the client's data and can lead to corruption if used,
	after qapi_OMSM_Free is called.

	@param[in]	pool_id			ID of the pool from which the buffer needs to 
								be allocated from. 

	@param[in]  client_id		Unique client ID corresponding to the caller. 

	@return \ref QAPI_OK
	@return \ref QAPI_OMSM_ERR_INVALID_PARAMETER
	@return \ref QAPI_OMSM_ERR_INVALID_CALL_SEQUENCE
*/

qapi_Status_t
qapi_OMSM_Free
(
	uint8_t               pool_id,
	uint16_t              client_id
);

/** @brief qapi_OMSM_Check_Status
	@details
	This api is used by client to check the status of their buffer in a Pool.
	The return will be of type qapi_OMSM_alloc_status_t which indicates
	the status of the buffer in a given pool. Clients can use this api to query the 
	status and then proceed accordingly.
	
	@param[in]	pool_id			ID of the pool from which the buffer needs to 
								be allocated from. 

	@param[in]  client_id		Unique client ID corresponding to the caller. 

	@param[out] status			Status of the client buffer. Could be one of 
								values in qapi_OMSM_alloc_status_t

	@return \ref QAPI_OK
	@return \ref QAPI_OMSM_ERR_INVALID_PARAMETER
	@return \ref QAPI_OMSM_ERR_INVALID_CALL_SEQUENCE
*/

qapi_Status_t
qapi_OMSM_Check_Status
(
	uint8_t                    pool_id,
	uint16_t                   client_id,
	qapi_OMSM_alloc_status_t  *status
);



/** @brief qapi_OMSM_Pool_Deinit
	@details
	De-initializes the pool and releases any resources allocated by the
	::qapi_OMSM_Pool_Init API.

	@attention
	This API cannot be used on the default AON pool. The call will be 
	ignored if the pool ID used in the API corresponds to default
	AON pool

	@param[in]	pool_id			ID of the pool from which the buffer needs to 
								be allocated from. 

	@return \ref QAPI_OK
	@return \ref QAPI_OMSM_ERR_INVALID_PARAMETER
	@return \ref QAPI_OMSM_ERR_INVALID_CALL_SEQUENCE
*/

qapi_Status_t
qapi_OMSM_Pool_Deinit
(
	uint8_t pool_id
);

/** @} */ /* group om_smem */

#endif /* __QAPI_OM_SMEM_H__ */
