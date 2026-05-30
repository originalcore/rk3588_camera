/*******************************************************************************
  * @file        thread.h
  * @brief       Thread wrapper
*******************************************************************************/

#ifndef __THREAD_H_
#define __THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <pthread.h>

typedef void *(*CameraThreadRoutine)(void *arg);

typedef struct
{
    pthread_t tid;
    int running;
} CameraThread;

int camera_thread_start(
    CameraThread *thread,
    CameraThreadRoutine routine,
    void *arg);

int camera_thread_join(
    CameraThread *thread);

int camera_thread_is_running(
    const CameraThread *thread);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
