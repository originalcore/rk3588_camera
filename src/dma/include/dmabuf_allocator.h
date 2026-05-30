/*******************************************************************************
  * @file        dmabuf_allocator.h
  * @brief       h File
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


#ifndef __DMABUF_ALLOCATOR_H_
#define __DMABUF_ALLOCATOR_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

typedef struct
{

    int fd;

    void *vaddr;

    size_t size;

} DMABuffer;

int dmabuf_allocate(
    DMABuffer *buf,
    size_t size);

void dmabuf_release(
    DMABuffer *buf);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

