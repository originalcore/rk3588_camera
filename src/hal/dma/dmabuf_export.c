/*******************************************************************************
  * @file        dmabuf_export.c
  * @brief       DMA-BUF export helpers
*******************************************************************************/

#include "dmabuf_export.h"

#include <unistd.h>

int dmabuf_export_dup(
    int dma_fd)
{
    if (dma_fd < 0)
        return -1;

    return dup(dma_fd);
}

void dmabuf_export_close(
    int dma_fd)
{
    if (dma_fd >= 0)
        close(dma_fd);
}
