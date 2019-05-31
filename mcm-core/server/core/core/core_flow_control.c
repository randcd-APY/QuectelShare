/******************************************************************************

    @file
    core_flow_control.c

    @brief
    Implements flow control utility.

    Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.

******************************************************************************/

#include "util_htable.h"
#include "core_flow_control.h"
#include "hlos_core.h"

#define RIL_MAX_FLOW_CONTROL_SIZE 50

utilHashTable core_flow_control_hash_table = NULL;
unsigned int  core_flow_control_identifier = 0;

typedef struct core_flow_control_message_type {
    int                     identifier;
    int                     message_arrival_identifier;
    core_handler_data_type *core_handler_data;
} core_flow_control_message_type;


/*===========================================================================

    FUNCTION: core_flow_control_hash_func

===========================================================================*/
/*!
    @brief
    hash function for flow control

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
unsigned int core_flow_control_hash_func
(
    void *key
)
{
    int hash_key = 0;

    hash_key = (int)key;
    hash_key = (hash_key % RIL_MAX_FLOW_CONTROL_SIZE);
    return hash_key;
}


/*===========================================================================

    FUNCTION: core_flow_control_equality_func

===========================================================================*/
/*!
    @brief
    equality check function for hash table

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
int core_flow_control_equality_func
(
    void *lookup_key,
    void *key
)
{
    return ((int)lookup_key == (int)key);
}


/*===========================================================================

  FUNCTION:  core_flow_control_list_add_evaluator

===========================================================================*/
/*!
    @brief
    flow control list add evaluator

    @return
    FALSE
*/
/*=========================================================================*/
int core_flow_control_list_add_evaluator
(
    util_list_node_data_type *to_be_added_data,
    util_list_node_data_type *to_be_evaluated_data
)
{
    return FALSE;
}

/*===========================================================================

  FUNCTION:  core_flow_control_list_delete_evaluator

===========================================================================*/
/*!
    @brief
    flow control list delete evaluator

    @return
    none
*/
/*=========================================================================*/
void core_flow_control_list_delete_evaluator
(
    util_list_node_data_type *to_be_deleted_data
)
{
    if(to_be_deleted_data)
    {
        util_memory_free(&to_be_deleted_data->user_data);
    }

    return;
}

