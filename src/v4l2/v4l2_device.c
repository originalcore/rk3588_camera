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

#include <fcntl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

static void v4l2_device_release_buffers(
    V4L2Device *dev)
{
    for (int i = 0; i < dev->buffer_count; i++)
    {
        if (dev->buffers[i].start &&
            dev->buffers[i].start != MAP_FAILED)
        {
            munmap(dev->buffers[i].start,
                   dev->buffers[i].length);
        }

        dev->buffers[i].start = NULL;
        dev->buffers[i].length = 0;
    }

    dev->buffer_count = 0;
}

int v4l2_device_open(
    V4L2Device *dev,
    const char *name)
{
    memset(dev, 0, sizeof(*dev));
    dev->fd = -1;

    dev->fd = open(name,
                   O_RDWR);
    if (dev->fd < 0)
        return -1;

    strncpy(dev->dev_name,
            name,
            sizeof(dev->dev_name) - 1);

    return 0;
}

int v4l2_device_query_capability(
    V4L2Device *dev)
{
    struct v4l2_capability cap;

    if (!dev ||
        dev->fd < 0)
        return -1;

    memset(&cap, 0, sizeof(cap));

    if (ioctl(dev->fd,
              VIDIOC_QUERYCAP,
              &cap) < 0)
        return -1;

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
        return -1;

    if (!(cap.capabilities & V4L2_CAP_STREAMING))
        return -1;

    return 0;
}

int v4l2_device_set_format(
    V4L2Device *dev,
    int width,
    int height,
    unsigned int pixfmt)
{
    struct v4l2_format fmt;

    memset(&fmt, 0, sizeof(fmt));

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = pixfmt;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(dev->fd,
              VIDIOC_S_FMT,
              &fmt) < 0)
        return -1;

    dev->width = fmt.fmt.pix.width;
    dev->height = fmt.fmt.pix.height;
    dev->pixfmt = fmt.fmt.pix.pixelformat;

    return 0;
}

int v4l2_device_request_buffer(
    V4L2Device *dev,
    int count)
{
    struct v4l2_requestbuffers req;

    if (count <= 0 ||
        count > MAX_BUFFER_COUNT)
        return -1;

    memset(&req, 0, sizeof(req));

    req.count = count;

    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(dev->fd,
              VIDIOC_REQBUFS,
              &req) < 0)
        return -1;

    if (req.count <= 0 ||
        req.count > MAX_BUFFER_COUNT)
        return -1;

    dev->buffer_count = 0;

    for (unsigned int i = 0; i < req.count; i++)
    {

        struct v4l2_buffer buf;

        memset(&buf, 0, sizeof(buf));

        buf.index = i;

        buf.type = req.type;

        buf.memory = req.memory;

        if (ioctl(dev->fd,
                  VIDIOC_QUERYBUF,
                  &buf) < 0)
        {
            v4l2_device_release_buffers(dev);
            return -1;
        }

        dev->buffers[i].length = buf.length;

        dev->buffers[i].start = mmap(NULL,
                                     buf.length,
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED,
                                     dev->fd,
                                     buf.m.offset);

        if (dev->buffers[i].start == MAP_FAILED)
        {
            dev->buffers[i].start = NULL;
            v4l2_device_release_buffers(dev);
            return -1;
        }

        dev->buffer_count++;

        if (ioctl(dev->fd,
                  VIDIOC_QBUF,
                  &buf) < 0)
        {
            v4l2_device_release_buffers(dev);
            return -1;
        }
    }

    return 0;
}

int v4l2_device_start(
    V4L2Device *dev)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (!dev ||
        dev->fd < 0)
        return -1;

    if (dev->streaming)
        return 0;

    if (ioctl(dev->fd,
              VIDIOC_STREAMON,
              &type) < 0)
        return -1;

    dev->streaming = 1;

    return 0;
}

int v4l2_device_stop(
    V4L2Device *dev)
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (!dev ||
        dev->fd < 0)
        return -1;

    if (!dev->streaming)
        return 0;

    if (ioctl(dev->fd,
              VIDIOC_STREAMOFF,
              &type) < 0)
        return -1;

    dev->streaming = 0;

    return 0;
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

    if (buf.index >= (unsigned int)dev->buffer_count)
        return -1;

    frame->data = dev->buffers[buf.index].start;

    frame->size = buf.bytesused;

    frame->index = buf.index;

    frame->width = dev->width;

    frame->height = dev->height;

    frame->pixfmt = dev->pixfmt;

    frame->timestamp =
        (uint64_t)buf.timestamp.tv_sec * 1000000ULL +
        (uint64_t)buf.timestamp.tv_usec;

    return 0;
}

int v4l2_device_queue(
    V4L2Device *dev,
    int index)
{
    struct v4l2_buffer buf;

    if (index < 0 ||
        index >= dev->buffer_count)
        return -1;

    memset(&buf, 0, sizeof(buf));

    buf.index = index;

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    buf.memory = V4L2_MEMORY_MMAP;

    return ioctl(dev->fd,
                 VIDIOC_QBUF,
                 &buf);
}

void v4l2_device_close(
    V4L2Device *dev)
{
    if (!dev)
        return;

    if (dev->fd >= 0 &&
        dev->streaming)
        v4l2_device_stop(dev);

    v4l2_device_release_buffers(dev);

    if (dev->fd >= 0)
    {
        close(dev->fd);
        dev->fd = -1;
    }
}
