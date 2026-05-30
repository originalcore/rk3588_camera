/*******************************************************************************
  * @file        camera.c
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
#include "camera.h"
#include "pipeline.h"

#include <linux/videodev2.h>
#include <stdlib.h>

static void camera_dispatch_frame(
    Camera *cam,
    Frame *frame)
{
    for (int i = 0;
         i < cam->listener_count;
         i++)
    {

        cam->listeners[i]->on_frame(
            cam->listeners[i],
            frame);
    }
}

int camera_add_listener(
    Camera *cam,
    FrameListener *listener)
{
    if (!cam ||
        !listener ||
        cam->listener_count >= MAX_FRAME_LISTENER)
        return -1;

    cam->listeners[cam->listener_count++] =
        listener;

    return 0;
}

int camera_set_pipeline(
    Camera *cam,
    PipelineNode *node)
{
    if (!cam)
        return -1;

    if (cam->pipeline &&
        cam->pipeline != node)
        pipeline_destroy(cam->pipeline);

    cam->pipeline = node;

    return 0;
}

Camera *camera_create(
    const char *dev)
{
    Camera *cam;

    cam = calloc(1,
                 sizeof(*cam));
    if (!cam)
        return NULL;

    if (camera_open(cam,
                    dev) < 0)
    {
        free(cam);
        return NULL;
    }

    return cam;
}

void camera_destroy(
    Camera *cam)
{
    if (!cam)
        return;

    camera_close(cam);
    free(cam);
}

int camera_open(
    Camera *cam,
    const char *dev)
{
    if (!cam ||
        !dev)
        return -1;

    cam->listener_count = 0;
    cam->pipeline = NULL;

    if (v4l2_device_open(&cam->device,
                         dev) < 0)
        return -1;

    if (v4l2_device_set_format(
            &cam->device,
            1920,
            1080,
            V4L2_PIX_FMT_NV12) < 0)
    {
        v4l2_device_close(&cam->device);
        return -1;
    }

    if (v4l2_device_request_buffer(
            &cam->device,
            4) < 0)
    {
        v4l2_device_close(&cam->device);
        return -1;
    }

    return 0;
}

int camera_start(
    Camera *cam)
{
    return v4l2_device_start(
               &cam->device);
}

int camera_poll(
    Camera *cam)
{
    Frame frame;

    if (v4l2_device_capture(
            &cam->device,
            &frame) < 0)
        return -1;

    camera_dispatch_frame(cam,
                          &frame);

    if (cam->pipeline &&
        pipeline_push_frame(cam->pipeline,
                            &frame) < 0)
        return -1;

    if (v4l2_device_queue(
            &cam->device,
            frame.index) < 0)
        return -1;

    return 0;
}

void camera_close(
    Camera *cam)
{
    if (!cam)
        return;

    if (cam->pipeline)
    {
        pipeline_destroy(cam->pipeline);
        cam->pipeline = NULL;
    }

    v4l2_device_close(&cam->device);
    cam->listener_count = 0;
}