/*===========================================================================

    FUNCTION: core_flow_control_remove_entry_evaluator

===========================================================================*/
/*!
    @brief
    remove an entry from flow control

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
void core_flow_control_remove_entry_evaluator
(
    util_list_node_data_type* core_flow_control_event_entry
)
{
    core_flow_control_event_type *core_flow_control_event = core_flow_control_event_entry;

    if (core_flow_control_event && core_flow_control_event->list_of_messages)
    {
        util_list_cleanup(core_flow_control_event->list_of_messages, NULL);
    }

    return;
}


/*===========================================================================

    FUNCTION: core_flow_control_init

===========================================================================*/
/*!
    @brief
    initialize flow control engine

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int core_flow_control_init
(
    void
)
{
    int res = EFAILURE;
    UTIL_LOG_MSG("entry");

    if (core_flow_control_hash_table)
    {
        return res;
    }

    core_flow_control_hash_table =
                                    util_hash_table_create(core_flow_control_hash_func,
                                                           core_flow_control_equality_func);
    if (core_flow_control_hash_table)
    {
        res = ESUCCESS;
    }

    return res;
}


/*===========================================================================

    FUNCTION: core_flow_control_deinit

===========================================================================*/
/*!
    @brief
    deinitialize flow control engine

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int core_flow_control_deinit
(
    void
)
{
    int res = EFAILURE;
    UTIL_LOG_MSG("entry");

    if (core_flow_control_hash_table)
    {
        util_hash_table_lock(core_flow_control_hash_table);

        util_hash_table_destroy(core_flow_control_hash_table,
                                core_flow_control_remove_entry_evaluator);

        util_hash_table_unlock(core_flow_control_hash_table);
        res = ESUCCESS;
    }

    return res;
}


/*===========================================================================

    FUNCTION: core_flow_control_find_next_list

===========================================================================*/
/*!
    @brief
    Finds pending message of same event type

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int core_flow_control_find_next_list
(
    util_list_node_data_type *to_be_found_data
)
{
    int res = FALSE;

    if (to_be_found_data)
    {
        res = TRUE;
    }

    return res;
}


/*===========================================================================

    FUNCTION: core_flow_control_add_entry

===========================================================================*/
/*!
    @brief
    add an entry to flowcontrol

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int core_flow_control_add_entry
(
    core_flow_control_event_type *core_flow_control_event
)
{
    int res = EFAILURE;
    core_flow_control_event_type *core_flow_control_event_entry = NULL;
    utilHashKeyValue  core_flow_control_hash_key_value = NULL;

    UTIL_LOG_MSG("entry");

    do {

        core_flow_control_event_entry = util_memory_alloc(sizeof(core_flow_control_event_type));

        if (!core_flow_control_event_entry)
        {
        UTIL_LOG_MSG("malloc fail");
        break;
        }

        core_flow_control_event_entry->message_id       = core_flow_control_event->message_id;
        core_flow_control_event_entry->attributes       = core_flow_control_event->attributes;
        core_flow_control_event_entry->max_size_of_list = core_flow_control_event->max_size_of_list;
        core_flow_control_event_entry->is_message_with_same_param = core_flow_control_event->is_message_with_same_param;

        core_flow_control_event_entry->family_pair_android_request_id =
                                                          core_flow_control_event->family_pair_android_request_id;

        core_flow_control_event_entry->list_of_messages = util_list_create(NULL,
                                                                           core_flow_control_list_add_evaluator,
                                                                           core_flow_control_list_delete_evaluator,
                                                                           UTIL_LIST_BIT_FIELD_CREATED_ON_HEAP);

        if (!core_flow_control_event_entry->list_of_messages)
        {
            UTIL_LOG_MSG("list create fail");
            break;
        }

        core_flow_control_hash_key_value = util_memory_alloc(sizeof(*core_flow_control_hash_key_value));
        if (!core_flow_control_hash_key_value)
        {
            UTIL_LOG_MSG("malloc fail");
            break;
        }

        UTIL_LOG_MSG("insert message %d %x", core_flow_control_event_entry->message_id, core_flow_control_event);
        core_flow_control_hash_key_value->key  = (void*)core_flow_control_event_entry->message_id;
        core_flow_control_hash_key_value->data = (void*)core_flow_control_event_entry;

        res = util_hash_table_insert(core_flow_control_hash_table,
                                     core_flow_control_hash_key_value);

    } while(0);

    if (res != ESUCCESS)
    {
        if (core_flow_control_hash_key_value)
        {
            util_memory_free(&core_flow_control_hash_key_value);
        }

        if (core_flow_control_event_entry)
        {
            if (core_flow_control_event_entry->list_of_messages)
            {
                util_list_cleanup(core_flow_control_event_entry->list_of_messages,
                                  core_flow_control_list_delete_evaluator);
            }
            util_memory_free(&core_flow_control_event_entry);
        }
    }

    return res;
}


/*===========================================================================

    FUNCTION: core_flow_control_register_message

===========================================================================*/
/*!
    brief
    register message with flow control

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int core_flow_control_register_messages
(
    core_flow_control_event_type *core_flow_control_events,
    int                           core_flow_control_event_arr_length
)
{
    int res    = ESUCCESS;
    int length = core_flow_control_event_arr_length;
    int i;
    core_flow_control_event_type *temp_core_flow_control_event = NULL;
    UTIL_LOG_MSG("entry");

    util_hash_table_lock(core_flow_control_hash_table);

    temp_core_flow_control_event = core_flow_control_events;
    if (core_flow_control_events)
    {
        for (i = 0; i < length && res == ESUCCESS; i++, temp_core_flow_control_event++)
        {
            res = core_flow_control_add_entry(temp_core_flow_control_event);
        }
    }
    util_hash_table_unlock(core_flow_control_hash_table);

    return res;
}


/*===========================================================================

    FUNCTION: core_flow_control_register_as_ring

===========================================================================*/
/*!
    @brief
    register messages as ring

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
void core_flow_control_register_as_ring
(
    const int *message_ids,
    int        length
)
{
    const int *message_id_ptr;
    int        message_id;
    int        i;

    core_flow_control_event_type    *current_ptr = NULL;
    core_flow_control_event_type    *prev_ptr    = NULL;
    core_flow_control_event_type    *first_ptr   = NULL;

    /* register messages as ring */
    if (NULL != message_ids && length)
    {
        util_hash_table_lock(core_flow_control_hash_table);
        message_id_ptr = message_ids;

        for (i = 0; i < length; i++, message_id_ptr++)
        {
            current_ptr = util_hash_table_lookup(core_flow_control_hash_table,
                                                 *message_id_ptr);

            if (NULL != prev_ptr)
            {
                prev_ptr->family_ring = current_ptr;
            }
            else
            {
                first_ptr = current_ptr;
            }
            prev_ptr = current_ptr;
        }

        if (NULL != current_ptr)
        {
            current_ptr->family_ring = first_ptr;
        }
        util_hash_table_unlock(core_flow_control_hash_table);
    }
}


