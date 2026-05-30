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

#include <stdlib.h>

CameraManager *camera_manager_create(void)
{
    return calloc(1, sizeof(CameraManager));
}

void camera_manager_destroy(        CameraManager *mgr)
{
    if (!mgr)
        return;

    for (int i = 0; i < mgr->camera_count; i++)
    {
        camera_destroy(mgr->cameras[i]);
        mgr->cameras[i] = NULL;
    }

    mgr->camera_count = 0;
    free(mgr);
}

int camera_manager_add(CameraManager *mgr, Camera *cam)
{
    if (!mgr || !cam || mgr->camera_count >= MAX_CAMERA_COUNT)
        return -1;

    mgr->cameras[mgr->camera_count++] = cam;

    return 0;
}

Camera *camera_manager_get(        CameraManager *mgr, int index)
{
    if (!mgr || index < 0 || index >= mgr->camera_count)
        return NULL;

    return mgr->cameras[index];
}
