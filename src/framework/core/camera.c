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
#include <string.h>

static void camera_set_error(
    Camera *cam)
{
    if (cam)
        cam->state = CAMERA_STATE_ERROR;
}

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

static int camera_validate_config(
    const CameraConfig *config)
{
    if (!config ||
        !config->device ||
        config->width <= 0 ||
        config->height <= 0 ||
        config->buffer_count <= 0 ||
        config->buffer_count > MAX_BUFFER_COUNT)
        return -1;

    return 0;
}

int camera_add_listener(
    Camera *cam,
    FrameListener *listener)
{
    if (!cam ||
        !listener ||
        cam->listener_count >= MAX_FRAME_LISTENER)
        return -1;

    cam->listeners[cam->listener_count++] = listener;

    return 0;
}

int camera_set_pipeline(
    Camera *cam,
    PipelineNode *node)
{
    if (!cam ||
        cam->state == CAMERA_STATE_RUNNING)
        return -1;

    if (cam->pipeline &&
        cam->pipeline != node)
        pipeline_destroy(cam->pipeline);

    cam->pipeline = node;

    return 0;
}

Camera *camera_create(
    const CameraConfig *config)
{
    Camera *cam;

    cam = calloc(1, sizeof(*cam));
    if (!cam)
        return NULL;

    cam->state = CAMERA_STATE_CLOSED;

    if (camera_open(cam, config) < 0)
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
    const CameraConfig *config)
{
    if (!cam ||
        camera_validate_config(config) < 0 ||
        cam->state != CAMERA_STATE_CLOSED)
        return -1;

    memset(&cam->config, 0, sizeof(cam->config));
    cam->config = *config;
    cam->listener_count = 0;
    cam->pipeline = NULL;

    if (v4l2_device_open(&cam->device,
                         config->device) < 0)
    {
        camera_set_error(cam);
        return -1;
    }

    if (v4l2_device_query_capability(&cam->device) < 0)
    {
        v4l2_device_close(&cam->device);
        camera_set_error(cam);
        return -1;
    }

    if (v4l2_device_set_format(
            &cam->device,
            config->width,
            config->height,
            config->pixfmt) < 0)
    {
        v4l2_device_close(&cam->device);
        camera_set_error(cam);
        return -1;
    }

    if (v4l2_device_request_buffer(
            &cam->device,
            config->buffer_count) < 0)
    {
        v4l2_device_close(&cam->device);
        camera_set_error(cam);
        return -1;
    }

    cam->state = CAMERA_STATE_OPEN;

    return 0;
}

int camera_start(
    Camera *cam)
{
    if (!cam ||
        cam->state != CAMERA_STATE_OPEN)
        return -1;

    if (cam->pipeline &&
        pipeline_start(cam->pipeline) < 0)
    {
        camera_set_error(cam);
        return -1;
    }

    if (v4l2_device_start(&cam->device) < 0)
    {
        if (cam->pipeline)
            pipeline_stop(cam->pipeline);

        camera_set_error(cam);
        return -1;
    }

    cam->state = CAMERA_STATE_RUNNING;

    return 0;
}

int camera_stop(
    Camera *cam)
{
    int ret = 0;

    if (!cam)
        return -1;

    if (cam->state == CAMERA_STATE_CLOSED)
        return 0;

    if (cam->state == CAMERA_STATE_RUNNING)
    {
        if (v4l2_device_stop(&cam->device) < 0)
            ret = -1;

        if (cam->pipeline &&
            pipeline_stop(cam->pipeline) < 0)
            ret = -1;
    }

    cam->state = CAMERA_STATE_OPEN;

    return ret;
}

int camera_poll(
    Camera *cam)
{
    Frame frame;

    if (!cam ||
        cam->state != CAMERA_STATE_RUNNING)
        return -1;

    if (v4l2_device_capture(&cam->device, &frame) < 0)
    {
        camera_set_error(cam);
        return -1;
    }

    if (cam->pipeline &&
        pipeline_push_frame(cam->pipeline, &frame) < 0)
    {
        v4l2_device_queue(&cam->device, frame.index);
        camera_set_error(cam);
        return -1;
    }

    camera_dispatch_frame(cam, &frame);

    if (v4l2_device_queue(&cam->device, frame.index) < 0)
    {
        camera_set_error(cam);
        return -1;
    }

    return 0;
}

void camera_close(
    Camera *cam)
{
    if (!cam)
        return;

    if (cam->state == CAMERA_STATE_RUNNING)
        camera_stop(cam);

    if (cam->pipeline)
    {
        pipeline_destroy(cam->pipeline);
        cam->pipeline = NULL;
    }

    v4l2_device_close(&cam->device);
    cam->listener_count = 0;
    cam->state = CAMERA_STATE_CLOSED;
}
