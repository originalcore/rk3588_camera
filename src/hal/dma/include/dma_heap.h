/*******************************************************************************
  * @file        dma_heap.h
  * @brief       h File
  * @author      weihui.jia
  * @attention
  * @brief       DMA heap allocation interface
  * @history
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-31  V1.0.0  weihui.jia Create
  * 
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/


#ifndef __DMA_HEAP_H_
#define __DMA_HEAP_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

typedef struct
{
    int fd;
    void *vaddr;
    size_t size;
} DMAHeapBuffer;

int dma_heap_alloc(
    const char *heap_name,
    size_t size,
    DMAHeapBuffer *buffer);

void dma_heap_free(
    DMAHeapBuffer *buffer);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
