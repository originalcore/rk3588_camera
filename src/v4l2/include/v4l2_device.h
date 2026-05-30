/*******************************************************************************
  * @file        v4l2_device.h
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


#ifndef __V4L2_DEVICE_H_
#define __V4L2_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "frame.h"

#define MAX_BUFFER_COUNT 8

typedef struct
{

    void *start;

    size_t length;

} VideoBuffer;

typedef struct
{

    int fd;

    char dev_name[64];

    int width;

    int height;

    unsigned int pixfmt;

    VideoBuffer buffers[MAX_BUFFER_COUNT];

    int buffer_count;

} V4L2Device;

int v4l2_device_open(
    V4L2Device *dev,
    const char *name);

int v4l2_device_query_capability(
    V4L2Device *dev);

int v4l2_device_set_format(
    V4L2Device *dev,
    int width,
    int height,
    unsigned int pixfmt);

int v4l2_device_request_buffer(
    V4L2Device *dev,
    int count);

int v4l2_device_start(
    V4L2Device *dev);

int v4l2_device_stop(
    V4L2Device *dev);

int v4l2_device_capture(
    V4L2Device *dev,
    Frame *frame);

int v4l2_device_queue(
    V4L2Device *dev,
    int index);

void v4l2_device_close(
    V4L2Device *dev);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
