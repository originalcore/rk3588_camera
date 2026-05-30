/*******************************************************************************
  * @file        dmabuf_import.c
  * @brief       DMA-BUF import helpers
*******************************************************************************/

#include "dmabuf_import.h"

#include <string.h>
#include <sys/mman.h>

int dmabuf_import_map(
    int dma_fd,
    size_t size,
    DMABufMapping *mapping)
{
    if (!mapping ||
        dma_fd < 0 ||
        size == 0)
        return -1;

    memset(mapping, 0, sizeof(*mapping));
    mapping->fd = dma_fd;
    mapping->size = size;
    mapping->vaddr = mmap(NULL,
                          size,
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED,
                          dma_fd,
                          0);
    if (mapping->vaddr == MAP_FAILED)
    {
        mapping->vaddr = NULL;
        mapping->size = 0;
        mapping->fd = -1;
        return -1;
    }

    return 0;
}

void dmabuf_import_unmap(
    DMABufMapping *mapping)
{
    if (!mapping)
        return;

    if (mapping->vaddr)
        munmap(mapping->vaddr, mapping->size);

    mapping->vaddr = NULL;
    mapping->size = 0;
    mapping->fd = -1;
}
