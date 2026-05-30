/*******************************************************************************
  * @file        main.c
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
#include "version.h"
#include "camera_manager.h"
#include "camera_log.h"
#include "pipeline.h"
#include "pipeline_node_factory.h"

#include <linux/videodev2.h>
static void print_version(void)
{
    CAMERA_LOG_INFO("%s", version_string);
}

int main(int argc, char *argv[])
{
    CameraManager *mgr;

    Camera *front_cam;

    PipelineNode *encoder;

    PipelineNode *rtsp;

    CameraConfig config = {
        .device = "/dev/video0",
        .width = 1920,
        .height = 1080,
        .pixfmt = V4L2_PIX_FMT_NV12,
        .buffer_count = 4,
    };

    (void)argc;
    (void)argv;

    if (camera_log_init("conf/zlog.conf") < 0)
        return 1;

    mgr = camera_manager_create();
    if (!mgr)
    {
        CAMERA_LOG_ERROR("failed to create camera manager");
        camera_log_fini();
        return 1;
    }

    print_version();

    front_cam = camera_create(&config);
    if (!front_cam)
    {
        CAMERA_LOG_ERROR("failed to open camera device");
        camera_manager_destroy(mgr);
        camera_log_fini();
        return 1;
    }

    if (camera_start(front_cam) < 0)
    {
        CAMERA_LOG_ERROR("failed to start camera device");
        camera_destroy(front_cam);
        camera_manager_destroy(mgr);
        camera_log_fini();
        return 1;
    }

    encoder = encoder_node_create();
    rtsp = rtsp_node_create();
    if (!encoder || !rtsp)
    {
        CAMERA_LOG_ERROR("failed to create pipeline nodes");
        pipeline_destroy(encoder);
        pipeline_destroy(rtsp);
        camera_destroy(front_cam);
        camera_manager_destroy(mgr);
        camera_log_fini();
        return 1;
    }

    encoder->next = rtsp;

    camera_set_pipeline(front_cam, encoder);

    if (camera_manager_add(mgr, front_cam) < 0)
    {
        CAMERA_LOG_ERROR("failed to add camera");
        camera_destroy(front_cam);
        camera_manager_destroy(mgr);
        camera_log_fini();
        return 1;
    }

    front_cam = NULL;

    while (1)
    {

        Camera *cam = camera_manager_get(mgr, 0);

        if (!cam || camera_poll(cam) < 0)
        {
            CAMERA_LOG_ERROR("failed to poll camera");
            break;
        }
    }

    camera_manager_destroy(mgr);
    camera_log_fini();

    return 0;
}
