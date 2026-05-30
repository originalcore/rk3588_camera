/*******************************************************************************
  * @file        frame.h
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

#ifndef __FRAME_H_
#define __FRAME_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>
#include <stdint.h>

typedef struct
{

    void *data;

    size_t size;

    int width;

    int height;

    uint32_t pixfmt;

    uint64_t timestamp;

    int dma_fd;

    int index;

} Frame;



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
