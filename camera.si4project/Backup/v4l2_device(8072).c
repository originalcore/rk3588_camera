/*******************************************************************************
  * @file        v4l2_device.c
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

#include "v4l2_device.h"

return 0;
}

int v4l2_device_request_buffer(
    V4L2Device *dev,
    int count)
{
    struct v4l2_requestbuffers req;

    memset(&req, 0, sizeof(req));

    req.count = count;

    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(dev->fd,
              VIDIOC_REQBUFS,
              &req) < 0)
        return -1;

    dev->buffer_count = req.count;

    for (int i = 0; i < req.count; i++)
    {

        struct v4l2_buffer buf;

        memset(&buf, 0, sizeof(buf));

        buf.index = i;

        buf.type = req.type;

        buf.memory = req.memory;

        ioctl(dev->fd,
              VIDIOC_QUERYBUF,
              &buf);

        dev->buffers[i].length = buf.length;

        dev->buffers[i].start = mmap(NULL,
                                     buf.length,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED,
                                     dev->fd,
                                     buf.m.offset);

        ioctl(dev->fd,
              VIDIOC_QBUF,
              &buf);
    }

    return 0;
}

int v4l2_device_start(
    V4L2Device *dev)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    return ioctl(dev->fd,
                 VIDIOC_STREAMON,
                 &type);
}

int v4l2_device_capture(
    V4L2Device *dev,
    Frame *frame)
{
    struct v4l2_buffer buf;

    memset(&buf, 0, sizeof(buf));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    buf.memory = V4L2_MEMORY_MMAP;

    if (ioctl(dev->fd,
              VIDIOC_DQBUF,
              &buf) < 0)
        return -1;

    frame->data = dev->buffers[buf.index].start;

    frame->size = buf.bytesused;

    frame->index = buf.index;

    frame->width = dev->width;

    frame->height = dev->height;

    return 0;
}

int v4l2_device_queue(
    V4L2Device *dev,
    int index)
{
    struct v4l2_buffer buf;

    memset(&buf, 0, sizeof(buf));

    buf.index = index;

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    buf.memory = V4L2_MEMORY_MMAP;

    return ioctl(dev->fd,
                 VIDIOC_QBUF,
                 &buf);
}

