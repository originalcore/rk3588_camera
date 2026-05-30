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
#include "camera_log.h"

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

static void camera_publish_event(
    Camera *cam,
    CameraEventType type,
    const Frame *frame,
    int code)
{
    CameraEvent event;

    if (!cam)
        return;

    camera_event_init(&event, type);
    event.code = code;

    if (frame)
        event.frame = *frame;

    event_bus_publish(&cam->event_bus, &event);
}

static void camera_pipeline_job(
    void *arg)
{
    Camera *cam = arg;
    Frame frame;
    int ret = 0;

    if (frame_queue_pop(&cam->queue, &frame) < 0)
        return;

    camera_publish_event(cam,
                         CAMERA_EVENT_FRAME_READY,
                         &frame,
                         0);

    if (cam->pipeline &&
        pipeline_push_frame(cam->pipeline, &frame) < 0)
        ret = -1;

    camera_dispatch_frame(cam, &frame);

    if (v4l2_device_queue(&cam->device, frame.index) < 0)
        ret = -1;

    if (ret < 0)
    {
        CAMERA_LOG_ERROR("failed to process queued frame");
        camera_set_error(cam);
        camera_publish_event(cam,
                             CAMERA_EVENT_LOST,
                             &frame,
                             ret);
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

int camera_subscribe_event(
    Camera *cam,
    CameraEventHandler handler,
    void *user_data)
{
    if (!cam)
        return -1;

    return event_bus_subscribe(&cam->event_bus,
                               handler,
                               user_data);
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

    if (frame_queue_init(&cam->queue) < 0)
    {
        camera_set_error(cam);
        return -1;
    }

    if (event_bus_init(&cam->event_bus) < 0)
    {
        frame_queue_deinit(&cam->queue);
        camera_set_error(cam);
        return -1;
    }

    if (v4l2_device_open(&cam->device,
                         config->device) < 0)
    {
        event_bus_deinit(&cam->event_bus);
        frame_queue_deinit(&cam->queue);
        camera_set_error(cam);
        return -1;
    }

    if (v4l2_device_query_capability(&cam->device) < 0)
    {
        v4l2_device_close(&cam->device);
        event_bus_deinit(&cam->event_bus);
        frame_queue_deinit(&cam->queue);
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
        event_bus_deinit(&cam->event_bus);
        frame_queue_deinit(&cam->queue);
        camera_set_error(cam);
        return -1;
    }

    if (v4l2_device_request_buffer(
            &cam->device,
            config->buffer_count) < 0)
    {
        v4l2_device_close(&cam->device);
        event_bus_deinit(&cam->event_bus);
        frame_queue_deinit(&cam->queue);
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

    frame_queue_reset(&cam->queue);

    if (thread_pool_init(&cam->pipeline_pool, 1) < 0)
    {
        v4l2_device_stop(&cam->device);

        if (cam->pipeline)
            pipeline_stop(cam->pipeline);

        camera_set_error(cam);
        return -1;
    }

    cam->state = CAMERA_STATE_RUNNING;
    camera_publish_event(cam,
                         CAMERA_EVENT_STREAM_ON,
                         NULL,
                         0);

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
        thread_pool_stop(&cam->pipeline_pool);
        thread_pool_deinit(&cam->pipeline_pool);

        if (v4l2_device_stop(&cam->device) < 0)
            ret = -1;

        if (cam->pipeline &&
            pipeline_stop(cam->pipeline) < 0)
            ret = -1;
    }

    cam->state = CAMERA_STATE_OPEN;
    camera_publish_event(cam,
                         CAMERA_EVENT_STREAM_OFF,
                         NULL,
                         ret);

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

    if (frame_queue_push(&cam->queue, &frame) < 0)
    {
        v4l2_device_queue(&cam->device, frame.index);
        CAMERA_LOG_WARN("frame queue is full, drop frame index=%d",
                        frame.index);
        camera_publish_event(cam,
                             CAMERA_EVENT_LOST,
                             &frame,
                             -1);
        return 0;
    }

    if (thread_pool_submit(&cam->pipeline_pool,
                           camera_pipeline_job,
                           cam) < 0)
    {
        Frame dropped;

        if (frame_queue_pop(&cam->queue, &dropped) == 0)
            v4l2_device_queue(&cam->device, dropped.index);

        CAMERA_LOG_WARN("pipeline thread pool is full, drop frame index=%d",
                        frame.index);
        camera_publish_event(cam,
                             CAMERA_EVENT_LOST,
                             &frame,
                             -1);
        return 0;
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
    event_bus_deinit(&cam->event_bus);
    frame_queue_deinit(&cam->queue);
    cam->listener_count = 0;
    cam->state = CAMERA_STATE_CLOSED;
}
