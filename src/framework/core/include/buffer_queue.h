/*******************************************************************************
  * @file        buffer_queue.h
  * @brief       Frame queue for asynchronous pipeline processing
*******************************************************************************/

#ifndef __BUFFER_QUEUE_H_
#define __BUFFER_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "frame.h"

#include <pthread.h>

#define FRAME_QUEUE_CAPACITY 32

typedef struct
{
    Frame frame[FRAME_QUEUE_CAPACITY];
    int read_idx;
    int write_idx;
    pthread_mutex_t mutex;
} FrameQueue;

int frame_queue_init(
    FrameQueue *queue);

void frame_queue_deinit(
    FrameQueue *queue);

void frame_queue_reset(
    FrameQueue *queue);

int frame_queue_push(
    FrameQueue *queue,
    const Frame *frame);

int frame_queue_pop(
    FrameQueue *queue,
    Frame *frame);

int frame_queue_is_empty(
    FrameQueue *queue);

int frame_queue_is_full(
    FrameQueue *queue);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
