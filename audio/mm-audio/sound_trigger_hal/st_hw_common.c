/* st_hw_common.c
 *
 * This library contains common functionality between
 * sound trigger hw and sound trigger extension interface.

 * Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#define LOG_TAG "sound_trigger_hw_common"
/* #define LOG_NDEBUG 0 */
#define LOG_NDDEBUG 0

/* #define VERY_VERBOSE_LOGGING */
#ifdef VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(a...) do { } while(0)
#endif

#include <cutils/log.h>
#include <hardware/sound_trigger.h>

#include "st_hw_common.h"

int stop_other_sessions(struct sound_trigger_device *stdev,
                         st_session_t *cur_ses)
{
    st_session_t *p_ses;
    struct listnode *p_ses_node;
    int status = 0;

    ALOGV("%s: list empty %s", __func__,
          list_empty(&stdev->sound_model_list) ? "true" : "false");

    if (stdev->audio_concurrency_active)
        return 0;

    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, st_session_t, list_node);

        /* Current session can already be in the list during SSR */
        if (p_ses == cur_ses)
            continue;

        status = st_session_pause(p_ses);
        if (status) {
            ALOGE("%s: error stopping session", __func__);
            return -EIO;
        }
    }
    return 0;
}

int start_other_sessions(struct sound_trigger_device *stdev,
                         st_session_t *cur_ses)
{
    st_session_t *p_ses;
    struct listnode *p_ses_node;
    int status = 0;

    ALOGV("%s: list empty %s", __func__,
       list_empty(&stdev->sound_model_list) ? "true" : "false");

    if (stdev->audio_concurrency_active)
        return 0;

    list_for_each(p_ses_node, &stdev->sound_model_list) {
        p_ses = node_to_item(p_ses_node, st_session_t, list_node);

        /* Current session can already be in the list during SSR */
        if (p_ses == cur_ses)
            continue;
        status = st_session_resume(p_ses);
        if (status) {
            ALOGE("%s: error restarting session", __func__);
            return -EIO;
        }
    }
    return 0;
}

st_session_t* get_sound_trigger_session(
                    struct sound_trigger_device *stdev,
                    sound_model_handle_t sound_model_handle)

{
    st_session_t *st_session = NULL;
    struct listnode *node;

    list_for_each(node, &stdev->sound_model_list) {
        st_session = node_to_item(node, st_session_t, list_node);
        if (st_session->sm_handle == sound_model_handle)
            return st_session;

    }
    return NULL;
}

/* ---------------- hw session notify thread --------------- */
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

typedef struct {
    sound_model_handle_t handle;
    st_session_event_id_t event;
    struct listnode node; /* membership in queue or pool */
    uint64_t when;
} request_t;

static struct {
    pthread_t thread;
    bool done;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    struct listnode queue;
    struct listnode pool;
    request_t requests[32];
    hw_session_notify_callback_t callback;
    bool inited;
} hw_session_notifier;

static inline uint64_t now()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000000000LL) + ts.tv_nsec;
}

static inline int hw_session_notifier_lock()
{
    return pthread_mutex_lock(&hw_session_notifier.lock);
}

static inline int hw_session_notifier_unlock()
{
    return pthread_mutex_unlock(&hw_session_notifier.lock);
}

static inline int hw_session_notifier_wait_l()
{
    int ret = -EINVAL;

    if (list_empty(&hw_session_notifier.queue)) {
        pthread_cond_wait(&hw_session_notifier.cond,
                          &hw_session_notifier.lock);
        return EINTR; /* wait can only be interrupted */
    }

    struct listnode *node = list_head(&hw_session_notifier.queue);
    request_t *r = node_to_item(node, request_t, node);
    int64_t interval = r->when - now();

    if (interval <= 0) {
        ALOGV("early exit\n");
        return 0;
    }

    struct timespec timeout;
    GET_WAIT_TIMESPEC(timeout, interval);
    ret = pthread_cond_timedwait(&hw_session_notifier.cond,
                                 &hw_session_notifier.lock, &timeout);
    switch (ret) {
        case 0:
            ret = EINTR; /* wait was interrupted */
            break;
        case ETIMEDOUT:
            ret = 0; /* time passed expired, proceed with removing first entry from list */
            break;
        default:
            ALOGE("%s: wait failed w/ ret %s\n", __func__, strerror(ret));
            break;
    }
    return ret;
}

static inline int hw_session_notifier_signal()
{
    return pthread_cond_signal(&hw_session_notifier.cond);
}

