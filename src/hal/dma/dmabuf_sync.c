/*******************************************************************************
  * @file        dmabuf_sync.c
  * @brief       DMA-BUF CPU/device synchronization
*******************************************************************************/

#include "dmabuf_sync.h"

#include <linux/dma-buf.h>
#include <string.h>
#include <sys/ioctl.h>

static int dmabuf_sync(
    int dma_fd,
    __u64 flags)
{
    struct dma_buf_sync sync;

    if (dma_fd < 0)
        return -1;

    memset(&sync, 0, sizeof(sync));
    sync.flags = flags;

    return ioctl(dma_fd, DMA_BUF_IOCTL_SYNC, &sync);
}

int dmabuf_sync_start_read(
    int dma_fd)
{
    return dmabuf_sync(dma_fd,
                       DMA_BUF_SYNC_START | DMA_BUF_SYNC_READ);
}

int dmabuf_sync_end_read(
    int dma_fd)
{
    return dmabuf_sync(dma_fd,
                       DMA_BUF_SYNC_END | DMA_BUF_SYNC_READ);
}

int dmabuf_sync_start_write(
    int dma_fd)
{
    return dmabuf_sync(dma_fd,
                       DMA_BUF_SYNC_START | DMA_BUF_SYNC_WRITE);
}

int dmabuf_sync_end_write(
    int dma_fd)
{
    return dmabuf_sync(dma_fd,
                       DMA_BUF_SYNC_END | DMA_BUF_SYNC_WRITE);
}
