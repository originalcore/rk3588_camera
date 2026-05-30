/*******************************************************************************
  * @file        camera_manager.h
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


#ifndef __CAMERA_MANAGER_H_
#define __CAMERA_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "camera.h"

#define MAX_CAMERA_COUNT 8

typedef struct
{

    Camera *cameras[MAX_CAMERA_COUNT];

    int camera_count;

} CameraManager;

CameraManager *camera_manager_create(void);

void camera_manager_destroy(    CameraManager *mgr);

int camera_manager_add(    CameraManager *mgr,    Camera *cam);

Camera *camera_manager_get(CameraManager *mgr,    int index);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