int hw_session_notifier_enqueue(sound_model_handle_t handle,
                                st_session_event_id_t event,
                                uint64_t delay_ms)
{
    int ret = 0;

    if (!hw_session_notifier.inited)
        return -ENODEV;

    hw_session_notifier_lock();
    if (list_empty(&hw_session_notifier.pool)) {
        ALOGE("%s: No space to queue request, try again", __func__);
        ret = -EAGAIN;
        goto exit;
    }

    struct listnode *req_node = list_head(&hw_session_notifier.pool);
    list_remove(req_node);
    request_t *r = node_to_item(req_node, request_t, node);
    r->handle = handle;
    r->event = event;
    r->when = now() + delay_ms * 1000000LL;

    struct listnode *node = list_head(&hw_session_notifier.queue);
    while (node != list_tail(&hw_session_notifier.queue)) {
        request_t *er = node_to_item(node, request_t, node);
        if (r->when >= er->when) {
            /* continue to next node */
            node = list_head(node);
        } else {
            /* insert the element before this node */
            break;
        }
    }
    /*
     * queue element before element "node". This is implicitly same
     * as list->push_back if the element has to be pushed to the
     * end of the list as in that case, node == head
     */
    list_add_tail(node, req_node);
    hw_session_notifier_signal();
exit:
    hw_session_notifier_unlock();
    return ret;
}

int hw_session_notifier_cancel(sound_model_handle_t handle,
                               st_session_event_id_t event) {
    int ret = -1;
    struct listnode *node, *tmp_node;

    if (!hw_session_notifier.inited)
        return -1;

    hw_session_notifier_lock();
    list_for_each_safe(node, tmp_node, &hw_session_notifier.queue) {
        request_t *r = node_to_item(node, request_t, node);
        if (r->handle == handle && r->event == event) {
            ALOGV("%s: found req with handle %d and ev %d to cancel",
                  __func__, handle, event);
            r->handle = -1;
            list_remove(node);
            list_add_tail(&hw_session_notifier.pool, node);
            ret = 0;
        }
    }
    hw_session_notifier_signal();
    hw_session_notifier_unlock();
    return ret;
}

static void hw_session_notify_process_once_l() {
    struct listnode *req_node = list_head(&hw_session_notifier.queue);
    request_t *r = node_to_item(req_node, request_t, node);

    list_remove(req_node);
    hw_session_notifier_unlock();
    switch (r->event) {
        case ST_SES_EV_DEFERRED_STOP:
            ALOGI("%s:[%d] hw notify deferred stop", __func__, r->handle);
            hw_session_notifier.callback(r->handle, ST_SES_EV_DEFERRED_STOP);
            break;
        default:
            break;
    }
    hw_session_notifier_lock();
    list_add_tail(&hw_session_notifier.pool, req_node);
}

void *hw_session_notifier_loop(void *arg __unused) {
    hw_session_notifier_lock();
    while (!hw_session_notifier.done) {
        int ret = hw_session_notifier_wait_l();
        switch (ret) {
            case 0: /* timer expired */
                hw_session_notify_process_once_l();
                break;
            case EINTR: /* timer interrupted due to next enqueue or some other event */
                continue;
            default:
                ALOGE("%s: wait_l returned err %d, exit loop!", __func__, ret);
                hw_session_notifier.done =  true;
                break;
        }
    }
    hw_session_notifier_unlock();
    ALOGI("%s thread loop exiting", __func__);
    return NULL;
}

int hw_session_notifier_init(hw_session_notify_callback_t cb) {
    pthread_attr_t attr;
    pthread_condattr_t c_attr;

    if (hw_session_notifier.inited)
        return -EINVAL;

    hw_session_notifier.inited = false;
    hw_session_notifier.done = false;

    pthread_mutex_init(&hw_session_notifier.lock, NULL);
    pthread_condattr_init(&c_attr);
    pthread_condattr_setclock(&c_attr, CLOCK_MONOTONIC);
    pthread_cond_init(&hw_session_notifier.cond, &c_attr);
    pthread_condattr_destroy(&c_attr);

    list_init(&hw_session_notifier.queue);
    list_init(&hw_session_notifier.pool);
    for (uint32_t i = 0; i < ARRAY_SIZE(hw_session_notifier.requests); i++) {
        list_init(&hw_session_notifier.requests[i].node);
        hw_session_notifier.requests[i].handle = -1;
        list_add_tail(&hw_session_notifier.pool,
                      &hw_session_notifier.requests[i].node);
    }
    pthread_attr_init(&attr);
    if (pthread_create(&hw_session_notifier.thread, &attr,
                       hw_session_notifier_loop, NULL)) {
        ALOGE("%s: Failed to create hw_notify thread w/ err %s",
              __func__,
              strerror(errno));
        return -1;
    }
    hw_session_notifier.callback = cb;
    hw_session_notifier.inited = true;
    ALOGV("%s: completed", __func__);
    return 0;
}

void hw_session_notifier_deinit()
{
    if (hw_session_notifier.inited) {
        hw_session_notifier_lock();
        hw_session_notifier.done = true;
        hw_session_notifier_signal();
        hw_session_notifier_unlock();
        pthread_join(hw_session_notifier.thread, NULL);
        hw_session_notifier.inited = false;
        ALOGV("%s: completed", __func__);
    }
}
