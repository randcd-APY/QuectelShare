/*
 * Copyright (c) 2017 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <glib.h>

typedef struct BufQueue BufQueue;

struct BufQueue
{
  GMutex mutex;
  GCond condition;
  GList *head;
  GList *tail;
  guint length;
  gboolean enabled;
};

BufQueue *buf_queue_new (void);
void buf_queue_free (BufQueue * queue);
void buf_queue_push (BufQueue * queue, gpointer data);
gpointer buf_queue_pop (BufQueue * queue);
void buf_queue_flush (BufQueue * queue);

