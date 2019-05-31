#ifndef __QAPI_DIAG_MML_IF_H__
#define __QAPI_DIAG_MML_IF_H__

/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.  
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// $QTI_LICENSE_QDN_C$

/*==============================================================================
 *
 *          DIAGNOSTICS - MML (Message Management Layer) INTERFACE
 *
 * NOTE: In the below text, MML is referred as transport agnostic messaging
 *       service.
 *============================================================================*/

 /**
 *
 * @file qapi_diag_mml_if.h
 * 
 * @brief Diagnostic-Message Management Layer Interface.
 * 
 * @addtogroup qapi_diag_mml_if
 * @{
 *
 * @details: These DIAG APIs provides MML layer means to register/de-register
 *           its transport agnostic APIs in order that DIAG debug message
 *           logging and CLI (command line interface) functionality be enabled
 *           on transports that are hidden/abstracted under transport agnostic
 *           messaging service like MML.
 * @}
 */
/*==============================================================================

                            INCLUDE FILES

==============================================================================*/
#include <stdint.h>
#include <qapi/qapi_types.h>

/** @addtogroup qapi_diag_mml_if
@{ */

/*==============================================================================
                            FUNCTION TYPES TYPEDEF
==============================================================================*/
/**
 * @brief   Prototype for DIAG callbacks to handle data transmission/reception
 *          to/from any transport agnostic messaging service registered to DIAG.
 *
 * @param   service_id      Transport agnostic messaging service identifier.
 * @param   buf_len         Length corresponding to data transmitted/received.
 * @param   buf             Buffer containing data transmitted/received.
 * @param   diag_cxt        DIAG specific context registered with transport
 *                          agnostic messaging service.
 *                          Currently, for DIAG its UNUSED and set as NULL.
 * @param   diag_data       DIAG specific data sent to transport agnostic
 *                          messaging service when DIAG invokes
 *                          qapi_diag_tr_service_tx_fn_type or
 *                          qapi_diag_tr_service_rx_fn_type.
 *                          Currently, for DIAG its UNUSED and set as NULL.
 *
 * @return  None.
 */
typedef void (* qapi_diag_tr_tx_rx_cb_fn_type)(uint8_t service_id,
                                               uint32_t buf_len,
                                               uint8_t *buf, void *diag_cxt,
                                               void *diag_data);


/*
 * @brief   transport agnostic messaging service registration from DIAG which
 *          the service has implemented.
 *
 * @param   service_id      Transport agnostic messaging service identifier.
 * @param   rx_cb           Rx callback from transport agnostic messaging
 *                          service registered to DIAG.
 * @param   tx_cb           Tx callback from transport agnostic messaging
 *                          service registered to DIAG.
 * @param   diag_cxt        DIAG specific context registered with transport
 *                          agnostic messaging service.
 *                          Currently, for DIAG its UNUSED and set as NULL.
 *
 * @return  Zero on success and non-zero on failures.
 *
 * @example
 *  For MML,
 *      service_id = MML_DIAG_SQ_ID distinguishes transport beneath MML
 *      diag_tx_rx_cb_fn_type is in sync with mmlCb_t
 */
typedef int (* qapi_diag_tr_service_register_fn_type)(uint8_t service_id,
                                            qapi_diag_tr_tx_rx_cb_fn_type rx_cb,
                                            qapi_diag_tr_tx_rx_cb_fn_type tx_cb,
                                            void *diag_cxt);

/*
 * @brief   Transport agnostic messaging service deregistration from DIAG which
 *          the service has implemented.
 *
 * @param   service_id      Transport agnostic messaging service identifier.
 * @param   diag_cxt        DIAG specific context registered with transport
 *                          agnostic messaging service.
 *                          Currently, for DIAG its UNUSED and set as NULL.
 *
 * @return  Zero on success and non-zero on failures.
 *
 * @example
 *  For MML,
 *      service_id = MML_DIAG_SQ_ID distinguishes transport beneath MML
 */
typedef int (* qapi_diag_tr_service_deregister_fn_type)(uint8_t service_id,
                                                        void *diag_cxt);

/*
 * Send data using transport specific service.
 *
 * @param   service_id      Transport agnostic messaging service identifier.
 * @param   buf_len         Length corresponding to data transmitted.
 * @param   buf             Buffer containing data transmitted.
 * @param   diag_data       DIAG specific data sent to transport agnostic
 *                          messaging service when DIAG invokes
 *                          qapi_diag_tr_service_tx_fn_type.
 *
 * @return  Zero on success and non-zero on failures.
 *
 * @example
 *  For MML,
 *      service_id = MML_DIAG_SQ_ID distinguishes transport beneath MML
 */
typedef int (* qapi_diag_tr_service_tx_fn_type)(uint8_t service_id,
                                                uint32_t buf_len,
                                                char *buf, void *diag_data);

/*
 * @brief   Receive data using transport agnostic messaging service.
 *
 * @param   service_id      Transport agnostic messaging service identifier.
 * @param   buf_len         Length corresponding to data received.
 * @param   buf             Buffer containing data received.
 * @param   diag_data       DIAG specific data sent to transport agnostic
 *                          messaging service when DIAG invokes
 *                          qapi_diag_tr_service_rx_fn_type.
 *
 * @return  Zero on success and non-zero on failures.
 *
 * @example
 *  For MML,
 *      service_id = MML_DIAG_SQ_ID distinguishes transport beneath MML
 */
