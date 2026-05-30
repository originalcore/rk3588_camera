/*******************************************************************************
  * @file        frame_listener.h
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

#ifndef __FRAME_LISTENER_H_
#define __FRAME_LISTENER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "frame.h"

typedef struct FrameListener FrameListener;

struct FrameListener
{

    void (*on_frame)(
        FrameListener *listener,
        Frame *frame);

    void *private_data;
};



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

