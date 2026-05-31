/*******************************************************************************
  * @file        dmabuf_export.h
  * @brief       h File
  * @author      weihui.jia
  * @attention
  * @brief       DMA-BUF export helpers
  * @history
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/


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
