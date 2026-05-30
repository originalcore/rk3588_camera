/*******************************************************************************
  * @file        dmabuf_sync.h
  * @brief       DMA-BUF CPU/device synchronization
*******************************************************************************/

#ifndef __DMABUF_SYNC_H_
#define __DMABUF_SYNC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmabuf_sync_start_read(
    int dma_fd);

int dmabuf_sync_end_read(
    int dma_fd);

int dmabuf_sync_start_write(
    int dma_fd);

int dmabuf_sync_end_write(
    int dma_fd);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
