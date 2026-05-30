/*******************************************************************************
  * @file        dmabuf_export.h
  * @brief       DMA-BUF export helpers
*******************************************************************************/

#ifndef __DMABUF_EXPORT_H_
#define __DMABUF_EXPORT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int dmabuf_export_dup(
    int dma_fd);

void dmabuf_export_close(
    int dma_fd);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
