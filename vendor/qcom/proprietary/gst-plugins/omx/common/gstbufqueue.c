/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <glib.h>

#include "gstbufqueue.h"

BufQueue *
buf_queue_new (void)
{
  BufQueue *queue;

  queue = g_slice_new0(BufQueue);

  g_cond_init(&queue->condition);
  g_mutex_init(&queue->mutex);
  queue->enabled = TRUE;

  return queue;
}

void
buf_queue_free (BufQueue * queue)
{
  g_cond_clear(&queue->condition);
  g_mutex_clear(&queue->mutex);

  g_list_free(queue->head);
  g_slice_free(BufQueue, queue);
}

void
buf_queue_push (BufQueue * queue, gpointer data)
{
  g_mutex_lock (&queue->mutex);

  queue->head = g_list_prepend (queue->head, data);
  if (!queue->tail)
    queue->tail = queue->head;
  queue->length++;

  g_cond_signal (&queue->condition);

  g_mutex_unlock (&queue->mutex);
}

gpointer
buf_queue_pop (BufQueue * queue)
{
  gpointer data = NULL;

  g_mutex_lock (&queue->mutex);

  if (!queue->enabled) {
    /* g_warning ("not enabled!"); */
    goto leave;
  }

  if (!queue->tail) {
    g_cond_wait (&queue->condition, &queue->mutex);
  }

  if (queue->tail) {
    GList *node = queue->tail;
    data = node->data;

    queue->tail = node->prev;
    if (queue->tail)
      queue->tail->next = NULL;
    else
      queue->head = NULL;
    queue->length--;
    g_list_free_1 (node);
  }

leave:
  g_mutex_unlock (&queue->mutex);

  return data;
}

void
buf_queue_flush (BufQueue * queue)
{
  g_mutex_lock (&queue->mutex);
  g_list_free (queue->head);
  queue->head = queue->tail = NULL;
  queue->length = 0;
  g_mutex_unlock (&queue->mutex);
}

