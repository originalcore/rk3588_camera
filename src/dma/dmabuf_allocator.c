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

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>

int dmabuf_allocate(
    DMABuffer *buf,
    size_t size)
{
    buf->size = size;

    buf->fd = -1;

    buf->vaddr = mmap(NULL,
                      size,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS,
                      -1,
                      0);

    if (buf->vaddr == MAP_FAILED)
        return -1;

    return 0;
}

void dmabuf_release(DMABuffer *buf)
{
    munmap(buf->vaddr, buf->size);
}

