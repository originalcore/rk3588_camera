/*******************************************************************************
  * @file        thread_pool.h
  * @brief       Fixed-size thread pool
*******************************************************************************/

#ifndef __THREAD_POOL_H_
#define __THREAD_POOL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <pthread.h>

#define THREAD_POOL_MAX_WORKERS 8
#define THREAD_POOL_MAX_JOBS 64

typedef void (*ThreadPoolJobFunc)(void *arg);

typedef struct
{
    ThreadPoolJobFunc func;
    void *arg;
} ThreadPoolJob;

typedef struct
{
    pthread_t thread[THREAD_POOL_MAX_WORKERS];
    ThreadPoolJob job[THREAD_POOL_MAX_JOBS];
    int worker_count;
    int read_idx;
    int write_idx;
    int stop;
    int mutex_initialized;
    int cond_initialized;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ThreadPool;

int thread_pool_init(
    ThreadPool *pool,
    int worker_count);

int thread_pool_submit(
    ThreadPool *pool,
    ThreadPoolJobFunc func,
    void *arg);

void thread_pool_stop(
    ThreadPool *pool);

void thread_pool_deinit(
    ThreadPool *pool);

int thread_pool_is_full(
    ThreadPool *pool);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
