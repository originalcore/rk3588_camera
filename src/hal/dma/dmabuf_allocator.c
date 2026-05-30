/*******************************************************************************
  * @file        dmabuf_allocator.c
  * @brief       c File
  * @author      weihui.jia
  * @attention
  * @history
  * Date       Version Author     Description
  * ========== ======= =========  ==============================================
  * 2026-5-30  V1.0.0  weihui.jia Create
  *
  * @Copyright (C)  2026 originl.Co.Ltd all right reserved
*******************************************************************************/
/*@{*/

#include "dmabuf_allocator.h"
#include "dma_heap.h"

#include <string.h>

int dmabuf_allocate(
    DMABuffer *buf,
    size_t size)
{
    DMAHeapBuffer heap_buffer;

    if (!buf)
        return -1;

    memset(buf, 0, sizeof(*buf));
    buf->fd = -1;

    if (dma_heap_alloc(NULL, size, &heap_buffer) < 0)
        return -1;

    buf->fd = heap_buffer.fd;
    buf->vaddr = heap_buffer.vaddr;
    buf->size = heap_buffer.size;

    return 0;
}

void dmabuf_release(DMABuffer *buf)
{
    DMAHeapBuffer heap_buffer;

    if (!buf)
        return;

    heap_buffer.fd = buf->fd;
    heap_buffer.vaddr = buf->vaddr;
    heap_buffer.size = buf->size;

    dma_heap_free(&heap_buffer);

    buf->fd = -1;
    buf->vaddr = NULL;
    buf->size = 0;
}
