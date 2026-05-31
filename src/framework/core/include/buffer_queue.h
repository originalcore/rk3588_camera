/*******************************************************************************
  * @file        buffer_queue.h
  * @brief       h File
  * @author      weihui.jia
  * @attention
  * @history
  * @brief       Frame queue for asynchronous pipeline processing
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/


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
