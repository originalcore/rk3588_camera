/*******************************************************************************
  * @file        media_controller.h
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

#ifndef __MEDIA_CONTROLLER_H_
#define __MEDIA_CONTROLLER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{

    int media_fd;

} MediaController;

int media_controller_open(
    MediaController *mc,
    const char *dev);

int media_controller_setup_link(
    MediaController *mc);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
