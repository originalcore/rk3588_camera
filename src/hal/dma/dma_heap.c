/*******************************************************************************
  * @file        dma_heap.c
  * @brief       c File
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

#include "dma_heap.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/dma-heap.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#ifndef DMA_HEAP_DEFAULT_NAME
#define DMA_HEAP_DEFAULT_NAME "/dev/dma_heap/system"
#endif

static const char *dma_heap_name_or_default(
    const char *heap_name)
{
    if (heap_name &&
        heap_name[0] != '\0')
        return heap_name;

    return DMA_HEAP_DEFAULT_NAME;
}

int dma_heap_alloc(
    const char *heap_name,
    size_t size,
    DMAHeapBuffer *buffer)
{
    struct dma_heap_allocation_data data;
    const char *name;
    int heap_fd;

    if (!buffer ||
        size == 0)
        return -1;

    memset(buffer, 0, sizeof(*buffer));
    buffer->fd = -1;
    name = dma_heap_name_or_default(heap_name);

    heap_fd = open(name, O_RDONLY | O_CLOEXEC);
    if (heap_fd < 0)
        return -1;

    memset(&data, 0, sizeof(data));
    data.len = size;
    data.fd_flags = O_RDWR | O_CLOEXEC;

    if (ioctl(heap_fd, DMA_HEAP_IOCTL_ALLOC, &data) < 0)
    {
        close(heap_fd);
        return -1;
    }

    close(heap_fd);

    buffer->fd = data.fd;
    buffer->size = size;
    buffer->vaddr = mmap(NULL,
                         size,
                         PROT_READ | PROT_WRITE,
                         MAP_SHARED,
                         buffer->fd,
                         0);
    if (buffer->vaddr == MAP_FAILED)
    {
        close(buffer->fd);
        buffer->fd = -1;
        buffer->vaddr = NULL;
        buffer->size = 0;
        errno = ENOMEM;
        return -1;
    }

    return 0;
}

void dma_heap_free(
    DMAHeapBuffer *buffer)
{
    if (!buffer)
        return;

    if (buffer->vaddr)
        munmap(buffer->vaddr, buffer->size);

    if (buffer->fd >= 0)
        close(buffer->fd);

    buffer->fd = -1;
    buffer->vaddr = NULL;
    buffer->size = 0;
}