typedef int (* qapi_diag_tr_service_rx_fn_type)(uint8_t service_id,
                                                uint32_t buf_len,
                                                char *buf, void *diag_data);

/*
 * @brief   Cleanup data transmitted using transport agnostic messaging service.
 *
 * @param   service_id      Transport agnostic messaging service identifier.
 * @param   buf             Buffer containing data transmitted.
 *
 * @return  Zero on success and non-zero on failures.
 *
 * @example
 *  For MML,
 *      service_id = MML_DIAG_SQ_ID distinguishes transport beneath MML
 */
typedef int (* qapi_diag_tr_service_tx_buf_free_fn_type)(uint8_t service_id,
                                                         uint8_t *buf);

/*
 * Cleanup data received using transport agnostic messaging service.
 *
 * @param   service_id      Transport agnostic messaging service identifier.
 * @param   buf             Buffer containing data received.
 *
 * @return  Zero on success and non-zero on failures.
 *
 * @example
 *  For MML,
 *      service_id = MML_DIAG_SQ_ID distinguishes transport beneath MML
 */
typedef int (* qapi_diag_tr_service_rx_buf_free_fn_type)(uint8_t service_id,
                                                         uint8_t *buf);


/*==============================================================================
                            STRUCTURE TYPEDEF
==============================================================================*/
/*
 * @brief   Structure that defines the transport agnostic messaging service APIs
 *          that the service implements. And when the service registers with
 *          DIAG using an instance of it, DIAG can then access these APIs.
 *
 * @note
 *      - The transport agnostic messaging service that registers with DIAG can
 *        have a static or dynamic instance of this. If dynamic then the calling
 *        service owns its creation/destruction based on invocation of
 *        qapi_diag_service_tr_register()/qapi_diag_service_tr_deregister()
 *        respectively.
 *      - All these members are input and shall be set by the transport agnostic
 *        service registering with DIAG.
 *      - Any of the APIs not implemented by the transport agnostic messaging
 *        service, shall be set as NULL by the service registering with DIAG.
 *
 * @example
 *  For MML,
 *      service_id = MML_DIAG_SQ_ID distinguishes transport beneath MML
 *      diag_tr_register = mmlServiceRegisterCb
 *      diag_tr_deregister = mmlServiceDeregisterCb
 *      diag_tr_tx = mmlServiceQueueSend
 *      diag_tr_rx = NULL
 *      diag_tr_tx_buf_free = NULL
 *      diag_tr_rx_buf_free = mmlReleaseBuf
 */
typedef struct qapi_diag_tr_service_context
{
    /**< Distinguish messaging service's underlying transport. */
    uint32_t                                    service_id;
    /**< To register DIAG with transport agnostic messaging service. */
    qapi_diag_tr_service_register_fn_type       diag_tr_register;

    /**< To de-register DIAG with transport agnostic messaging service. */
    qapi_diag_tr_service_deregister_fn_type     diag_tr_deregister;

    /**< To send DIAG packet to transport agnostic messaging service. */
    qapi_diag_tr_service_tx_fn_type             diag_tr_tx;

    /**< To prepare for reception of DIAG packet from transport agnostic messaging service. */
    qapi_diag_tr_service_rx_fn_type             diag_tr_rx;

    /**< For DIAG to release transport agnostic messaging service's transmission data, if any. */
    qapi_diag_tr_service_tx_buf_free_fn_type    diag_tr_tx_buf_free;

    /**< For DIAG to release transport agnostic messaging service's reception data, if any. */
    qapi_diag_tr_service_rx_buf_free_fn_type    diag_tr_rx_buf_free;
} qapi_diag_tr_service_context_t;

/*==============================================================================
                            FUNCTION DECLATATIONS
==============================================================================*/
/*==============================================================================
  FUNCTION      qapi_diag_tr_service_register
==============================================================================*/
/*
 * @brief   Register the transport agnostic messaging service context especially
 *          its APIs for DIAG use.
 *
 * @param[in]   service_ctx     Transport agnostic messaging service context
 *                              registered with DIAG for its use.
 *
 * @return  true on successful registration and false elsewise.
 *          If false, one reason would be output drain channel is not rightly
 *          set. Check diag_props.xml to verify that out drain channel is set to
 *          DIAG_OP_DRAIN_CH_EXT_MML
 *
 * @note    The transport agnostic messaging service that registers with DIAG
 *          can have a static or dynamic instance of this. If dynamic then
 *          service owns its creation before entering into
 *          qapi_diag_tr_service_register().
 */
qbool_t qapi_diag_tr_service_register(qapi_diag_tr_service_context_t *service_ctx);

/*==============================================================================
  FUNCTION      qapi_diag_tr_service_deregister
==============================================================================*/
/*
 * @brief   De-register the transport agnostic messaging service context
 *          especially its APIs, so that DIAG can no longer use them.
 *
 * @param[in]   service_ctx     Transport agnostic messaging service context
 *                              registered with DIAG for its use, that is to be
 *                              invalidated.
 *
 * @return  true on successful registration and false elsewise.
 *
 * @note    The transport agnostic messaging service that registers with DIAG
 *          can have a static or dynamic instance of this. If dynamic then
 *          service owns its destruction after return from 
 *          qapi_diag_tr_service_deregister().
 */
qbool_t qapi_diag_tr_service_deregister(qapi_diag_tr_service_context_t *service_ctx);
/** @} */ /* end_addtogroup qapi_diag_mml_if */

#endif /*__QAPI_DIAG_MML_IF_H__ */

