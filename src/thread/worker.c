/*******************************************************************************
  * @file        worker.c
  * @brief       Thread pool worker entry
*******************************************************************************/

#include "worker.h"
#include "thread_pool.h"

void *thread_pool_worker_main(
    void *arg)
{
    ThreadPool *pool = arg;

    while (1)
    {
        ThreadPoolJob job;

        pthread_mutex_lock(&pool->mutex);

        while (!pool->stop &&
               pool->read_idx == pool->write_idx)
            pthread_cond_wait(&pool->cond, &pool->mutex);

        if (pool->stop &&
            pool->read_idx == pool->write_idx)
        {
            pthread_mutex_unlock(&pool->mutex);
            break;
        }

        job = pool->job[pool->read_idx];
        pool->read_idx =
            (pool->read_idx + 1) % THREAD_POOL_MAX_JOBS;

        pthread_mutex_unlock(&pool->mutex);

        job.func(job.arg);
    }

    return NULL;
}
