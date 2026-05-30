/*******************************************************************************
  * @file        camera.h
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


#ifndef __CAMERA_H_
#define __CAMERA_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "frame.h"
#include "frame_listener.h"
#include "pipeline_node.h"
#include "v4l2_device.h"

#define MAX_FRAME_LISTENER 16

typedef struct
{

    V4L2Device device;

    FrameListener *listeners[MAX_FRAME_LISTENER];

    int listener_count;

    PipelineNode *pipeline;

} Camera;

Camera *camera_create(
    const char *dev);

void camera_destroy(
    Camera *cam);

int camera_open(
    Camera *cam,
    const char *dev);

int camera_start(
    Camera *cam);

int camera_poll(
    Camera *cam);

int camera_add_listener(
    Camera *cam,
    FrameListener *listener);

int camera_set_pipeline(
    Camera *cam,
    PipelineNode *node);

void camera_close(
    Camera *cam);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
