/*******************************************************************************
  * @file        dmabuf_import.h
  * @brief       DMA-BUF import helpers
*******************************************************************************/

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
