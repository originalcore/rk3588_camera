/*******************************************************************************
  * @file        dmabuf_export.c
  * @brief       c File
  * @author      weihui.jia
  * @attention
  * @history
  * @brief       DMA-BUF export helpers
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/

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