/*===========================================================================

    FUNCTION: core_flow_control_delete_pending_messages_from_family

===========================================================================*/
/*!
    @brief
    delete pending messages and send response

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
void core_flow_control_delete_pending_messages_from_family
(
    core_flow_control_event_type  *flow_control_event,
    int                            error
)
{
    int                              res = FALSE;
    core_flow_control_event_type    *pending_event = NULL;
    util_list_node_data_type        *holder_candidate;
    core_flow_control_message_type  *tmp_flow_control_message;
    core_flow_control_event_type    *iter;
    core_handler_data_type          *core_handler_data;

    UTIL_LOG_MSG("entry");

    if (NULL != flow_control_event)
    {
        if (flow_control_event->attributes & CORE_FLOW_CONTROL_ATTR_IN_FAMILY_RING)
        {
            // look up through whole family ring
            iter = flow_control_event;
            do
            {
                holder_candidate = util_list_find(iter->list_of_messages,
                core_flow_control_find_next_list);

                while ( holder_candidate != NULL )
                {
                    tmp_flow_control_message = (core_flow_control_message_type*)holder_candidate->user_data;
                    core_handler_data = tmp_flow_control_message->core_handler_data;

                    /* call response handler helper */
                    hlos_core_send_response_handler(CRI_ERR_INTERNAL_V01,
                                                    core_handler_data->event_data,
                                                    NULL,
                                                    0);

                    util_list_delete(flow_control_event->list_of_messages,
                                     holder_candidate, NULL);

                    holder_candidate = util_list_find(iter->list_of_messages,
                                                      core_flow_control_find_next_list);
                }
                iter = iter->family_ring;

            } while (iter != flow_control_event);
        }
        else
        {
            holder_candidate = util_list_find(flow_control_event->list_of_messages,
            core_flow_control_find_next_list);

            while ( holder_candidate != NULL )
            {
                tmp_flow_control_message = (core_flow_control_message_type*)holder_candidate->user_data;
                core_handler_data = tmp_flow_control_message->core_handler_data;

                /* call response handler helper */
                hlos_core_send_response_handler(CRI_ERR_INTERNAL_V01,
                                                core_handler_data->event_data,
                                                NULL,
                                                0);

                util_list_delete(flow_control_event->list_of_messages,
                                 tmp_flow_control_message, NULL);

                holder_candidate = util_list_find(flow_control_event->list_of_messages,
                                                  core_flow_control_find_next_list);
            }
        }
    }

    return;
}



/*===========================================================================

    FUNCTION: core_flow_control_find_pending_message_of_same_type

===========================================================================*/
/*!
    @brief
    Finds pending message of same event type

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
core_flow_control_message_type *core_flow_control_find_pending_message_of_same_type
(
    core_flow_control_event_type *flow_control_event
)
{
    core_flow_control_message_type *res = NULL;

    if (NULL != flow_control_event)
    {
        if (flow_control_event->size_of_list > 0)
        {
            /* get first message from queue */
            res = util_list_find(flow_control_event->list_of_messages,
            core_flow_control_find_next_list);
        }
    }

    return res;
}

