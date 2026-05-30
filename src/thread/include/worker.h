/*******************************************************************************
  * @file        worker.h
  * @brief       Thread pool worker entry
*******************************************************************************/

#ifndef __WORKER_H_
#define __WORKER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void *thread_pool_worker_main(
    void *arg);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
