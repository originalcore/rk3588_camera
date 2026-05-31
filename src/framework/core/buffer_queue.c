/*******************************************************************************
  * @file        buffer_queue.c
  * @brief       c File
  * @author      weihui.jia
  * @attention
  * @history
  * @brief      Frame queue for asynchronous pipeline processing
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/

#include "buffer_queue.h"

#include <string.h>

static int frame_queue_next_index(
    int index)
{
    return (index + 1) % FRAME_QUEUE_CAPACITY;
}

int frame_queue_init(
    FrameQueue *queue)
{
    if (!queue)
        return -1;

    memset(queue, 0, sizeof(*queue));

    if (pthread_mutex_init(&queue->mutex, NULL) != 0)
        return -1;

    return 0;
}

void frame_queue_deinit(
    FrameQueue *queue)
{
    if (!queue)
        return;

    pthread_mutex_destroy(&queue->mutex);
}

void frame_queue_reset(
    FrameQueue *queue)
{
    if (!queue)
        return;

    pthread_mutex_lock(&queue->mutex);
    queue->read_idx = 0;
    queue->write_idx = 0;
    pthread_mutex_unlock(&queue->mutex);
}

int frame_queue_push(
    FrameQueue *queue,
    const Frame *frame)
{
    int next_idx;

    if (!queue ||
        !frame)
        return -1;

    pthread_mutex_lock(&queue->mutex);

    next_idx = frame_queue_next_index(queue->write_idx);
    if (next_idx == queue->read_idx)
    {
        pthread_mutex_unlock(&queue->mutex);
        return -1;
    }

    queue->frame[queue->write_idx] = *frame;
    queue->write_idx = next_idx;

    pthread_mutex_unlock(&queue->mutex);

    return 0;
}

int frame_queue_pop(
    FrameQueue *queue,
    Frame *frame)
{
    if (!queue ||
        !frame)
        return -1;

    pthread_mutex_lock(&queue->mutex);

    if (queue->read_idx == queue->write_idx)
    {
        pthread_mutex_unlock(&queue->mutex);
        return -1;
    }

    *frame = queue->frame[queue->read_idx];
    queue->read_idx = frame_queue_next_index(queue->read_idx);

    pthread_mutex_unlock(&queue->mutex);

    return 0;
}

int frame_queue_is_empty(
    FrameQueue *queue)
{
    int empty;

    if (!queue)
        return 1;

    pthread_mutex_lock(&queue->mutex);
    empty = queue->read_idx == queue->write_idx;
    pthread_mutex_unlock(&queue->mutex);

    return empty;
}

int frame_queue_is_full(
    FrameQueue *queue)
{
    int full;

    if (!queue)
        return 0;

    pthread_mutex_lock(&queue->mutex);
    full = frame_queue_next_index(queue->write_idx) == queue->read_idx;
    pthread_mutex_unlock(&queue->mutex);

    return full;
}