/*===========================================================================

    FUNCTION: core_flow_control_find_pending_request

===========================================================================*/
/*!
    @brief
    Finds pending pending request of same type or family

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int core_flow_control_find_pending_request
(
    core_flow_control_event_type  *flow_control_event,
    core_flow_control_event_type **flow_control_pending_event
)
{
    int res = FALSE;
    core_flow_control_event_type*  pending_event = NULL;
    core_flow_control_event_type*  holder_candidate;
    core_flow_control_event_type*  iter;
    core_flow_control_event_type*  pair;
    int                             candidate_identiefier = 0;

    UTIL_LOG_MSG("entry");

    if (NULL != flow_control_event)
    {
        if (flow_control_event->attributes & CORE_FLOW_CONTROL_ATTR_IN_FAMILY_RING)
        {
            // check first if pair is defined
            if ((flow_control_event->attributes &
                 CORE_FLOW_CONTROL_ATTR_FAMILY_RING_DEFINED_PAIR) &&
                 NIL != flow_control_event->family_pair_android_request_id)
            {
                pair = util_hash_table_lookup(core_flow_control_hash_table,
                           flow_control_event->family_pair_android_request_id);
                if (pair && pair->size_of_list > 0)
                {
                    res               = TRUE;
                    pending_event = pair;
                }
            }

            // look up through whole family ring
            if ( !res )
            {
                iter = flow_control_event;
                do
                {
                    holder_candidate = util_list_find(iter->list_of_messages,
                    core_flow_control_find_next_list);

                    if (NULL != holder_candidate)
                    {
                        if ((NIL == candidate_identiefier) ||
                            (iter->identifier < candidate_identiefier))
                        {
                            candidate_identiefier = iter->identifier;
                            pending_event         = iter;
                            res                   = TRUE;
                        }
                    }

                    iter = iter->family_ring;
                } while (iter != flow_control_event && iter);
            }
        }
        else
        {
            holder_candidate = util_list_find(flow_control_event->list_of_messages,
                                              core_flow_control_find_next_list);
            if ( NULL != holder_candidate)
            {
                pending_event     = flow_control_event;
                res               = TRUE;
            }
        }
    }

    if (NULL != flow_control_pending_event)
    {
        *flow_control_pending_event = pending_event;
    }

    return res;
}


/*===========================================================================

    FUNCTION: core_flow_control_check_event

===========================================================================*/
/*!
    @brief
    checks what needs to be done for a new event

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
int core_flow_control_check_event
(
    core_handler_data_type *core_handler_data
)
{
    core_flow_control_event_type    *flow_control_event         = NULL;
    core_flow_control_event_type    *flow_control_pending_event = NULL;
    core_flow_control_message_type  *flow_control_message       = NULL;
    core_flow_control_message_type  *tmp_flow_control_message   = NULL;
    int                              drop_pending               = FALSE;
    int                              log_flags                  = 0;
    unsigned int                     flow_control_event_attr;
    int                              event_id;
    int                              any_already_awaiting_exec;
    int                              log_waiting_for_execution_exec_req_holder;
    int                              i;
    int                              message_id;

    cri_core_error_type              cri_error    = CRI_ERR_NONE_V01;
    core_flow_control_req_decision   result       = CORE_FLOW_CONTROL_REQ_DEC_INVALID;

    if ( core_handler_data->event_category != CORE_HANDLER_HLOS_REQUEST )
    {
        result = CORE_FLOW_CONTROL_REQ_DEC_PROCESS;
        return result;
    }

    do
    {
        event_id = core_get_event_id(core_handler_data);
        UTIL_LOG_MSG( "event id %d", event_id);

        util_hash_table_lock(core_flow_control_hash_table);

        flow_control_event = util_hash_table_lookup(core_flow_control_hash_table,
                                                    event_id);

        if (!flow_control_event)
        {
            result = CORE_FLOW_CONTROL_REQ_DEC_PROCESS;
            break;
        }

        flow_control_event_attr = flow_control_event->attributes;

        UTIL_LOG_MSG( "handling caps %x hex", flow_control_event_attr);

        /* Check for flow control exemption */
        if (flow_control_event_attr & CORE_FLOW_CONTROL_ATTR_FLOW_CONTROL_EXEMPT)
        {
            result = CORE_FLOW_CONTROL_REQ_DEC_PROCESS;
            log_flags |= LOG_ONREQUEST_FLOWCONTROL_EXEMPT;
            break;
        }

        any_already_awaiting_exec = core_flow_control_find_pending_request(flow_control_event,
                                                                           &flow_control_pending_event);

        UTIL_LOG_MSG( "already waiting for exec %d", any_already_awaiting_exec );

        /* Check if we should drop pending messages*/
        if ((flow_control_event_attr & CORE_FLOW_CONTROL_ATTR_MULTIPLE_AUTO_DROP_ON_DIFF_PARAMS) &&
             (NULL != flow_control_pending_event))
        {
            UTIL_LOG_MSG( "auto drop check" );
            if (flow_control_pending_event != flow_control_event)
            {
                drop_pending = TRUE;
            }
            else
            {
                for (i = 0; i < flow_control_event->max_size_of_list; i++)
                {
                    tmp_flow_control_message = util_list_find(flow_control_event->list_of_messages,
                                                               core_flow_control_find_next_list);
                    if ( tmp_flow_control_message )
                    {
                        drop_pending =
                             !flow_control_event->is_message_with_same_param(tmp_flow_control_message->core_handler_data,
                                                                             core_handler_data);
                    }
                }
            }
        }

        UTIL_LOG_MSG("Drop pending messages of similar type %d", drop_pending);
        if (drop_pending)
        {
            core_flow_control_delete_pending_messages_from_family(flow_control_event,
                                                                  FALSE);
            flow_control_pending_event = NULL;
        }

        /* Check if new request can accommodated. */
        if (flow_control_event->max_size_of_list <= flow_control_event->size_of_list)
        {
            cri_error = QMI_ERR_INTERNAL_V01;
            result    = CORE_FLOW_CONTROL_REQ_DEC_DELETE;
            log_flags |= LOG_ONREQUEST_FLOWCONTROL_EXEMPT;
            break;
        }

        /* Add to the list */
        core_flow_control_identifier++;

        // handles integer over flow
        if (core_flow_control_identifier == 0)
        {
            core_flow_control_identifier++;
        }

        flow_control_message = util_memory_alloc(sizeof(*flow_control_message));
        if (!flow_control_message)
        {
            cri_error = QMI_ERR_INTERNAL_V01;
            result    = CORE_FLOW_CONTROL_REQ_DEC_DELETE;
            log_flags |= LOG_ONREQUEST_FLOWCONTROL_EXEMPT;
            break;
        }
        else
        {
            result    = CORE_FLOW_CONTROL_REQ_DEC_PROCESS;
            log_flags |= LOG_ONREQUEST_DISPATCHED;
        }

        flow_control_message->identifier = core_flow_control_identifier;
        flow_control_message->core_handler_data = core_handler_data;
        util_list_enque(flow_control_event->list_of_messages,
                        flow_control_message, UTIL_LIST_BIT_FIELD_NONE);

        UTIL_LOG_MSG( "pending req kind %x hex, awaitingexec req kind %x hex",
                         flow_control_event, (uint32_t)flow_control_pending_event);

        if (NULL != flow_control_pending_event)
        {
            result    = CORE_FLOW_CONTROL_REQ_DEC_HOLD;
            log_flags |= LOG_ONREQUEST_FLOWCONTROL_DEFERRED_BUSY;
            if ( NULL != flow_control_pending_event)
            {
                message_id = flow_control_pending_event->message_id;
            }
            break;
        }

        UTIL_LOG_MSG( "token under exec %d", flow_control_event->identifier );

    } while (FALSE);

    util_hash_table_unlock(core_flow_control_hash_table);

    if (cri_error != CRI_ERR_NONE_V01 && tmp_flow_control_message)
    {
        hlos_core_send_response_handler(cri_error,
                                        tmp_flow_control_message->core_handler_data->event_data,
                                        NULL,
                                        0);
    }

    return result;
}


