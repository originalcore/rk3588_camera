/*******************************************************************************
  * @file        dmabuf_sync.h
  * @brief       h File
  * @author      weihui.jia
  * @attention
  * @brief       DMA-BUF CPU/device synchronization
  * @history
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/


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
