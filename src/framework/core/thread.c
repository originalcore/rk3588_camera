/*******************************************************************************
  * @file        thread.c
  * @brief       Thread wrapper
*******************************************************************************/

#include "thread.h"

#include <string.h>

int camera_thread_start(
    CameraThread *thread,
    CameraThreadRoutine routine,
    void *arg)
{
    if (!thread ||
        !routine ||
        thread->running)
        return -1;

    memset(thread, 0, sizeof(*thread));

    if (pthread_create(&thread->tid, NULL, routine, arg) != 0)
        return -1;

    thread->running = 1;

    return 0;
}

int camera_thread_join(
    CameraThread *thread)
{
    if (!thread ||
        !thread->running)
        return 0;

    if (pthread_join(thread->tid, NULL) != 0)
        return -1;

    thread->running = 0;

    return 0;
}

int camera_thread_is_running(
    const CameraThread *thread)
{
    return thread && thread->running;
}
