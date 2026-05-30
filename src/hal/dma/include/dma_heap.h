/*******************************************************************************
  * @file        dma_heap.h
  * @brief       DMA heap allocation interface
*******************************************************************************/

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
