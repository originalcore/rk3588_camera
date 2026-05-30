/*******************************************************************************
  * @file        thread_pool.c
  * @brief       Fixed-size thread pool
*******************************************************************************/

#include "thread_pool.h"
#include "worker.h"

#include <string.h>

static int thread_pool_next_index(
    int index)
{
    return (index + 1) % THREAD_POOL_MAX_JOBS;
}

int thread_pool_init(
    ThreadPool *pool,
    int worker_count)
{
    if (!pool ||
        worker_count <= 0 ||
        worker_count > THREAD_POOL_MAX_WORKERS)
        return -1;

    memset(pool, 0, sizeof(*pool));

    if (pthread_mutex_init(&pool->mutex, NULL) != 0)
        return -1;
    pool->mutex_initialized = 1;

    if (pthread_cond_init(&pool->cond, NULL) != 0)
    {
        pthread_mutex_destroy(&pool->mutex);
        pool->mutex_initialized = 0;
        return -1;
    }
    pool->cond_initialized = 1;

    for (int i = 0; i < worker_count; i++)
    {
        if (pthread_create(&pool->thread[i],
                           NULL,
                           thread_pool_worker_main,
                           pool) != 0)
        {
            thread_pool_stop(pool);
            thread_pool_deinit(pool);
            return -1;
        }

        pool->worker_count++;
    }

    return 0;
}

int thread_pool_submit(
    ThreadPool *pool,
    ThreadPoolJobFunc func,
    void *arg)
{
    int next_idx;

    if (!pool ||
        !func)
        return -1;

    if (!pool->mutex_initialized ||
        !pool->cond_initialized)
        return -1;

    pthread_mutex_lock(&pool->mutex);

    if (pool->stop)
    {
        pthread_mutex_unlock(&pool->mutex);
        return -1;
    }

    next_idx = thread_pool_next_index(pool->write_idx);
    if (next_idx == pool->read_idx)
    {
        pthread_mutex_unlock(&pool->mutex);
        return -1;
    }

    pool->job[pool->write_idx].func = func;
    pool->job[pool->write_idx].arg = arg;
    pool->write_idx = next_idx;

    pthread_cond_signal(&pool->cond);
    pthread_mutex_unlock(&pool->mutex);

    return 0;
}

void thread_pool_stop(
    ThreadPool *pool)
{
    if (!pool)
        return;

    pthread_mutex_lock(&pool->mutex);
    pool->stop = 1;
    pthread_cond_broadcast(&pool->cond);
    pthread_mutex_unlock(&pool->mutex);

    for (int i = 0; i < pool->worker_count; i++)
        pthread_join(pool->thread[i], NULL);

    pool->worker_count = 0;
}

void thread_pool_deinit(
    ThreadPool *pool)
{
    if (!pool)
        return;

    if (pool->worker_count > 0)
        thread_pool_stop(pool);

    if (pool->cond_initialized)
    {
        pthread_cond_destroy(&pool->cond);
        pool->cond_initialized = 0;
    }

    if (pool->mutex_initialized)
    {
        pthread_mutex_destroy(&pool->mutex);
        pool->mutex_initialized = 0;
    }
}

int thread_pool_is_full(
    ThreadPool *pool)
{
    int full;

    if (!pool)
        return 0;

    pthread_mutex_lock(&pool->mutex);
    full = thread_pool_next_index(pool->write_idx) == pool->read_idx;
    pthread_mutex_unlock(&pool->mutex);

    return full;
}