/*===========================================================================

    FUNCTION:  util_hash_table_compare_list_element_key

===========================================================================*/
/*!
    @brief
    Compare key with a list element

    @return
    TRUE or FALSE
*/
/*=========================================================================*/
int core_flow_control_compare_list_element_key
(
    const util_list_node_data_type *node_data,
    void *key
)
{

    int ret = FALSE;
    if (node_data)
    {
       if((int)key == (int)((hlos_core_hlos_request_data_type*)(((core_flow_control_message_type*) (node_data->user_data))->core_handler_data)->event_data)->event_id)
        {
            ret = TRUE;
        }
    }

    return ret;
}


/*===========================================================================

    FUNCTION: core_flow_control_inform_end_of_message

===========================================================================*/
/*!
    @brief
    Informs end of a message and deletes its entry and sends response

    @return
    SUCCESS or FAILURE
*/
/*=========================================================================*/
core_flow_control_resp_decision core_flow_control_inform_end_of_message
(
    core_handler_data_type *core_handler_data,
    void                   *resp_data,
    int                     resp_data_len
)
{
    core_flow_control_event_type    *flow_control_event         = NULL;
    core_flow_control_message_type  *flow_control_message       = NULL;
    core_flow_control_message_type  *tmp_flow_control_message   = NULL;
    int                               log_flags                  = 0;
    unsigned int                      flow_control_event_attr;
    int                               i;
    int                               event_id;
    int                               same_param;
    util_list_node_data_type         *holder_candidate = NULL;
    util_list_node_data_type         *event_node = NULL;

    core_flow_control_resp_decision  result    = CORE_FLOW_CONTROL_RESP_DEC_PROCESSED;
    cri_core_error_type               cri_error = CRI_ERR_NONE_V01;

    do
    {
        event_id = core_get_event_id(core_handler_data);
        UTIL_LOG_MSG( "event id %d", event_id);
        util_hash_table_lock(core_flow_control_hash_table);

        flow_control_event = util_hash_table_lookup(core_flow_control_hash_table,
                                                    event_id);

        if (!flow_control_event)
        {
            result = CORE_FLOW_CONTROL_RESP_DEC_INVALID;
            break;
        }

        flow_control_event_attr = flow_control_event->attributes;

        UTIL_LOG_MSG( "handling caps %x hex", flow_control_event_attr);

        /* Check for flow control exemption */
        if (flow_control_event_attr & CORE_FLOW_CONTROL_ATTR_FLOW_CONTROL_EXEMPT)
        {
            result = CORE_FLOW_CONTROL_RESP_DEC_EXEMPT;
            log_flags |= LOG_ONREQUEST_FLOWCONTROL_EXEMPT;
            break;
        }

        /* retrieve message from the flow control queue */
        event_node =
                           util_list_find_data_in_list_with_param(flow_control_event->list_of_messages,
                                                                  core_flow_control_compare_list_element_key,
                                                                  event_id);
        if (event_node)
        {
            flow_control_message = event_node->user_data;
        }
        else
        {
            result = CORE_FLOW_CONTROL_RESP_DEC_INVALID;
            break;
        }

        // call response handler helper
        hlos_core_send_response_handler(cri_error,
                                        flow_control_message->core_handler_data->event_data,
                                        resp_data,
                                        resp_data_len);

        /* check for other messages of same type */
        if ((flow_control_event_attr & CORE_FLOW_CONTROL_ATTR_ALLOW_MULTIPLE) &&
             !(flow_control_event_attr & CORE_FLOW_CONTROL_ATTR_NO_AUTO_RESPONSE) )
        {

            holder_candidate = util_list_find(flow_control_event->list_of_messages,
                                              core_flow_control_find_next_list);

            for (i = 0; i < flow_control_event->max_size_of_list; i++)
            {
                if ( holder_candidate == NULL )
                {
                    break;
                }
                tmp_flow_control_message = holder_candidate->user_data;
                if (tmp_flow_control_message != flow_control_message)
                {
                    same_param =
                         flow_control_event->is_message_with_same_param(tmp_flow_control_message->core_handler_data,
                                                                         core_handler_data);
                    if (same_param)
                    {
                        /* call response handler helper */
                        hlos_core_send_response_handler(cri_error,
                                                        tmp_flow_control_message->core_handler_data->event_data,
                                                        resp_data,
                                                        resp_data_len);

                        util_list_delete(flow_control_event->list_of_messages,
                                         holder_candidate, NULL);
                    }
                }
                holder_candidate = util_list_retrieve_successor(flow_control_event->list_of_messages,
                                                                holder_candidate);
            }
        }

        // clean up
        util_list_delete(flow_control_event->list_of_messages,
                         event_node, NULL);

    } while (FALSE);

    util_hash_table_unlock(core_flow_control_hash_table);

    return result;
}
