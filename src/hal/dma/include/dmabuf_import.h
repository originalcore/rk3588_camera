/*******************************************************************************
  * @file        dmabuf_import.h
  * @brief       h File
  * @author      weihui.jia
  * @attention
  * @brief       DMA-BUF import helpers
  * @history
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/


#ifndef __DMABUF_IMPORT_H_
#define __DMABUF_IMPORT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

typedef struct
{
    int fd;
    void *vaddr;
    size_t size;
} DMABufMapping;

int dmabuf_import_map(
    int dma_fd,
    size_t size,
    DMABufMapping *mapping);

void dmabuf_import_unmap(
    DMABufMapping *mapping);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
