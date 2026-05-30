/*******************************************************************************
  * @file        camera_manager.c
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

#include "camera_manager.h"

int camera_manager_add(
        CameraManager *mgr,
        Camera *cam)
{
    if (!mgr ||
        !cam ||
        mgr->camera_count >= MAX_CAMERA_COUNT)
        return -1;

    mgr->cameras[mgr->camera_count++] =
        cam;

    return 0;
}

Camera *camera_manager_get(
        CameraManager *mgr,
        int index)
{
    if (!mgr ||
        index < 0 ||
        index >= mgr->camera_count)
        return NULL;

    return mgr->cameras[index];
}
